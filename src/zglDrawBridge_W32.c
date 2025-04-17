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

#include "zglCommands.h"
#include "zglObjects.h"
#include "zglUtils.h"
#include "qwadro/inc/sim/akxRenderer.h"

#define FORCE_ARG_REBIND TRUE
#define UNBIND_GL_BUF FALSE
//#define COHERENT_PUSHABLES

_ZGL void DpuMarkDebugStep(zglDpu* dpu, avxColor const color, afxString const* label)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    gl->DebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, AvxGetColorRgba8(color), GL_DEBUG_SEVERITY_NOTIFICATION, label->len, label->start); _ZglThrowErrorOccuried();
}

_ZGL void DpuPushDebugScope(zglDpu* dpu, avxColor const color, afxString const* label)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

    gl->PushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, AvxGetColorRgba8(color), label->len, label->start); _ZglThrowErrorOccuried();
    ++dpu->dbgScopeStackTop;
}

_ZGL void DpuPopDebugScope(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    
    if (!dpu->dbgScopeStackTop) AfxThrowError();
    else
    {
        gl->PopDebugGroup(); _ZglThrowErrorOccuried();
        --dpu->dbgScopeStackTop;
    }
}

// RENDERING SCOPE

_ZGL void DpuConcludeDrawScope(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

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

    //_DpuBindAndSyncCanv(dpu, GL_READ_FRAMEBUFFER, 0, TRUE);
    //_DpuBindAndSyncCanv(dpu, GL_DRAW_FRAMEBUFFER, 0, TRUE);
    
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
                afxReal const *rgba = dt->clearValue.rgba;
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
    }
    else
    {
        avxRange canvWhd = AvxGetCanvasArea(canv, AVX_ORIGIN_ZERO);

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
                storeBitmask |= AFX_BIT(i);
                break;
            }
            case avxStoreOp_DISCARD:
            case avxStoreOp_DONT_CARE:
            default:
            {
                storeBufs[storeCnt] = GL_NONE;
                storeCnt++;
                storeBitmask |= AFX_BIT(8 + i);

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

        afxBool clipped =  ((area->x && (area->x > 0)) ||
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
            dpu->nextScisUpdMask |= AFX_BIT(iter);
        }

        // Effectively clear the buffers.

        for (afxUnit i = 0; i < clearCnt; i++)
        {
            switch (clearBufs[i])
            {
            case GL_DEPTH_ATTACHMENT:
            {
                dt = d;
                gl->ClearBufferfv(GL_DEPTH, 0, &dt->clearValue.depth); _ZglThrowErrorOccuried();
                break;
            }
            case GL_DEPTH_STENCIL_ATTACHMENT:
            {
                dt = d;
                gl->ClearBufferfi(GL_DEPTH_STENCIL, 0, dt->clearValue.depth, dt->clearValue.stencil); _ZglThrowErrorOccuried();
                break;
            }
            case GL_STENCIL_ATTACHMENT:
            {
                dt = s;
                GLint sCv = dt->clearValue.stencil;
                gl->ClearBufferiv(GL_STENCIL, 0, &sCv); _ZglThrowErrorOccuried();
                break;
            }
            default:
            {
                dt = &c[i];
                afxReal const* rgba = dt->clearValue.rgba;
                GLint dbi = clearBufs[i] - GL_COLOR_ATTACHMENT0;
                gl->ClearBufferfv(GL_COLOR, /*GL_DRAW_BUFFER0 +*/ dbi, rgba); _ZglThrowErrorOccuried();
                break;
            }
            }
        }
    }
}

typedef enum avxPipelienAccess
{
    avxPipelienAccess_NONE,
    // All read accesses, always valid in any access mask, and is treated as equivalent 
    // to setting all READ access flags that are valid where it is used.
    avxPipelienAccess_MEMORY_R = AFX_BIT(0),
    // All write accesses, always valid in any access mask, and is treated as equivalent 
    // to setting all WRITE access flags that are valid where it is used.
    avxPipelienAccess_MEMORY_W = AFX_BIT(1),
    // read access to indirect command data read as part of an indirect build, trace, drawing or 
    // dispatching command. Such access occurs in the DRAW_INDIRECT pipeline stage.
    avxPipelienAccess_INDIRECT_R = AFX_BIT(2),
    // read access to an index buffer as part of an indexed drawing command. 
    // Such access occurs in the VERTEX_INPUT pipeline stage.
    avxPipelienAccess_INDEX_R = AFX_BIT(3),
    // read access to a vertex buffer as part of a drawing command. 
    // Such access occurs in the VERTEX_INPUT_BIT pipeline stage.
    avxPipelienAccess_VERTEX_R = AFX_BIT(4),
    // read access to a uniform buffer in any shader pipeline stage.
    avxPipelienAccess_UNIFORM_R = AFX_BIT(5),
    // read access to an input attachment within a render pass during subpass shading or fragment shading.
    avxPipelienAccess_INPUT_ATTACH_R = AFX_BIT(6),
    // read access to a uniform texel buffer, sampled image, storage buffer, physical storage buffer, 
    // shader binding table, storage texel buffer, or storage image in any shader pipeline stage.
    avxPipelienAccess_SHADER_R = AFX_BIT(7),
    // write access to a storage buffer, physical storage buffer, storage texel buffer, or storage image in any shader pipeline stage.
    avxPipelienAccess_SHADER_W = AFX_BIT(8),
    avxPipelienAccess_SHADER = (avxPipelienAccess_SHADER_R | avxPipelienAccess_SHADER_W),

    // read access to a color attachment, such as via blending (other than advanced blend operations), 
    // logic operations or certain render pass load operations in the COLOR_OUT pipeline stage or via 
    // fragment shader tile image reads in the FRAGMENT_SHADER pipeline stage.
    avxPipelienAccess_COLOR_OUT_R = AFX_BIT(9),
    // write access to a color, resolve, or depth/stencil resolve attachment during a render pass or via 
    // certain render pass load and store operations. Such access occurs in the COLOR_OUT pipeline stage.
    avxPipelienAccess_COLOR_OUT_W = AFX_BIT(10),
    avxPipelienAccess_COLOR_OUT = (avxPipelienAccess_COLOR_OUT_R | avxPipelienAccess_COLOR_OUT_W),

    // read access to a depth/stencil attachment, via depth or stencil operations or certain render pass load 
    // operations in the EARLY_TESTS or LATE_TESTS pipeline stages or via fragment shader tile image reads 
    // in the FRAGMENT_SHADER pipeline stage.
    avxPipelienAccess_DEPTH_OUT_R = AFX_BIT(11),
    // write access to a depth/stencil attachment, via depth or stencil operations or certain render pass load and store operations. 
    // Such access occurs in the EARLY_TESTS or LATE_TESTS pipeline stages.
    avxPipelienAccess_DEPTH_OUT_W = AFX_BIT(12),
    avxPipelienAccess_DEPTH_OUT = (avxPipelienAccess_DEPTH_OUT_R | avxPipelienAccess_DEPTH_OUT_W),

    // read access to an image or buffer in a copy operation. Such access occurs in ALL_TRANSFER pipeline stage.
    avxPipelienAccess_TRANSFER_R = AFX_BIT(13),
    // write access to an image or buffer in a clear or copy operation. Such access occurs in the ALL_TRANSFER_BIT pipeline stage.
    avxPipelienAccess_TRANSFER_W = AFX_BIT(14),
    avxPipelienAccess_TRANSFER = (avxPipelienAccess_TRANSFER_R | avxPipelienAccess_TRANSFER_W),

    // read access by a host operation. Accesses of this type are not performed through a resource, but directly on memory. 
    // Such access occurs in the HOST pipeline stage.
    avxPipelienAccess_HOST_R = AFX_BIT(15),
    // write access by a host operation. Accesses of this type are not performed through a resource, but directly on memory. 
    // Such access occurs in the HOST pipeline stage.
    avxPipelienAccess_HOST_W = AFX_BIT(16),
    avxPipelienAccess_HOST = (avxPipelienAccess_HOST_R | avxPipelienAccess_HOST_W)
} avxPipelineAccess;

