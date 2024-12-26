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

//#define _AVOID_WGL_FUNCS 1

extern afxClassConfig _ZglDexuMgrCfg;

_ZGL LRESULT WINAPI _ZglWndHndlngPrcW32Callback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

_ZGL afxError _ZglProcessDeletionQueue(glVmt const* gl, afxInterlockedQueue* deletionQueue)
{
    afxError err = AFX_ERR_NONE;

    _zglDeleteGlRes res;
    while (AfxPopInterlockedQueue(deletionQueue, &res))
    {
        switch (res.type)
        {
        case GL_BUFFER:
            AFX_ASSERT(gl->IsBuffer(res.gpuHandle));
            gl->DeleteBuffers(1, &res.gpuHandle); _ZglThrowErrorOccuried();
            AFX_ASSERT(!gl->IsBuffer(res.gpuHandle));
            break;
        case GL_TEXTURE:
            AFX_ASSERT(gl->IsTexture(res.gpuHandle));
            gl->DeleteTextures(1, &res.gpuHandle); _ZglThrowErrorOccuried();
            AFX_ASSERT(!gl->IsTexture(res.gpuHandle));
            break;
        case GL_RENDERBUFFER:
            AFX_ASSERT(gl->IsRenderBuffer(res.gpuHandle));
            gl->DeleteRenderbuffers(1, &res.gpuHandle); _ZglThrowErrorOccuried();
            AFX_ASSERT(!gl->IsRenderBuffer(res.gpuHandle));
            break;
        case GL_FRAMEBUFFER:
            AFX_ASSERT(gl->IsFramebuffer(res.gpuHandle));
            gl->DeleteFramebuffers(1, &res.gpuHandle); _ZglThrowErrorOccuried();
            AFX_ASSERT(!gl->IsFramebuffer(res.gpuHandle));
            break;
        case GL_SAMPLER:
            AFX_ASSERT(gl->IsSampler(res.gpuHandle));
            gl->DeleteSamplers(1, &res.gpuHandle); _ZglThrowErrorOccuried();
            AFX_ASSERT(!gl->IsSampler(res.gpuHandle));
            break;
        case GL_PROGRAM:
            AFX_ASSERT(gl->IsProgram(res.gpuHandle));
            gl->DeleteProgram(res.gpuHandle); _ZglThrowErrorOccuried();
            AFX_ASSERT(!gl->IsProgram(res.gpuHandle));
            break;
        case GL_SHADER:
            AFX_ASSERT(gl->IsShader(res.gpuHandle));
            gl->DeleteShader(res.gpuHandle); _ZglThrowErrorOccuried();
            AFX_ASSERT(!gl->IsShader(res.gpuHandle));
            break;
        case GL_VERTEX_ARRAY:
            AFX_ASSERT(gl->IsVertexArray(res.gpuHandle));
            gl->GenVertexArrays(1, &res.gpuHandle); _ZglThrowErrorOccuried();
            AFX_ASSERT(!gl->IsVertexArray(res.gpuHandle));
            break;
        case GL_PROGRAM_PIPELINE:
            AFX_ASSERT(gl->IsProgramPipeline(res.gpuHandle));
            gl->DeleteProgramPipelines(1, &res.gpuHandle); _ZglThrowErrorOccuried();
            AFX_ASSERT(!gl->IsProgramPipeline(res.gpuHandle));
            break;
        case GL_SYNC_FENCE:
            AFX_ASSERT(gl->IsSync(res.gpuHandlePtr));
            gl->DeleteSync(res.gpuHandlePtr); _ZglThrowErrorOccuried();
            AFX_ASSERT(!gl->IsSync(res.gpuHandlePtr));
            break;
        case GL_QUERY_BUFFER:
            AFX_ASSERT(gl->IsQuery(res.gpuHandle));
            gl->DeleteQueries(1, &res.gpuHandle); _ZglThrowErrorOccuried();
            AFX_ASSERT(!gl->IsQuery(res.gpuHandle));
            break;
        default:
            AfxLogError("");
            break;
        }
    }
    return err;
}

