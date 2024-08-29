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

#define _CRT_SECURE_NO_WARNINGS 1
#define WIN32_LEAN_AND_MEAN 1
#include <dwmapi.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <assert.h>

#include "zglCommands.h"
#include "zglObjects.h"

#include "qwadro/exec/afxSystem.h"

//#define _AVOID_WGL_FUNCS 1

extern afxClassConfig _SglDexuMgrCfg;

_ZGL LRESULT WINAPI _SglWndHndlngPrcW32Callback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    afxError err = AFX_ERR_NONE;
    //afxObject obj = (afxObject)(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    if (message == WM_SHOWWINDOW)
    {
        // Sent to a window when the window is about to be hidden or shown.
        return 0; // If an application processes this message, it should return zero.
    }
    return DefWindowProcA(hWnd, message, wParam, lParam);
}

_ZGL afxError _SglProcessDeletionQueue(glVmt const* gl, afxInterlockedQueue* deletionQueue)
{
    afxError err = AFX_ERR_NONE;

    _sglDeleteGlRes res;
    while (AfxPopInterlockedQueue(deletionQueue, &res))
    {
        switch (res.type)
        {
        case GL_BUFFER:
            AfxAssert(gl->IsBuffer(res.gpuHandle));
            gl->DeleteBuffers(1, &res.gpuHandle); _SglThrowErrorOccuried();
            AfxAssert(!gl->IsBuffer(res.gpuHandle));
            break;
        case GL_TEXTURE:
            AfxAssert(gl->IsTexture(res.gpuHandle));
            gl->DeleteTextures(1, &res.gpuHandle); _SglThrowErrorOccuried();
            AfxAssert(!gl->IsTexture(res.gpuHandle));
            break;
        case GL_RENDERBUFFER:
            AfxAssert(gl->IsRenderBuffer(res.gpuHandle));
            gl->DeleteRenderbuffers(1, &res.gpuHandle); _SglThrowErrorOccuried();
            AfxAssert(!gl->IsRenderBuffer(res.gpuHandle));
            break;
        case GL_FRAMEBUFFER:
            AfxAssert(gl->IsFramebuffer(res.gpuHandle));
            gl->DeleteFramebuffers(1, &res.gpuHandle); _SglThrowErrorOccuried();
            AfxAssert(!gl->IsFramebuffer(res.gpuHandle));
            break;
        case GL_SAMPLER:
            AfxAssert(gl->IsSampler(res.gpuHandle));
            gl->DeleteSamplers(1, &res.gpuHandle); _SglThrowErrorOccuried();
            AfxAssert(!gl->IsSampler(res.gpuHandle));
            break;
        case GL_PROGRAM:
            AfxAssert(gl->IsProgram(res.gpuHandle));
            gl->DeleteProgram(res.gpuHandle); _SglThrowErrorOccuried();
            AfxAssert(!gl->IsProgram(res.gpuHandle));
            break;
        case GL_SHADER:
            AfxAssert(gl->IsShader(res.gpuHandle));
            gl->DeleteShader(res.gpuHandle); _SglThrowErrorOccuried();
            AfxAssert(!gl->IsShader(res.gpuHandle));
            break;
        case GL_VERTEX_ARRAY:
            AfxAssert(gl->IsVertexArray(res.gpuHandle));
            gl->GenVertexArrays(1, &res.gpuHandle); _SglThrowErrorOccuried();
            AfxAssert(!gl->IsVertexArray(res.gpuHandle));
            break;
        case GL_PROGRAM_PIPELINE:
            AfxAssert(gl->IsProgramPipeline(res.gpuHandle));
            gl->DeleteProgramPipelines(1, &res.gpuHandle); _SglThrowErrorOccuried();
            AfxAssert(!gl->IsProgramPipeline(res.gpuHandle));
            break;
        case GL_SYNC_FENCE:
            AfxAssert(gl->IsSync(res.gpuHandlePtr));
            gl->DeleteSync(res.gpuHandlePtr); _SglThrowErrorOccuried();
            AfxAssert(!gl->IsSync(res.gpuHandlePtr));
            break;
        case GL_QUERY_BUFFER:
            AfxAssert(gl->IsQuery(res.gpuHandle));
            gl->DeleteQueries(1, &res.gpuHandle); _SglThrowErrorOccuried();
            AfxAssert(!gl->IsQuery(res.gpuHandle));
            break;
        default:
            AfxLogError("");
            break;
        }
    }
    return err;
}

