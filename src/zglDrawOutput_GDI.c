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

#pragma comment(lib,"dwmapi.lib")


#include "zglUtils.h"
#include "zglCommands.h"
#include "zglObjects.h"

_ZGL afxUnit _ZglDoutIsSuspended(afxDrawOutput dout)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DOUT, 1, &dout);
    AfxLockFutexShared(&dout->m.suspendSlock);
    afxUnit suspendCnt = dout->m.suspendCnt;
    AfxUnlockFutexShared(&dout->m.suspendSlock);
    return suspendCnt;
}

_ZGL afxUnit _ZglDoutSuspendFunction(afxDrawOutput dout)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DOUT, 1, &dout);
    AfxLockFutex(&dout->m.suspendSlock);
    afxUnit suspendCnt = ++dout->m.suspendCnt;
    AfxUnlockFutex(&dout->m.suspendSlock);
    return suspendCnt;
}

_ZGL afxUnit _ZglDoutResumeFunction(afxDrawOutput dout)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DOUT, 1, &dout);
    AfxLockFutex(&dout->m.suspendSlock);
    afxUnit suspendCnt = --dout->m.suspendCnt;
    AfxUnlockFutex(&dout->m.suspendSlock);
    return suspendCnt;
}

_ZGL afxError _ZglDoutPresentGdi(afxDrawQueue dque, avxPresentation* ctrl, afxDrawOutput dout, afxUnit bufIdx)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DOUT, 1, &dout);

    AFX_ASSERT(dout->m.presentingBufIdx == bufIdx); // must be called by _AvxDoutPresent() to handle sync.

    HDC bkpHdc = wglGetCurrentDCGDI();
    HGLRC bkpGlrc = wglGetCurrentContextGDI();
    afxBool mustRestoreCtx = FALSE;

    if (bkpHdc != dout->hDC)
    {
        if (!wglMakeCurrentGDI(dout->hDC, dout->hSwapRC))
        {
            AfxThrowError();
            return err;
        }
        mustRestoreCtx = TRUE;
    }

    glVmt const* gl = dout->gl;

    avxRange whd;
    avxRaster buf;
    avxCanvas canv;
    AvxGetDrawOutputBuffer(dout, bufIdx, &buf);
    AvxGetDrawOutputCanvas(dout, bufIdx, &canv);
    AFX_ASSERT_OBJECTS(afxFcc_CANV, 1, &canv);
    AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &buf);
    whd = AvxGetCanvasArea(canv, AVX_ORIGIN_ZERO);

    if (buf->glHandle)
    {
        if (!dout->gdiGlSwap[bufIdx].swapFboReady)
        {
            if (!dout->gdiGlSwap[bufIdx].swapFbo)
            {
                gl->GenFramebuffers(1, &dout->gdiGlSwap[bufIdx].swapFbo); _ZglThrowErrorOccuried();
                gl->BindFramebuffer(GL_READ_FRAMEBUFFER, dout->gdiGlSwap[bufIdx].swapFbo); _ZglThrowErrorOccuried();
                AFX_ASSERT(gl->IsFramebuffer(dout->gdiGlSwap[bufIdx].swapFbo));
            }
            else
            {
                AFX_ASSERT(gl->IsFramebuffer(dout->gdiGlSwap[bufIdx].swapFbo));
                gl->BindFramebuffer(GL_READ_FRAMEBUFFER, dout->gdiGlSwap[bufIdx].swapFbo); _ZglThrowErrorOccuried();
            }
            _ZglBindFboAttachment(gl, GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, buf->glTarget, buf->glHandle, 0, 0);
            dout->gdiGlSwap[bufIdx].swapFboReady = TRUE;
        }
        else
        {
            AFX_ASSERT(gl->IsFramebuffer(dout->gdiGlSwap[bufIdx].swapFbo));
            gl->BindFramebuffer(GL_READ_FRAMEBUFFER, dout->gdiGlSwap[bufIdx].swapFbo); _ZglThrowErrorOccuried();
        }

        static GLenum const invBufs[] =
        {
            GL_COLOR,
            GL_DEPTH,
            GL_STENCIL
        };
        static GLbitfield const clearBitmask = GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT*/;

        //gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, ARRAY_SIZE(invBufs), invBufs); _ZglThrowErrorOccuried();
        gl->BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); _ZglThrowErrorOccuried();
        //gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, ARRAY_SIZE(invBufs), invBufs); _ZglThrowErrorOccuried();
        gl->Clear(clearBitmask);  _ZglThrowErrorOccuried();
        //gl->ClearBufferfv(GL_COLOR, /*GL_DRAW_BUFFER0 +*/ 0, AFX_V4D_ZERO); _ZglThrowErrorOccuried();

        afxInt x = (dout->m.presentTransform & avxVideoTransform_FLIP_H) ? whd.w : 0;
        afxInt y = (dout->m.presentTransform & avxVideoTransform_FLIP_V) ? whd.h : 0;
        afxInt w = (dout->m.presentTransform & avxVideoTransform_FLIP_H) ? 0 : whd.w;
        afxInt h = (dout->m.presentTransform & avxVideoTransform_FLIP_V) ? 0 : whd.h;

        gl->BlitFramebuffer(0, 0, whd.w, whd.h, x, y, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST); _ZglThrowErrorOccuried();
        //gl->BindFramebuffer(GL_READ_FRAMEBUFFER, 0); _ZglThrowErrorOccuried(); _ZglThrowErrorOccuried();
        //gl->BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); _ZglThrowErrorOccuried(); _ZglThrowErrorOccuried();