_ZGL void _ZglDsysEnqueueDeletion(afxDrawSystem dsys, afxUnit exuIdx, afxUnit type, afxSize gpuHandle)
{
    //AfxEntry("dsys=%p", dsys);
    afxError err = AFX_ERR_NONE;

    AFX_ASSERT_OBJECTS(afxFcc_DSYS, 1, &dsys);

    afxDrawBridge dexu = dsys->m.bridges[exuIdx];
    afxDrawDevice ddev = dexu->m.ddev;
    AFX_ASSERT_OBJECTS(afxFcc_DDEV, 1, &ddev);
    
    _zglDeleteGlRes delRes;
    delRes.gpuHandlePtr = (void*)gpuHandle;
    delRes.type = type;

    while(!AfxPushInterlockedQueue(&dexu->deletionQueue, &delRes))
        AfxYield();

    dexu->m.schedCnt += 1;
    AfxSignalCondition2(&dexu->m.schedCnd);
}

_ZGL afxError _ZglCreateHwSurface(LPWNDCLASSEXA cls, HWND* phWnd, HDC* phDC, int* pPixFmt, PIXELFORMATDESCRIPTOR* pPfd)
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

    if (!(hWnd = CreateWindowExA(dwExStyle, cls->lpszClassName, cls->lpszClassName, wndStyles | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1, NIL, NIL, cls->hInstance, NIL)))
        AfxThrowError();
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
                AFX_ASSERT(dcPxlFmt);
                AFX_ASSERT(formatCount);
                AfxZero(&dcPfd, sizeof(dcPfd));

                wglDescribePixelFormatSIG(hDC, dcPxlFmt, sizeof(dcPfd), &dcPfd);
                wglSetPixelFormatSIG(hDC, dcPxlFmt, hWnd, &dcPfd);

                AFX_ASSERT(dcPfd.dwFlags & pfdFlags);
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

_ZGL afxError _ZglCreateHwContext(afxModule oglDll, HDC hDC, HGLRC hShareCtx, HGLRC* phGLRC, glVmt* gl)
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