_ZGL void _DpuPlacePipelineBarrier(zglDpu* dpu, avxPipelineStage dstStage, avxPipelineAccess dstAcc)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

    // Because GL pretend to be synchronous in execution model, it only expects a destination stage and access.

    avxPipelineAccess access;
    GLbitfield glAccess = NIL;

    if (access & avxPipelienAccess_VERTEX_R)
    {
        glAccess |= GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT;
        // If set, vertex data sourced from buffer objects after the barrier will reflect data written by shaders prior to the barrier.
        // The set of buffer objects affected by this bit is derived from the buffer object bindings used for generic vertex attributes derived from the GL_VERTEX_ATTRIB_ARRAY_BUFFER bindings.
    }
    
    if (access & avxPipelienAccess_INDEX_R)
    {
        glAccess |= GL_ELEMENT_ARRAY_BARRIER_BIT;
        // If set, vertex array indices sourced from buffer objects after the barrier will reflect data written by shaders prior to the barrier.
        // The buffer objects affected by this bit are derived from the GL_ELEMENT_ARRAY_BUFFER binding.
    }
    
    if (access & avxPipelienAccess_UNIFORM_R)
    {
        glAccess |= GL_UNIFORM_BARRIER_BIT;
        // Shader uniforms sourced from buffer objects after the barrier will reflect data written by shaders prior to the barrier.
    }
    
    if (access & avxPipelienAccess_SHADER_R)
    {
        glAccess |= GL_TEXTURE_FETCH_BARRIER_BIT;
        // Texture fetches from shaders, including fetches from buffer object memory via buffer textures, after the barrier will reflect data written by shaders prior to the barrier.
    }
    
    if (access & avxPipelienAccess_SHADER_W)
    {
        glAccess |= GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;
        // Memory accesses using shader image load, store, and atomic built - in functions issued after the barrier will reflect data written by shaders prior to the barrier.
        // Additionally, image stores and atomics issued after the barrier will not execute until all memory accesses(e.g., loads, stores, texture fetches, vertex fetches) initiated prior to the barrier complete.
    }
    
    if (access & avxPipelienAccess_INDIRECT_R)
    {
        glAccess |= GL_COMMAND_BARRIER_BIT;
        // Command data sourced from buffer objects by Draw*Indirect commands after the barrier will reflect data written by shaders prior to the barrier.
        // The buffer objects affected by this bit are derived from the GL_DRAW_INDIRECT_BUFFER binding.
    }
    
    if (access & (avxPipelienAccess_TRANSFER))
    {
        glAccess |= GL_PIXEL_BUFFER_BARRIER_BIT;
        // Reads and writes of buffer objects via the GL_PIXEL_PACK_BUFFER and GL_PIXEL_UNPACK_BUFFER bindings(via glReadPixels, glTexSubImage1D, etc.) after the barrier will reflect data written by shaders prior to the barrier.
        // Additionally, buffer object writes issued after the barrier will wait on the completion of all shader writes initiated prior to the barrier.
    }
    
    if (access & (avxPipelienAccess_TRANSFER))
    {
        glAccess |= GL_TEXTURE_UPDATE_BARRIER_BIT;
        // Writes to a texture via glTex(Sub)Image*, glCopyTex(Sub)Image*, glCompressedTex(Sub)Image*, and reads via glGetTexImage after the barrier will reflect data written by shaders prior to the barrier.
        // Additionally, texture writes from these commands issued after the barrier will not execute until all shader writes initiated prior to the barrier complete.
    }
    
    if (access & (avxPipelienAccess_TRANSFER))
    {
        glAccess |= GL_BUFFER_UPDATE_BARRIER_BIT;
        // Reads or writes via glBufferSubData, glCopyBufferSubData, or glGetBufferSubData, or to buffer object memory mapped by glMapBuffer or glMapBufferRange after the barrier will reflect data written by shaders prior to the barrier.
        // Additionally, writes via these commands issued after the barrier will wait on the completion of any shader writes to the same memory initiated prior to the barrier.
    }
    
    if (access & (avxPipelienAccess_TRANSFER))
    {
        glAccess |= GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT;
        // Access by the client to persistent mapped regions of buffer objects will reflect data written by shaders prior to the barrier.
        // Note that this may cause additional synchronization operations.
    }
    
    if (access & (avxPipelienAccess_COLOR_OUT | avxPipelienAccess_DEPTH_OUT))
    {
        glAccess |= GL_FRAMEBUFFER_BARRIER_BIT;
        // Reads and writes via framebuffer object attachments after the barrier will reflect data written by shaders prior to the barrier.
        // Additionally, framebuffer writes issued after the barrier will wait on the completion of all shader writes issued prior to the barrier.
    }
    
    if (access & avxPipelienAccess_SHADER_W)
    {
        glAccess |= GL_TRANSFORM_FEEDBACK_BARRIER_BIT;
        // Writes via transform feedback bindings after the barrier will reflect data written by shaders prior to the barrier.
        // Additionally, transform feedback writes issued after the barrier will wait on the completion of all shader writes issued prior to the barrier.
    }
    
    if (access & avxPipelienAccess_SHADER_W)
    {
        glAccess |= GL_ATOMIC_COUNTER_BARRIER_BIT;
        // Accesses to atomic counters after the barrier will reflect writes prior to the barrier.
    }

    if (access & avxPipelienAccess_SHADER_W)
    {
        glAccess |= GL_SHADER_STORAGE_BARRIER_BIT;
        // Accesses to shader storage blocks after the barrier will reflect writes prior to the barrier.
    }

    if (access & avxPipelienAccess_SHADER_W)
    {
        glAccess |= GL_QUERY_BUFFER_BARRIER_BIT;
        // Writes of buffer objects via the GL_QUERY_BUFFER binding after the barrier will reflect data written by shaders prior to the barrier.
        // Additionally, buffer object writes issued after the barrier will wait on the completion of all shader writes initiated prior to the barrier.
    }

    /*
        glMemoryBarrier defines a barrier ordering the memory transactions issued prior to the command relative to those issued after the barrier. 
        For the purposes of this ordering, memory transactions performed by shaders are considered to be issued by the rendering command that triggered the execution of the shader. 
        barriers is a bitfield indicating the set of operations that are synchronized with shader stores; the bits used in barriers are as follows:

        GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT
        If set, vertex data sourced from buffer objects after the barrier will reflect data written by shaders prior to the barrier. 
        The set of buffer objects affected by this bit is derived from the buffer object bindings used for generic vertex attributes derived from the GL_VERTEX_ATTRIB_ARRAY_BUFFER bindings.

        GL_ELEMENT_ARRAY_BARRIER_BIT
        If set, vertex array indices sourced from buffer objects after the barrier will reflect data written by shaders prior to the barrier. 
        The buffer objects affected by this bit are derived from the GL_ELEMENT_ARRAY_BUFFER binding.

        GL_UNIFORM_BARRIER_BIT
        Shader uniforms sourced from buffer objects after the barrier will reflect data written by shaders prior to the barrier.

        GL_TEXTURE_FETCH_BARRIER_BIT
        Texture fetches from shaders, including fetches from buffer object memory via buffer textures, after the barrier will reflect data written by shaders prior to the barrier.

        GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
        Memory accesses using shader image load, store, and atomic built-in functions issued after the barrier will reflect data written by shaders prior to the barrier. 
        Additionally, image stores and atomics issued after the barrier will not execute until all memory accesses (e.g., loads, stores, texture fetches, vertex fetches) initiated prior to the barrier complete.

        GL_COMMAND_BARRIER_BIT
        Command data sourced from buffer objects by Draw*Indirect commands after the barrier will reflect data written by shaders prior to the barrier. 
        The buffer objects affected by this bit are derived from the GL_DRAW_INDIRECT_BUFFER binding.

        GL_PIXEL_BUFFER_BARRIER_BIT
        Reads and writes of buffer objects via the GL_PIXEL_PACK_BUFFER and GL_PIXEL_UNPACK_BUFFER bindings (via glReadPixels, glTexSubImage1D, etc.) after the barrier will reflect data written by shaders prior to the barrier. 
        Additionally, buffer object writes issued after the barrier will wait on the completion of all shader writes initiated prior to the barrier.

        GL_TEXTURE_UPDATE_BARRIER_BIT
        Writes to a texture via glTex(Sub)Image*, glCopyTex(Sub)Image*, glCompressedTex(Sub)Image*, and reads via glGetTexImage after the barrier will reflect data written by shaders prior to the barrier. 
        Additionally, texture writes from these commands issued after the barrier will not execute until all shader writes initiated prior to the barrier complete.

        GL_BUFFER_UPDATE_BARRIER_BIT
        Reads or writes via glBufferSubData, glCopyBufferSubData, or glGetBufferSubData, or to buffer object memory mapped by glMapBuffer or glMapBufferRange after the barrier will reflect data written by shaders prior to the barrier. 
        Additionally, writes via these commands issued after the barrier will wait on the completion of any shader writes to the same memory initiated prior to the barrier.

        GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT
        Access by the client to persistent mapped regions of buffer objects will reflect data written by shaders prior to the barrier. 
        Note that this may cause additional synchronization operations.

        GL_FRAMEBUFFER_BARRIER_BIT
        Reads and writes via framebuffer object attachments after the barrier will reflect data written by shaders prior to the barrier. 
        Additionally, framebuffer writes issued after the barrier will wait on the completion of all shader writes issued prior to the barrier.

        GL_TRANSFORM_FEEDBACK_BARRIER_BIT
        Writes via transform feedback bindings after the barrier will reflect data written by shaders prior to the barrier. 
        Additionally, transform feedback writes issued after the barrier will wait on the completion of all shader writes issued prior to the barrier.

        GL_ATOMIC_COUNTER_BARRIER_BIT
        Accesses to atomic counters after the barrier will reflect writes prior to the barrier.

        GL_SHADER_STORAGE_BARRIER_BIT
        Accesses to shader storage blocks after the barrier will reflect writes prior to the barrier.

        GL_QUERY_BUFFER_BARRIER_BIT
        Writes of buffer objects via the GL_QUERY_BUFFER binding after the barrier will reflect data written by shaders prior to the barrier. 
        Additionally, buffer object writes issued after the barrier will wait on the completion of all shader writes initiated prior to the barrier.

        If barriers is GL_ALL_BARRIER_BITS, shader memory accesses will be synchronized relative to all the operations described above.

        // -----
        glMemoryBarrierByRegion behaves as described above for glMemoryBarrier, with two differences:

        First, it narrows the region under consideration so that only reads and writes of prior fragment shaders that are invoked for a smaller region of the framebuffer will be completed/reflected prior to subsequent reads and writes of following fragment shaders. 
        The size of the region is implementation-dependent and may be as small as one framebuffer pixel.

        Second, it only applies to memory transactions that may be read by or written by a fragment shader. 
        Therefore, only the barrier bits

        GL_ATOMIC_COUNTER_BARRIER_BIT

        GL_FRAMEBUFFER_BARRIER_BIT

        GL_SHADER_IMAGE_ACCESS_BARRIER_BIT

        GL_SHADER_STORAGE_BARRIER_BIT

        GL_TEXTURE_FETCH_BARRIER_BIT

        GL_UNIFORM_BARRIER_BIT

        are supported.

        When barriers is GL_ALL_BARRIER_BITS, shader memory accesses will be synchronized relative to all these barrier bits, 
        but not to other barrier bits specific to glMemoryBarrier. This implies that reads and writes for scatter/gather-like algorithms may or may not be completed/reflected after a glMemoryBarrierByRegion command. 
        However, for uses such as deferred shading, where a linked list of visible surfaces with the head at a framebuffer address may be constructed, and the entirety of the list is only dependent on previous executions at that framebuffer address, glMemoryBarrierByRegion may be significantly more efficient than glMemoryBarrier.
    */

    GLbitfield flags = NIL;
#if 0
    if (dst & avxPipelineStage_VERTEX)
        flags |= GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT;
    else if (dst & avxPipelineStage_COPY)
        flags |= ;
    else if (dst & avxPipelineStage_COPY)
        flags |= ;
    else if (dst & avxPipelineStage_COPY)
        flags |= ;
    else if (dst & avxPipelineStage_COPY)
        flags |= ;
    else if (dst & avxPipelineStage_COPY)
        flags |= ;
#endif
    gl->MemoryBarrier(GL_ALL_BARRIER_BITS);
}

// RESOURCE BINDING

_ZGL void DpuBindBuffers(zglDpu* dpu, afxUnit set, afxUnit first, afxUnit cnt, avxBufferedMap const maps[])
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_RANGE(_ZGL_MAX_ENTRY_PER_LEGO, first, cnt);
    AFX_ASSERT_RANGE(_ZGL_MAX_LEGO_PER_BIND, set, 1);

    // deferred because it requires the pipeline ligature info.

    for (afxUnit i = 0; i < cnt; i++)
    {
        avxBufferedMap const* map = &maps[i];

        afxUnit entryIdx = first + i;
        avxBuffer buf = map->buf;
        afxUnit32 offset = map->offset;
        afxUnit32 range = map->range;

        dpu->nextLs[set][entryIdx].buf = buf;
        dpu->nextLs[set][entryIdx].offset = offset;
        dpu->nextLs[set][entryIdx].range = range;
        dpu->nextLsUpdMask[set] |= AFX_BIT(entryIdx);
    }
}

_ZGL void DpuBindRasters(zglDpu* dpu, afxUnit set, afxUnit first, afxUnit cnt, avxRaster const rasters[])
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_RANGE(_ZGL_MAX_ENTRY_PER_LEGO, first, cnt);
    AFX_ASSERT_RANGE(_ZGL_MAX_LEGO_PER_BIND, set, 1);

    // deferred because it requires the pipeline ligature info.

    for (afxUnit i = 0; i < cnt; i++)
    {
        afxUnit entryIdx = first + i;
        avxRaster ras = rasters[i];
        dpu->nextLs[set][entryIdx].ras = ras;
        dpu->nextLsUpdMask[set] |= AFX_BIT(entryIdx);
    }
}

