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

_ZGL afxError _SglBindFboAttachment(glVmt const* gl, GLenum glTarget, GLenum glAttachment, GLenum texTarget, GLuint texHandle, GLint level, GLuint layer, GLuint z)
{
    afxError err = AFX_ERR_NONE;

    switch (texTarget)
    {
    case GL_RENDERBUFFER:
    {
        gl->FramebufferRenderbuffer(glTarget, glAttachment, GL_RENDERBUFFER, texHandle); _SglThrowErrorOccuried();
    }
    case GL_TEXTURE_2D:
    {
        gl->FramebufferTexture2D(glTarget, glAttachment, texTarget, texHandle, level); _SglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_1D_ARRAY:
    {
        gl->FramebufferTexture2D(glTarget, glAttachment, texTarget, texHandle, level); _SglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_1D:
    {
        gl->FramebufferTexture1D(glTarget, glAttachment, texTarget, texHandle, level); _SglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_3D:
    {
        gl->FramebufferTexture3D(glTarget, glAttachment, texTarget, texHandle, level, z); _SglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_2D_ARRAY:
    {
        gl->FramebufferTexture3D(glTarget, glAttachment, texTarget, texHandle, level, layer); _SglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_CUBE_MAP:
    default:
        gl->FramebufferTexture(glTarget, glAttachment, texHandle, level); _SglThrowErrorOccuried();
        break;
    };

    return err;
}

_ZGL afxError _DpuBindAndSyncCanv(zglDpu* dpu, GLenum glTarget, avxCanvas canv)
{
    //AfxEntry("canv=%p", canv);
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    afxUnit exuIdx = dpu->exuIdx;
    afxBool bound = (canv == dpu->activeRs.canv);

    if (!canv)
    {
        if (dpu->activeRs.canv || dpu->activeRs.canvGpuHandle)
        {
            gl->BindFramebuffer(glTarget, 0); _SglThrowErrorOccuried();
            dpu->activeRs.canvGpuHandle = NIL;
            dpu->activeRs.canv = NIL;
            dpu->nextRs.canv = NIL;
        }
    }
    else
    {
        AfxAssertObjects(1, &canv, afxFcc_CANV);
        GLuint glHandle = canv->glHandle;
        zglUpdateFlags devUpdReq = (canv->updFlags & ZGL_UPD_FLAG_DEVICE);
            
        if ((!glHandle) || (devUpdReq & ZGL_UPD_FLAG_DEVICE_INST))
        {
            if (glHandle)
            {
                AfxAssert(gl->IsFramebuffer(glHandle));
                gl->DeleteFramebuffers(1, &(glHandle)); _SglThrowErrorOccuried();
                canv->glHandle = NIL;
                glHandle = NIL;
            }
            gl->GenFramebuffers(1, &(glHandle)); _SglThrowErrorOccuried();
            gl->BindFramebuffer(glTarget, glHandle); _SglThrowErrorOccuried();
            AfxAssert(gl->IsFramebuffer(glHandle));
            canv->glHandle = glHandle;
            AfxAssert(!bound);
            bound = TRUE;
            dpu->activeRs.canv = canv;

            afxUnit surCnt;
            AfxCountDrawBufferSlots(canv, &surCnt, NIL);
            afxRaster surfaces[_ZGL_MAX_SURF_PER_CANV];
            
            if (surCnt)
            {
                AfxAssert(_ZGL_MAX_SURF_PER_CANV >= surCnt);
                AfxGetDrawBuffers(canv, 0, surCnt, surfaces);

                afxUnit dsSurIdx[2];
                AfxGetDepthBufferSlots(canv, &dsSurIdx[0], &dsSurIdx[1]);
                afxBool combinedDs = (dsSurIdx[1] == dsSurIdx[0]);

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
                    else if ((dsSurIdx[1] == i) && !combinedDs)
                        glAttachment = GL_STENCIL_ATTACHMENT;
                    else
                    {
                        glAttachment = GL_COLOR_ATTACHMENT0 + colorIdx;
                        ++colorIdx;
                    }

                    afxRaster ras = surfaces[i];

                    if (!ras)
                    {
                        AfxThrowError();
                        glTexHandle = 0;
                        glTexTarget = GL_TEXTURE_2D;
                    }
                    else
                    {
                        AfxAssertObjects(1, &ras, afxFcc_RAS);
                        AfxAssert(AfxTestRasterUsage(ras, afxRasterUsage_DRAW));

                        DpuBindAndSyncRas(dpu, ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT, ras);

                        if (afxRasterUsage_DRAW == AfxTestRasterUsage(ras, afxRasterUsage_ALL))
                        {
                            glTexHandle = ras->glRboHandle;
                            glTexTarget = GL_RENDERBUFFER;
                            AfxAssert(gl->IsRenderBuffer(glTexHandle));
                        }
                        else
                        {
                            glTexHandle = ras->glHandle;
                            glTexTarget = ras->glTarget;
                            AfxAssert(gl->IsTexture(glTexHandle));
                        }
                    }
                    _SglBindFboAttachment(&dpu->gl, glTarget, glAttachment, glTexTarget, glTexHandle, 0, 0, 0);
                }
            }

            switch (gl->CheckFramebufferStatus(glTarget))
            {
            case GL_FRAMEBUFFER_COMPLETE:
                canv->updFlags &= ~(ZGL_UPD_FLAG_DEVICE);
                AfxLogEcho("avxCanvas %p hardware-side data instanced.", canv);
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                AfxLogError("Not all framebuffer attachment points are framebuffer attachment complete.");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                AfxLogError("No images are attached to the framebuffer.");
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                AfxLogError("The combination of internal formats of the attached images violates an implementation-dependent set of restrictions.");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                AfxLogError("Incomplete draw frame buffer.");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                AfxLogError("Incomplete read frame buffer.");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                AfxLogError("Incomplete multisample.");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                AfxLogError("Incomplete layer targets.");
                break;
            case GL_FRAMEBUFFER_UNDEFINED:
                AfxLogError("Default framebuffer does not exist");
                break;
            default:
                AfxLogError("UNKNOWN");
                _SglThrowErrorOccuried();
                break;
            }

            dpu->activeRs.canvGpuHandle = glHandle;
            dpu->activeRs.canv = canv;
            dpu->nextRs.canv = NIL;
        }
        else
        {
            if ((canv != dpu->activeRs.canv) ||
                (glHandle != dpu->activeRs.canvGpuHandle)
                )
            {
                AfxAssert(gl->IsFramebuffer(glHandle));
                gl->BindFramebuffer(glTarget, glHandle); _SglThrowErrorOccuried();
                dpu->activeRs.canvGpuHandle = glHandle;
                dpu->activeRs.canv = canv;
                dpu->nextRs.canv = NIL;
            }
        }
    }
    return err;
}

#if 0
_ZGL afxError _SglReadjustCanvasCb(avxCanvas canv, afxWhd const whd)
{
	afxError err = AFX_ERR_NONE;
	AfxAssertObjects(1, &canv, afxFcc_CANV);
	AfxAssert(whd);
	AfxAssert(whd[0]);
	AfxAssert(whd[1]);
    afxWhd minWhd = { ZGL_MAX_CANVAS_WIDTH, ZGL_MAX_CANVAS_HEIGHT, ZGL_MAX_CANVAS_LAYERS }, surWhd;
    
    for (afxUnit i = 0; i < canv->m.slotCnt; i++)
    {
        afxRaster ras = canv->m.slots[i].ras;

        if (ras)
        {
            AfxAssertObjects(1, &ras, afxFcc_RAS);
            AfxGetRasterExtent(ras, 0, surWhd);
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
	AfxAssertObjects(1, &canv, afxFcc_CANV);

	//canv->m.colorCnt = 0;
    //canv->m.surfCnt = 0;
	return err;
}

_ZGL afxError _SglCanvDtor(avxCanvas canv)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &canv, afxFcc_CANV);

    afxDrawContext dctx = AfxGetProvider(canv);

    for (afxUnit i = 0; i < canv->m.slotCnt; i++)
    {
        afxRaster ras = canv->m.slots[i].ras;

        if (ras)
        {
            AfxAssertObjects(1, &ras, afxFcc_RAS);
            AfxReleaseObjects(1, &ras);
            canv->m.slots[i].ras = NIL;
        }
    }

    if (canv->glHandle)
    {
        _SglDctxEnqueueDeletion(dctx, 0, GL_FRAMEBUFFER, (afxSize)canv->glHandle);
        canv->glHandle = 0;
    }

    if (_AvxCanvStdImplementation.dtor(canv))
        AfxThrowError();

    return err;
}

_ZGL afxError _SglCanvCtor(avxCanvas canv, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;

    if (_AvxCanvStdImplementation.ctor(canv, args, invokeNo)) AfxThrowError();
    else
    {
        //canv->m.readjust = _SglReadjustCanvasCb;
        canv->updFlags = ZGL_UPD_FLAG_DEVICE_INST;
        canv->glHandle = 0;
        canv->storeBitmask = NIL;
    }
    return err;
}
