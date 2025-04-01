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

////////////////////////////////////////////////////////////////////////////////
// FRAME BUFFER                                                               //
////////////////////////////////////////////////////////////////////////////////

_ZGL afxError _ZglBindFboAttachment(glVmt const* gl, GLenum glTarget, GLenum glAttachment, GLenum texTarget, GLuint texHandle, GLint level, GLuint z)
{
    afxError err = AFX_ERR_NONE;

    switch (texTarget)
    {
    case GL_RENDERBUFFER:
    {
        gl->FramebufferRenderbuffer(glTarget, glAttachment, GL_RENDERBUFFER, texHandle); _ZglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_2D:
    {
        gl->FramebufferTexture2D(glTarget, glAttachment, texTarget, texHandle, level); _ZglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_1D_ARRAY:
    case GL_TEXTURE_2D_ARRAY:
    {
        gl->FramebufferTexture(glTarget, glAttachment, texHandle, level); _ZglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_1D:
    {
        gl->FramebufferTexture1D(glTarget, glAttachment, texTarget, texHandle, level); _ZglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_3D:
    {
        gl->FramebufferTexture3D(glTarget, glAttachment, texTarget, texHandle, level, z); _ZglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_CUBE_MAP:
    default:
        gl->FramebufferTextureLayer(glTarget, glAttachment, texHandle, level, z); _ZglThrowErrorOccuried();
        break;
    };

    return err;
}

_ZGL afxError _ZglBindFboAttachmentDirect(glVmt const* gl, GLuint fbo, GLenum glAttachment, GLenum texTarget, GLuint texHandle, GLint level, GLuint z)
{
    afxError err = AFX_ERR_NONE;

    switch (texTarget)
    {
    case GL_RENDERBUFFER:
    {
        gl->NamedFramebufferRenderbuffer(fbo, glAttachment, GL_RENDERBUFFER, texHandle); _ZglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_2D:
    case GL_TEXTURE_1D:
    {
        gl->NamedFramebufferTexture(fbo, glAttachment, texHandle, level); _ZglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_2D_ARRAY:
    case GL_TEXTURE_1D_ARRAY:
    case GL_TEXTURE_CUBE_MAP:
    case GL_TEXTURE_3D:
    {
        gl->NamedFramebufferTextureLayer(fbo, glAttachment, texHandle, level, z); _ZglThrowErrorOccuried();
        break;
    }
    default: _ZglThrowErrorOccuried(); break;
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
    }
    else
    {
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
                    _ZglBindFboAttachment(dpu->gl, glTarget, glAttachment, glTexTarget, glTexHandle, 0, 0);
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
                    _ZglBindFboAttachment(dpu->gl, glTarget, glAttachment, glTexTarget, glTexHandle, ras->m.baseLod, ras->m.baseLayer);
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

            dpu->canvGpuHandle = glHandle;
            dpu->canv = canv;
            dpu->canv = NIL;
        }
    }
    return err;
}

#if 0
_ZGL afxError _ZglReadjustCanvasCb(avxCanvas canv, afxWhd const whd)
{
	afxError err = AFX_ERR_NONE;
	AFX_ASSERT_OBJECTS(afxFcc_CANV, 1, &canv);
	AFX_ASSERT(whd);
	AFX_ASSERT(whd[0]);
	AFX_ASSERT(whd[1]);
    afxWhd minWhd = { ZGL_MAX_CANVAS_WIDTH, ZGL_MAX_CANVAS_HEIGHT, ZGL_MAX_CANVAS_LAYERS }, surWhd;
    
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