#if 0
    //gl->Flush();
        SwapBuffers(dout->hDC);
#else
        wglSwapBuffersGDI(dout->hDC);
#endif
    }

    if (dout->m.presentAlpha && (dout->m.presentAlpha != avxVideoAlpha_OPAQUE))
    {
        if (!dout->m.exclusive)
            DwmFlush();
    }

    if (mustRestoreCtx)
    {
        wglMakeCurrentGDI(bkpHdc, bkpGlrc);
    }

    AfxYield();
    //AfxSleep(1);

    return err;
}

#if 0
_ZGL afxError _DpuPresentDout_BlitCanvToGdi(zglDpu* dpu, afxDrawOutput dout, afxUnit outBufIdx)
{
    afxError err = AFX_ERR_NONE;

    //if (_ZglActivateDout(dpu, dout))
        //AfxThrowError();

    AFX_ASSERT(dout->m.presentingBufIdx == (afxAtom32)AFX_INVALID_INDEX);
    dout->m.presentingBufIdx = outBufIdx;

    avxCanvas canv;
    AvxGetDrawOutputCanvas(dout, outBufIdx, &canv);
    AFX_ASSERT_OBJECTS(afxFcc_CANV, 1, &canv);
    //AFX_ASSERT(surf->state == AFX_SURF_STATE_PENDING);
    glVmt const* gl = dpu->gl;

    afxWhd whd;
    AvxQueryDrawOutputExtent(dout, &whd, NIL);
#if 0
    int a = 0;
    HDC dc = dout->hDC;
    HDC dc2 = CreateCompatibleDC(dc);
    
    if (wglSetPixelFormatSIG(dc2, dout->dcPixFmt, NIL, NIL))
        a = 1;

    HBITMAP bmp = CreateCompatibleBitmap(dc, whd[0], whd[1]);
    
    if (wglSetPixelFormatSIG(dc2, dout->dcPixFmt, NIL, NIL))
        a  = 1;

    SelectObject(dc2, bmp);
    
    if (wglSetPixelFormatSIG(dc2, dout->dcPixFmt, NIL, NIL))
        a = 1;

    if (wglMakeContextCurrentSIG(dc2, dc, dpu->glrc))
        a = 1;
#endif
    dpu->activeRs.canv = NIL;
    gl->BindFramebuffer(GL_READ_FRAMEBUFFER, canv->glHandle); _ZglThrowErrorOccuried();
    gl->BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); _ZglThrowErrorOccuried();
    //gl->Clear(GL_COLOR_BUFFER_BIT);  _ZglThrowErrorOccuried();

    afxInt x = (dout->m.presentTransform & avxVideoTransform_FLIP_H) ? whd.x : 0;
    afxInt y = (dout->m.presentTransform & avxVideoTransform_FLIP_V) ? whd.y : 0;
    afxInt w = (dout->m.presentTransform & avxVideoTransform_FLIP_H) ? 0 : whd.w;
    afxInt h = (dout->m.presentTransform & avxVideoTransform_FLIP_V) ? 0 : whd.h;

    gl->BlitFramebuffer(0, 0, whd.w, whd.h, x, y, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST); _ZglThrowErrorOccuried();
    gl->BindFramebuffer(GL_READ_FRAMEBUFFER, 0); _ZglThrowErrorOccuried();
    gl->Flush(); _ZglThrowErrorOccuried(); // flush is presenting/swapping

    HDC dc = dout->hDC;
    AFX_ASSERT(dc);

    //BitBlt(dc, 0, 0, whd[0], whd[1], dc2, 0, 0, SRCCOPY);
    wglSwapBuffersSIG(dc, NIL); // deadlocks all
    //SwapBuffers(dc);
    //wglPresentBuffersSIG(dc, (int)dpu->glrc);
    //cur = wglMakeContextCurrentSIG(dpu->dc, dpu->dc, dpu->glrc);
    //DeleteObject(bmp);
    //DeleteDC(dc2);

    //if (dout->exclusive)
    {
        //afxUnit cnt = dout->refreshRate;
        //while (--cnt) DwmFlush();
        DwmFlush();
        //Sleep(1);
        //AfxYield();
    }
    return err;
}