_ZGL void _SglDctxEnqueueDeletion(afxDrawContext dctx, afxNat exuIdx, afxNat type, afxSize gpuHandle)
{
    //AfxEntry("dctx=%p", dctx);
    afxError err = AFX_ERR_NONE;

    AfxAssertObjects(1, &dctx, afxFcc_DCTX);

    afxDrawDevice ddev = AfxGetParent(dctx);
    AfxAssertObjects(1, &ddev, afxFcc_DDEV);    
    
    afxDrawBridge dexu = dctx->m.exus[exuIdx];
    
    _sglDeleteGlRes delRes;
    delRes.gpuHandlePtr = (void*)gpuHandle;
    delRes.type = type;

    while(!AfxPushInterlockedQueue(&dexu->deletionQueue, &delRes))
        AfxYield();

    dexu->schedCnt += 1;
    AfxSignalCondition2(&dexu->schedCnd);
}

_ZGL afxError _SglCreateHwSurface(LPWNDCLASSEXA cls, HWND* phWnd, HDC* phDC, int* pPixFmt, PIXELFORMATDESCRIPTOR* pPfd)
{
    afxError err = NIL;
    HDC bkpHdc = wglGetCurrentDrawDCSIG();
    HGLRC bkpGlrc = wglGetCurrentContextSIG();

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
    DWORD dwExStyle = WS_EX_TOPMOST;
    DWORD wndStyles = WS_POPUP;

    HWND hWnd = NIL;
    HDC hDC = NIL;
    HGLRC hGLRC = NIL;
    int dcPxlFmt = 0;
    PIXELFORMATDESCRIPTOR dcPfd = { 0 };

    if (!(hWnd = CreateWindowExA(dwExStyle, cls->lpszClassName, cls->lpszClassName, wndStyles | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1, NIL, NIL, cls->hInstance, NIL))) AfxThrowError();
    else
    {
        if (!(hDC = GetDC(hWnd))) AfxThrowError();
        else
        {
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

            int pxlAttrPairs[][2] =
            {
                { WGL_SUPPORT_OPENGL_ARB, GL_TRUE },
                { WGL_DRAW_TO_WINDOW_ARB, GL_TRUE },
#if 0
                { WGL_SUPPORT_GDI_ARB, GL_FALSE },
                { WGL_DRAW_TO_PBUFFER_ARB, GL_FALSE },
                { WGL_DRAW_TO_BITMAP_ARB, GL_FALSE },
                { WGL_BIND_TO_TEXTURE_RGB_ARB, GL_FALSE },
                { WGL_BIND_TO_TEXTURE_RGBA_ARB, GL_FALSE },
#endif
                { WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB },
                { WGL_DOUBLE_BUFFER_ARB, GL_TRUE },
                { WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB },

                // ARGB8
                { WGL_COLOR_BITS_ARB, GetDeviceCaps(hDC, BITSPIXEL) },
                { WGL_ALPHA_BITS_ARB, 8 },
                { WGL_ALPHA_SHIFT_ARB, 24 },
                { WGL_RED_BITS_ARB, 8 },
                { WGL_RED_SHIFT_ARB, 16 },
                { WGL_GREEN_BITS_ARB, 8 },
                { WGL_GREEN_SHIFT_ARB, 8 },
                { WGL_BLUE_BITS_ARB, 8 },
                { WGL_BLUE_SHIFT_ARB, 0 },

                { WGL_DEPTH_BITS_ARB, 0 }, // { WGL_DEPTH_BITS_ARB, 24 },
                { WGL_STENCIL_BITS_ARB, 0 }, // { WGL_STENCIL_BITS_ARB, 8 },
#if 0
                { WGL_AUX_BUFFERS_ARB, 0 },
                { WGL_ACCUM_BITS_ARB, 0 },
                { WGL_ACCUM_ALPHA_BITS_ARB, 0 },
                { WGL_ACCUM_RED_BITS_ARB, 0 },
                { WGL_ACCUM_GREEN_BITS_ARB, 0 },
                { WGL_ACCUM_BLUE_BITS_ARB, 0 },
#endif
                { WGL_TRANSPARENT_ARB, GL_FALSE },//{ WGL_TRANSPARENT_ARB, GL_TRUE },
                //{ WGL_SWAP_METHOD_ARB, WGL_SWAP_EXCHANGE_ARB },
                { WGL_SAMPLE_BUFFERS_ARB, GL_FALSE },
                //{ WGL_SAMPLE_BUFFERS_ARB, GL_TRUE },  // works on Intel, didn't work on Mesa
                { WGL_SAMPLES_ARB, 0 },  // works on Intel, didn't work on Mesa
                //{ WGL_COLORSPACE_EXT, WGL_COLORSPACE_SRGB_EXT }, // WGL_COLORSPACE_LINEAR_EXT // works on Mesa, didn't work on Intel
                //{ WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE }, // works on Mesa, didn't work on Intel
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

            UINT formatCount;
            
            //if (unitIdx == 0)
                //wglLoadWsiSymbolsSIG(opengl32, NIL);

            if (!wgl.ChoosePixelFormatARB(hDC, &pxlAttrPairs[0][0], NIL, 1, &dcPxlFmt, &formatCount)) AfxThrowError();
            else
            {
                AfxAssert(dcPxlFmt);
                AfxAssert(formatCount);
                AfxZero(&dcPfd, sizeof(dcPfd));

                wglDescribePixelFormatSIG(hDC, dcPxlFmt, sizeof(dcPfd), &dcPfd);
                wglSetPixelFormatSIG(hDC, dcPxlFmt, hWnd, &dcPfd);

                AfxAssert(dcPfd.dwFlags & pfdFlags);
            }

            if (err)
                ReleaseDC(hWnd, hDC), hDC = NIL;
        }

        if (err)
            DestroyWindow(hWnd), hWnd = NIL;
    }

    *phWnd = hWnd;
    *phDC = hDC;
    *pPixFmt = dcPxlFmt;
    *pPfd = dcPfd;

    wglMakeContextCurrentSIG(bkpHdc, bkpHdc, bkpGlrc);

    return err;
}

_ZGL afxError _SglCreateHwContext(afxModule oglDll, HDC hDC, HGLRC hShareCtx, HGLRC* phGLRC, glVmt* gl)
{
    afxError err = NIL;
    HDC bkpHdc = wglGetCurrentDrawDCSIG();
    HGLRC bkpGlrc = wglGetCurrentContextSIG();

    HMODULE hOglDll = AfxGetModuleIdd(oglDll);

    int ctxAttrPairs[][2] =
    {
        // GL Extensions Viewer 6.0.9.0 benchmark tests appointed 3.2 core as faster profile when 3.1 was not. 4.0 caused major drops.
        //{ WGL_CONTEXT_MAJOR_VERSION_ARB, dpu->verMajor },
        { WGL_CONTEXT_MAJOR_VERSION_ARB, 3 },
        //{ WGL_CONTEXT_MINOR_VERSION_ARB, dpu->verMinor },
        { WGL_CONTEXT_MINOR_VERSION_ARB, 2 },
        { WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB }, // WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
        { WGL_CONTEXT_FLAGS_ARB, /*WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB*/
#ifndef ZGL_DONT_USE_ROBUST_ACCESS
        | WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB
#endif
#ifdef _AFX_DEBUG
        /* | */ WGL_CONTEXT_DEBUG_BIT_ARB
#endif
        },
        { NIL, NIL }
    };

    // share can't be active in another thread
    HGLRC hGLRC = NIL;

    if (!(hGLRC = wglCreateContextAttribsSIG(hDC, hShareCtx, (void*)ctxAttrPairs))) AfxThrowError();
    else
    {
        if (!(wglMakeContextCurrentSIG(hDC, hDC, hGLRC))) AfxThrowError();
        else
        {
            if (gl)
                wglLoadSymbolsSIG(hOglDll, 0, sizeof(*gl) / sizeof(gl->ptr), (void**)gl, FALSE);
        }

        if (err)
            wglDeleteContextSIG(hGLRC), hGLRC = NIL;
    }

    *phGLRC = hGLRC;

    wglMakeContextCurrentSIG(bkpHdc, bkpHdc, bkpGlrc);

    return err;
}

_ZGL afxResult _SglDdevIoctrlCb(afxDrawDevice ddev, afxNat reqCode, va_list va)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &ddev, afxFcc_DDEV);
    afxResult rslt = 0;

    switch (reqCode)
    {
    case 0:
    {
        
        break;
    }
    case 1: // init
    {
        if (!ddev->idd)
        {
            if (!(ddev->idd = AfxAllocate(1, sizeof(ddev->idd[0]), 0, AfxHere()))) AfxThrowError();
            else
            {
                afxUri uri;
                afxNat devPagIdx = ddev->dev.manifestPagNo, recIdx;
                afxDriver icd = AfxGetDeviceDriver(&ddev->dev);
                afxManifest const* ini = AfxGetDriverManifest(icd);

                if (!AfxFindManifestRecord(ini, devPagIdx, &AfxString("OpenglDll"), &recIdx) || !AfxGetManifestUri(ini, devPagIdx, recIdx, &uri)) AfxThrowError();
                else
                {
                    if (AfxLoadModule(&uri, NIL, &ddev->idd->oglDll)) AfxThrowError();
                    else
                    {
                        //SglLoadOpenGlVmt(ddev->opengl32, 0, sizeof(wglNames) / sizeof(wglNames[0]), wglNames, ddev->p);

                        ddev->idd->wndClss.cbSize = sizeof(ddev->idd->wndClss); // only on EX
                        ddev->idd->wndClss.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
                        ddev->idd->wndClss.lpfnWndProc = _SglWndHndlngPrcW32Callback;
                        ddev->idd->wndClss.cbClsExtra = sizeof(ddev);
                        ddev->idd->wndClss.cbWndExtra = sizeof((afxDrawOutput)0);
                        ddev->idd->wndClss.hInstance = GetModuleHandleA(NULL);
                        ddev->idd->wndClss.hIcon = LoadIconA(NULL, IDI_SHIELD);
                        ddev->idd->wndClss.hCursor = LoadCursorA(NULL, IDC_ARROW);
                        ddev->idd->wndClss.hbrBackground = NULL;
                        ddev->idd->wndClss.lpszMenuName = NULL;
                        ddev->idd->wndClss.lpszClassName = "OpenGL/Vulkan Continuous Integration --- Qwadro Video Graphics Infrastructure (c) 2017 SIGMA --- Public Test Build";
                        ddev->idd->wndClss.hIconSm = LoadIconA(NULL, IDI_SHIELD);

                        if (!(RegisterClassExA(&(ddev->idd->wndClss)))) AfxThrowError();
                        else
                        {
                            AfxLogComment("Listing mem available display devices...\n");

                            //HDC hdc = NIL;
                            afxNat idx = 0;
                            while (1)
                            {
                                DISPLAY_DEVICE dispdev = { 0 };
                                dispdev.cb = sizeof(dispdev);

                                if (!(EnumDisplayDevicesA(NULL, idx, &dispdev, NIL))) break;
                                else
                                {
                                    AfxLogComment("#%u = %s (%s) %x", idx, dispdev.DeviceString, dispdev.DeviceName, dispdev.StateFlags);
                                    idx++;
                                }
                            }

                            afxNat verMajor, verMinor, verPatch;
                            wglLoadWsiSymbolsSIG(AfxGetModuleIdd(ddev->idd->oglDll), &verMajor, &verMinor, &verPatch);
                            ddev->idd->oglVerMajor = verMajor;
                            ddev->idd->oglVerMinor = verMinor;
                            ddev->idd->oglVerPatch = verPatch;

                            HWND tmpHwnd;
                            HDC tmpHdc;
                            int pixelFmt;
                            PIXELFORMATDESCRIPTOR pfd;

                            if (_SglCreateHwSurface(&ddev->idd->wndClss, &tmpHwnd, &tmpHdc, &pixelFmt, &pfd)) AfxThrowError();
                            else
                            {
                                HDC bkpHdc = wglGetCurrentDrawDCSIG();
                                HGLRC bkpGlrc = wglGetCurrentContextSIG();

                                HGLRC tmpHglrc;
                                glVmt gl;

                                if (_SglCreateHwContext(ddev->idd->oglDll, tmpHdc, NIL, &tmpHglrc, &gl)) AfxThrowError();
                                else
                                {
                                    wglMakeContextCurrentSIG(tmpHdc, tmpHdc, tmpHglrc);

                                    SglDetectDeviceFeatures(&gl, tmpHdc, &ddev->features);
                                    SglDetectDeviceLimits(&gl, &ddev->limits);

                                    ddev->idd->hasDxInterop2 = wglHasExtensionSIG(tmpHdc, "WGL_NV_DX_interop2");
                                    ddev->idd->hasDxInterop1 = wglHasExtensionSIG(tmpHdc, "WGL_NV_DX_interop");

                                    if (ddev->idd->hasDxInterop1 || ddev->idd->hasDxInterop2)
                                    {
                                        //ddev->idd->useDxgiSwapchain = FALSE;
                                    }

#if 0
                                    if (0 == unitIdx)
                                    {
                                        GLint n = 0;
                                        gl->GetIntegerv(GL_NUM_EXTENSIONS, &n); _SglThrowErrorOccuried();
                                        AfxLogEcho("Listing #%03u \"core\" GL supported extensions.", n);

                                        for (GLint i = 0; i < n; i++)
                                        {
                                            const char* extension = (const char*)gl->GetStringi(GL_EXTENSIONS, i); _SglThrowErrorOccuried();
                                            AfxLogEcho("%.3u %s ext found.", i, extension);
                                        }
                                    }
#endif

                                    wglMakeContextCurrentSIG(NIL, NIL, NIL);
                                    wglDeleteContextSIG(tmpHglrc), tmpHglrc = NIL;
                                }
                                ReleaseDC(tmpHwnd, tmpHdc), tmpHdc = NIL;
                                DestroyWindow(tmpHwnd), tmpHwnd = NIL;
                                wglMakeContextCurrentSIG(bkpHdc, bkpHdc, bkpGlrc);
                            }

                            if (err)
                                UnregisterClassA(ddev->idd->wndClss.lpszClassName, ddev->idd->wndClss.hInstance);
                        }

                        if (err)
                            AfxReleaseObjects(1, &ddev->idd->oglDll);
                    }
                }

                if (err)
                    AfxDeallocate(ddev->idd);
            }
        }
        break;
    }
    case 2: // Terminates the device. The device must first be stopped and then closed (in that order). This function should be called before the system shuts down.
    {
        if (ddev->idd)
        {
            AfxExhaustChainedClasses(&ddev->dev.classes);

            UnregisterClassA(ddev->idd->wndClss.lpszClassName, ddev->idd->wndClss.hInstance);

            if (ddev->idd->oglDll)
                AfxReleaseObjects(1, &ddev->idd->oglDll);

            AfxDeallocate(ddev->idd);
            ddev->idd = NIL;
        }
        break;
    }
    case 3: // Opens the device. This function must only be called after the device has been initialized.
    {
        afxDrawContext dctx = va_arg(va, afxDrawContext);

        if (dctx)
        {

        }
        break;
    }
    case 4: // Closes the device. This function must only be called after the device has been stopped.
    {
        afxDrawContext dctx = va_arg(va, afxDrawContext);

        if (dctx)
        {

        }
        break;
    }
    case 5: // run up the bridges. Starts it.
        // This function must only be called after the device has been initialized and opened (in that order).
    {
        afxDrawContext dctx = va_arg(va, afxDrawContext);

        if (dctx)
        {
            for (afxNat i = 0; i < dctx->m.exuCnt; i++)
            {
                _SglDexuStartCb(dctx->m.exus[i]);
            }
        }
        break;
    }
    case 6: // run down the bridges. Stops it. This function must be called before any attempt is made to close (and then terminate, if required) the device.
    {
        afxDrawContext dctx = va_arg(va, afxDrawContext);

        if (dctx)
        {
            for (afxNat i = 0; i < dctx->m.exuCnt; i++)
            {
                _SglDexuStopCb(dctx->m.exus[i]);
            }
        }
        break;
    }
    default: break;
    }

    // Note the sequence of function calls used to run-up and run-down the engine is as follows:
    // - RwEngineInit, RwEngineOpen, RwEngineStart to run-up; and
    // - RwEngineStop, RwEngineClose, RwEngineTerm to run-down.

    return rslt;
}