_ZGL void DpuBindSamplers(zglDpu* dpu, afxUnit set, afxUnit first, afxUnit cnt, avxSampler const samplers[])
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_RANGE(_ZGL_MAX_ENTRY_PER_LEGO, first, cnt);
    AFX_ASSERT_RANGE(_ZGL_MAX_LEGO_PER_BIND, set, 1);

    // deferred because it requires the pipeline ligature info.

    for (afxUnit i = 0; i < cnt; i++)
    {
        afxUnit entryIdx = first + i;
        avxSampler smp = samplers[i];
        dpu->nextLs[set][entryIdx].smp = smp;
        dpu->nextLsUpdMask[set] |= AFX_BIT(entryIdx);
    }
}

_ZGL void DpuPushConstants(zglDpu* dpu, afxUnit32 offset, afxUnit32 siz, void const* data)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

    // copy right into mapped memory.
    AfxCopy(&dpu->pushConstMappedMem[offset], data, siz);
    dpu->shouldPushConsts = TRUE;
    dpu->shouldPushConstRange = (dpu->shouldPushConstBase + dpu->shouldPushConstRange) > (offset + siz) ? dpu->shouldPushConstRange : siz;
    dpu->shouldPushConstBase = AFX_MIN(dpu->shouldPushConstBase, offset);
}

_ZGL void _ZglFlushLsChanges(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    avxLigature liga;

    if (!(liga = dpu->activeLiga))
        return;

    AFX_ASSERT_OBJECTS(afxFcc_LIGA, 1, &liga);

#ifdef COHERENT_PUSHABLES // TODO: Test it to avoid COHERENT mapping.

#else
    if (liga->m.pushables
#ifndef FORCE_ARG_REBIND
        && dpu->shouldPushConsts
#endif
        )
    {
        if (gl->FlushMappedNamedBufferRange)
        {
            gl->FlushMappedNamedBufferRange(dpu->pushConstUbo, dpu->shouldPushConstBase, dpu->shouldPushConstRange); _ZglThrowErrorOccuried();
        }
        else
        {
            gl->BindBuffer(GL_COPY_WRITE_BUFFER, dpu->pushConstUbo); _ZglThrowErrorOccuried();
            gl->FlushMappedBufferRange(GL_COPY_WRITE_BUFFER, dpu->shouldPushConstBase, dpu->shouldPushConstRange); _ZglThrowErrorOccuried();
        }
        dpu->shouldPushConstBase = 0;
        dpu->shouldPushConstRange = 0;
        dpu->shouldPushConsts = FALSE;
    }
#endif

    // BIND RESOURCES (TEXTURES, SAMPLERS AND BUFFERS)

    for (afxUnit i = 0; i < liga->m.setCnt; i++)
    {
        afxUnit set = liga->m.sets[i].set;
        afxMask updMask = dpu->nextLsUpdMask[set];

#ifndef FORCE_ARG_REBIND
        if (!updMask) // skip if has not updates
            continue;
#endif

        dpu->nextLsUpdMask[set] = NIL;

        for (afxUnit j = 0; j < liga->m.sets[i].entryCnt; j++)
        {
            afxUnit resIdx = liga->m.sets[i].baseEntry + j;
            avxLigatureEntry const *entry = &liga->m.totalEntries[resIdx];

#ifndef FORCE_ARG_REBIND
            if (!(updMask & AFX_BIT(entry->binding))) // skip if not changed
                continue;
#endif

            AFX_ASSERT(entry->type);
            afxUnit binding = entry->binding;
            afxUnit glUnit = (set * _ZGL_MAX_ENTRY_PER_LEGO) + binding;
            afxBool reqUpd = FALSE, reqUpd2 = FALSE;
            GLuint glHandle = 0, glHandle2 = 0;
            afxSize bufSiz = 0;

            switch (entry->type)
            {
            case avxShaderParam_UNIFORM:
            case avxShaderParam_BUFFER:
            {
                afxUnit offset = dpu->nextLs[set][binding].offset;
                afxUnit range = dpu->nextLs[set][binding].range;
                avxBuffer buf = dpu->nextLs[set][binding].buf;
                afxBool rebind = FALSE;
                GLenum glTarget = GL_INVALID_ENUM;

                if (entry->type == avxShaderParam_UNIFORM)
                    glTarget = GL_UNIFORM_BUFFER;
                else if (entry->type == avxShaderParam_BUFFER)
                    glTarget = GL_SHADER_STORAGE_BUFFER;
                else AfxThrowError();

                afxUnit bufUniqId = buf ? buf->bufUniqueId : dpu->activeLs[set][binding].bufUniqId;

#ifndef FORCE_ARG_REBIND
                if ((dpu->activeLs[set][binding].buf != buf) ||
                    (dpu->activeLs[set][binding].offset != offset) ||
                    (dpu->activeLs[set][binding].range != range)
#if !0
                    || (dpu->activeLs[set][binding].bufUniqId != bufUniqId)
#endif
                    )
#endif
                {
                    dpu->activeLs[set][binding].buf = buf;
                    dpu->activeLs[set][binding].offset = offset;
                    dpu->activeLs[set][binding].range = range;

                    dpu->activeLs[set][binding].bufUniqId = bufUniqId;
                    //dpu->nextLs[set][binding].buf = NIL; // force update in "next first time".
                    rebind = TRUE;
                }
                
#if !0
                if (rebind)
#endif
                {
                    if (!buf)
                    {
                        gl->BindBufferBase(glTarget, glUnit, 0); _ZglThrowErrorOccuried();
                    }
                    else
                    {
                        DpuBindAndSyncBuf(dpu, glTarget, buf, FALSE);
                        bufSiz = AvxGetBufferCapacity(buf, 0);

                        if (offset || range)
                        {
                            AFX_ASSERT(range);
                            AFX_ASSERT_RANGE(bufSiz, offset, range);
                            gl->BindBufferRange(glTarget, glUnit, buf->glHandle, offset, AFX_ALIGNED_SIZE(range, AFX_SIMD_ALIGNMENT)); _ZglThrowErrorOccuried();
                        }
                        else
                        {
                            gl->BindBufferBase(glTarget, glUnit, buf->glHandle); _ZglThrowErrorOccuried();
                        }
                    }
                }
                break;
            }
            case avxShaderParam_TEXTURE:
            case avxShaderParam_RASTER:
            case avxShaderParam_SAMPLER:
            {
                avxSampler samp = dpu->nextLs[set][binding].smp;
                avxRaster ras = dpu->nextLs[set][binding].ras;
                afxBool rebindRas = FALSE;
                afxBool rebindSamp = FALSE;

                afxUnit rasUniqId = ras ? ras->rasUniqueId : dpu->activeLs[set][binding].rasUniqId;
                afxUnit sampUniqId = samp ? samp->smpUniqueId : dpu->activeLs[set][binding].smpUniqId;

#ifndef FORCE_ARG_REBIND
                if ((dpu->activeLs[set][binding].ras != ras)

#if !0
                    || (dpu->activeLs[set][binding].rasUniqId != rasUniqId)
#endif
                    )
#endif
                {
                    dpu->activeLs[set][binding].rasUniqId = rasUniqId;
                    dpu->activeLs[set][binding].ras = ras;
                    //dpu->nextLs[set][binding].ras = NIL; // force update in "next first time".
                    rebindRas = TRUE;
                }

#ifndef FORCE_ARG_REBIND
                if ((dpu->activeLs[set][binding].smp != samp)
#if !0
                    || (dpu->activeLs[set][binding].smpUniqId != sampUniqId)
#endif
                    )
#endif
                {
                    dpu->activeLs[set][binding].smpUniqId = sampUniqId;
                    dpu->activeLs[set][binding].smp = samp;
                    //dpu->nextLs[set][binding].smp = NIL; // force update in "next first time".
                    rebindSamp = TRUE;
                }

#if !0
                if (rebindRas)
#endif
                {
                    if (entry->type == avxShaderParam_RASTER || entry->type == avxShaderParam_TEXTURE)
                    {
                        if (!ras)
                        {
                            if (gl->BindTextureUnit)
                            {
                                gl->BindTextureUnit(glUnit, 0); _ZglThrowErrorOccuried();
                            }
                            else
                            {
                                gl->ActiveTexture(GL_TEXTURE0 + glUnit); _ZglThrowErrorOccuried();
                                gl->BindTexture(GL_TEXTURE_2D, 0); _ZglThrowErrorOccuried();
                            }
                        }
                        else
                        {
                            if (gl->BindTextureUnit)
                            {
                                DpuBindAndSyncRas(dpu, glUnit, ras, FALSE);
                                gl->BindTextureUnit(glUnit, ras->glHandle); _ZglThrowErrorOccuried();
                            }
                            else
                            {
                                DpuBindAndSyncRas(dpu, glUnit, ras, TRUE);
                                gl->ActiveTexture(GL_TEXTURE0 + glUnit); _ZglThrowErrorOccuried();
                                gl->BindTexture(ras->glTarget, ras->glHandle); _ZglThrowErrorOccuried();
                            }
                        }
                    }
                }

#if !0
                if (rebindSamp)
#endif
                {
                    if (entry->type == avxShaderParam_SAMPLER || entry->type == avxShaderParam_TEXTURE)
                    {
                        if (!samp)
                        {
                            gl->BindSampler(glUnit, 0); _ZglThrowErrorOccuried();
                        }
                        else
                        {
                            _DpuBindAndSyncSamp(dpu, glUnit, samp);
                            gl->BindSampler(glUnit, samp->glHandle); _ZglThrowErrorOccuried();
                        }
                    }
                }
                break;
            }
            case avxShaderParam_IMAGE:
            {
                avxRaster ras = dpu->nextLs[set][binding].ras;
                afxBool rebindRas = FALSE;
                afxBool rebindSamp = FALSE;

                afxUnit rasUniqId = ras ? ras->rasUniqueId : dpu->activeLs[set][binding].rasUniqId;

#ifndef FORCE_ARG_REBIND
                if ((dpu->activeLs[set][binding].ras != ras)
#if !0
                    || (dpu->activeLs[set][binding].rasUniqId != rasUniqId)
#endif
                    )
#endif
                {
                    dpu->activeLs[set][binding].rasUniqId = rasUniqId;
                    dpu->activeLs[set][binding].ras = ras;
                    //dpu->nextLs[set][binding].ras = NIL; // force update in "next first time".
                    rebindRas = TRUE;
                }

#if !0
                if (rebindRas)
#endif
                {
                    AFX_ASSERT(gl->BindImageTexture);

                    if (!ras)
                    {
                        gl->BindImageTexture(glUnit, NIL, 0, FALSE, 0, GL_READ_ONLY, GL_R32F); _ZglThrowErrorOccuried();
                    }
                    else
                    {
                        gl->BindImageTexture(glUnit, ras->glHandle, ras->m.baseLod, (ras->m.extent.d > 1), ras->m.baseLayer, GL_READ_WRITE, ras->glIntFmt); _ZglThrowErrorOccuried();
                    }
                }
                break;
            }
            case avxShaderParam_FETCH:
            case avxShaderParam_TSBO:
            {
                afxUnit offset = dpu->nextLs[set][binding].offset;
                afxUnit range = dpu->nextLs[set][binding].range;
                avxBuffer buf = dpu->nextLs[set][binding].buf;
                afxBool rebind = FALSE;
                GLenum glTarget = GL_TEXTURE_BUFFER;

                afxUnit bufUniqId = buf ? buf->bufUniqueId : dpu->activeLs[set][binding].bufUniqId;

#ifndef FORCE_ARG_REBIND
                if ((dpu->activeLs[set][binding].buf != buf) ||
                    (dpu->activeLs[set][binding].offset != offset) ||
                    (dpu->activeLs[set][binding].range != range)
#if !0
                    || (dpu->activeLs[set][binding].bufUniqId != bufUniqId)
#endif
                    )
#endif
                {
                    dpu->activeLs[set][binding].buf = buf;
                    dpu->activeLs[set][binding].offset = offset;
                    dpu->activeLs[set][binding].range = range;

                    dpu->activeLs[set][binding].bufUniqId = bufUniqId;
                    //dpu->nextLs[set][binding].buf = NIL; // force update in "next first time".
                    rebind = TRUE;
                }

#if !0
                if (rebind)
#endif
                {
                    if (!buf)
                    {
                        if (entry->type == avxShaderParam_TSBO)
                        {
                            AFX_ASSERT(gl->BindImageTexture);
                            gl->BindImageTexture(glUnit, NIL, 0, FALSE, 0, GL_READ_WRITE, GL_NONE); _ZglThrowErrorOccuried();
                        }
                        else
                        {
                            if (gl->BindTextureUnit)
                            {
                                gl->BindTextureUnit(glUnit, 0); _ZglThrowErrorOccuried();
                            }
                            else
                            {
                                gl->ActiveTexture(GL_TEXTURE0 + glUnit); _ZglThrowErrorOccuried();
                                gl->BindTexture(glTarget, 0); _ZglThrowErrorOccuried();
                            }
                        }
                    }
                    else
                    {
                        if (entry->type == avxShaderParam_TSBO)
                        {
                            DpuBindAndSyncBuf(dpu, glTarget, buf, FALSE);
                            AFX_ASSERT(gl->BindImageTexture);
                            gl->BindImageTexture(glUnit, buf->glTexHandle, 0, FALSE, 0, GL_READ_WRITE, buf->glTexIntFmt);
                        }
                        else
                        {
                            if (gl->BindTextureUnit)
                            {
                                DpuBindAndSyncBuf(dpu, glTarget, buf, FALSE);
                                gl->BindTextureUnit(glUnit, buf->glTexHandle); _ZglThrowErrorOccuried();
                            }
                            else
                            {
                                DpuBindAndSyncBuf(dpu, glTarget, buf, TRUE);
                                gl->ActiveTexture(GL_TEXTURE0 + glUnit); _ZglThrowErrorOccuried();
                                gl->BindTexture(glTarget, buf->glTexHandle); _ZglThrowErrorOccuried();
                            }
                        }
                    }
                }
                break;
            }
            default:
            {
                AfxReportError("");
            }
            }
        }
    }
}

