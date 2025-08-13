/*
 *             :::::::::::     :::     :::::::::   ::::::::      :::
 *                 :+:       :+: :+:   :+:    :+: :+:    :+:   :+: :+:
 *                 +:+      +:+   +:+  +:+    +:+ +:+         +:+   +:+
 *                 +#+     +#++:++#++: +#++:++#:  :#:        +#++:++#++:
 *                 +#+     +#+     +#+ +#+    +#+ +#+   +#+# +#+     +#+
 *                 #+#     #+#     #+# #+#    #+# #+#    #+# #+#     #+#
 *                 ###     ###     ### ###    ###  ########  ###     ###
 *
 *                  Q W A D R O   E X E C U T I O N   E C O S Y S T E M
 *
 *                                   Public Test Build
 *                               (c) 2017 SIGMA FEDERATION
 *                             <https://sigmaco.org/qwadro/>
 */


#include "zglUtils.h"
#include "zglCommands.h"
#include "zglObjects.h"

_ZGL afxError _ZglBindFboAttachment(glVmt const* gl, GLenum glTarget, GLuint fbo, GLenum glAttachment, GLenum texTarget, GLuint texHandle, GLint level, GLuint z, afxBool multilayered)
{
    afxError err = AFX_ERR_NONE;

    if (fbo)
    {
        switch (texTarget)
        {
        case GL_RENDERBUFFER:
        {
            gl->NamedFramebufferRenderbuffer(fbo, glAttachment, GL_RENDERBUFFER, texHandle); _ZglThrowErrorOccuried();
            break;
        }
        case GL_TEXTURE_2D:
        case GL_TEXTURE_1D:
        case GL_TEXTURE_CUBE_MAP:
            // Multisampled textures are valid FBO attachments and must be handled via glNamedFramebufferTexture (entire texture, not layers)
        case GL_TEXTURE_2D_MULTISAMPLE:
        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        {
            GLenum texTarget2;
            if (texTarget != GL_TEXTURE_CUBE_MAP) texTarget2 = texTarget;
            else
            {
                // Where cubeMapFace is one of: GL_TEXTURE_CUBE_MAP_POSITIVE_X, ..., GL_TEXTURE_CUBE_MAP_NEGATIVE_Z.
                // Why? Cube maps are not layered textures in the OpenGL sense; you attach one face at a time using FramebufferTexture2D.
                texTarget2 = GL_TEXTURE_CUBE_MAP_POSITIVE_X + z;
            }
            gl->NamedFramebufferTexture(fbo, glAttachment, texHandle, level); _ZglThrowErrorOccuried();
            break;
        }
        case GL_TEXTURE_2D_ARRAY:
        case GL_TEXTURE_1D_ARRAY:
        case GL_TEXTURE_CUBE_MAP_ARRAY:
        case GL_TEXTURE_3D:
        {
            if (multilayered)
            {
                gl->NamedFramebufferTextureLayer(fbo, glAttachment, texHandle, level, z); _ZglThrowErrorOccuried();
            }
            else
            {
                gl->NamedFramebufferTexture(fbo, glAttachment, texHandle, level); _ZglThrowErrorOccuried();
            }
            break;
        }
        default: _ZglThrowErrorOccuried(); break;
        };

        // RETURN IF DSA
        return err;
    }

    switch (texTarget)
    {
    case GL_RENDERBUFFER:
    {
        // This is the right call for attaching a renderbuffer.
        gl->FramebufferRenderbuffer(glTarget, glAttachment, GL_RENDERBUFFER, texHandle); _ZglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_2D:
    case GL_TEXTURE_2D_MULTISAMPLE:
    case GL_TEXTURE_CUBE_MAP:
    {
        GLenum texTarget2;
        if (texTarget != GL_TEXTURE_CUBE_MAP) texTarget2 = texTarget;
        else
        {
            // Where cubeMapFace is one of: GL_TEXTURE_CUBE_MAP_POSITIVE_X, ..., GL_TEXTURE_CUBE_MAP_NEGATIVE_Z.
            // Why? Cube maps are not layered textures in the OpenGL sense; you attach one face at a time using FramebufferTexture2D.
            texTarget2 = GL_TEXTURE_CUBE_MAP_POSITIVE_X + z;
        }
        // These targets require glFramebufferTexture2D, where texTarget is either GL_TEXTURE_2D or GL_TEXTURE_2D_MULTISAMPLE.
        gl->FramebufferTexture2D(glTarget, glAttachment, texTarget2, texHandle, level); _ZglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_1D_ARRAY:
    case GL_TEXTURE_2D_ARRAY:
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
    case GL_TEXTURE_CUBE_MAP_ARRAY:
    {
        // glFramebufferTexture() attaches all layers, not a specific one.
        // If you're intending to attach a specific layer, you should use glFramebufferTextureLayer() instead.
        // Otherwise, this is okay if you're intentionally attaching the entire layered texture (e.g. for layered rendering with gl_Layer in geometry shaders).
        if (multilayered)
        {
            // all layers
            gl->FramebufferTexture(glTarget, glAttachment, texHandle, level); _ZglThrowErrorOccuried();
        }
        else
        {
            // one layer
            gl->FramebufferTextureLayer(glTarget, glAttachment, texHandle, level, z); _ZglThrowErrorOccuried();
        }
        break;
    }
    case GL_TEXTURE_3D:
    {
        // glFramebufferTexture3D is part of EXT_framebuffer_object, not core OpenGL.
        // In core OpenGL, you should use: glFramebufferTextureLayer(glTarget, glAttachment, texHandle, level, z);
#if 0
        gl->FramebufferTexture3D(glTarget, glAttachment, texTarget, texHandle, level, z); _ZglThrowErrorOccuried();
#else
        gl->FramebufferTextureLayer(glTarget, glAttachment, texHandle, level, z); _ZglThrowErrorOccuried();
#endif
        break;
    }
    case GL_TEXTURE_1D:
    {
        // This is valid in OpenGL 3.0+ (or 2.0 with GL_EXT_framebuffer_object).
        // But note: glFramebufferTexture1D is rarely used today, and not supported in Core Profile from OpenGL 4.5 onward.
        gl->FramebufferTexture1D(glTarget, glAttachment, texTarget, texHandle, level); _ZglThrowErrorOccuried();
        break;
    }
    default:
        if (multilayered)
        {
            // all layers
            gl->FramebufferTexture(glTarget, glAttachment, texHandle, level); _ZglThrowErrorOccuried();
        }
        else
        {
            // one layer
            gl->FramebufferTextureLayer(glTarget, glAttachment, texHandle, level, z); _ZglThrowErrorOccuried();
        }
        break;
    };
    return err;
}

_ZGL afxError _DpuBindAndSyncCanv(zglDpu* dpu, GLenum glTarget, avxCanvas canv, afxBool keepBound)
{
    //AfxEntry("canv=%p", canv);
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    afxUnit exuIdx = dpu->m.exuIdx;
    afxBool bound = (canv == dpu->canv);

    if (!canv)
    {
        //if (dpu->canv || dpu->canvGpuHandle)
        {
            gl->BindFramebuffer(glTarget, 0); _ZglThrowErrorOccuried();
            dpu->canvGpuHandle = NIL;
            dpu->canv = NIL;
        }
        return err;
    }

    AFX_ASSERT_OBJECTS(afxFcc_CANV, 1, &canv);
    GLuint glHandle = canv->glHandle[dpu->m.exuIdx];
    zglUpdateFlags devUpdReq = (canv->updFlags & ZGL_UPD_FLAG_DEVICE);

    if (glHandle && !(devUpdReq & ZGL_UPD_FLAG_DEVICE_INST))
    {
        //if ((canv != dpu->canv) || (glHandle != dpu->canvGpuHandle))
        {
            AFX_ASSERT(gl->IsFramebuffer(glHandle));
            gl->BindFramebuffer(glTarget, glHandle); _ZglThrowErrorOccuried();
            dpu->canvGpuHandle = glHandle;
            dpu->canv = canv;
            //dpu->canv = NIL;
        }
    }
    else
    {
        if (glHandle)
        {
            AFX_ASSERT(gl->IsFramebuffer(glHandle));
            gl->DeleteFramebuffers(1, &(glHandle)); _ZglThrowErrorOccuried();
            canv->glHandle[dpu->m.exuIdx] = NIL;
            glHandle = NIL;
        }
#if 0
        if ((!keepBound) && gl->CreateFramebuffers)
        {
            gl->CreateFramebuffers(1, &glHandle); _ZglThrowErrorOccuried();

            if (canv->m.tag.len)
            {
                gl->ObjectLabel(GL_FRAMEBUFFER, glHandle, canv->m.tag.len, (GLchar const*)canv->m.tag.start); _ZglThrowErrorOccuried();
            }
        }
        else
#endif
        {
            gl->GenFramebuffers(1, &(glHandle)); _ZglThrowErrorOccuried();
            gl->BindFramebuffer(glTarget, glHandle); _ZglThrowErrorOccuried();
            AFX_ASSERT(gl->IsFramebuffer(glHandle));
            bound = TRUE;

            if (canv->m.tag.len)
            {
                gl->ObjectLabel(GL_FRAMEBUFFER, glHandle, canv->m.tag.len, (GLchar const*)canv->m.tag.start); _ZglThrowErrorOccuried();
            }
        }

        canv->glHandle[dpu->m.exuIdx] = glHandle;
        dpu->canv = canv;

        afxUnit dsSurIdx[2] = { AFX_INVALID_INDEX, AFX_INVALID_INDEX };
        afxUnit surCnt = AvxQueryDrawBufferSlots(canv, &surCnt, &dsSurIdx[0], &dsSurIdx[1]);
        afxBool combinedDs = (dsSurIdx[1] == dsSurIdx[0]) && (dsSurIdx[0] != AFX_INVALID_INDEX);
        avxRaster surfaces[_ZGL_MAX_SURF_PER_CANV];
        AFX_ASSERT(_ZGL_MAX_SURF_PER_CANV >= surCnt);
        AvxGetDrawBuffers(canv, 0, surCnt, surfaces);

        afxUnit colorIdx = 0;
        GLenum glAttachment;
        GLuint glTexHandle;
        GLenum glTexTarget;

        for (afxUnit i = 0; i < surCnt; i++)
        {
            if (dsSurIdx[0] == i) // depth
            {
                if (!combinedDs)
                    glAttachment = GL_DEPTH_ATTACHMENT;
                else
                    glAttachment = GL_DEPTH_STENCIL_ATTACHMENT;
            }
            else if (dsSurIdx[1] == i)
            {
                if (!combinedDs)
                    glAttachment = GL_STENCIL_ATTACHMENT;
                else
                    glAttachment = GL_DEPTH_STENCIL_ATTACHMENT;
            }
            else
            {
                glAttachment = GL_COLOR_ATTACHMENT0 + colorIdx;
                ++colorIdx;
            }

            avxRaster ras = surfaces[i];

            if (!ras)
            {
                AfxThrowError();
                glTexHandle = 0;
                glTexTarget = GL_TEXTURE_2D;
                _ZglBindFboAttachment(dpu->gl, glTarget, NIL, glAttachment, glTexTarget, glTexHandle, 0, 0, (canv->m.whdMin.d > 1));
            }
            else
            {
                AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &ras);
                AFX_ASSERT(AvxTestRasterUsage(ras, avxRasterUsage_DRAW));

                DpuBindAndSyncRas(dpu, ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT, ras, FALSE);

                if (avxRasterUsage_DRAW == AvxTestRasterUsage(ras, avxRasterUsage_ALL))
                {
                    glTexHandle = ras->glHandle;
                    glTexTarget = GL_RENDERBUFFER;
                    AFX_ASSERT(gl->IsRenderBuffer(glTexHandle));
                }
                else
                {
                    glTexHandle = ras->glHandle;
                    glTexTarget = ras->glTarget;
                    AFX_ASSERT(gl->IsTexture(glTexHandle));
                }
                _ZglBindFboAttachment(dpu->gl, glTarget, NIL, glAttachment, glTexTarget, glTexHandle, ras->m.baseLod, ras->m.baseLayer, (canv->m.whdMin.d > 1));
            }
        }

        switch (gl->CheckFramebufferStatus(glTarget))
        {
        case GL_FRAMEBUFFER_COMPLETE:
            canv->updFlags &= ~(ZGL_UPD_FLAG_DEVICE);
            //AfxReportMessage("avxCanvas %p hardware-side data instanced.", canv);
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            AfxReportError("Not all framebuffer attachment points are framebuffer attachment complete.");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            AfxReportError("No images are attached to the framebuffer.");
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            AfxReportError("The combination of internal formats of the attached images violates an implementation-dependent set of restrictions.");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            AfxReportError("Incomplete draw frame buffer.");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            AfxReportError("Incomplete read frame buffer.");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            AfxReportError("Incomplete multisample.");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            AfxReportError("Incomplete layer targets.");
            break;
        case GL_FRAMEBUFFER_UNDEFINED:
            AfxReportError("Default framebuffer does not exist");
            break;
        default:
            AfxReportError("UNKNOWN");
            _ZglThrowErrorOccuried();
            break;
        }

        if (keepBound)
        {
            AFX_ASSERT(bound);
            dpu->canvGpuHandle = glHandle;
            dpu->canv = canv;
        }
        else
        {
            gl->BindFramebuffer(glTarget, 0); _ZglThrowErrorOccuried();
            dpu->canvGpuHandle = NIL;
            dpu->canv = NIL;
        }
    }
    return err;
}

_ZGL void DpuConcludeDrawScope(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

    AFX_ASSERT(dpu->inDrawScope != FALSE); // This is a transfer operation.
    dpu->inDrawScope = FALSE;

    // https://graphics1600.rssing.com/chan-25333833/all_p4.html
    // Switch to zero to immediately trigger any pending buffer operation.

    afxUnit invalidCnt = dpu->invalidDrawBufCnt;
    afxBool clipped = dpu->drawAreaClipped;

#if 0
    if (invalidCnt)
    {
        if (clipped)
        {
            gl->InvalidateSubFramebuffer(GL_DRAW_FRAMEBUFFER, invalidCnt, dpu->invalidDrawBufs, dpu->drawArea.x, dpu->drawArea.y, dpu->drawArea.w, dpu->drawArea.h); _ZglThrowErrorOccuried();
        }
        else
        {
            gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, invalidCnt, dpu->invalidDrawBufs); _ZglThrowErrorOccuried();
        }
    }
#endif

    _DpuBindAndSyncCanv(dpu, GL_READ_FRAMEBUFFER, 0, TRUE);
    _DpuBindAndSyncCanv(dpu, GL_DRAW_FRAMEBUFFER, 0, TRUE);

    if (dpu->mustCloseDrawScopeDgbGrp)
    {
        gl->PopDebugGroup(); _ZglThrowErrorOccuried();
    }

    gl->Flush(); _ZglThrowErrorOccuried(); // flush was presenting/swapping without wglSwapBuffers.

    avxCanvas canv = dpu->canv;

    if (canv)
    {
        afxUnit surCnt;
        surCnt = AvxQueryDrawBufferSlots(canv, NIL, NIL, NIL);
#if 0
        if (surCnt)
        {
            AFX_ASSERT(_ZGL_MAX_SURF_PER_CANV >= surCnt);
            avxRaster surfaces[_ZGL_MAX_SURF_PER_CANV];
            AvxGetDrawBuffers(canv, 0, surCnt, surfaces);

            for (afxUnit i = 0; i < surCnt; i++)
            {
                avxRaster ras = surfaces[i];

                if (ras)
                {
                    AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &ras);
                    ras->updFlags |= ZGL_UPD_FLAG_HOST_FLUSH;
                }
            }
        }
#endif
    }
}