_ZGL afxError _DpuPresentDout_BlitSurfToGdi(zglDpu* dpu, afxDrawOutput dout, afxUnit outBufIdx)
{
    afxError err = AFX_ERR_NONE;

    //if (_ZglActivateDout(dpu, dout))
        //AfxThrowError();

    AFX_ASSERT(dout->m.presentingBufIdx == (afxAtom32)AFX_INVALID_INDEX);
    dout->m.presentingBufIdx = outBufIdx;

    avxRaster surf;
    AvxGetDrawOutputBuffer(dout, outBufIdx, &surf);
    AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &surf);
    //AFX_ASSERT(surf->state == AFX_SURF_STATE_PENDING);
    glVmt const* gl = dpu->gl;

#if 0

    afxDrawSystem dsys = dpu->activeDsys;
    AFX_ASSERT_OBJECTS(afxFcc_DSYS, 1, &dsys);

    afxWhd extent;
    AvxGetRasterExtent(surf, 0, extent);

    _zglCmdBeginSynthesis cmdBeginOp = { 0 };
    cmdBeginOp.defFbo = TRUE;
    cmdBeginOp.canv = NIL;
    afxRect area = { 0 };
    area.w = extent[0];
    area.h = extent[1];
    cmdBeginOp.rasterCnt = 1;
    cmdBeginOp.rasters[0] = (avxDrawTarget const) { .loadOp = avxLoadOp_CLEAR, .storeOp = avxStoreOp_STORE, .clearValue = { .color = { 0.3, 0.1, 0.3, 1 } } };
    DpuCommenceDrawScope(dpu, NIL, &area, 1, 1, cmdBeginOp.rasters, NIL, NIL, TRUE);
    _DpuBindRasterizer(dpu, dsys->presentRazr, NIL, NIL);
    avxViewport vp = { 0 };
    vp.extent[0] = area.w;
    vp.extent[1] = area.h;
    vp.depth[1] = 1.f;
    DpuSetViewports(dpu, 0, 1, &vp);
    DpuBindRasters(dpu, 0, 0, 1, &surf, (afxUnit[]) { 0 });
    DpuBindSamplers(dpu, 0, 0, 1, &dsys->presentSmp);
    avxDrawIndirect cmdDraw = { 0 };
    cmdDraw.vtxCnt = 4;
    cmdDraw.instCnt = 1;
    DpuDraw(dpu, &cmdDraw);
    DpuConcludeDrawScope(dpu);
#else
    dpu->activeTmpFboIdx = (dpu->activeTmpFboIdx + 1) % (sizeof(dpu->tmpFbo) / sizeof(dpu->tmpFbo[0]));
    GLuint tmpFbo = dpu->tmpFbo[dpu->activeTmpFboIdx];

    if (!tmpFbo)
    {
        gl->GenFramebuffers(1, &(tmpFbo)); _ZglThrowErrorOccuried();
    }
    else
    {
        gl->DeleteFramebuffers(1, &(tmpFbo)); _ZglThrowErrorOccuried();
        gl->GenFramebuffers(1, &(tmpFbo)); _ZglThrowErrorOccuried();
    }
    dpu->activeRs.canv = NIL;
    gl->BindFramebuffer(GL_READ_FRAMEBUFFER, tmpFbo); _ZglThrowErrorOccuried();
    AFX_ASSERT(gl->IsFramebuffer(tmpFbo));
    _ZglBindFboAttachment(gl, GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, surf->glTarget, surf->glHandle, 0, 0, 0);

    gl->BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); _ZglThrowErrorOccuried();
    gl->Clear(GL_COLOR_BUFFER_BIT);  _ZglThrowErrorOccuried();

    afxWhd whd;
    AvxQueryDrawOutputExtent(dout, &whd, NIL);

    afxInt x = (dout->m.presentTransform & avxVideoTransform_FLIP_H) ? whd.x : 0;
    afxInt y = (dout->m.presentTransform & avxVideoTransform_FLIP_V) ? whd.y : 0;
    afxInt w = (dout->m.presentTransform & avxVideoTransform_FLIP_H) ? 0 : whd.w;
    afxInt h = (dout->m.presentTransform & avxVideoTransform_FLIP_V) ? 0 : whd.h;

    gl->BlitFramebuffer(0, 0, whd.w, whd.h, x, y, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST); _ZglThrowErrorOccuried();
    gl->BindFramebuffer(GL_READ_FRAMEBUFFER, 0); _ZglThrowErrorOccuried();
    gl->Flush(); _ZglThrowErrorOccuried();