_ZGL void _ZglFlushVsChanges(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    avxVertexInput vin;

    if (!(vin = dpu->activeVin))
        return;

    AFX_ASSERT_OBJECTS(afxFcc_VIN, 1, &vin);

    // BIND RESOURCES (VERTEX AND INDEX BUFFERS)

    zglVertexInputState* activeVinBindings = &vin->bindings;
    zglVertexInputState* nextVinBindings = &dpu->nextVinBindings;
    afxMask sourcesUpdMask = nextVinBindings->sourcesUpdMask;

#ifndef FORCE_ARG_REBIND
    if (sourcesUpdMask)
#endif
    {
        nextVinBindings->sourcesUpdMask = NIL;

        for (afxUnit i = 0; i < vin->m.srcCnt; i++)
        {
            avxVertexFetch const* stream = &vin->m.srcs[i];
            afxUnit streamIdx = stream->srcIdx;
            AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, streamIdx, 1);

#ifndef FORCE_ARG_REBIND
            if (!(sourcesUpdMask & AFX_BIT(streamIdx))) // skip if has not updates
                continue;
#endif

            avxBuffer nextBuf = nextVinBindings->sources[streamIdx].buf;
            afxUnit nextOff = nextVinBindings->sources[streamIdx].offset;
            afxUnit nextRange = nextVinBindings->sources[streamIdx].range;
            afxUnit nextStride = nextVinBindings->sources[streamIdx].stride;
            afxBool forceRebind = TRUE;

            afxUnit bufUniqueId = nextBuf ? nextBuf->bufUniqueId : activeVinBindings->sources[streamIdx].bufUniqueId;

#ifndef FORCE_ARG_REBIND
            if ((forceRebind) ||
                (activeVinBindings->sources[streamIdx].bufUniqueId != bufUniqueId) || // if it has not a GL handle yet or it is different.
                (activeVinBindings->sources[streamIdx].buf != nextBuf) ||
                (activeVinBindings->sources[streamIdx].offset != nextOff) ||
                (activeVinBindings->sources[streamIdx].range != nextRange) ||
                (activeVinBindings->sources[streamIdx].stride != nextStride))
#endif
            {
                if (!nextBuf)
                {
                    AFX_ASSERT(gl->BindVertexBuffer);
                    gl->BindVertexBuffer(streamIdx, 0, 0, 16); _ZglThrowErrorOccuried();

                    activeVinBindings->sources[streamIdx].buf = 0;
                    activeVinBindings->sources[streamIdx].offset = 0;
                    activeVinBindings->sources[streamIdx].range = 0;
                    activeVinBindings->sources[streamIdx].stride = 0;
                }
                else
                {
                    activeVinBindings->sources[streamIdx].buf = nextBuf;
                    activeVinBindings->sources[streamIdx].offset = nextOff;
                    activeVinBindings->sources[streamIdx].range = nextRange;
                    activeVinBindings->sources[streamIdx].stride = nextStride;

                    AFX_ASSERT(nextRange);
                    AFX_ASSERT(nextStride);

                    if (!nextBuf->glHandle)
                        DpuBindAndSyncBuf(dpu, GL_ARRAY_BUFFER, nextBuf, FALSE);

                    AFX_ASSERT(gl->BindVertexBuffer);
                    gl->BindVertexBuffer(streamIdx, nextBuf->glHandle, nextOff, nextStride); _ZglThrowErrorOccuried();
                }

                activeVinBindings->sources[streamIdx].bufUniqueId = bufUniqueId;

                //nextVinBindings->sources[streamIdx].buf = NIL; // force update in "next first time".
            }
        }
    }

#ifndef FORCE_ARG_REBIND
    if (nextVinBindings->iboUpdReq)
#endif
    {
        nextVinBindings->iboUpdReq = FALSE;

        avxBuffer nextBuf = nextVinBindings->idxSrcBuf;
        afxUnit nextOff = nextVinBindings->idxSrcOff;
        afxUnit nextRange = nextVinBindings->idxSrcRange;
        afxUnit nextStride = nextVinBindings->idxSrcSiz;
        afxBool forceRebind = FALSE;
        afxBool bound = FALSE;

        afxUnit bufUniqueId = nextBuf ? nextBuf->bufUniqueId : activeVinBindings->iboUniqueId;

#ifndef FORCE_ARG_REBIND
        if ((forceRebind) ||
            (activeVinBindings->iboUniqueId != bufUniqueId) || // if it has not a GL handle yet or it is different.
            (activeVinBindings->idxSrcBuf != nextBuf) ||
            (activeVinBindings->idxSrcOff != nextOff) ||
            (activeVinBindings->idxSrcRange != nextRange) ||
            (activeVinBindings->idxSrcSiz != nextStride))
#endif
        {
            DpuBindAndSyncBuf(dpu, GL_ELEMENT_ARRAY_BUFFER, nextBuf, TRUE);

            if (!nextBuf)
            {
                //gl->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

                activeVinBindings->idxSrcBuf = NIL;
                activeVinBindings->idxSrcOff = 0;
                activeVinBindings->idxSrcRange = 0;
                activeVinBindings->idxSrcSiz = 0;
            }
            else
            {
                //DpuBindAndSyncBuf(dpu, GL_ELEMENT_ARRAY_BUFFER, nextBuf);
                //gl->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->glHandle);

                activeVinBindings->idxSrcBuf = nextBuf;
                activeVinBindings->idxSrcOff = nextOff;
                activeVinBindings->idxSrcRange = nextRange;
                activeVinBindings->idxSrcSiz = nextStride;
            }

            activeVinBindings->iboUniqueId = bufUniqueId;
            //nextVinBindings->idxSrcBuf = NIL; // force update in "next first time".
        }
    }
}

_ZGL void DpuDraw(zglDpu* dpu, avxDrawIndirect const* data)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    afxUnit cnt;

    //_DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    //dpu->activePip = dpu->nextPip;

    if (!dpu->activePip)
    {
        AfxThrowError();
        return;
    }

    _ZglFlushTsChanges(dpu);
    _ZglFlushRsChanges(dpu);
    _ZglFlushLsChanges(dpu);
    _ZglFlushVsChanges(dpu);

    //if (dpu->nextVtxInStreamUpdMask || dpu->nextVtxInAttribUpdMask || dpu->flushIbb)
        //_ZglFlushVertexInputStateChanges(dpu, FALSE);

    //AFX_ASSERT(!dpu->nextVinBindingsMask);
    //AFX_ASSERT(!dpu->nextVtxInAttribUpdMask);
    //AFX_ASSERT(!dpu->flushIbb);

    AFX_ASSERT(dpu->activePip);
    afxUnit32 vtxCnt = data->vtxCnt;
    AFX_ASSERT(vtxCnt);
    //AFX_ASSERT(cmd->instCnt);
    GLenum top = AfxToGlTopology(dpu->primTop);

    afxUnit firstInst = data->baseInst;
    afxUnit firstVtx = data->baseVtx;
    afxUnit instCnt = data->instCnt;

#if 0//FORCE_GL_GENERIC_FUNCS
    AFX_ASSERT(gl->DrawArraysInstancedBaseInstance);
    gl->DrawArraysInstancedBaseInstance(top, firstVtx, vtxCnt, instCnt, firstInst); _ZglThrowErrorOccuried();
#else
    if (instCnt)
    {
        if (firstInst)
        {
            AFX_ASSERT(gl->DrawArraysInstancedBaseInstance);
            gl->DrawArraysInstancedBaseInstance(top, firstVtx, vtxCnt, instCnt, firstInst); _ZglThrowErrorOccuried();
        }
        else
        {
            gl->DrawArraysInstanced(top, firstVtx, vtxCnt, instCnt); _ZglThrowErrorOccuried();
        }
    }
    else
    {
        gl->DrawArrays(top, firstVtx, vtxCnt); _ZglThrowErrorOccuried();
    }
