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

//#define USE_SCREEN_DC 1

extern afxClassConfig _ZglDexuMgrCfg;

_ZGL afxString const targaSignature = AFX_STRING(
    "           :::::::::::     :::     :::::::::   ::::::::      :::               \n"
    "               :+:       :+: :+:   :+:    :+: :+:    :+:   :+: :+:             \n"
    "               +:+      +:+   +:+  +:+    +:+ +:+         +:+   +:+            \n"
    "               +#+     +#++:++#++: +#++:++#:  :#:        +#++:++#++:           \n"
    "               +#+     +#+     +#+ +#+    +#+ +#+   +#+# +#+     +#+           \n"
    "               #+#     #+#     #+# #+#    #+# #+#    #+# #+#     #+#           \n"
    "               ###     ###     ### ###    ###  ########  ###     ###           \n"
    "                                                                               \n"
    "              Q W A D R O   E X E C U T I O N   E C O S Y S T E M              \n"
    "                                                                               \n"
    "                               Public Test Build                               \n"
    "                           (c) 2017 SIGMA FEDERATION                           \n"
    "                                www.sigmaco.org                                \n"
    "                                                                               \n"
);

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
            AfxReportError("");
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

                //if (!AfxFindInitializationRecord(ini, devPagIdx, &AFX_STRING("OpenglDll"), &recIdx) || !AfxGetManifestUri(ini, devPagIdx, recIdx, &uri)) AfxThrowError();
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
                            AfxReportComment("Listing mem available display devices...\n");

                            //HDC hdc = NIL;
                            afxUnit idx = 0;
                            while (1)
                            {
                                DISPLAY_DEVICE dispdev = { 0 };
                                dispdev.cb = sizeof(dispdev);

                                if (!(EnumDisplayDevicesA(NULL, idx, &dispdev, NIL))) break;
                                else
                                {
                                    AfxReportComment("#%u = %s (%s) %x", idx, dispdev.DeviceString, dispdev.DeviceName, dispdev.StateFlags);
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
                                        AfxReportMessage("Listing #%03u \"core\" GL supported extensions.", n);

                                        for (GLint i = 0; i < n; i++)
                                        {
                                            const char* extension = (const char*)gl->GetStringi(GL_EXTENSIONS, i); _ZglThrowErrorOccuried();
                                            AfxReportMessage("%.3u %s ext found.", i, extension);
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
                    AvxWaitForIdleDrawBridge(dexu, AFX_TIME_INFINITE);
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

    static afxDrawCapabilities const portCaps[] =
    {
        {
            .capabilities = afxDrawPortFlag_DRAW | afxDrawPortFlag_COMPUTE | afxDrawPortFlag_TRANSFER | afxDrawPortFlag_VIDEO,
            .minQueCnt = 4,
            .maxQueCnt = 16,
            .acceleration = afxAcceleration_DPU
        },
        {
            .capabilities = afxDrawPortFlag_COMPUTE | afxDrawPortFlag_TRANSFER,
            .minQueCnt = 2,
            .maxQueCnt = 16,
            .acceleration = afxAcceleration_DPU
        },
        {
            .capabilities = afxDrawPortFlag_TRANSFER,
            .minQueCnt = 2,
            .maxQueCnt = 16,
            .acceleration = afxAcceleration_DPU
        }
    };

    if (_AVX_DDEV_CLASS_CONFIG.ctor(ddev, (void*[]) { icd, (void*)info, args[2] }, 0))
    {
        AfxThrowError();
        return err;
    }

    afxUri uri;
    afxUnit devPagIdx = ddev->m.dev.manifestPagNo, recIdx;
    afxManifest const* ini = AfxGetModuleManifest(icd);

    //if (!AfxFindInitializationRecord(ini, devPagIdx, &AFX_STRING("OpenglDll"), &recIdx) || !AfxGetManifestUri(ini, devPagIdx, recIdx, &uri)) AfxThrowError();
    //else
    AfxMakeUri(&uri, 0, "//./c/opengl32.dll", 0);
    {
        if (AfxLoadModule(&uri, NIL, &ddev->oglDll)) AfxThrowError();
        else
        {
            //ZglLoadOpenGlVmt(ddev->opengl32, 0, sizeof(wglNames) / sizeof(wglNames[0]), wglNames, ddev->p);

            afxUnit verMajor, verMinor, verPatch;
            //wglLoadWsiSymbolsSIG(AfxGetModuleIdd(ddev->oglDll), &verMajor, &verMinor, &verPatch);

            wglLoadModule(AfxGetModuleIdd(ddev->oglDll), &verMajor, &verMinor, &verPatch);

            ddev->oglVerMajor = verMajor;
            ddev->oglVerMinor = verMinor;
            ddev->oglVerPatch = verPatch;

            HWND tmpHwnd;
            HDC tmpHdc;
            int pixelFmt;
            PIXELFORMATDESCRIPTOR pfd;
            if (_ZglCreateHwSurface(0, 0, &tmpHwnd, &tmpHdc, &pixelFmt, &pfd)) AfxThrowError();
            else
            {
                HDC bkpHdc = wglGetCurrentDCGDI();
                HGLRC bkpGlrc = wglGetCurrentContextGDI();


#if _AFX_DEBUG
                afxBool echoSymbols = TRUE;
                        
#else
                afxBool echoSymbols = FALSE;
#endif

                glVmt gl;
                HGLRC tmpHglrc;
                if (_ZglCreateHwContext(tmpHdc, NIL, verMajor, verMinor, FALSE, &tmpHglrc, &gl, echoSymbols)) AfxThrowError();
                else
                {
                    wglMakeCurrentGDI(tmpHdc, tmpHglrc);

                    TestCoreSymbols(AfxGetModuleIdd(ddev->oglDll), &gl);

                    ZglDetectDeviceFeatures(&gl, tmpHdc, &ddev->m.features);
                    ZglDetectDeviceLimits(&gl, &ddev->m.limits);

                    //EnumDisplayMonitors(tmpHdc, NIL, );

                    ddev->hasDxInterop2 = wglHasExtensionSIG(tmpHdc, "WGL_NV_DX_interop2");
                    ddev->hasDxInterop1 = wglHasExtensionSIG(tmpHdc, "WGL_NV_DX_interop");
                    
                    // 16384 * 16384 * 4 / 1024 / 1024 = 1024MB

                    if (ddev->hasDxInterop1 || ddev->hasDxInterop2)
                    {
                        //ddev->idd->useDxgiSwapchain = FALSE;
                    }

#if !0
                    if (1)
                    {
                        GLint n = 0;
                        gl.GetIntegerv(GL_NUM_EXTENSIONS, &n);
                        AfxReportMessage("GL: Listing #%03u supported extensions.", n);

                        for (GLint i = 0; i < n; i++)
                        {
                            const char* extension = (const char*)gl.GetStringi(GL_EXTENSIONS, i);
                            AfxReportMessage("  %3u %s", i, extension);
                        }
                    }
#endif

                    for (afxUnit i = 0; i < ddev->m.portCnt; i++)
                    {
                        afxDrawCapabilities* caps = &ddev->m.ports[i].caps;
                        *caps = portCaps[i];
                    }

                    wglMakeCurrentGDI(NIL, NIL);
                    wglDeleteContextGDI(tmpHglrc), tmpHglrc = NIL;
                }
                ReleaseDC(tmpHwnd, tmpHdc), tmpHdc = NIL;
                DestroyWindow(tmpHwnd), tmpHwnd = NIL;
                wglMakeCurrentGDI(bkpHdc, bkpGlrc);
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

    afxClassConfig viddClsCfg = _AVX_VDU_CLASS_CONFIG;
    viddClsCfg.fixedSiz = sizeof(AFX_OBJ(afxDisplay));
    viddClsCfg.ctor = (void*)_ZglViddCtorCb;
    viddClsCfg.dtor = (void*)_ZglViddDtorCb;

    if (_AvxImplementDrawSystem(icd, &viddClsCfg, &ddevClsCfg, &dsysClsCfg))
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
                .dev.urn = AFX_STRING("tarzgl4"),
                .dev.type = afxDeviceType_DRAW,
                .dev.ioctl = (void*)_ZglDdevIoctrlCb,

                .features = drawFeatures,
                .clipSpaceDepth = avxClipSpaceDepth_NEG_ONE_TO_ONE,

                .portCnt = 3
            },
            {
                .dev.urn = AFX_STRING("tarzgl3"),
                .dev.type = afxDeviceType_DRAW,
                .dev.ioctl = (void*)_ZglDdevIoctrlCb,

                .features = drawFeatures,
                .clipSpaceDepth = avxClipSpaceDepth_NEG_ONE_TO_ONE,

                .portCnt = 3
            }
        };

        afxDrawDevice ddevices[ARRAY_SIZE(ddevInfos)] = { 0 };

        if (_AvxRegisterDrawDevices(icd, /*ARRAY_SIZE(ddevInfos)*/1, ddevInfos, ddevices))
        {
            AfxThrowError();
            return err;
        }
        else
        {
            avxDisplayInfo viddInfos[]=
            {
                {
                    .dev.urn = AFX_STRING("dwm"),
                    .dev.type = afxDeviceType_DISPLAY,
                    .dev.ioctl = (void*)_ZglViddIoctrlCb,
                },
                {
                    .dev.urn = AFX_STRING("dwmglw"),
                    .dev.type = afxDeviceType_DISPLAY,
                    .dev.ioctl = (void*)_ZglViddIoctrlCb,
                },
                {
                    .dev.urn = AFX_STRING("dwmglw2"),
                    .dev.type = afxDeviceType_DISPLAY,
                    .dev.ioctl = (void*)_ZglViddIoctrlCb,
                }
            };
            afxDisplay vidds[ARRAY_SIZE(viddInfos)];
            _AvxRegisterDisplays(icd, ARRAY_SIZE(viddInfos), viddInfos, vidds);
        }
    }
    return err;
}
