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


#include "zglCommands.h"
#include "zglObjects.h"

_ZGL afxNat _SglDoutIsSuspended(afxDrawOutput dout)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dout, afxFcc_DOUT);
    AfxEnterSlockShared(&dout->m.suspendSlock);
    afxNat suspendCnt = dout->m.suspendCnt;
    AfxExitSlockShared(&dout->m.suspendSlock);
    return suspendCnt;
}

_ZGL afxNat _SglDoutSuspendFunction(afxDrawOutput dout)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dout, afxFcc_DOUT);
    AfxEnterSlockExclusive(&dout->m.suspendSlock);
    afxNat suspendCnt = ++dout->m.suspendCnt;
    AfxExitSlockExclusive(&dout->m.suspendSlock);
    return suspendCnt;
}

_ZGL afxNat _SglDoutResumeFunction(afxDrawOutput dout)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dout, afxFcc_DOUT);
    AfxEnterSlockExclusive(&dout->m.suspendSlock);
    afxNat suspendCnt = --dout->m.suspendCnt;
    AfxExitSlockExclusive(&dout->m.suspendSlock);
    return suspendCnt;
}

_ZGL afxError _SglDoutPresentGdi(afxDrawOutput dout, afxNat bufIdx, afxSize unk)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dout, afxFcc_DOUT);

    AfxAssert(dout->m.presentingBufIdx == bufIdx); // must be called by _AvxPresentDrawOutput() to handle sync.

    HDC bkpHdc = wglGetCurrentDrawDCSIG();
    HGLRC bkpGlrc = wglGetCurrentContextSIG();

    if (!wglMakeContextCurrentSIG(dout->hDC, NIL, dout->hSwapRC)) AfxThrowError();
    else
    {
        afxDrawContext dctx;
        AfxGetDrawOutputContext(dout, &dctx);
        glVmt const* gl = &dctx->gl;

        afxWhd whd;
        afxRaster buf;
        avxCanvas canv;
        AfxGetDrawOutputBuffers(dout, bufIdx, 1, &buf);
        AfxGetDrawOutputCanvases(dout, bufIdx, 1, &canv);
        AfxAssertObjects(1, &canv, afxFcc_CANV);
        AfxAssertObjects(1, &buf, afxFcc_RAS);
        AfxGetCanvasExtent(canv, whd);

        gl->BindFramebuffer(GL_READ_FRAMEBUFFER, dout->swapFbo); _SglThrowErrorOccuried();
        AfxAssert(gl->IsFramebuffer(dout->swapFbo));
        _SglBindFboAttachment(gl, GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, buf->glTarget, buf->glHandle, 0, 0, 0);
        gl->BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); _SglThrowErrorOccuried();
        //gl->Clear(GL_COLOR_BUFFER_BIT);  _SglThrowErrorOccuried();
        gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 1, (GLenum[]) { GL_BACK_LEFT }); _SglThrowErrorOccuried();

        afxInt x = (dout->m.presentTransform & avxPresentTransform_FLIP_H) ? whd[0] : 0;
        afxInt y = (dout->m.presentTransform & avxPresentTransform_FLIP_V) ? whd[1] : 0;
        afxInt w = (dout->m.presentTransform & avxPresentTransform_FLIP_H) ? 0 : whd[0];
        afxInt h = (dout->m.presentTransform & avxPresentTransform_FLIP_V) ? 0 : whd[1];

        gl->BlitFramebuffer(0, 0, whd[0], whd[1], x, y, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST); _SglThrowErrorOccuried();
        gl->BindFramebuffer(GL_READ_FRAMEBUFFER, 0); _SglThrowErrorOccuried();
        //gl->Flush(); _SglThrowErrorOccuried();

        wglSwapBuffersSIG(dout->hDC, NIL); // deadlocks all
        //SwapBuffers(dout->hDC);
        //DwmFlush();

        wglMakeContextCurrentSIG(bkpHdc, bkpHdc, bkpGlrc);
    }

    return err;
}

_ZGL afxBool _SglDouVmtGetIddCb(afxDrawOutput dout, afxNat code, void* dst)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dout, afxFcc_DOUT);

    switch (code)
    {
    case 0:
    {
        *(HDC*)dst = dout->hDC;
        break;
    }
    case 1:
    {
        *(int*)dst = dout->dcPixFmt;
        break;
    }
    default: break;
    }
    return FALSE;
}