#endif
    dpu->m.numOfFedVertices += vtxCnt;
    dpu->m.numOfFedInstances += instCnt;

    //AfxReportMessage("Geometry drawn. vtxCnt %u, instCnt %u, firstVtx %u, firstInst %u", vtxCnt, instCnt, firstVtx, firstInst);
}

_ZGL void DpuDrawIndexed(zglDpu* dpu, avxDrawIndexedIndirect const* data)
{
    /*
        When the command is executed, primitives are assembled using the current primitive topology and indexCount vertices whose indices are retrieved from the index buffer.
        The index buffer is treated as an array of tightly packed unsigned integers of size defined by the vkCmdBindIndexStream::indexType parameter with which the buffer was bound.

        The first vertex index is at an offset of firstIndex × indexSize + offset within the bound index buffer, where offset is the offset specified by CmdBindIndexStream and indexSize is the byte size of the type specified by indexType.
        Subsequent index values are retrieved from consecutive locations in the index buffer.
        Indices are first compared to the primitive restart value, then zero extended to 32 bits (if the indexType is VK_INDEX_TYPE_UINT8_EXT or VK_INDEX_TYPE_UINT16) and have vertexOffset added to them, before being supplied as the vertexIndex value.

        The primitives are drawn instanceCount times with instanceIndex starting with firstInstance and increasing sequentially for each instance.
        The assembled primitives execute the bound graphics pipeline.
    */

    afxError err = AFX_ERR_NONE;

    //_DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    //dpu->activePip = dpu->nextPip;

    if (!dpu->activePip)
    {
        AfxThrowError();
        return;
    }

    _ZglFlushTsChanges(dpu);
    _ZglFlushRsChanges(dpu);
    _ZglFlushLsChanges(dpu);
    _ZglFlushVsChanges(dpu);

    //if (dpu->nextVtxInStreamUpdMask || dpu->nextVtxInAttribUpdMask || dpu->flushIbb)
        //_ZglFlushVertexInputStateChanges(dpu, TRUE);

    //AFX_ASSERT(!dpu->nextVinBindingsMask);
    //AFX_ASSERT(!dpu->nextVtxInAttribUpdMask);
    //AFX_ASSERT(!dpu->flushIbb);

    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &dpu->activeVin->bindings.idxSrcBuf);
    //AFX_ASSERT(dpu->state.vertexBindingCnt);

    afxUnit idxCnt = data->idxCnt;
    AFX_ASSERT(idxCnt);
    //AFX_ASSERT(cmd->instCnt);

    static const GLenum idxSizGl[] =
    {
        0,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_SHORT,
        0,
        GL_UNSIGNED_INT
    };

    afxSize idxSiz = dpu->activeVin->bindings.idxSrcSiz;
    afxSize idxBaseOff = dpu->activeVin->bindings.idxSrcOff;

    GLint vtxOff2 = data->vtxOffset;
    afxUnit32 firstIdx = data->baseIdx;
    afxUnit32 instCnt = data->instCnt;
    afxUnit32 firstInst = data->baseInst;
    afxSize dataOff = (idxBaseOff + (idxSiz * firstIdx));

    GLenum top = AfxToGlTopology(dpu->primTop);

    glVmt const* gl = dpu->gl;

#if 0//FORCE_GL_GENERIC_FUNCS
    AFX_ASSERT(gl->DrawElementsInstancedBaseVertexBaseInstance);
    gl->DrawElementsInstancedBaseVertexBaseInstance(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, instCnt, vtxOff2, firstInst); _ZglThrowErrorOccuried();
#else
    if (instCnt)
    {
        if (firstInst)
        {
            AFX_ASSERT(gl->DrawElementsInstancedBaseVertexBaseInstance);
            gl->DrawElementsInstancedBaseVertexBaseInstance(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, instCnt, vtxOff2, firstInst); _ZglThrowErrorOccuried();
        }
        else
        {
            if (vtxOff2)
            {
                gl->DrawElementsInstancedBaseVertex(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, instCnt, vtxOff2); _ZglThrowErrorOccuried();
            }
            else
            {
                gl->DrawElementsInstanced(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, instCnt); _ZglThrowErrorOccuried();
            }
        }
    }
    else
    {
        if (vtxOff2)
        {
            gl->DrawElementsBaseVertex(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, vtxOff2); _ZglThrowErrorOccuried();
        }
        else
        {
            gl->DrawElements(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff); _ZglThrowErrorOccuried();
        }
    }
#endif

    //dpu->numOfFedVertices += cmd->idxCnt;
    dpu->m.numOfFedIndices += idxCnt;
    dpu->m.numOfFedInstances += instCnt;

    //AfxReportMessage("Indexed geometry drawn. idxCnt %u, instCnt %u, baseIdx %u, vtxOff %u, baseInst %u", idxCnt, instCnt, firstIdx, vtxOff, firstInst);
}

_ZGL void DpuDrawIndirect(zglDpu* dpu, avxBuffer buf, afxUnit32 offset, afxUnit32 drawCnt, afxUnit32 stride)
{
    afxError err = AFX_ERR_NONE;

    //_DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    //dpu->activePip = dpu->nextPip;

    if (!dpu->activePip)
    {
        AfxThrowError();
        return;
    }

    _ZglFlushTsChanges(dpu);
    _ZglFlushRsChanges(dpu);
    _ZglFlushLsChanges(dpu);
    _ZglFlushVsChanges(dpu);

    glVmt const* gl = dpu->gl;

    GLenum top = AfxToGlTopology(dpu->primTop);
    void* offPtr = (void*)offset;

    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);
    DpuBindAndSyncBuf(dpu, GL_DRAW_INDIRECT_BUFFER, buf, TRUE);

#if 0//FORCE_GL_GENERIC_FUNCS
    AFX_ASSERT(gl->MultiDrawArraysIndirect);
    gl->MultiDrawArraysIndirect(top, offPtr, drawCnt, stride); _ZglThrowErrorOccuried();
#else
    if (1 >= drawCnt)
    {
        gl->DrawArraysIndirect(top, offPtr); _ZglThrowErrorOccuried();
    }
    else
    {
        AFX_ASSERT(gl->MultiDrawArraysIndirect);
        gl->MultiDrawArraysIndirect(top, offPtr, drawCnt, stride); _ZglThrowErrorOccuried();
    }
#endif
}

_ZGL void DpuDrawIndirectCount(zglDpu* dpu, avxBuffer buf, afxUnit32 offset, avxBuffer cntBuf, afxUnit32 cntBufOff, afxUnit32 maxDrawCnt, afxUnit32 stride)
{
    afxError err = AFX_ERR_NONE;

    //_DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    //dpu->activePip = dpu->nextPip;


    if (!dpu->activePip)
    {
        AfxThrowError();
        return;
    }

    _ZglFlushTsChanges(dpu);
    _ZglFlushRsChanges(dpu);
    _ZglFlushLsChanges(dpu);
    _ZglFlushVsChanges(dpu);

    glVmt const* gl = dpu->gl;

    GLenum top = AfxToGlTopology(dpu->primTop);
    void* offPtr = (void*)offset;

    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);
    DpuBindAndSyncBuf(dpu, GL_DRAW_INDIRECT_BUFFER, buf, TRUE);
    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &cntBuf);
    DpuBindAndSyncBuf(dpu, GL_PARAMETER_BUFFER, cntBuf, TRUE);

    /*
    The command void MultiDrawArraysIndirectCount( enum mode, const void *indirect, intptr drawcount, intptr maxdrawcount, sizei stride );
    behaves similarly to MultiDrawArraysIndirect, except that drawcount defines an offset (in bytes) into the buffer object bound to the PARAMETER_BUFFER
    binding point at which a single sizei typed value is stored, which contains the draw count.
    maxdrawcount specifies the maximum number of draws that are expected to be stored in the buffer. If the value stored at drawcount into the buffer is greater
    than maxdrawcount, the implementation stops processing draws after maxdrawcount parameter sets. drawcount must be a multiple of four.

    Errors
    In addition to errors that would be generated by MultiDrawArraysIndirect:
    An INVALID_OPERATION error is generated if no buffer is bound to the PARAMETER_BUFFER binding point.
    An INVALID_VALUE error is generated if drawcount is not a multiple of four.
    An INVALID_OPERATION error is generated if reading a sizei typed value from the buffer bound to the PARAMETER_BUFFER target at the offset specified by drawcount would result in an out-of-bounds access.
    */

    AFX_ASSERT(gl->MultiDrawArraysIndirectCount);
    gl->MultiDrawArraysIndirectCount(top, offPtr, cntBufOff, maxDrawCnt, stride); _ZglThrowErrorOccuried();
}

_ZGL void DpuDrawIndexedIndirect(zglDpu* dpu, avxBuffer buf, afxUnit32 offset, afxUnit32 drawCnt, afxUnit32 stride)
{
    afxError err = AFX_ERR_NONE;

    //_DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    //dpu->activePip = dpu->nextPip;

    if (!dpu->activePip)
    {
        AfxThrowError();
        return;
    }

    _ZglFlushTsChanges(dpu);
    _ZglFlushRsChanges(dpu);
    _ZglFlushLsChanges(dpu);
    _ZglFlushVsChanges(dpu);

    glVmt const* gl = dpu->gl;

    static const GLenum idxSizGl[] =
    {
        0,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_SHORT,
        0,
        GL_UNSIGNED_INT
    };
    afxSize idxSiz = dpu->activeVin->bindings.idxSrcSiz;
    GLenum top = AfxToGlTopology(dpu->primTop);
    void* offPtr = (void*)offset;

    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);
    DpuBindAndSyncBuf(dpu, GL_DRAW_INDIRECT_BUFFER, buf, TRUE);

#if 0//FORCE_GL_GENERIC_FUNCS
    AFX_ASSERT(gl->MultiDrawElementsIndirect);
    gl->MultiDrawElementsIndirect(top, idxSizGl[idxSiz], offPtr, drawCnt, stride); _ZglThrowErrorOccuried();
#else
    if (1 >= drawCnt)
    {
        gl->DrawElementsIndirect(top, idxSizGl[idxSiz], offPtr); _ZglThrowErrorOccuried();
    }
    else
    {
        AFX_ASSERT(gl->MultiDrawElementsIndirect);
        gl->MultiDrawElementsIndirect(top, idxSizGl[idxSiz], offPtr, drawCnt, stride); _ZglThrowErrorOccuried();
    }
#endif
}