_ZGL void DpuCommenceDrawScope(zglDpu* dpu, avxCanvas canv, afxRect const* area, afxUnit baseLayer, afxUnit layerCnt, afxUnit cCnt, avxDrawTarget const* c, avxDrawTarget const* d, avxDrawTarget const* s, afxString const* dbgTag, afxBool defFbo)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

    AFX_ASSERT(dpu->inDrawScope == FALSE); // This is a transfer operation.
    dpu->inDrawScope = TRUE;

    avxDrawTarget const* dt;

    if (dbgTag && dbgTag->len)
    {
        gl->PushDebugGroup(GL_DEBUG_SOURCE_THIRD_PARTY, 0, dbgTag->len, (const GLchar*)dbgTag->start); _ZglThrowErrorOccuried();
        dpu->mustCloseDrawScopeDgbGrp = TRUE;
    }

    /*
        The application should always call glClear() for every render target at the start of
        each render target's rendering sequence, provided that the previous contents of the
        attachments are not needed, of course. This explicitly tells the driver we do not
        need the previous state for this frame of rendering, and thus we avoid reading it
        back from memory, as well as putting any undefined buffer contents into a defined "clear color" state.

        One common mistake which is seen here is only clearing part of the framebuffer; i.e.
        calling glClear() while only a portion of the render target is active because of a
        scissor rectangle with only partial screen coverage. We can only completely drop the
        render state when it applies to whole surfaces, so a clear of the whole render target
        should be performed where possible.

        The final requirement placed on the application for efficient use of FBOs in the OpenGL ES
        API is that it should tell the driver which of the color / depth / stencil attachments are
        simply transient working buffers, the value of which can be discarded at the end of rendering
        the current render pass. For example, nearly every 3D render will use color and depth, but for
        most applications the depth buffer is transient and can be safely invalidated. Failure to
        invalidate the unneeded buffers may result in them being written back to memory, wasting memory
        bandwidth and increasing energy consumption of the rendering process.

        The most common mistake at this point is to treat glInvalidateFramebuffer() as equivalent to glClear()
        and place the invalidate call for frame N state at the first use of that FBO in frame N+1. This is too late!
        The purpose of the invalidate call is to tell the driver that that the buffers do not need to be kept,
        so we need to modify the work submission to the GPU for frame which produces those buffers. Telling us
        in the next frame is often after the original frame has been processed. The application needs to ensure
        that the driver knows which buffers are transient before the framebuffer is flushed. Therefore transient
        buffers in frame N should be indicated by calling glInvalidateFramebuffer()before unbinding the FBO in frame N.
        For example:

        #define ALL_BUFFERS COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT | STENCIL_BUFFER_BIT
        static const GLEnum invalid_ap[2] = { GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT };

        glClear( ALL_BUFFERS )    // Clear initial state
        glDraw...( ... )          // Draw something to FBO 0 (window surface)

        glBindFramebuffer( 1 )    // Switch away from FBO 0, does not trigger rendering
        glClear( ALL_BUFFERS )    // Clear initial state
        glDraw...( ... )          // Draw something to FBO 1
        ...                      // Draw FBO 1 to completion
        glInvalidateFramebuffer( GL_FRAMEBUFFER, 2, &invalid_ap[0] ); // Only keep color

        glBindFramebuffer(0)      // Switch to FBO 0, unbind and flush FBO 1 for rendering
        glDraw...( ... )          // Draw something else to FBO 0 (window surface)

        glBindFramebuffer( 2 )    // Switch away from FBO 0, does not trigger rendering
        glClear( ALL_BUFFERS )    // Clear initial state
        glDraw...( ... )          // Draw something to FBO 2
        ...                      // Draw FBO 2 to completion
        glInvalidateFramebuffer( GL_FRAMEBUFFER, 2, &invalid_ap[0] ); // Only keep color

        glBindFramebuffer(0)      // Switch to FBO 0, unbind and flush FBO 2 for rendering
        glDraw...( ... )          // Draw something else to FBO 0 (window surface)

        eglSwapBuffers()          // Tell EGL we have finished, flush FBO 0 for rendering
    */



    if (!canv)
    {
        if (defFbo)
        {
            //_DpuBindAndSyncCanv(gl, GL_READ_FRAMEBUFFER, canv);
            _DpuBindAndSyncCanv(dpu, GL_DRAW_FRAMEBUFFER, canv, TRUE);
            //dpu->activeRs.canv = canv;

            dt = &c[0];

            switch (dt->storeOp)
            {
            case avxStoreOp_STORE:
            {
                gl->DrawBuffer(GL_BACK); _ZglThrowErrorOccuried();
                break;
            }
            case avxStoreOp_DISCARD:
            case avxStoreOp_DONT_CARE:
            default:
            {
                gl->DrawBuffer(GL_NONE); _ZglThrowErrorOccuried();
                gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 1, (GLenum[]) { GL_BACK }); _ZglThrowErrorOccuried();
                break;
            }
            }

            switch (dt->loadOp)
            {
            case avxLoadOp_LOAD: break;
            case avxLoadOp_CLEAR:
            {
                gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 1, (GLenum[]) { GL_BACK }); _ZglThrowErrorOccuried();
                afxReal const *rgba = dt->clearVal.rgba;
                gl->ClearColor(rgba[0], rgba[1], rgba[2], rgba[3]); _ZglThrowErrorOccuried();
                gl->Clear(GL_COLOR_BUFFER_BIT); _ZglThrowErrorOccuried();
                break;
            }
            case avxLoadOp_DONT_CARE:
            default:
            {
                gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 1, (GLenum[]) { GL_BACK }); _ZglThrowErrorOccuried();
                break;
            }
            }
        }
        else
        {
            AfxThrowError();
        }
        // RETURN
        return;
    }

    afxRect areaMax;
    AvxGetCanvasArea(canv, AVX_ORIGIN_ZERO, &areaMax);
    avxRange canvWhd = { areaMax.w, areaMax.h, 1 };

    afxUnit maxColSurCnt;
    afxUnit dsSurIdx[2] = { AFX_INVALID_INDEX, AFX_INVALID_INDEX };
    afxUnit maxSurCnt = AvxQueryDrawBufferSlots(canv, &maxColSurCnt, &dsSurIdx[0], &dsSurIdx[1]);
    afxBool hasDs = ((dsSurIdx[1] != AFX_INVALID_INDEX) || (dsSurIdx[0] != AFX_INVALID_INDEX));
    afxBool combinedDs = (hasDs && (dsSurIdx[1] == dsSurIdx[0]));
    cCnt = AFX_MIN(cCnt, maxColSurCnt);

    afxUnit storeCnt = 0;
    GLenum storeBufs[_ZGL_MAX_SURF_PER_CANV];
    afxMask storeBitmask = NIL;

    afxUnit clearCnt = 0;
    GLenum clearBufs[_ZGL_MAX_SURF_PER_CANV];
    afxUnit invalidCnt = 0;
    GLenum invalidBufs[_ZGL_MAX_SURF_PER_CANV];

    GLenum glAttach = GL_NONE;
    dpu->invalidDrawBufCnt = 0;

    for (afxUnit i = 0; i < cCnt; i++)
    {
        glAttach = GL_COLOR_ATTACHMENT0 + i;
        dt = &c[i];

        if (!(i < maxColSurCnt))
            continue;

        switch (dt->storeOp)
        {
        case avxStoreOp_STORE:
        {
            storeBufs[storeCnt] = glAttach;
            storeCnt++;
            storeBitmask |= AFX_BITMASK(i);
            break;
        }
        case avxStoreOp_DISCARD:
        case avxStoreOp_DONT_CARE:
        default:
        {
            storeBufs[storeCnt] = GL_NONE;
            storeCnt++;
            storeBitmask |= AFX_BITMASK(8 + i);

            invalidBufs[invalidCnt] = glAttach;
            dpu->invalidDrawBufs[invalidCnt] = glAttach;
            dpu->invalidDrawBufCnt = invalidCnt;
            ++invalidCnt;
            break;
        }
        }

        // Prepare cleanups and invalidations.

        if (dt->loadOp != avxLoadOp_LOAD)
        {
            /*
                The application should always call glClear() for every render target at the start of
                each render target's rendering sequence, provided that the previous contents of the
                attachments are not needed, of course.
            */
            clearBufs[clearCnt] = glAttach;
            ++clearCnt;
        }
    }

    if ((dsSurIdx[0] != AFX_INVALID_INDEX)) // depth (and stencil)
    {
        dt = d;
        glAttach = (s && combinedDs) ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;

        // Prepare cleanups and invalidations.

        if (dt && (dt->loadOp != avxLoadOp_LOAD))
        {
            /*
                The application should always call glClear() for every render target at the start of
                each render target's rendering sequence, provided that the previous contents of the
                attachments are not needed, of course.
            */
            clearBufs[clearCnt] = glAttach;
            ++clearCnt;
        }

        if (dt && (dt->storeOp != avxStoreOp_STORE))
        {
            invalidBufs[invalidCnt] = glAttach;
            ++invalidCnt;
        }
    }

    if ((dsSurIdx[1] != AFX_INVALID_INDEX) && !combinedDs) // stencil
    {
        dt = s;
        glAttach = GL_STENCIL_ATTACHMENT;

        // Prepare cleanups and invalidations.

        if (dt && (dt->loadOp != avxLoadOp_LOAD))
        {
            /*
                The application should always call glClear() for every render target at the start of
                each render target's rendering sequence, provided that the previous contents of the
                attachments are not needed, of course.
            */
            clearBufs[clearCnt] = glAttach;
            ++clearCnt;
        }

        if (dt && (dt->storeOp != avxStoreOp_STORE))
        {
            invalidBufs[invalidCnt] = glAttach;
            ++invalidCnt;
        }
    }

    // TODO iterate for each canvas surface against arguments

    _DpuBindAndSyncCanv(dpu, GL_DRAW_FRAMEBUFFER, canv, TRUE);
    _DpuBindAndSyncCanv(dpu, GL_READ_FRAMEBUFFER, canv, TRUE);
    GLuint glHandle = canv->glHandle[dpu->m.exuIdx];
    AFX_ASSERT(gl->IsFramebuffer(glHandle));
    //dpu->activeRs.canv = canv;

    afxBool clipped = ((area->x && (area->x > 0)) ||
        (area->y && (area->y > 0)) ||
        (area->w && (area->w < canvWhd.w)) ||
        (area->h && (area->h < canvWhd.h)));
    dpu->drawAreaClipped = clipped;

    if (invalidCnt)
    {
        if (clipped)
        {
            gl->InvalidateSubFramebuffer(GL_DRAW_FRAMEBUFFER, invalidCnt, dpu->invalidDrawBufs, area->x, area->y, area->w, area->h); _ZglThrowErrorOccuried();
        }
        else
        {
            gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, invalidCnt, dpu->invalidDrawBufs); _ZglThrowErrorOccuried();
        }
    }