_ZGL afxError _DpuPresentDout_BlitCanvToGdi(sglDpu* dpu, afxDrawOutput dout, afxNat outBufIdx)
{
    afxError err = AFX_ERR_NONE;

    //if (_SglActivateDout(dpu, dout))
        //AfxThrowError();

    AfxAssert(dout->m.presentingBufIdx == (afxAtom32)AFX_INVALID_INDEX);
    dout->m.presentingBufIdx = outBufIdx;

    avxCanvas canv;
    AfxGetDrawOutputCanvases(dout, outBufIdx, 1, &canv);
    AfxAssertObjects(1, &canv, afxFcc_CANV);
    //AfxAssert(surf->state == AFX_SURF_STATE_PENDING);
    glVmt const* gl = &dpu->gl;

    afxWhd whd;
    AfxGetDrawOutputExtent(dout, whd);
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
    dpu->activeRasterState.pass.canv = NIL;
    gl->BindFramebuffer(GL_READ_FRAMEBUFFER, canv->glHandle); _SglThrowErrorOccuried();
    gl->BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); _SglThrowErrorOccuried();
    //gl->Clear(GL_COLOR_BUFFER_BIT);  _SglThrowErrorOccuried();

    afxInt x = (dout->m.presentTransform & avxPresentTransform_FLIP_H) ? whd[0] : 0;
    afxInt y = (dout->m.presentTransform & avxPresentTransform_FLIP_V) ? whd[1] : 0;
    afxInt w = (dout->m.presentTransform & avxPresentTransform_FLIP_H) ? 0 : whd[0];
    afxInt h = (dout->m.presentTransform & avxPresentTransform_FLIP_V) ? 0 : whd[1];

    gl->BlitFramebuffer(0, 0, whd[0], whd[1], x, y, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST); _SglThrowErrorOccuried();
    gl->BindFramebuffer(GL_READ_FRAMEBUFFER, 0); _SglThrowErrorOccuried();
    gl->Flush(); _SglThrowErrorOccuried(); // flush is presenting/swapping

    HDC dc = dout->hDC;
    AfxAssert(dc);

    //BitBlt(dc, 0, 0, whd[0], whd[1], dc2, 0, 0, SRCCOPY);
    wglSwapBuffersSIG(dc, NIL); // deadlocks all
    //SwapBuffers(dc);
    //wglPresentBuffersSIG(dc, (int)dpu->glrc);
    //cur = wglMakeContextCurrentSIG(dpu->dc, dpu->dc, dpu->glrc);
    //DeleteObject(bmp);
    //DeleteDC(dc2);

    //if (dout->exclusive)
    {
        //afxNat cnt = dout->refreshRate;
        //while (--cnt) DwmFlush();
        DwmFlush();
        //Sleep(1);
        //AfxYield();
    }
    return err;
}