_ZGL void DpuDrawIndexedIndirectCount(zglDpu* dpu, avxBuffer buf, afxUnit32 offset, avxBuffer cntBuf, afxUnit32 cntBufOff, afxUnit32 maxDrawCnt, afxUnit32 stride)
{
    afxError err = AFX_ERR_NONE;

    //_DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    //dpu->activePip = dpu->nextPip;

    if (!dpu->activePip)
    {
        AfxThrowError();
        return;
    }

    _ZglFlushTsChanges(dpu);
    _ZglFlushRsChanges(dpu);
    _ZglFlushLsChanges(dpu);
    _ZglFlushVsChanges(dpu);

    glVmt const* gl = dpu->gl;

    static const GLenum idxSizGl[] =
    {
        0,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_SHORT,
        0,
        GL_UNSIGNED_INT
    };
    afxSize idxSiz = dpu->activeVin->bindings.idxSrcSiz;
    GLenum top = AfxToGlTopology(dpu->primTop);
    void* offPtr = (void*)offset;

    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);
    DpuBindAndSyncBuf(dpu, GL_DRAW_INDIRECT_BUFFER, buf, TRUE);
    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &cntBuf);
    DpuBindAndSyncBuf(dpu, GL_PARAMETER_BUFFER, cntBuf, TRUE);

    /*
    The command void MultiDrawElementsIndirectCount( enum mode, enum type, const void *indirect, intptr drawcount, sizei maxdrawcount, sizei stride );
    behaves similarly to MultiDrawElementsIndirect, except that drawcount defines an offset (in bytes) into the buffer object bound to the PARAMETER_BUFFER
    binding point at which a single sizei typed value is stored, which contains the draw count. 
    maxdrawcount specifies the maximum number of draws that are expected to be stored in the buffer. 
    If the value stored at drawcount into the buffer is greater than maxdrawcount, the implementation stops processing draws after maxdrawcount parameter sets. 
    drawcount must be a multiple of four.
        
    Errors
    In addition to errors that would be generated by MultiDrawElementsIndirect:
    An INVALID_OPERATION error is generated if no buffer is bound to the PARAMETER_BUFFER binding point.
    An INVALID_VALUE error is generated if drawcount is not a multiple of four.
    An INVALID_VALUE error is generated if maxdrawcount is negative.
    An INVALID_OPERATION error is generated if reading a sizei typed value
    from the buffer bound to the PARAMETER_BUFFER target at the offset specified by drawcount would result in an out-of-bounds access.
    */

    AFX_ASSERT(gl->MultiDrawElementsIndirectCount);
    gl->MultiDrawElementsIndirectCount(top, idxSizGl[idxSiz], offPtr, cntBufOff, maxDrawCnt, stride); _ZglThrowErrorOccuried();
}

_ZGL void DpuDispatch(zglDpu* dpu, afxUnit grpCntX, afxUnit grpCntY, afxUnit grpCntZ)
{
    afxError err = AFX_ERR_NONE;

    //_DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    //dpu->activePip = dpu->nextPip;

    if (!dpu->activePip)
    {
        AfxThrowError();
        return;
    }

    _ZglFlushLsChanges(dpu);

    glVmt const* gl = dpu->gl;

    AFX_ASSERT(gl->DispatchCompute);
    gl->DispatchCompute(grpCntX, grpCntY, grpCntZ); _ZglThrowErrorOccuried();
}

_ZGL void DpuDispatchIndirect(zglDpu* dpu, avxBuffer buf, afxUnit32 offset)
{
    afxError err = AFX_ERR_NONE;

    //_DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    //dpu->activePip = dpu->nextPip;

    if (!dpu->activePip)
    {
        AfxThrowError();
        return;
    }

    _ZglFlushLsChanges(dpu);

    glVmt const* gl = dpu->gl;

    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);
    DpuBindAndSyncBuf(dpu, GL_DISPATCH_INDIRECT_BUFFER, buf, TRUE);

    AFX_ASSERT(gl->DispatchComputeIndirect);
    gl->DispatchComputeIndirect(offset); _ZglThrowErrorOccuried();
}
#if 0
_ZGL void _DpuCmdUpdateUniformVector(zglDpu* dpu, _zglCmdUniformVectorEXT const* cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

    GLuint loc = gl->GetUniformLocation(dpu->activePip->glHandle, cmd->name);
    gl->Uniform4fv(loc, cmd->cnt, cmd->v);
}

_ZGL void _DpuCmdUpdateUniformMatrix(zglDpu* dpu, _zglCmdUniformMatrixEXT const* cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

    GLuint loc = gl->GetUniformLocation(dpu->activePip->glHandle, cmd->name);
    //gl->UniformMatrix4fv(loc, cmd->cnt, GL_FALSE, cmd->m);
    AfxThrowError();
}
#endif
_ZGL void _DpuEndOfCommands(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

}

// QUEUE STUFF

_ZGL afxError _DpuWork_Transfer(zglDpu* dpu, _avxIoReqPacket* subm)
{
    afxError err = AFX_ERR_NONE;
    //AfxAssertObject(dexu, afxFcc_DEXU);

    switch (subm->Transfer.srcFcc)
    {
    case NIL:
    {
        switch (subm->Transfer.dstFcc)
        {
        case afxFcc_BUF: // raw to buf
        {
            avxBuffer buf = subm->Transfer.dst.buf;
            AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);

            if (DpuUpdateBuffer(dpu, subm->Transfer.dst.buf, subm->Transfer.src.src, subm->Transfer.opCnt, subm->Transfer.bufOps))
                AfxThrowError();

            AfxDisposeObjects(1, &buf);
            break;
        }
        case afxFcc_RAS: // raw to ras
            if (_DpuUpdateRaster(dpu, subm->Transfer.dst.ras, subm->Transfer.src.src, subm->Transfer.opCnt, subm->Transfer.rasOps))
                AfxThrowError();

            avxRaster ras = subm->Transfer.dst.ras;
            AfxDisposeObjects(1, &ras);
            break;
        default: AfxThrowError(); break;
        }
        break;
    }
    case afxFcc_BUF:
    {
        switch (subm->Transfer.dstFcc)
        {
        case NIL: // buf to raw
        {
            if (DpuDumpBuffer(dpu, subm->Transfer.src.buf, subm->Transfer.dst.dst, subm->Transfer.opCnt, subm->Transfer.bufOps))
                AfxThrowError();

            avxBuffer buf = subm->Transfer.src.buf;
            AfxDisposeObjects(1, &buf);
            break;
        }
        case afxFcc_RAS:
        {
            if (DpuUnpackRaster(dpu, subm->Transfer.dst.ras, subm->Transfer.src.buf, subm->Transfer.opCnt, subm->Transfer.rasOps))
                AfxThrowError();

            avxBuffer buf = subm->Transfer.src.buf;
            avxRaster ras = subm->Transfer.dst.ras;
            AfxDisposeObjects(1, &ras);
            AfxDisposeObjects(1, &buf);
            break;
        }
        case afxFcc_IOB: // buf to iob
        {
            if (_DpuDownloadBuffer(dpu, subm->Transfer.src.buf, subm->Transfer.dst.iob, subm->Transfer.opCnt, subm->Transfer.bufOps))
                AfxThrowError();

            afxStream iob = subm->Transfer.dst.iob;
            avxBuffer buf = subm->Transfer.src.buf;
            AfxDisposeObjects(1, &buf);
            AfxDisposeObjects(1, &iob);
            break;
        }
        default: AfxThrowError(); break;
        }
        break;
    }
    case afxFcc_RAS:
    {
        switch (subm->Transfer.dstFcc)
        {
        case NIL: // ras to raw
        {
            if (_DpuDumpRaster(dpu, subm->Transfer.src.ras, subm->Transfer.dst.dst, subm->Transfer.opCnt, subm->Transfer.rasOps))
                AfxThrowError();

            avxRaster ras = subm->Transfer.src.ras;
            AfxDisposeObjects(1, &ras);
            break;
        }
        case afxFcc_BUF:
        {
            if (DpuPackRaster(dpu, subm->Transfer.src.ras, subm->Transfer.dst.buf, subm->Transfer.opCnt, subm->Transfer.rasOps))
                AfxThrowError();

            avxBuffer buf = subm->Transfer.dst.buf;
            avxRaster ras = subm->Transfer.src.ras;
            AfxDisposeObjects(1, &ras);
            AfxDisposeObjects(1, &buf);
            break;
        }
        case afxFcc_IOB: // ras to iob
        {
            if (_DpuDownloadRaster(dpu, subm->Transfer.src.ras, subm->Transfer.dst.iob, subm->Transfer.opCnt, subm->Transfer.rasOps))
                AfxThrowError();

            afxStream iob = subm->Transfer.dst.iob;
            avxRaster ras = subm->Transfer.src.ras;
            AfxDisposeObjects(1, &ras);
            AfxDisposeObjects(1, &iob);
            break;
        }
        default: AfxThrowError(); break;
        }
        break;
    }
    case afxFcc_IOB:
    {
        switch (subm->Transfer.dstFcc)
        {
        case afxFcc_BUF: // iob to buf
        {
            if (_DpuUploadBuffer(dpu, subm->Transfer.dst.buf, subm->Transfer.src.iob, subm->Transfer.opCnt, subm->Transfer.bufOps))
                AfxThrowError();

            afxStream iob = subm->Transfer.src.iob;
            avxBuffer buf = subm->Transfer.dst.buf;
            AfxDisposeObjects(1, &buf);
            AfxDisposeObjects(1, &iob);
            break;
        }
        case afxFcc_RAS: // iob to ras
        {
            if (_DpuUploadRaster(dpu, subm->Transfer.dst.ras, subm->Transfer.src.iob, subm->Transfer.opCnt, subm->Transfer.rasOps))
                AfxThrowError();

            avxRaster ras = subm->Transfer.dst.ras;
            afxStream iob = subm->Transfer.src.iob;
            AfxDisposeObjects(1, &ras);
            AfxDisposeObjects(1, &iob);
            break;
        }
        default: AfxThrowError(); break;
        }
        break;
    }
    default: AfxThrowError(); break;
    }
    return err;
}