#endif

    HDC dc = dout->hDC;
    AFX_ASSERT(dc);
    wglSwapBuffersSIG(dc, dout->hWnd); // deadlocks all

    //if (dout->exclusive)
    {
        //afxUnit cnt = dout->refreshRate;
        //while (--cnt) DwmFlush();
        DwmFlush();
        //Sleep(1);
        //AfxYield();
    }
    return err;
}

#endif

#if 0
_ZGL afxError _DpuPresentDout(zglDpu* dpu, afxDrawOutput dout, afxUnit outBufIdx)
{
    afxError err = AFX_ERR_NONE;

    afxDrawSystem dsys = dpu->activeDsys;
    AFX_ASSERT_OBJECTS(afxFcc_DSYS, 1, &dsys);

    AFX_ASSERT(dout->m.presentingBufIdx == (afxAtom32)AFX_INVALID_INDEX);

    //dout->presentingBufIdx = outBufIdx;

    _DpuPresentDout_BlitCanvToGdi(dpu, dout, outBufIdx);
    //_DpuPresentDout_BlitSurfToGdi(dpu, dout, outBufIdx);

    afxClock currClock;
    AfxGetClock(&currClock);
    ++dout->m.outNo;

    if ((1.0 <= AfxGetSecondsElapsed(&dout->m.outCntResetClock, &currClock)))
    {
        dout->m.outCntResetClock = currClock;
        dout->m.outRate = dout->m.outNo; // 681 no showing (presenting from overlay thread (acquirer)), 818 frozen (present from draw thread (worker))
        dout->m.outNo = 0;

        afxReal64 ct = AfxGetSecondsElapsed(&dout->m.startClock, &currClock);
        afxReal64 dt = AfxGetSecondsElapsed(&dout->m.lastClock, &currClock);
        dout->m.lastClock = currClock;

        if (AfxTestObjectFcc(dout->m.endpointNotifyObj, afxFcc_WND))
        {
            AfxFormatWindowTitle(dout->m.endpointNotifyObj, "%0f, %u --- Qwadro Video Graphics Infrastructure --- Qwadro Execution Ecosystem (c) 2017 SIGMA --- Public Test Build", dt, dout->m.outRate);
        }

        if (dout->m.endpointNotifyFn)
        {
            dout->m.endpointNotifyFn(dout->m.endpointNotifyObj, outBufIdx);
        }
    }

    dout->m.presentingBufIdx = (afxAtom32)AFX_INVALID_INDEX;
    AfxPushInterlockedQueue(&dout->m.freeBuffers, (afxUnit[]){ outBufIdx });
    AfxDecAtom32(&dout->m.submCnt);
    
    return err;
}
#endif