_ZGL afxError _DpuPresentDout_BlitSurfToGdi(sglDpu* dpu, afxDrawOutput dout, afxNat outBufIdx)
{
    afxError err = AFX_ERR_NONE;

    //if (_SglActivateDout(dpu, dout))
        //AfxThrowError();

    AfxAssert(dout->m.presentingBufIdx == (afxAtom32)AFX_INVALID_INDEX);
    dout->m.presentingBufIdx = outBufIdx;

    afxRaster surf;
    AfxGetDrawOutputBuffers(dout, outBufIdx, 1, &surf);
    AfxAssertObjects(1, &surf, afxFcc_RAS);
    //AfxAssert(surf->state == AFX_SURF_STATE_PENDING);
    glVmt const* gl = &dpu->gl;

#if 0

    afxDrawContext dctx = dpu->activeDctx;
    AfxAssertObjects(1, &dctx, afxFcc_DCTX);

    afxWhd extent;
    AfxGetRasterExtent(surf, 0, extent);

    _sglCmdBeginSynthesis cmdBeginOp = { 0 };
    cmdBeginOp.defFbo = TRUE;
    cmdBeginOp.canv = NIL;
    afxRect area = { 0 };
    area.w = extent[0];
    area.h = extent[1];
    cmdBeginOp.rasterCnt = 1;
    cmdBeginOp.rasters[0] = (avxDrawTarget const) { .loadOp = avxLoadOp_CLEAR, .storeOp = avxStoreOp_STORE, .clearValue = { .color = { 0.3, 0.1, 0.3, 1 } } };
    _DpuBeginDrawScope(dpu, NIL, &area, 1, 1, cmdBeginOp.rasters, NIL, NIL, TRUE);
    _DpuBindRasterizer(dpu, dctx->presentRazr, NIL, NIL);
    afxViewport vp = { 0 };
    vp.extent[0] = area.w;
    vp.extent[1] = area.h;
    vp.depth[1] = 1.f;
    _DpuSetViewports(dpu, 0, 1, &vp);
    _DpuBindRasters(dpu, 0, 0, 1, &surf, (afxNat[]) { 0 });
    _DpuBindSamplers(dpu, 0, 0, 1, &dctx->presentSmp);
    avxDrawIndirectCmd cmdDraw = { 0 };
    cmdDraw.vtxCnt = 4;
    cmdDraw.instCnt = 1;
    _DpuDraw(dpu, &cmdDraw);
    _DpuEndDrawScope(dpu);
#else
    dpu->activeTmpFboIdx = (dpu->activeTmpFboIdx + 1) % (sizeof(dpu->tmpFbo) / sizeof(dpu->tmpFbo[0]));
    GLuint tmpFbo = dpu->tmpFbo[dpu->activeTmpFboIdx];

    if (!tmpFbo)
    {
        gl->GenFramebuffers(1, &(tmpFbo)); _SglThrowErrorOccuried();
    }
    else
    {
        gl->DeleteFramebuffers(1, &(tmpFbo)); _SglThrowErrorOccuried();
        gl->GenFramebuffers(1, &(tmpFbo)); _SglThrowErrorOccuried();
    }
    dpu->activeRasterState.pass.canv = NIL;
    gl->BindFramebuffer(GL_READ_FRAMEBUFFER, tmpFbo); _SglThrowErrorOccuried();
    AfxAssert(gl->IsFramebuffer(tmpFbo));
    _SglBindFboAttachment(gl, GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, surf->glTarget, surf->glHandle, 0, 0, 0);

    gl->BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); _SglThrowErrorOccuried();
    gl->Clear(GL_COLOR_BUFFER_BIT);  _SglThrowErrorOccuried();

    afxWhd whd;
    AfxGetDrawOutputExtent(dout, whd);

    afxInt x = (dout->m.presentTransform & avxPresentTransform_FLIP_H) ? whd[0] : 0;
    afxInt y = (dout->m.presentTransform & avxPresentTransform_FLIP_V) ? whd[1] : 0;
    afxInt w = (dout->m.presentTransform & avxPresentTransform_FLIP_H) ? 0 : whd[0];
    afxInt h = (dout->m.presentTransform & avxPresentTransform_FLIP_V) ? 0 : whd[1];

    gl->BlitFramebuffer(0, 0, whd[0], whd[1], x, y, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST); _SglThrowErrorOccuried();
    gl->BindFramebuffer(GL_READ_FRAMEBUFFER, 0); _SglThrowErrorOccuried();
    gl->Flush(); _SglThrowErrorOccuried();
#endif

    HDC dc = dout->hDC;
    AfxAssert(dc);
    wglSwapBuffersSIG(dc, dout->hWnd); // deadlocks all

    //if (dout->exclusive)
    {
        //afxNat cnt = dout->refreshRate;
        //while (--cnt) DwmFlush();
        DwmFlush();
        //Sleep(1);
        //AfxYield();
    }
    return err;
}
#if 0
_ZGL afxError _DpuPresentDout(sglDpu* dpu, afxDrawOutput dout, afxNat outBufIdx)
{
    afxError err = AFX_ERR_NONE;

    afxDrawContext dctx = dpu->activeDctx;
    AfxAssertObjects(1, &dctx, afxFcc_DCTX);

    AfxAssert(dout->m.presentingBufIdx == (afxAtom32)AFX_INVALID_INDEX);

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
            AfxFormatWindowCaption(dout->m.endpointNotifyObj, "%0f, %u --- Qwadro Video Graphics Infrastructure --- Qwadro Execution Ecosystem (c) 2017 SIGMA --- Public Test Build", dt, dout->m.outRate);
        }

        if (dout->m.endpointNotifyFn)
        {
            dout->m.endpointNotifyFn(dout->m.endpointNotifyObj, outBufIdx);
        }
    }

    dout->m.presentingBufIdx = (afxAtom32)AFX_INVALID_INDEX;
    AfxPushInterlockedQueue(&dout->m.freeBuffers, (afxNat[]){ outBufIdx });
    AfxDecAtom32(&dout->m.submCnt);
    
    return err;
}
#endif