#if 0
_ZGL afxError _DpuExecSubmStamp(zglDpu* dpu, afxDrawBridge dexu, afxUnit queIdx, _zglQueueingStamp* subm)
{
    afxError err = AFX_ERR_NONE;

    for (afxUnit itemIdx = 0; itemIdx < subm->itemCnt; itemIdx++)
    {
        afxDrawOutput dout = subm->items[itemIdx].dout;
        AFX_ASSERT_OBJECTS(afxFcc_DOUT, 1, &dout);
        afxUnit outBufIdx = subm->items[itemIdx].bufIdx;

        afxDrawSystem dsys = dpu->activeDsys;
        AFX_ASSERT_OBJECTS(afxFcc_DSYS, 1, &dsys);

        afxWhd whd;
        avxCanvas canv;
        AvxGetDrawOutputCanvas(dout, outBufIdx, &canv);
        AvxGetCanvasArea(canv, whd);

        afxRect area = { 0 };
        area.extent[0] = whd[0];
        area.extent[1] = whd[1];
        avxDrawTarget const dt = { .loadOp = avxLoadOp_LOAD,.storeOp = avxStoreOp_STORE };
        DpuCommenceDrawScope(dpu, canv, &area, whd[2], 1, &dt, NIL, NIL, FALSE);

        avxViewport vp;
        vp.extent[0] = whd[0];
        vp.extent[1] = whd[1];
        vp.depth[1] = 1;
        DpuSetViewports(dpu, 0, 1, &vp);

        _DpuBindRasterizer(dpu, dsys->fntRazr, NIL, NIL);

        akxViewConstants vc;
        AfxM4dReset(vc.v);
        AfxComputeOffcenterOrthographicMatrix(vc.p, 0, vp.extent[0], 0, vp.extent[1], -1.f, 1.f, &AVX_CLIP_SPACE_OPENGL);
        //AfxComputeBasicOrthographicMatrix(vc.p, vp.extent[0] / vp.extent[1], 1.0, 3.0, &AVX_CLIP_SPACE_OPENGL);
        DpuBufRw(dpu, dsys->fntUnifBuf, 0, sizeof(vc), FALSE, &vc);

        DpuBindBuffers(dpu, 0, 0, 1, &dsys->fntUnifBuf, (afxUnit const[]) {0}, (afxUnit const[]) { 0 });

        DpuBindSamplers(dpu, 0, 1, 1, &dsys->fntSamp);
        DpuBindRasters(dpu, 0, 1, 1, &dsys->fntRas, NIL);

        afxReal x = subm->origin[0];
        afxReal y = subm->origin[1];
        afxUnit numchar = subm->caption.s.len;
        afxReal r = 1, g = 1, b = 1;
        afxReal x2 = x;

        afxReal* verts;
        _DpuRemapBuf(dpu, dsys->fntDataBuf, 0, AvxGetBufferCapacity(dsys->fntDataBuf, 0), NIL, (void**)&verts);
        //afxReal* verts = (void*)dsys->fntDataBuf->m.bytemap;// AvxMapBuffer(dsys->fntDataBuf, 0, 2048, avxBufferFlag_W, TRUE); // TODO map directly
        void const* bufStart = verts;

        for (char const *ptr = subm->caption.str.start, i = 0; *ptr != '\0'; ptr++)
        {
            // Decrement 'y' for any CR's
            if (*ptr == '\n')
            {
                x = x2;
                y -= 16;
                continue;
            }

#if !0
            // Just advance spaces instead of rendering empty quads
            if (*ptr == ' ')
            {
                x += 16;
                numchar--;
                continue;
            }
#endif
            // Emit position, atlas offset, and color for this character
            *verts++ = x;
            *verts++ = y;
            *verts++ = (float)(*ptr % 16)*0.0625f; // *verts++ = (float)(*ptr % 16)*0.0625f;
            *verts++ = 1.0f - (float)(*ptr / 16)*0.0625f; // *verts++ = 1.0f - (float)(*ptr / 16)*0.0625f;
            //*verts++ = r;
            //*verts++ = g;
            //*verts++ = b;
            *verts++ = 1.f;

            // Advance one character
            x += 16;
        }

        _DpuRemapBuf(dpu, dsys->fntDataBuf, 0, 0, NIL, NIL);
        //AvxUnmapBuffer(dsys->fntDataBuf, FALSE);

        zglBufferInfo bufi = { 0 };
        bufi.buf = dsys->fntDataBuf;
        bufi.stride = 20;
        DpuBindVertexBuffers(dpu, 0, 1, &bufi);
        
        avxDrawIndirect dic;
        dic.vtxCnt = 4;
        dic.firstInst = 0;
        dic.firstVtx = 0;
        dic.instCnt = numchar;
        DpuDraw(dpu, &dic);

        DpuConcludeDrawScope(dpu);
        AfxDecAtom32(&dout->submCnt);
    }

    return err;
}
#endif

_ZGL afxInt ZGL_DPU_THR_PROC(afxDrawBridge dexu)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DEXU, 1, &dexu);

    afxThread thr;
    AfxGetThread(&thr);
    AFX_ASSERT(thr == dexu->m.worker);

    afxDrawDevice ddev = AvxGetBridgedDrawDevice(dexu, NIL);

    zglDpu* dpu = &dexu->dpu;
    glVmt const* gl = dpu->gl;

    if (!wglMakeCurrentGDI(dexu->hDC, dexu->hRC))
        AfxThrowError();

    AfxMakeString(&dexu->subsysName, 0, (afxChar const*)gl->GetString(GL_RENDERER), 0); _ZglThrowErrorOccuried();
    AfxMakeString(&dexu->subsysVer, 0, (afxChar const*)gl->GetString(GL_VERSION), 0); _ZglThrowErrorOccuried();

    AfxReportf(6, NIL, "SIGMA GL/2 Draw Execution Unit %.03u\n\t%.*s %.*s", dexu->m.portId, AfxPushString(&dexu->subsysName), AfxPushString(&dexu->subsysVer));

#if 0
    AfxReportHint("%s", dpu->wgl.GetExtensionsStringARB(dpu->dc));
#endif

    dpu->m.exuIdx = dexu->m.exuIdx;
    dpu->m.portId = dexu->m.portId;
    dpu->m.dexu = dexu;
    dpu->m.dsys = AvxGetBridgedDrawSystem(dexu);
    dpu->m.thr = thr;
    dexu->m.dpu = (void*)dpu;

#if _AFX_DEBUG
    gl->Enable(GL_DEBUG_OUTPUT);
    gl->Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    gl->DebugMessageCallback(_glDbgMsgCb, NIL);
    //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

    // RESET DEFAULT STATE TO OPENGL DEFAULTS

    dpu->frontFaceCw = FALSE;
    dpu->cullMode = avxCullMode_NONE;// avxCullMode_BACK;
    dpu->fillMode = avxFillMode_FACE;

    dpu->depthBiasConstFactor = 0.f;
    dpu->depthBiasSlopeScale = 0.f;

    dpu->lineWidth = 1.f;
    
    dpu->depthTestEnabled = FALSE;
    dpu->depthCompareOp = avxCompareOp_LESS;
    dpu->depthWriteDisabled = FALSE;
    dpu->depthBounds[0] = 0.f;
    dpu->depthBounds[1] = 1.f;
    
    dpu->stencilTestEnabled = FALSE;
    dpu->stencilFront.compareOp = avxCompareOp_ALWAYS;
    dpu->stencilFront.reference = 0;
    dpu->stencilFront.compareMask = (dpu->stencilFront.writeMask = 0xFFFFFFFF);
    dpu->stencilFront.failOp = (dpu->stencilFront.passOp = (dpu->stencilFront.depthFailOp = avxStencilOp_KEEP));
    dpu->stencilBack.compareOp = avxCompareOp_ALWAYS;
    dpu->stencilBack.reference = 0;
    dpu->stencilBack.compareMask = (dpu->stencilBack.writeMask = 0xFFFFFFFF);
    dpu->stencilBack.failOp = (dpu->stencilBack.passOp = (dpu->stencilBack.depthFailOp = avxStencilOp_KEEP));
    
    dpu->anyBlendEnabled = FALSE;

    for (afxUnit i = 0; i < 8; i++)
    {
        dpu->outs[i].blendConfig.aBlendOp = avxBlendOp_ADD;
        dpu->outs[i].blendConfig.rgbBlendOp = avxBlendOp_ADD;
        dpu->outs[i].blendConfig.aSrcFactor = avxBlendFactor_ONE;
        dpu->outs[i].blendConfig.rgbSrcFactor = avxBlendFactor_ONE;
        dpu->outs[i].blendConfig.aDstFactor = avxBlendFactor_ZERO;
        dpu->outs[i].blendConfig.rgbDstFactor = avxBlendFactor_ZERO;
        dpu->outs[i].discardMask = NIL;
    }

    // In Qwadro specification, seamless cubemap sampling is ALWAYS enabled by default.
    // As SIGMA GL/2 is only bound to Qwadro, actually there is not way of disable it.
    gl->Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS); _ZglThrowErrorOccuried();
    //gl->ClipControl(/*GL_LOWER_LEFT_EXT*/0x8CA1, /*GL_ZERO_TO_ONE_EXT*/0x935F); _ZglThrowErrorOccuried();// set screen origin to top to bottm, and set depth to [ 0, 1 ]

    // Scissor test is ALWAYS enabled.
    // Our need it to crop the specified canvas rect, for example.
    gl->Enable(GL_SCISSOR_TEST); _ZglThrowErrorOccuried();

    // empty VAO used when a vertex input is not provided to avoid crash on vertex fetch.
    gl->GenVertexArrays(1, &dpu->emptyVao); _ZglThrowErrorOccuried();
    gl->BindVertexArray(dpu->emptyVao); _ZglThrowErrorOccuried();
    gl->BindVertexArray(0); _ZglThrowErrorOccuried();

    // FBOs used to blit rasters.
    gl->GenFramebuffers(1, &dpu->fboOpSrc); _ZglThrowErrorOccuried();
    gl->BindFramebuffer(GL_READ_FRAMEBUFFER, dpu->fboOpSrc); _ZglThrowErrorOccuried();
    gl->BindFramebuffer(GL_READ_FRAMEBUFFER, 0); _ZglThrowErrorOccuried();
    gl->GenFramebuffers(1, &dpu->fboOpDst); _ZglThrowErrorOccuried();
    gl->BindFramebuffer(GL_DRAW_FRAMEBUFFER, dpu->fboOpDst); _ZglThrowErrorOccuried();
    gl->BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); _ZglThrowErrorOccuried();
    dpu->fboOpSrcAnnex = NIL;
    dpu->fboOpDstAnnex = NIL;

    GLint datai;
    gl->GetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &datai); _ZglThrowErrorOccuried();

    dpu->texPackUnitIdx = datai - 1;
    dpu->texUnpackUnitIdx = datai - 2;
    dpu->srcPboGpuHandle = NIL;
    dpu->dstPboGpuHandle = NIL;

    // GL_MAX_COMBINED_UNIFORM_BLOCKS --- the maximum number of uniform blocks per program.The value must be at least 70.
    gl->GetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &datai); _ZglThrowErrorOccuried();

    gl->GenBuffers(1, &dpu->pushConstUbo); _ZglThrowErrorOccuried();
    gl->BindBuffer(GL_UNIFORM_BUFFER, dpu->pushConstUbo); _ZglThrowErrorOccuried();
#ifdef COHERENT_PUSHABLES
    GLbitfield pushUboCreateFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_DYNAMIC_STORAGE_BIT;
    GLbitfield pushUboAccessFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
#else
    GLbitfield pushUboCreateFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT;
    GLbitfield pushUboAccessFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