_ZGL afxError afxIcdHook(afxDriver icd, afxUri const* manifest)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &icd, afxFcc_ICD);

    static afxDrawFeatures drawFeatures =
    {
        .robustBufAccess = TRUE,
        .fullDrawIdxUint32 = TRUE,
        .rasterCubeArray = TRUE,
        .independentBlend = TRUE,
        .primShader = TRUE,
        .dualSrcBlend = TRUE,
        .logicOp = TRUE,
        .multiDrawIndirect = TRUE,
        .drawIndirectFirstInst = TRUE,
        .depthClamp = TRUE,
        .depthBiasClamp = TRUE,
        .fillModeNonSolid = TRUE,
        .multiViewport = TRUE,
        .dxt = TRUE,
        .shaderClipDist = TRUE,
        .shaderCullDist = TRUE,
        .alphaToOne = TRUE
    };

    //static afxDrawLimits drawLimits;

    static afxDrawPortCaps const drawPortCaps[] =
    {
        {
            .flags = afxDrawBridgeFlag_DRAW | afxDrawBridgeFlag_COMPUTE | afxDrawBridgeFlag_TRANSFER | afxDrawBridgeFlag_VHS,
            .minQueCnt = 4,
            .maxQueCnt = 16,

            .draw = TRUE,
            .compute = TRUE,
            .transfer = TRUE,
            .present = TRUE,
        },
        {
            .flags = afxDrawBridgeFlag_COMPUTE | afxDrawBridgeFlag_TRANSFER,
            .minQueCnt = 2,
            .maxQueCnt = 16,

            .compute = TRUE,
            .transfer = TRUE,
        },
        {
            .flags = afxDrawBridgeFlag_TRANSFER,
            .minQueCnt = 2,
            .maxQueCnt = 16,

            .transfer = TRUE,
        }
    };

    afxClassConfig ddgeClsCfg = _AvxDexuStdImplementation;
    ddgeClsCfg.fixedSiz = sizeof(AFX_OBJ(afxDrawBridge));
    ddgeClsCfg.ctor = (void*)_SglDexuCtorCb;
    ddgeClsCfg.dtor = (void*)_SglDexuDtorCb;

    afxClassConfig dctxClsCfg = _AvxDctxStdImplementation;
    dctxClsCfg.fixedSiz = sizeof(AFX_OBJ(afxDrawContext));
    dctxClsCfg.ctor = (void*)_SglDctxCtorCb;
    dctxClsCfg.dtor = (void*)_SglDctxDtorCb;

    afxClassConfig doutClsCfg = _AvxDoutStdImplementation;
    doutClsCfg.fixedSiz = sizeof(AFX_OBJ(afxDrawOutput));
    doutClsCfg.ctor = (void*)_SglDoutCtorCb;
    doutClsCfg.dtor = (void*)_SglDoutDtorCb;

    afxClassConfig dinClsCfg = _AvxDinStdImplementation;
    dinClsCfg.fixedSiz = sizeof(AFX_OBJ(afxDrawInput));
    dinClsCfg.ctor = (void*)_SglDinCtorCb;

    afxDrawDeviceInfo const ddevInfos[] =
    {
        {
            .dev.urn = AfxString("targl3"),
            .dev.type = afxDeviceType_DRAW,
            .dev.ioctl = (void*)_SglDdevIoctrlCb,

            .features = drawFeatures,
            .clipSpace = AVX_CLIP_SPACE_OPENGL,

            .portCnt = AFX_COUNTOF(drawPortCaps),
            .portCaps = drawPortCaps,

            .relinkDoutCb = _SglRelinkDoutCb,

            .ddgeClsCfg = &ddgeClsCfg,
            .dctxClsCfg = &dctxClsCfg,
            .doutClsCfg = &doutClsCfg,
            .dinClsCfg = &dinClsCfg,
        },
        {
            .dev.urn = AfxString("targl4"),
            .dev.type = afxDeviceType_DRAW,
            .dev.ioctl = (void*)_SglDdevIoctrlCb,

            .features = drawFeatures,
            .clipSpace = AVX_CLIP_SPACE_OPENGL,

            .portCnt = AFX_COUNTOF(drawPortCaps),
            .portCaps = drawPortCaps,

            .relinkDoutCb = _SglRelinkDoutCb,

            .ddgeClsCfg = &ddgeClsCfg,
            .dctxClsCfg = &dctxClsCfg,
            .doutClsCfg = &doutClsCfg,
            .dinClsCfg = &dinClsCfg,
        }
    };
    
    afxDrawDevice ddevices[AFX_COUNTOF(ddevInfos)] = { 0 };

    if (_AvxRegisterDrawDevices(icd, AFX_COUNTOF(ddevInfos), ddevInfos, ddevices)) AfxThrowError();
    else
    {

    }

    return err;
}