_ZGL afxResult _ZglDdevIoctrlCb(afxDrawDevice ddev, afxUnit reqCode, va_list va)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DDEV, 1, &ddev);
    afxResult rslt = 0;

    switch (reqCode)
    {
    case 0:
    {
        
        break;
    }
    case 1: // init
    {
#if 0
        if (!ddev->idd)
        {
            if (AfxAllocate(sizeof(ddev->idd[0]), 0, AfxHere(), (void**)&ddev->idd)) AfxThrowError();
            else
            {
                afxUri uri;
                afxUnit devPagIdx = ddev->dev.manifestPagNo, recIdx;
                afxModule icd = AfxGetDeviceDriver(&ddev->dev);
                afxManifest const* ini = AfxGetModuleManifest(icd);

                //if (!AfxFindInitializationRecord(ini, devPagIdx, &AfxString("OpenglDll"), &recIdx) || !AfxGetManifestUri(ini, devPagIdx, recIdx, &uri)) AfxThrowError();
                //else
                AfxMakeUri(&uri, 0, "//./c/opengl32.dll", 0);
                {
                    if (AfxLoadModule(&uri, NIL, &ddev->idd->oglDll)) AfxThrowError();
                    else
                    {
                        //ZglLoadOpenGlVmt(ddev->opengl32, 0, sizeof(wglNames) / sizeof(wglNames[0]), wglNames, ddev->p);

                        ddev->idd->wndClss.cbSize = sizeof(ddev->idd->wndClss); // only on EX
                        ddev->idd->wndClss.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
                        ddev->idd->wndClss.lpfnWndProc = _ZglWndHndlngPrcW32Callback;
                        ddev->idd->wndClss.cbClsExtra = sizeof(ddev);
                        ddev->idd->wndClss.cbWndExtra = sizeof((afxDrawOutput)0);
                        ddev->idd->wndClss.hInstance = GetModuleHandleA(NULL);
                        ddev->idd->wndClss.hIcon = LoadIconA(NULL, IDI_SHIELD);
                        ddev->idd->wndClss.hCursor = LoadCursorA(NULL, IDC_ARROW);
                        ddev->idd->wndClss.hbrBackground = NULL;
                        ddev->idd->wndClss.lpszMenuName = NULL;
                        ddev->idd->wndClss.lpszClassName = "SIGMA GL/2 Continuous Integration --- Qwadro Video Graphics Infrastructure (c) 2017 SIGMA --- Public Test Build";
                        ddev->idd->wndClss.hIconSm = LoadIconA(NULL, IDI_SHIELD);

                        if (!(RegisterClassExA(&(ddev->idd->wndClss)))) AfxThrowError();
                        else
                        {
                            AfxLogComment("Listing mem available display devices...\n");

                            //HDC hdc = NIL;
                            afxUnit idx = 0;
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

                            afxUnit verMajor, verMinor, verPatch;
                            wglLoadWsiSymbolsSIG(AfxGetModuleIdd(ddev->idd->oglDll), &verMajor, &verMinor, &verPatch);
                            ddev->idd->oglVerMajor = verMajor;
                            ddev->idd->oglVerMinor = verMinor;
                            ddev->idd->oglVerPatch = verPatch;
                            
                            HWND tmpHwnd;
                            HDC tmpHdc;
                            int pixelFmt;
                            PIXELFORMATDESCRIPTOR pfd;

                            if (_ZglCreateHwSurface(&ddev->idd->wndClss, &tmpHwnd, &tmpHdc, &pixelFmt, &pfd)) AfxThrowError();
                            else
                            {
                                HDC bkpHdc = wglGetCurrentDrawDCSIG();
                                HGLRC bkpGlrc = wglGetCurrentContextSIG();

                                HGLRC tmpHglrc;
                                glVmt gl;

                                if (_ZglCreateHwContext(ddev->idd->oglDll, tmpHdc, NIL, &tmpHglrc, &gl)) AfxThrowError();
                                else
                                {
                                    wglMakeContextCurrentSIG(tmpHdc, tmpHdc, tmpHglrc);

                                    ZglDetectDeviceFeatures(&gl, tmpHdc, &ddev->features);
                                    ZglDetectDeviceLimits(&gl, &ddev->limits);

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
                                        gl->GetIntegerv(GL_NUM_EXTENSIONS, &n); _ZglThrowErrorOccuried();
                                        AfxLogEcho("Listing #%03u \"core\" GL supported extensions.", n);

                                        for (GLint i = 0; i < n; i++)
                                        {
                                            const char* extension = (const char*)gl->GetStringi(GL_EXTENSIONS, i); _ZglThrowErrorOccuried();
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
                            AfxDisposeObjects(1, &ddev->idd->oglDll);
                    }
                }

                if (err)
                    AfxDeallocate((void**)&ddev->idd, AfxHere());
            }
        }
#endif
        break;
    }
    case 2: // Terminates the device. The device must first be stopped and then closed (in that order). This function should be called before the system shuts down.
    {
#if 0
        if (ddev->idd)
        {
            AfxExhaustChainedClasses(&ddev->dev.classes);

            UnregisterClassA(ddev->idd->wndClss.lpszClassName, ddev->idd->wndClss.hInstance);

            if (ddev->idd->oglDll)
                AfxDisposeObjects(1, &ddev->idd->oglDll);

            AfxDeallocate((void**)&ddev->idd, AfxHere());
            ddev->idd = NIL;
        }
#endif
        break;
    }
    case 3: // Opens the device. This function must only be called after the device has been initialized.
    {
        afxDrawSystem dsys = va_arg(va, afxDrawSystem);

        if (dsys)
        {

        }
        break;
    }
    case 4: // Closes the device. This function must only be called after the device has been stopped.
    {
        afxDrawSystem dsys = va_arg(va, afxDrawSystem);

        if (dsys)
        {

        }
        break;
    }
    case 5: 
        // run up the bridges. 
        // Starts it.
        // This function must only be called after the device has been initialized and opened (in that order).
    {
        afxDrawSystem dsys = va_arg(va, afxDrawSystem);

        if (!dsys) AfxThrowError();
        else
        {
            for (afxUnit i = 0; i < dsys->m.bridgeCnt; i++)
            {
                afxDrawBridge dexu = dsys->m.bridges[i];
                AFX_ASSERT_OBJECTS(afxFcc_DEXU, 1, &dexu);

                if (!AfxIsThreadRunning(dexu->m.worker))
                {
                    AfxRunThread(dexu->m.worker, (void*)dexu->m.workerProc, dexu);
                }
            }
        }
        break;
    }
    case 6:
        // run down the bridges. 
        // Stops it. 
        // This function must be called before any attempt is made to close (and then terminate, if required) the device.
    {
        afxDrawSystem dsys = va_arg(va, afxDrawSystem);

        if (!dsys) AfxThrowError();
        else
        {
            for (afxUnit i = 0; i < dsys->m.bridgeCnt; i++)
            {
                afxDrawBridge dexu = dsys->m.bridges[i];
                AFX_ASSERT_OBJECTS(afxFcc_DEXU, 1, &dexu);

                afxInt exitCode = 0;
                do
                {
                    AfxRequestThreadInterruption(dexu->m.worker);
                    AfxWaitForIdleDrawBridge(dexu, AFX_TIME_INFINITE);
                } while (!AfxWaitForThreadExit(dexu->m.worker, &exitCode));
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

_ZGL afxError _ZglDdevDtorCb(afxDrawDevice ddev)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DDEV, 1, &ddev);

    AfxExhaustChainedClasses(&ddev->m.dev.classes);

    if (_AVX_DDEV_CLASS_CONFIG.dtor(ddev))
        AfxThrowError();

    if (ddev->oglDll)
        AfxDisposeObjects(1, &ddev->oglDll);

    if (ddev->d3d9Dll)
        AfxDisposeObjects(1, &ddev->d3d9Dll);

    UnregisterClassA(ddev->wndClss.lpszClassName, ddev->wndClss.hInstance);

    return err;
}

_ZGL afxError _ZglDdevCtorCb(afxDrawDevice ddev, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DDEV, 1, &ddev);

    afxModule icd = args[0];
    AFX_ASSERT_OBJECTS(afxFcc_MDLE, 1, &icd);
    afxDrawDeviceInfo const* info = (afxDrawDeviceInfo const *)(args[1]) + invokeNo;
    AFX_ASSERT(info);

    static afxDrawPortCaps const portCaps[] =
    {
        {
            .capabilities = afxDrawPortFlag_DRAW | afxDrawPortFlag_COMPUTE | afxDrawPortFlag_TRANSFER | afxDrawPortFlag_VHS,
            .minQueCnt = 4,
            .maxQueCnt = 16,
            .acceleration = afxAcceleration_IGPU
        },
        {
            .capabilities = afxDrawPortFlag_COMPUTE | afxDrawPortFlag_TRANSFER,
            .minQueCnt = 2,
            .maxQueCnt = 16,
            .acceleration = afxAcceleration_IGPU
        },
        {
            .capabilities = afxDrawPortFlag_TRANSFER,
            .minQueCnt = 2,
            .maxQueCnt = 16,
            .acceleration = afxAcceleration_IGPU
        }
    };

    if (_AVX_DDEV_CLASS_CONFIG.ctor(ddev, (void*[]) { icd, (void*)info }, 0))
    {
        AfxThrowError();
        return err;
    }

    afxUri uri;
    afxUnit devPagIdx = ddev->m.dev.manifestPagNo, recIdx;
    afxManifest const* ini = AfxGetModuleManifest(icd);

    //if (!AfxFindInitializationRecord(ini, devPagIdx, &AfxString("OpenglDll"), &recIdx) || !AfxGetManifestUri(ini, devPagIdx, recIdx, &uri)) AfxThrowError();
    //else
    AfxMakeUri(&uri, 0, "//./c/opengl32.dll", 0);
    {
        if (AfxLoadModule(&uri, NIL, &ddev->oglDll)) AfxThrowError();
        else
        {
            //ZglLoadOpenGlVmt(ddev->opengl32, 0, sizeof(wglNames) / sizeof(wglNames[0]), wglNames, ddev->p);

            static afxChar const* const classNames[] =
            {
                "SIGMA GL/2 Continuous Integration --- v3 --- Qwadro Video Graphics Infrastructure (c) 2017 SIGMA --- Public Test Build",
                "SIGMA GL/2 Continuous Integration --- v4 --- Qwadro Video Graphics Infrastructure (c) 2017 SIGMA --- Public Test Build",
            };

            ddev->wndClss.cbSize = sizeof(ddev->wndClss); // only on EX
            ddev->wndClss.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
            ddev->wndClss.lpfnWndProc = _ZglWndHndlngPrcW32Callback;
            ddev->wndClss.cbClsExtra = sizeof(ddev);
            ddev->wndClss.cbWndExtra = sizeof((afxDrawOutput)0);
            ddev->wndClss.hInstance = GetModuleHandleA(NULL);
            ddev->wndClss.hIcon = LoadIconA(NULL, IDI_SHIELD);
            ddev->wndClss.hCursor = LoadCursorA(NULL, IDC_ARROW);
            ddev->wndClss.hbrBackground = NULL;
            ddev->wndClss.lpszMenuName = NULL;
            ddev->wndClss.lpszClassName = classNames[invokeNo]; // it is to avoid class name colision. It must be unique per process.
            ddev->wndClss.hIconSm = LoadIconA(NULL, IDI_SHIELD);

            if (!(RegisterClassExA(&(ddev->wndClss)))) AfxThrowError();
            else
            {
                AfxLogComment("Listing mem available display devices...\n");

                //HDC hdc = NIL;
                afxUnit idx = 0;
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

                afxUnit verMajor, verMinor, verPatch;
                wglLoadWsiSymbolsSIG(AfxGetModuleIdd(ddev->oglDll), &verMajor, &verMinor, &verPatch);
                ddev->oglVerMajor = verMajor;
                ddev->oglVerMinor = verMinor;
                ddev->oglVerPatch = verPatch;

                HWND tmpHwnd;
                HDC tmpHdc;
                int pixelFmt;
                PIXELFORMATDESCRIPTOR pfd;

                if (_ZglCreateHwSurface(&ddev->wndClss, &tmpHwnd, &tmpHdc, &pixelFmt, &pfd)) AfxThrowError();
                else
                {
                    HDC bkpHdc = wglGetCurrentDrawDCSIG();
                    HGLRC bkpGlrc = wglGetCurrentContextSIG();

                    HGLRC tmpHglrc;
                    glVmt gl;

                    if (_ZglCreateHwContext(ddev->oglDll, tmpHdc, NIL, &tmpHglrc, &gl)) AfxThrowError();
                    else
                    {
                        wglMakeContextCurrentSIG(tmpHdc, tmpHdc, tmpHglrc);

                        ZglDetectDeviceFeatures(&gl, tmpHdc, &ddev->m.features);
                        ZglDetectDeviceLimits(&gl, &ddev->m.limits);

                        ddev->hasDxInterop2 = wglHasExtensionSIG(tmpHdc, "WGL_NV_DX_interop2");
                        ddev->hasDxInterop1 = wglHasExtensionSIG(tmpHdc, "WGL_NV_DX_interop");

                        if (ddev->hasDxInterop1 || ddev->hasDxInterop2)
                        {
                            //ddev->idd->useDxgiSwapchain = FALSE;
                        }

#if 0
                        if (0 == unitIdx)
                        {
                            GLint n = 0;
                            gl->GetIntegerv(GL_NUM_EXTENSIONS, &n); _ZglThrowErrorOccuried();
                            AfxLogEcho("Listing #%03u \"core\" GL supported extensions.", n);

                            for (GLint i = 0; i < n; i++)
                            {
                                const char* extension = (const char*)gl->GetStringi(GL_EXTENSIONS, i); _ZglThrowErrorOccuried();
                                AfxLogEcho("%.3u %s ext found.", i, extension);
                            }
                        }
#endif

                        for (afxUnit i = 0; i < ddev->m.portCnt; i++)
                        {
                            afxDrawPortCaps* caps = &ddev->m.ports[i].caps;
                            *caps = portCaps[i];
                        }

                        wglMakeContextCurrentSIG(NIL, NIL, NIL);
                        wglDeleteContextSIG(tmpHglrc), tmpHglrc = NIL;
                    }
                    ReleaseDC(tmpHwnd, tmpHdc), tmpHdc = NIL;
                    DestroyWindow(tmpHwnd), tmpHwnd = NIL;
                    wglMakeContextCurrentSIG(bkpHdc, bkpHdc, bkpGlrc);
                }

                if (err)
                    UnregisterClassA(ddev->wndClss.lpszClassName, ddev->wndClss.hInstance);
            }

            if (err)
                AfxDisposeObjects(1, &ddev->oglDll);
        }
    }

    if (err && _AVX_DDEV_CLASS_CONFIG.dtor(ddev))
        AfxThrowError();

    return err;
}

_ZGL afxError afxIcdHook(afxModule icd, afxUri const* manifest)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_MDLE, 1, &icd);

    afxClassConfig ddevClsCfg = _AVX_DDEV_CLASS_CONFIG;
    ddevClsCfg.fixedSiz = sizeof(AFX_OBJECT(afxDrawDevice));
    ddevClsCfg.ctor = (void*)_ZglDdevCtorCb;
    ddevClsCfg.dtor = (void*)_ZglDdevDtorCb;

    afxClassConfig dsysClsCfg = _AVX_DSYS_CLASS_CONFIG;
    dsysClsCfg.fixedSiz = sizeof(AFX_OBJ(afxDrawSystem));
    dsysClsCfg.ctor = (void*)_ZglDsysCtorCb;
    dsysClsCfg.dtor = (void*)_ZglDsysDtorCb;

    if (_AvxImplementDrawSystem(icd, NIL, &ddevClsCfg, &dsysClsCfg))
    {
        AfxThrowError();
        return err;
    }
    else
    {
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

        afxDrawDeviceInfo const ddevInfos[] =
        {
            {
                .dev.urn = AfxString("tarzgl3"),
                .dev.type = afxDeviceType_DRAW,
                .dev.ioctl = (void*)_ZglDdevIoctrlCb,

                .features = drawFeatures,
                .clipSpaceDepth = avxClipSpaceDepth_NEG_ONE_TO_ONE,

                .portCnt = 3
            },
            {
                .dev.urn = AfxString("tarzgl4"),
                .dev.type = afxDeviceType_DRAW,
                .dev.ioctl = (void*)_ZglDdevIoctrlCb,

                .features = drawFeatures,
                .clipSpaceDepth = avxClipSpaceDepth_NEG_ONE_TO_ONE,

                .portCnt = 3
            }
        };

        afxDrawDevice ddevices[ARRAY_SIZE(ddevInfos)] = { 0 };

        if (_AvxRegisterDrawDevices(icd, ARRAY_SIZE(ddevInfos), ddevInfos, ddevices))
        {
            AfxThrowError();
            return err;
        }
    }
    return err;
}