_ZGL afxError _SglRelinkDoutCb(afxDrawDevice ddev, afxDrawContext dctx, afxNat cnt, afxDrawOutput douts[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &ddev, afxFcc_DDEV);
    AfxTryAssertObjects(1, &dctx, afxFcc_DCTX);
    AfxAssertObjects(cnt, douts, afxFcc_DOUT);
    
    for (afxNat i = 0; i < cnt; i++)
    {
        afxDrawOutput dout = douts[i];

        if (dout->hSwapRC)
        {
            afxDrawContext currDctx;
            AfxGetDrawOutputContext(dout, &currDctx);
            glVmt const* gl = &currDctx->gl;

            HDC bkpHdc = wglGetCurrentDrawDCSIG();
            HGLRC bkpGlrc = wglGetCurrentContextSIG();
            
            wglMakeContextCurrentSIG(dout->hDC, dout->hDC, dout->hSwapRC);

            gl->DeleteFramebuffers(1, &dout->swapFbo); _SglThrowErrorOccuried();
            dout->swapFbo = 0;

            wglMakeContextCurrentSIG(bkpHdc, bkpHdc, bkpGlrc);

            wglDeleteContextSIG(dout->hSwapRC), dout->hSwapRC = NIL;
        }

        if (dout->hDC)
        {
            ReleaseDC(dout->hWnd, dout->hDC), dout->hDC = NIL;            
        }

        if (dctx)
        {
            dout->hDC = GetDC(dout->hWnd);
            
            if (!dout->dcPixFmt)
            {
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
                    { WGL_DOUBLE_BUFFER_ARB, GL_TRUE }, // single buffer not supported anymore today. Qwadro will just virtualizes it.
                    { WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB },

                    // ARGB8
                    { WGL_COLOR_BITS_ARB, GetDeviceCaps(dout->hDC, BITSPIXEL) },
                    { WGL_ALPHA_BITS_ARB, 8 },
#if 0
                    { WGL_ALPHA_SHIFT_ARB, 24 },
                    { WGL_RED_BITS_ARB, 8 },
                    { WGL_RED_SHIFT_ARB, 16 },
                    { WGL_GREEN_BITS_ARB, 8 },
                    { WGL_GREEN_SHIFT_ARB, 8 },
                    { WGL_BLUE_BITS_ARB, 8 },
                    { WGL_BLUE_SHIFT_ARB, 0 },
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
                    { WGL_TRANSPARENT_ARB, (dout->m.presentAlpha && dout->m.presentAlpha != avxPresentAlpha_OPAQUE) },
                    //{ WGL_SWAP_METHOD_ARB, (dout->m.presentMode && dout->m.presentMode == avxPresentMode_IMMEDIATE) ? WGL_SWAP_COPY_ARB : WGL_SWAP_EXCHANGE_ARB },
                    { WGL_SAMPLE_BUFFERS_ARB,  GL_FALSE },  // works on Intel, didn't work on Mesa
                    { WGL_SAMPLES_ARB, 0 }, // works on Intel, didn't work on Mesa
                    //{ WGL_COLORSPACE_EXT, dout->colorSpc == avxColorSpace_SRGB ? WGL_COLORSPACE_SRGB_EXT : (dout->colorSpc == avxColorSpace_LINEAR ? WGL_COLORSPACE_LINEAR_EXT : NIL) }, // works on Mesa, didn't work on Intel
                    //{ WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, (dout->colorSpc == avxColorSpace_SRGB) }, // works on Mesa, didn't work on Intel
#if 0
                    { WGL_SWAP_METHOD_ARB, WGL_SWAP_EXCHANGE_ARB },

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
                if (!wgl.ChoosePixelFormatARB(dout->hDC, &pxlAttrPairs[0][0], 0, 1, &dout->dcPixFmt, &fmtCnt)) AfxThrowError();
                else
                {
                    AfxAssert(dout->dcPixFmt);
                    AfxAssert(fmtCnt);

                    wglDescribePixelFormatSIG(dout->hDC, dout->dcPixFmt, sizeof(dout->dcPfd), &dout->dcPfd);
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
                    //dout->wglDcPxlFmt = _SglChoosePixelFormat(dout->wglDc, &(pfd));
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
                    AfxAssert(dout->dcPfd.dwFlags & pfdFlags);

                    if (!wglSetPixelFormatSIG(dout->hDC, dout->dcPixFmt, dout->hWnd, &dout->dcPfd))
                        AfxThrowError();
                }
            }

            afxWhd const screenRes =
            {
                GetDeviceCaps(dout->hDC, HORZRES),
                GetDeviceCaps(dout->hDC, VERTRES),
                GetDeviceCaps(dout->hDC, PLANES)
            };
            afxReal64 physAspRatio = (afxReal64)GetDeviceCaps(dout->hDC, HORZSIZE) / (afxReal64)GetDeviceCaps(dout->hDC, VERTSIZE);
            afxReal refreshRate = GetDeviceCaps(dout->hDC, VREFRESH);
            AfxResetDrawOutputResolution(dout, screenRes, refreshRate, physAspRatio);

            HDC bkpHdc = wglGetCurrentDrawDCSIG();
            HGLRC bkpGlrc = wglGetCurrentContextSIG();

            if (_SglCreateHwContext(ddev->idd->oglDll, dout->hDC,  /*dctx->m.exus[0]->hRC*/dctx->hPrimeRC, &dout->hSwapRC, NIL)) AfxThrowError();
            else
            {
                if (!wglMakeContextCurrentSIG(dout->hDC, dout->hDC, dout->hSwapRC)) AfxThrowError();
                else
                {
                    glVmt const* gl = &dctx->gl;

#if _AFX_DEBUG
                    gl->Enable(GL_DEBUG_OUTPUT);
                    gl->Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                    gl->DebugMessageCallback(_glDbgMsgCb, NIL);
                    //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

                    dout->swapFbo = 0;
                    gl->GenFramebuffers(1, &dout->swapFbo); _SglThrowErrorOccuried();
                    gl->BindFramebuffer(GL_FRAMEBUFFER, dout->swapFbo); _SglThrowErrorOccuried();
                    AfxAssert(gl->IsFramebuffer(dout->swapFbo));

                    if (dout->m.presentMode == avxPresentMode_FIFO)
                        wglSwapIntervalSIG(1); // when greater than 0 can causes lag
                    else
                        wglSwapIntervalSIG(0);
                }
            }
            wglMakeContextCurrentSIG(bkpHdc, bkpHdc, bkpGlrc);
        }
    }
    return err;
}