_ZGL afxError _ZglRelinkDoutCb(afxDrawOutput dout)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DOUT, 1, &dout);
    
    afxDrawSystem dsys = AvxGetDrawOutputContext(dout);
    AFX_ASSERT_OBJECTS(afxFcc_DSYS, 1, &dsys);
    
    afxDrawBridge dexu;
    AvxGetDrawBridges(dsys, 0, 1, &dexu);
    afxDrawDevice ddev = AvxGetBridgedDrawDevice(dexu, NIL);
    AFX_ASSERT_OBJECTS(afxFcc_DDEV, 1, &ddev);

    glVmt const* gl = dout->gl;

    if (dout->hSwapRC)
    {
        afxDrawSystem currDsys = AvxGetDrawOutputContext(dout);

        HDC bkpHdc = wglGetCurrentDCGDI();
        HGLRC bkpGlrc = wglGetCurrentContextGDI();

        if (!wglMakeCurrentGDI(dout->hDC, dout->hSwapRC))
            AfxThrowError();

        if (dout->gdiGlSwap)
        {
            for (afxUnit i = 0; i < dout->m.bufCnt; i++)
            {
                if (dout->gdiGlSwap[i].swapFbo)
                {
                    AFX_ASSERT(gl->IsFramebuffer(dout->gdiGlSwap[i].swapFbo));
                    gl->DeleteFramebuffers(1, &dout->gdiGlSwap[i].swapFbo); _ZglThrowErrorOccuried();
                    dout->gdiGlSwap[i].swapFbo = NIL;
                    dout->gdiGlSwap[i].swapFboReady = FALSE;
                }
            }
        }

        wglMakeCurrentGDI(bkpHdc, bkpGlrc);

        wglDeleteContextGDI(dout->hSwapRC);
        dout->hSwapRC = NIL;
    }

    if (dout->hDC)
    {
        ReleaseDC(dout->hWnd, dout->hDC);
        dout->hDC = NIL;
    }

    dout->hDC = GetDC(dout->hWnd);
            
    if (!dout->dcPixFmt)
    {
        avxFormatDescription pfd;
        AvxDescribeFormats(1, &dout->m.bufFmt[0], &pfd);

        int pxlAttrPairCnt = 0;
        int pxlAttrPairs[][2] =
        {
            { WGL_SUPPORT_OPENGL_ARB, GL_TRUE }, // suportar o que se não OpenGL na fucking API do OpenGL???
            { WGL_DRAW_TO_WINDOW_ARB, GL_TRUE },
#if 0
            { WGL_SUPPORT_GDI_ARB, GL_FALSE },
            { WGL_DRAW_TO_PBUFFER_ARB, GL_FALSE },
            { WGL_DRAW_TO_BITMAP_ARB, GL_FALSE },
            { WGL_BIND_TO_TEXTURE_RGB_ARB, GL_FALSE },
            { WGL_BIND_TO_TEXTURE_RGBA_ARB, GL_FALSE },
#endif
            { WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB },
            { WGL_DOUBLE_BUFFER_ARB, GL_TRUE }, // single buffered is not supported by drivers today. Qwadro will just virtualizes it.
            { WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB },

            /*
                WGL_SWAP_METHOD_ARB
                If the pixel format supports a back buffer, then this indicates
                how they are swapped. If this attribute is set to
                WGL_SWAP_EXCHANGE_ARB then swapping exchanges the front and back
                buffer contents; if it is set to WGL_SWAP_COPY_ARB then swapping
                copies the back buffer contents to the front buffer; if it is
                set to WGL_SWAP_UNDEFINED_ARB then the back buffer contents are
                copied to the front buffer but the back buffer contents are
                undefined after the operation. If the pixel format does not
                support a back buffer then this parameter is set to
                WGL_SWAP_UNDEFINED_ARB. The <iLayerPlane> parameter is ignored
                if this attribute is specified.
            */
            { WGL_SWAP_METHOD_ARB, WGL_SWAP_EXCHANGE_ARB },

            /*
                WGL_COLOR_BITS_ARB
                The number of color bitplanes in each color buffer. For RGBA pixel types,
                it is the size of the color buffer, "excluding" the alpha bitplanes.
                For color-index pixels, it is the size of the color index buffer.
            */

            // ARGB8
            { WGL_COLOR_BITS_ARB, pfd.bpp - pfd.bpc[3] }, // WGL_COLOR_BITS_ARB must not include alpha bits.
            { WGL_ALPHA_BITS_ARB, pfd.bpc[3] },
#if !0
            { WGL_RED_BITS_ARB, pfd.bpc[0] },
            { WGL_GREEN_BITS_ARB, pfd.bpc[1] },
            { WGL_BLUE_BITS_ARB, pfd.bpc[2] },
#endif
#if !0
            { WGL_ALPHA_SHIFT_ARB, pfd.swizzle[3] },
            { WGL_RED_SHIFT_ARB, pfd.swizzle[0] },
            { WGL_GREEN_SHIFT_ARB, pfd.swizzle[1] },
            { WGL_BLUE_SHIFT_ARB, pfd.swizzle[2] },
#endif
            { WGL_DEPTH_BITS_ARB, 0 }, // No Qwadro, não é possível desenhar arbitrariamente no default framebuffer. Logo, não há necessidade de stencil.
            { WGL_STENCIL_BITS_ARB, 0 },  // No Qwadro, não é possível desenhar arbitrariamente no default framebuffer. Logo, não há necessidade de stencil.

#if 0
            { WGL_AUX_BUFFERS_ARB, dout->m.bufCnt },
            { WGL_ACCUM_BITS_ARB, 0 },
            { WGL_ACCUM_ALPHA_BITS_ARB, 0 },
            { WGL_ACCUM_RED_BITS_ARB, 0 },
            { WGL_ACCUM_GREEN_BITS_ARB, 0 },
            { WGL_ACCUM_BLUE_BITS_ARB, 0 },
#endif
            { WGL_TRANSPARENT_ARB, (dout->m.presentAlpha && (dout->m.presentAlpha != avxVideoAlpha_OPAQUE)) },
            //{ WGL_SAMPLE_BUFFERS_ARB,  GL_FALSE },  // works on Intel, didn't work on Mesa
            //{ WGL_SAMPLES_ARB, 0 }, // works on Intel, didn't work on Mesa
            //{ WGL_COLORSPACE_EXT, dout->colorSpc == avxColorSpace_STANDARD ? WGL_COLORSPACE_SRGB_EXT : (dout->colorSpc == avxColorSpace_LINEAR ? WGL_COLORSPACE_LINEAR_EXT : NIL) }, // works on Mesa, didn't work on Intel
            { WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, (pfd.flags & avxFormatFlag_sRGB) }, // works on Mesa, didn't work on Intel
#if 0
            { WGL_NUMBER_OVERLAYS_ARB, 0 },
            { WGL_NUMBER_UNDERLAYS_ARB, 0 },

            { WGL_SHARE_DEPTH_ARB, FALSE },
            { WGL_SHARE_STENCIL_ARB, FALSE },
            { WGL_SHARE_ACCUM_ARB, FALSE },

            { WGL_STEREO_ARB, FALSE },
#endif
            { NIL, NIL },
        };

        UINT fmtCnt;
                
        //if (!wglChooseBestPixelFormatSIG(dout->hDC, &pxlAttrPairs[0][0], 0, 1, &dout->dcPixFmt, &fmtCnt)) AfxThrowError();
        if (!wglChoosePixelFormatARB(dout->hDC, &pxlAttrPairs[0][0], 0, 1, &dout->dcPixFmt, &fmtCnt)) AfxThrowError();
        else
        {
            AFX_ASSERT(dout->dcPixFmt);
            AFX_ASSERT(fmtCnt);

#if 0
            pfd.nSize = sizeof(pfd);
            pfd.nVersion = 1;
            pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DIRECT3D_ACCELERATED | PFD_SUPPORT_COMPOSITION;
            pfd.iPixelType = PFD_TYPE_RGBA;
            pfd.cColorBits = 24;
            pfd.cAlphaBits = 8;
            pfd.cDepthBits = 24;
            pfd.cStencilBits = 8;
#endif
            //dout->wglDcPxlFmt = _ZglChoosePixelFormat(dout->wglDc, &(pfd));
            DWORD pfdFlags = PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL
#if 0
                | PFD_SWAP_EXCHANGE
#endif
#if !0
                | PFD_DIRECT3D_ACCELERATED
#endif
#if 0
                | PFD_SUPPORT_COMPOSITION
#endif
                ;

            if (!wglDescribePixelFormatGDI(dout->hDC, dout->dcPixFmt, sizeof(dout->dcPfd), &dout->dcPfd))
            {
                if (!DescribePixelFormat(dout->hDC, dout->dcPixFmt, sizeof(dout->dcPfd), &dout->dcPfd))
                    AfxThrowError();
                else if (!SetPixelFormat(dout->hDC, dout->dcPixFmt, &dout->dcPfd))
                    AfxThrowError();
            }
            else
            {
                if (!wglSetPixelFormatGDI(dout->hDC, dout->dcPixFmt, &dout->dcPfd))
                    if (!SetPixelFormat(dout->hDC, dout->dcPixFmt, &dout->dcPfd))
                        AfxThrowError();
            }

            AFX_ASSERT(dout->dcPfd.dwFlags & pfdFlags);
        }
    }

    avxRange const screenRes =
    {
        GetDeviceCaps(dout->hDC, HORZRES),
        GetDeviceCaps(dout->hDC, VERTRES),
        GetDeviceCaps(dout->hDC, PLANES)
    };
    afxReal64 physAspRatio = (afxReal64)GetDeviceCaps(dout->hDC, HORZSIZE) / (afxReal64)GetDeviceCaps(dout->hDC, VERTSIZE);
    afxReal refreshRate = GetDeviceCaps(dout->hDC, VREFRESH);
    AvxModifyDrawOutputSettings(dout, physAspRatio, refreshRate, screenRes, FALSE);

    HDC bkpHdc = wglGetCurrentDCGDI();
    HGLRC bkpGlrc = wglGetCurrentContextGDI();

    if (_ZglCreateHwContext(dout->hDC, /*dsys->m.exus[0]->hRC*/dsys->hPrimeRC, dsys->glVerMaj, dsys->glVerMin, dsys->robustCtx, &dout->hSwapRC, NIL, FALSE)) AfxThrowError();
    else
    {
        dout->gl = &dsys->gl;
        gl = dout->gl;

        if (!wglMakeCurrentGDI(dout->hDC, dout->hSwapRC)) AfxThrowError();
        else
        {
#if _AFX_DEBUG
            gl->Enable(GL_DEBUG_OUTPUT);
            gl->Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            gl->DebugMessageCallback(_glDbgMsgCb, NIL);
            //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

            if ((dout->m.presentMode & avxPresentFlag_NO_TEARING) == avxPresentFlag_NO_TEARING)
                wglSwapIntervalEXT(1); // when greater than 0 can causes lag
            else
                wglSwapIntervalEXT(0);
        }

        if (err)
            wglDeleteContextGDI(dout->hSwapRC), dout->hSwapRC = NIL;
    }
    wglMakeCurrentGDI(bkpHdc, bkpGlrc);
    return err;
}