#endif
    gl->BufferStorage(GL_UNIFORM_BUFFER, ddev->m.limits.maxPushConstsSiz, NIL, pushUboCreateFlags); _ZglThrowErrorOccuried();
    dpu->pushConstMappedMem = gl->MapBufferRange(GL_UNIFORM_BUFFER, 0, ddev->m.limits.maxPushConstsSiz, pushUboAccessFlags); _ZglThrowErrorOccuried();
    gl->BindBuffer(GL_UNIFORM_BUFFER, 0); _ZglThrowErrorOccuried();
    dpu->pushConstUboIdx = datai - 1; // the last uniform block
    gl->BindBufferBase(GL_UNIFORM_BUFFER, dpu->pushConstUboIdx, dpu->pushConstUbo); _ZglThrowErrorOccuried();

    dpu->emulatedDrawParams = !wglHasExtensionSIG(dexu->hDC, "GL_ARB_shader_draw_parameters");

    dpu->m.instanced = TRUE;
    dpu->m.running = TRUE;
    
    while (1)
    {
#if 0
        AfxLockMutex(&dexu->m.schedCndMtx);
        afxTimeSpec ts = { 0 };
        ts.nsec = 1 * 100000; // 100000 = 0.0001 sec

        while (!dexu->m.schedCnt)
            AfxWaitTimedCondition(&dexu->m.schedCnd, &dexu->m.schedCndMtx, &ts);

        dexu->m.schedCnt = 0;
        AfxUnlockMutex(&dexu->m.schedCndMtx);
#endif
        afxBool ctxEntered = FALSE;

        if (!ctxEntered)
        {
            HGLRC rc = wglGetCurrentContextGDI();

            if (rc == dexu->hRC) ctxEntered = TRUE;
            else
            {
                if (!(ctxEntered = !!wglMakeCurrentGDI(dexu->hDC, dexu->hRC)))
                {
                    AfxThrowError();
                    AfxReportError("DPU %u failed to take active context.", dexu->m.portId);
                }
            }

            if (ctxEntered)
                dpu->m.dsys = AvxGetBridgedDrawSystem(dexu);
        }

        if (ctxEntered)
        {
            dpu->m.numOfFedVertices = 0;
            dpu->m.numOfFedIndices = 0;
            dpu->m.numOfFedInstances = 0;
        }

        dexu->m.procCb(dpu);

        _ZglProcessDeletionQueue(gl, &dexu->deletionQueue);

        if (AfxShouldThreadBeInterrupted())
            break;

        AfxYield();
    }

    gl->DeleteVertexArrays(1, &dpu->emptyVao); _ZglThrowErrorOccuried();
    gl->DeleteFramebuffers(1, &dpu->fboOpSrc); _ZglThrowErrorOccuried();
    gl->DeleteFramebuffers(1, &dpu->fboOpDst); _ZglThrowErrorOccuried();

    gl->DeleteBuffers(1, &dpu->pushConstUbo); _ZglThrowErrorOccuried();

    wglMakeCurrentGDI(NIL, NIL);

    return 0;
}

_ZGL afxBool _Dpu_ProcCb(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT(dpu);

    glVmt const* gl = dpu->gl;
    afxDrawBridge dexu = dpu->m.dexu;
    _avxIoReqLut const* iorpVmt = dexu->m.iorpVmt;

    afxClass const* dqueCls = _AvxGetDrawQueueClass(dexu);
    afxUnit totalQueCnt = dqueCls->instCnt;

    for (afxUnit queIdx = 0; queIdx < totalQueCnt; queIdx++)
    {
        afxDrawQueue dque;
        if (!AvxGetDrawQueues(dexu, queIdx, 1, &dque) || !dque)
        {
            AfxThrowError();
            continue;
        }
        AFX_ASSERT_OBJECTS(afxFcc_DQUE, 1, &dque);

        if (dque->m.portId != dexu->m.portId)
            continue;

        if (AfxTryLockMutex(&dque->m.iorpChnMtx))
        {
            afxBool mustWaitCnt = 0;

            _avxIoReqPacket* iorp;
            AFX_ITERATE_CHAIN_B2F(_avxIoReqPacket, iorp, hdr.chain, &dque->m.iorpChn)
            {
                AFX_ASSERT(dque->m.iorpChn.cnt);

                AFX_ASSERT(iorpVmt->f[iorp->hdr.id]);
                iorpVmt->f[iorp->hdr.id](dpu, iorp);

                if (iorp->hdr.completionFence)
                {
                    ++mustWaitCnt;
                }
                else
                {
                    // if we have not to wait, delete it right new.
                    AfxGetTime(&iorp->hdr.complTime);
                    _AvxDquePopIoReqPacket(dque, iorp);
                }
            }

            GLsync glFence = NIL;
            if (mustWaitCnt)
            {
                glFence = gl->FenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0); _ZglThrowErrorOccuried();
            }

            while (mustWaitCnt)
            {
                // To block all CPU operations until a sync object is signaled, you call this function:
                GLenum rslt = gl->ClientWaitSync(glFence, GL_SYNC_FLUSH_COMMANDS_BIT, /*timeout*/0); _ZglThrowErrorOccuried();

                /*
                    This function will not return until one of two things happens: the sync​ object parameter becomes signaled, or a number of nanoseconds greater than or equal to the timeout​ parameter passes.
                    If timeout​ is zero, the function will simply check to see if the sync object is signaled and return immediately.
                    Note that the fact that timeout​ is in nanoseconds does not imply that this function has true nanosecond granularity in its timeout; you are only guaranteed that at least that much time will pass.
                */

                switch (rslt)
                {
                case GL_ALREADY_SIGNALED: // the sync object was signaled before the function was called.
                case GL_CONDITION_SATISFIED: // the sync object was signaled within the given timeout period.
                {
                    AFX_ITERATE_CHAIN_B2F(_avxIoReqPacket, iorp, hdr.chain, &dque->m.iorpChn)
                    {
                        AFX_ASSERT(dque->m.iorpChn.cnt);

                        avxFence fenc = iorp->hdr.completionFence;

                        if (fenc)
                        {
                            _ZglSignalFence(dpu, fenc);

                            AfxGetTime(&iorp->hdr.complTime);
                            _AvxDquePopIoReqPacket(dque, iorp);
                            //mustWaitCnt--;
                            mustWaitCnt = 0; // quit while
                        }
                    }
                    break;
                }
                case GL_TIMEOUT_EXPIRED: // the sync object did not signal within the given timeout period.
                    AfxYield(); // once we have locked inside this loop, give way.
                    break;
                case GL_WAIT_FAILED: // If an OpenGL Error occurred, then GL_WAIT_FAILED will be returned in addition to raising an error.
                default:
                    mustWaitCnt = 0; // quit while
                    break;
                }
            }

            if (glFence)
            {
                gl->DeleteSync(glFence); _ZglThrowErrorOccuried();
            }

            AfxUnlockMutex(&dque->m.iorpChnMtx);
            AfxSignalCondition(&dque->m.idleCnd);
            AfxYield();
        }
    }
    return TRUE;
}

_ZGL afxError _ZglDexuDtorCb(afxDrawBridge dexu)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DEXU, 1, &dexu);

    afxDrawSystem dsys = AvxGetBridgedDrawSystem(dexu);

    //AvxWaitForDrawSystem(dsys, AFX_TIME_INFINITE);
    //AvxWaitForDrawSystem(dsys, AFX_TIME_INFINITE); // yes, two times.

    if (_AVX_DEXU_CLASS_CONFIG.dtor(dexu))
        AfxThrowError();

    wglDeleteContextGDI(dexu->hRC);
    ReleaseDC(dexu->hWnd, dexu->hDC);
    DestroyWindow(dexu->hWnd);

    AfxDismantleInterlockedQueue(&dexu->deletionQueue);

    return err;
}

_ZGL afxError _ZglDexuCtorCb(afxDrawBridge dexu, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DEXU, 1, &dexu);

    afxDrawSystem dsys = args[0];
    AFX_ASSERT_OBJECTS(afxFcc_DSYS, 1, &dsys);
    _avxDexuAcquisition const* cfg = AFX_CAST(_avxDexuAcquisition const*, args[1]) + invokeNo;
    
    afxDrawDevice ddev = cfg->ddev;
    AFX_ASSERT_OBJECTS(afxFcc_DDEV, 1, &ddev);

    if (_ZglCreateHwSurface(0, 0, &dexu->hWnd, &dexu->hDC, &dexu->dcPxlFmt, &dexu->dcPfd))
    {
        AfxThrowError();
        return err;
    }

    HDC bkpHdc = wglGetCurrentDCGDI();
    HGLRC bkpGlrc = wglGetCurrentContextGDI();
    HGLRC hShareRC = dsys->hPrimeRC;

    AfxZero(&dexu->dpu, sizeof(dexu->dpu));
    if (_ZglCreateHwContext(dexu->hDC, hShareRC, dsys->glVerMaj, dsys->glVerMin, dsys->robustCtx, &dexu->hRC, NIL, FALSE))
    {
        AfxThrowError();
        ReleaseDC(dexu->hWnd, dexu->hDC), DestroyWindow(dexu->hWnd);
        return err;
    }

    dexu->dpu.gl = &dsys->gl;

    if (err)
    {
        wglDeleteContextGDI(dexu->hRC);
        ReleaseDC(dexu->hWnd, dexu->hDC), DestroyWindow(dexu->hWnd);
    }

#if 0
    afxBool shared = FALSE;
    afxDrawBridge primeExu = NIL;
    afxUnit primeExuIdx = 0;
    for (afxUnit i = 0; i < dsys->m.exuCnt; i++)
    {
        if (dsys->m.exus[i] && (ddev == AfxGetProvider(dsys->m.exus[i])));
        {
            primeExuIdx = i;
            primeExu = dsys->m.exus[primeExuIdx];
            shared = TRUE;
        }
    }
#endif

#if 0
    dexu->vinExt.extId = afxFcc_DEXU;
    AfxInstallClassExtension(_AvxDsysGetVtxdClass(dsys), &dexu->vinExt);

    dexu->pipExt.extId = afxFcc_DEXU;
    AfxInstallClassExtension(_AvxDsysGetPipClass(dsys), &dexu->pipExt);

    dexu->canvExt.extId = afxFcc_DEXU;
    AfxInstallClassExtension(_AvxDsysGetCanvClass(dsys), &dexu->canvExt);
#endif

    if (_AVX_DEXU_CLASS_CONFIG.ctor(dexu, (void*[]) { dsys, (void*)cfg }, 0))
    {
        AfxThrowError();
        wglDeleteContextGDI(dexu->hRC);
        ReleaseDC(dexu->hWnd, dexu->hDC), DestroyWindow(dexu->hWnd);
        return err;
    }

    AfxDeployInterlockedQueue(&dexu->deletionQueue, sizeof(_zglDeleteGlRes), 32);

    static _avxIoReqLut const dpuIorpVmt =
    {
        .Callback = _AvxDpuWork_CallbackCb,
        .Execute = _AvxDpuWork_ExecuteCb,
        .Transfer = _DpuWork_Transfer,
        .Remap = _DpuWork_Remap,
        .SyncMaps = _DpuWork_SyncMaps
    };

    dexu->m.cmdVmt = &cmdDevmt;
    dexu->m.iorpVmt = &dpuIorpVmt;
    dexu->m.workerProc = ZGL_DPU_THR_PROC;
    dexu->m.procCb = (void*)_Dpu_ProcCb;

    if (err)
    {
        _AVX_DEXU_CLASS_CONFIG.dtor(dexu);
        wglDeleteContextGDI(dexu->hRC);
        ReleaseDC(dexu->hWnd, dexu->hDC), DestroyWindow(dexu->hWnd);
    }
    return err;
}