#if 0
    if (canv->storeBitmask != storeBitmask)
#endif
    {
        canv->storeBitmask = storeBitmask; // cache it
        gl->DrawBuffers(storeCnt, storeBufs); _ZglThrowErrorOccuried();

        // What to do with depth/stencil storage set to discard?
        // Should we force a glDepthMask to zero right there?
        // Should we force a glStencilMask to zero right there?
    }

    // Effectively invalidate the buffers.

    // TODO: Sanitize area to canvas' bounds.
    GLint scissor[4];
    scissor[0] = area->x;
    scissor[1] = area->y;
    scissor[2] = area->w;
    scissor[3] = area->h;
    AFX_ASSERT(area->w);
    AFX_ASSERT(area->h);
    AFX_ASSERT(gl->ScissorArrayv);
    gl->ScissorArrayv(0, 1, scissor);

    // We must store the scissor rects to avoid DPU to further apply invalid rects.
    for (afxUnit iter = 0; iter < ZGL_MAX_VIEWPORTS; iter++)
    {
        dpu->nextScisRects[iter] = *area;
        dpu->nextScisUpdMask |= AFX_BITMASK(iter);
    }

    // Effectively clear the buffers.

    for (afxUnit i = 0; i < clearCnt; i++)
    {
        switch (clearBufs[i])
        {
        case GL_DEPTH_ATTACHMENT:
        {
            dt = d;
            gl->ClearBufferfv(GL_DEPTH, 0, &dt->clearVal.depth); _ZglThrowErrorOccuried();
            break;
        }
        case GL_DEPTH_STENCIL_ATTACHMENT:
        {
            dt = d;
            gl->ClearBufferfi(GL_DEPTH_STENCIL, 0, dt->clearVal.depth, dt->clearVal.stencil); _ZglThrowErrorOccuried();
            break;
        }
        case GL_STENCIL_ATTACHMENT:
        {
            dt = s;
            GLint sCv = dt->clearVal.stencil;
            gl->ClearBufferiv(GL_STENCIL, 0, &sCv); _ZglThrowErrorOccuried();
            break;
        }
        default:
        {
            dt = &c[i];
            afxReal const* rgba = dt->clearVal.rgba;
            GLint dbi = clearBufs[i] - GL_COLOR_ATTACHMENT0;
            gl->ClearBufferfv(GL_COLOR, /*GL_DRAW_BUFFER0 +*/ dbi, rgba); _ZglThrowErrorOccuried();
            break;
        }
        }
    }
}