_ZGL afxError _SglAdjustDrawOutput(afxDrawOutput dout, afxWhd const whd)
{
    afxError err = AFX_ERR_NONE;
    _AfxAdjustDrawOutput(dout, whd);
    //createSwapchainDxgi(dout);
    return err;
}

_ZGL afxError _SglDoutCtorCb(afxDrawOutput dout, void** args, afxNat invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dout, afxFcc_DOUT);

    afxDrawDevice ddev = args[0];
    AfxAssertObjects(1, &ddev, afxFcc_DDEV);
    afxDrawOutputConfig const* cfg = ((afxDrawOutputConfig const *)args[1]) + invokeNo;
    AfxAssert(cfg);

    if (_AvxDoutStdImplementation.ctor(dout, (void*[]) { ddev, (void*)cfg }, 0)) AfxThrowError();
    else
    {
        dout->m.lockCb = NIL;
        dout->m.iddCb = _SglDouVmtGetIddCb;
        dout->m.adjustCb = _SglAdjustDrawOutput;
        dout->m.presentCb = _SglDoutPresentGdi;

        dout->hInst = cfg->w32.hInst;
        dout->hWnd = cfg->w32.hWnd;
        dout->hDC = NIL;
        dout->dcPixFmt = 0;
        dout->isWpp = FALSE;
        dout->hSwapRC = NIL;

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
                AfxResetDrawOutputResolution(dout, screenRes, refreshRate, physAspRatio);
            }
            ReleaseDC(hWnd, dc);
#endif

#if 0
            afxDrawDevice ddev = AfxGetDrawOutputDevice(dout);

            if (ddev->idd->useDxgiSwapchain)
            {
                if (ddev->idd->hasDxInterop1)
                {
                    HRESULT hr;
                    AfxAssert(hr);
                    hr = ddev->idd->Direct3DCreate9Ex(D3D_SDK_VERSION, &dout->d3dsw9.d3d9Ex);

                    if (FAILED(hr))
                    {
                        AfxThrowError();
                        //return -1;
                    }

                    D3DPRESENT_PARAMETERS d3dpp =
                    {
                        .Windowed = TRUE,
                        .BackBufferWidth = dout->m.whd[0],
                        .BackBufferHeight = dout->m.whd[1],
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
        if (err && _AvxDoutStdImplementation.dtor(dout))
            AfxThrowError();
    }
    return err;
}

_ZGL afxError _SglDoutDtorCb(afxDrawOutput dout)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dout, afxFcc_DOUT);

    _SglDoutSuspendFunction(dout);

    if (dout->m.idd)
    {
        AfxDeallocate(dout->m.idd);
        dout->m.idd = NIL;
    }

    AfxAssert(!dout->hSwapRC); // disconnect should have released it.

    if (dout->hSwapRC)
        wglDeleteContextSIG(dout->hSwapRC);
    
    AfxAssert(!dout->hDC); // disconnect should have released it.

    if (dout->hDC)
        ReleaseDC(dout->hWnd, dout->hDC);

    if (_AvxDoutStdImplementation.dtor(dout))
        AfxThrowError();

    return err;
}