_ZGL afxError _ZglDoutAdjust(afxDrawOutput dout, avxRange const whd)
{
    afxError err = AFX_ERR_NONE;
    _AvxDoutImplAdjustCb(dout, whd);
    //createSwapchainDxgi(dout);

    if (dout->gdiGlSwap)
        for (afxUnit i = 0; i < dout->m.bufCnt; i++)
            dout->gdiGlSwap[i].swapFboReady = FALSE;

    return err;
}

_ZGL afxError _ZglDoutIoctl(afxDrawOutput dout, afxUnit code, va_list ap)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DOUT, 1, &dout);

    switch (code)
    {
    case 0:
    {
        *va_arg(ap, HDC*) = dout->hDC;
        break;
    }
    case 1:
    {
        *va_arg(ap, int*) = dout->dcPixFmt;
        break;
    }
    default: break;
    }
    return err;
}

_ZGL _avxDoutDdi const _ZGL_DOUT_DDI =
{
    .ioctl = _ZglDoutIoctl,
    .adjust = _ZglDoutAdjust,
    .reqBuf = NIL,
    .recycBuf = NIL
};

_ZGL afxError _ZglDoutCtorCb(afxDrawOutput dout, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DOUT, 1, &dout);

    afxDrawSystem dsys = args[0];
    AFX_ASSERT_OBJECTS(afxFcc_DSYS, 1, &dsys);
    afxDrawOutputConfig const* cfg = ((afxDrawOutputConfig const *)args[1]) + invokeNo;
    AFX_ASSERT(cfg);

    if (_AVX_DOUT_CLASS_CONFIG.ctor(dout, (void*[]) { dsys, (void*)cfg }, 0))
    {
        AfxThrowError();
        return err;
    }

    dout->m.pimpl = &_ZGL_DOUT_DDI;

    dout->m.lockCb = NIL;
    dout->m.presentCb = _ZglDoutPresentGdi;

    dout->hInst = cfg->w32.hInst;
    dout->hWnd = cfg->w32.hWnd;
    dout->hDC = NIL;
    dout->dcPixFmt = 0;
    dout->isWpp = FALSE;
    dout->hSwapRC = NIL;

    dout->gdiGlSwap = NIL;
    AfxAllocate(dout->m.bufCnt * sizeof(dout->gdiGlSwap[0]), 0, AfxHere(), (void**)&dout->gdiGlSwap);

    for (afxUnit i = 0; i < dout->m.bufCnt; i++)
    {
        dout->gdiGlSwap[i].swapFbo = NIL;
        dout->gdiGlSwap[i].swapFboReady = FALSE;
    }

    if (_ZglRelinkDoutCb(dout))
        AfxThrowError();

    HWND hWnd = cfg->w32.hWnd;

    if (hWnd)
    {

        // fetch capabilities of the visual display unit.
#if 0
        HDC dc = GetWindowDC(hWnd);

        if (!dc) AfxThrowError();
        else
        {
            afxWhd const screenRes =
            {
                GetDeviceCaps(dc, HORZRES),
                GetDeviceCaps(dc, VERTRES),
                GetDeviceCaps(dc, PLANES)
            };
            afxReal64 physAspRatio = (afxReal64)GetDeviceCaps(dc, HORZSIZE) / (afxReal64)GetDeviceCaps(dc, VERTSIZE);
            afxReal refreshRate = GetDeviceCaps(dc, VREFRESH);
            AvxModifyDrawOutputSettings(dout, physAspRatio, refreshRate, screenRes, FALSE);
        }
        ReleaseDC(hWnd, dc);
#endif

#if 0
        afxDrawDevice ddev = AfxGetDrawOutputDevice(dout);

        if (ddev->useDxgiSwapchain)
        {
            if (ddev->hasDxInterop1)
            {
                HRESULT hr;
                AFX_ASSERT(hr);
                hr = ddev->Direct3DCreate9Ex(D3D_SDK_VERSION, &dout->d3dsw9.d3d9Ex);

                if (FAILED(hr))
                {
                    AfxThrowError();
                    //return -1;
                }

                D3DPRESENT_PARAMETERS d3dpp =
                {
                    .Windowed = TRUE,
                    .BackBufferWidth = dout->m.extent.w,
                    .BackBufferHeight = dout->m.extent.h,
                    // Add one frame for the backbuffer and one frame of "slack" to reduce contention with the window manager when acquiring the backbuffer
                    .BackBufferCount = dout->m.bufCnt,
                    .SwapEffect = 1 ? D3DSWAPEFFECT_FLIPEX : D3DSWAPEFFECT_FLIP,
                    // Automatically get the backbuffer format from the display format
                    .BackBufferFormat = D3DFMT_X8R8G8B8,
                    .PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE,
                    .hDeviceWindow = dout->hWnd,
                };

                hr = IDirect3D9Ex_CreateDeviceEx(dout->d3dsw9.d3d9Ex, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, dout->hWnd,
                    D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE | D3DCREATE_MULTITHREADED | D3DCREATE_NOWINDOWCHANGES,
                    &d3dpp, NULL, &dout->d3dsw9.d3d9DevEx);

                if (FAILED(hr))
                {
                    AfxThrowError();
                    //return -1;
                }

                hr = IDirect3DDevice9Ex_CreateAdditionalSwapChain(dout->d3dsw9.d3d9DevEx, &d3dpp, &dout->d3dsw9.d3d9Sw);

                if (FAILED(hr))
                {
                    AfxThrowError();
                }

                IDirect3DSurface9* backBuf;
                IDirect3DDevice9Ex_GetBackBuffer(dout->d3dsw9.d3d9DevEx, 0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuf);

                IDirect3DTexture9* sharedTexture;
                hr = IDirect3DDevice9Ex_CreateTexture(dout->d3dsw9.d3d9DevEx, 800, 600, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &sharedTexture, NULL);

                if (FAILED(hr))
                {
                    AfxThrowError();
                }

                IDirect3DSurface9* pSurface = NULL;
                hr = IDirect3DTexture9_GetSurfaceLevel(sharedTexture, 0, &pSurface);

                if (FAILED(hr))
                {
                    AfxThrowError();
                }
            }
        }
#endif
    }

    if (err && _AVX_DOUT_CLASS_CONFIG.dtor(dout))
        AfxThrowError();

    return err;
}

_ZGL afxError _ZglDoutDtorCb(afxDrawOutput dout)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DOUT, 1, &dout);

    _ZglDoutSuspendFunction(dout);

    if (dout->m.idd)
    {
        AfxDeallocate((void**)&dout->m.idd, AfxHere());
        dout->m.idd = NIL;
    }

    if (dout->gdiGlSwap)
        AfxDeallocate((void**)&dout->gdiGlSwap, AfxHere());

    AFX_ASSERT(!dout->hSwapRC); // disconnect should have released it.

    if (dout->hSwapRC)
        wglDeleteContextGDI(dout->hSwapRC);
    
    AFX_ASSERT(!dout->hDC); // disconnect should have released it.

    if (dout->hDC)
        ReleaseDC(dout->hWnd, dout->hDC);

    if (_AVX_DOUT_CLASS_CONFIG.dtor(dout))
        AfxThrowError();

    return err;
}