_ZGL afxError _ZglDpuResolveCanvas(zglDpu* dpu, avxCanvas src, avxCanvas dst, afxUnit opCnt, avxRasterCopy const ops[])
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    AFX_ASSERT_OBJECTS(afxFcc_CANV, 1, &dst);
    AFX_ASSERT_OBJECTS(afxFcc_CANV, 1, &src);

    AFX_ASSERT(dpu->inDrawScope == FALSE); // This is a transfer operation.

    /*
        vkCmdResolveImage
        Purpose: Resolves a multisampled image to a non-multisampled image.
        Filter: Always uses nearest filtering (VK_FILTER_NEAREST).
        Restrictions:
            Source must be multisampled.
            Destination must be single-sampled.
            Format must match.
            Only supports resolving a 1:1 region (no scaling or filtering choice).

        To mimic vkCmdResolveImage, we should:
            Use nearest filtering only (to match Vulkan's behavior).
            Ensure that we resolve from a multisample framebuffer into a single-sample texture.
            Avoid any format conversion or scaling.
    */

    for (afxUnit sbufIdx = 0, dbufIdx = 0; (sbufIdx < src->m.slotCnt) && (dbufIdx < dst->m.slotCnt); sbufIdx++, dbufIdx++)
    {
        if (src->m.slots[sbufIdx].ras && dst->m.slots[dbufIdx].ras)
            _ZglDpuResolveRaster(dpu, src->m.slots[sbufIdx].ras, dst->m.slots[dbufIdx].ras, opCnt, ops);
    }
    return err;
}

_ZGL afxError _ZglDpuBlitCanvas(zglDpu* dpu, avxCanvas src, avxCanvas dst, afxUnit opCnt, avxRasterBlit const ops[], avxTexelFilter flt)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    AFX_ASSERT_OBJECTS(afxFcc_CANV, 1, &dst);
    AFX_ASSERT_OBJECTS(afxFcc_CANV, 1, &src);

    AFX_ASSERT(dpu->inDrawScope == FALSE); // This is a transfer operation.

    /*
        vkCmdBlitImage
        Purpose: General-purpose image copy, potentially with format conversion and filtering.
        Filter: User-specified --- VK_FILTER_NEAREST or VK_FILTER_LINEAR.
        More flexible, can scale images and convert formats.

        To match Vulkan's vkCmdBlitImage, OpenGL's glBlitFramebuffer can be used similarly,
        but with either GL_LINEAR or GL_NEAREST as needed. Or, for more general flexibility
        (e.g., for mipmap generation or format conversion), a shader-based blit might be required.
    */

    for (afxUnit sbufIdx = 0, dbufIdx = 0; (sbufIdx < src->m.slotCnt) && (dbufIdx < dst->m.slotCnt); sbufIdx++, dbufIdx++)
    {
        if (src->m.slots[sbufIdx].ras && dst->m.slots[dbufIdx].ras)
            _ZglDpuBlitRaster(dpu, src->m.slots[sbufIdx].ras, dst->m.slots[dbufIdx].ras, opCnt, ops, flt);
    }
    return err;
}

#if 0
_ZGL afxError _ZglReadjustCanvasCb(avxCanvas canv, afxWarp const whd)
{
	afxError err = AFX_ERR_NONE;
	AFX_ASSERT_OBJECTS(afxFcc_CANV, 1, &canv);
	AFX_ASSERT(whd);
	AFX_ASSERT(whd[0]);
	AFX_ASSERT(whd[1]);
    afxWarp minWhd = { ZGL_MAX_CANVAS_WIDTH, ZGL_MAX_CANVAS_HEIGHT, ZGL_MAX_CANVAS_LAYERS }, surWhd;
    
    for (afxUnit i = 0; i < canv->m.slotCnt; i++)
    {
        avxRaster ras = canv->m.slots[i].ras;

        if (ras)
        {
            AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &ras);
            AvxGetRasterExtent(ras, 0, surWhd);
            AfxWhdMin(minWhd, minWhd, surWhd);
        }
    }

    if ((whd[0] > minWhd[0]) || (whd[1] > minWhd[1]))
        AfxThrowError();

    if ((whd[0] != minWhd[0]) || (whd[1] != minWhd[1]))
    {
        canv->m.whd[0] = whd[0] < minWhd[0] ? whd[0] : minWhd[0];
        canv->m.whd[1] = whd[1] < minWhd[1] ? whd[1] : minWhd[1];

        for (afxUnit i = 0; i < )
        canv->updFlags[exuIdx] |= ZGL_UPD_FLAG_DEVICE_INST;
    }
	return err;
}
#endif

_ZGL afxError _AfxCanvDropAllSurfaces(avxCanvas canv)
{
	afxError err = AFX_ERR_NONE;
	AFX_ASSERT_OBJECTS(afxFcc_CANV, 1, &canv);

	//canv->m.colorCnt = 0;
    //canv->m.surfCnt = 0;
	return err;
}

_ZGL afxError _ZglCanvDtor(avxCanvas canv)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_CANV, 1, &canv);

    afxDrawSystem dsys = AfxGetProvider(canv);

    for (afxUnit i = 0; i < canv->m.slotCnt; i++)
    {
        avxRaster ras = canv->m.slots[i].ras;

        if (ras)
        {
            AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &ras);
            AfxDisposeObjects(1, &ras);
            canv->m.slots[i].ras = NIL;
        }
    }

    for (afxUnit i = 0; i < ZGL_MAX_FBO_HANDLES; i++)
    {
        if (canv->glHandle[i])
        {
            _ZglDsysEnqueueDeletion(dsys, i, GL_FRAMEBUFFER, (afxSize)canv->glHandle[i]);
            canv->glHandle[i] = 0;
        }
    }

    if (_AVX_CANV_CLASS_CONFIG.dtor(canv))
        AfxThrowError();

    return err;
}

_ZGL afxError _ZglCanvCtor(avxCanvas canv, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;

    if (_AVX_CANV_CLASS_CONFIG.ctor(canv, args, invokeNo)) AfxThrowError();
    else
    {
        //canv->m.readjust = _ZglReadjustCanvasCb;

        afxDrawSystem dsys = AfxGetProvider(canv);
        canv->canvUniqueId = ++dsys->canvUniqueId;

        canv->updFlags = ZGL_UPD_FLAG_DEVICE_INST;
        AfxZero(canv->glHandle, sizeof(canv->glHandle));
        canv->storeBitmask = NIL;
    }
    return err;
}
