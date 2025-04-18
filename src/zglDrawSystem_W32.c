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

_ZGL afxError _ZglDsysDtorCb(afxDrawSystem dsys)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DSYS, 1, &dsys);

    AvxWaitForDrawSystem(dsys, AFX_TIME_INFINITE);

    AfxDisposeObjects(1, &dsys->presentRazr);
    AfxDisposeObjects(1, &dsys->presentSmp);
    
    AfxExhaustChainedClasses(&dsys->m.ctx.classes);

    //AvxWaitForDrawSystem(dsys, AFX_TIME_INFINITE);

    if (_AVX_DSYS_CLASS_CONFIG.dtor(dsys))
        AfxThrowError();

    AfxDeregisterChainedClasses(&dsys->m.ctx.classes);
#if 0
    AfxDismountClass(&dsys->m.razrMgr);
    AfxDismountClass(&dsys->m.pipMgr);
    AfxDismountClass(&dsys->m.shaders);
    AfxDismountClass(&dsys->m.vinputs);
    AfxDismountClass(&dsys->m.canvases);
    AfxDismountClass(&dsys->m.rasters);
    AfxDismountClass(&dsys->m.buffers);
    AfxDismountClass(&dsys->m.samplers);
    AfxDismountClass(&dsys->m.schemas);
    AfxDismountClass(&dsys->m.queries);
    AfxDismountClass(&dsys->m.ctx.fences);
    AfxDismountClass(&dsys->m.ctx.semaphores);
#endif

    wglDeleteContextGDI(dsys->hPrimeRC);
    ReleaseDC(dsys->hPrimeWnd, dsys->hPrimeDC);
    DestroyWindow(dsys->hPrimeWnd);

    return err;
}

_ZGL afxError _ZglDsysCtorCb(afxDrawSystem dsys, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DSYS, 1, &dsys);

    afxModule icd = args[0];
    AFX_ASSERT_OBJECTS(afxFcc_MDLE, 1, &icd);
    _avxDsysAcquisition const* cfg = AFX_CAST(_avxDsysAcquisition const*, args[1]) + invokeNo;
    _avxDexuAcquisition* bridgeCfgs = AFX_CAST(_avxDexuAcquisition*, args[2]) + invokeNo;

    afxDrawDevice ddev = bridgeCfgs[0].ddev;
    AFX_ASSERT_OBJECTS(afxFcc_DDEV, 1, &ddev);

    dsys->glVerMaj = ddev->oglVerMajor;
    dsys->glVerMin = ddev->oglVerMinor;
    dsys->robustCtx = !!cfg->reqFeatures.robustBufAccess;

    if (_ZglCreateHwSurface(0, 0, &dsys->hPrimeWnd, &dsys->hPrimeDC, &dsys->primeDcPixFmt, &dsys->primeDcPfd))
    {
        AfxThrowError();
        return err;
    }

    HDC bkpHdc = wglGetCurrentDCGDI();
    HGLRC bkpGlrc = wglGetCurrentContextGDI();

    HGLRC hShareRC = NIL;

    if (_ZglCreateHwContext(dsys->hPrimeDC, hShareRC, dsys->glVerMaj, dsys->glVerMin, dsys->robustCtx, &dsys->hPrimeRC, &dsys->gl, FALSE))
    {
        AfxThrowError();
        ReleaseDC(dsys->hPrimeWnd, dsys->hPrimeDC), DestroyWindow(dsys->hPrimeWnd);
        return err;
    }

    _avxDsysAcquisition cfg2 = *cfg;

    afxClassConfig bufClsCfg = _AVX_BUF_CLASS_CONFIG;
    bufClsCfg.fixedSiz = sizeof(AFX_OBJECT(avxBuffer));
    bufClsCfg.ctor = (void*)_BufCtorCb;
    bufClsCfg.dtor = (void*)_BufDtorCb;

    afxClassConfig rasClsCfg = _AVX_RAS_CLASS_CONFIG;
    rasClsCfg.fixedSiz = sizeof(AFX_OBJECT(avxRaster));
    rasClsCfg.ctor = (void*)_ZglRasCtor;
    rasClsCfg.dtor = (void*)_ZglRasDtor;

    afxClassConfig canvClsCfg = _AVX_CANV_CLASS_CONFIG;
    canvClsCfg.fixedSiz = sizeof(AFX_OBJECT(avxCanvas));
    canvClsCfg.ctor = (void*)_ZglCanvCtor;
    canvClsCfg.dtor = (void*)_ZglCanvDtor;

    afxClassConfig sampClsCfg = _AVX_SAMP_CLASS_CONFIG;
    sampClsCfg.fixedSiz = sizeof(AFX_OBJECT(avxSampler));
    sampClsCfg.ctor = (void*)_ZglSampCtor;
    sampClsCfg.dtor = (void*)_ZglSampDtor;

    afxClassConfig ligaClsCfg = _AVX_LIGA_CLASS_CONFIG;
    ligaClsCfg.fixedSiz = sizeof(AFX_OBJECT(avxLigature));
    ligaClsCfg.ctor = (void*)_ZglLigaCtor;
    ligaClsCfg.dtor = (void*)_ZglLigaDtor;

    afxClassConfig vtxdClsCfg = _AVX_VIN_CLASS_CONFIG;
    vtxdClsCfg.fixedSiz = sizeof(AFX_OBJECT(avxVertexInput));
    vtxdClsCfg.ctor = (void*)_ZglVinCtor;
    vtxdClsCfg.dtor = (void*)_ZglVinDtor;

    afxClassConfig shdClsCfg = _AVX_SHD_CLASS_CONFIG;
    shdClsCfg.fixedSiz = sizeof(AFX_OBJECT(avxShader));
    shdClsCfg.ctor = (void*)_ZglShdCtor;
    shdClsCfg.dtor = (void*)_ZglShdDtor;

    afxClassConfig pipClsCfg = _AVX_PIP_CLASS_CONFIG;
    pipClsCfg.fixedSiz = sizeof(AFX_OBJECT(avxPipeline));
    pipClsCfg.ctor = (void*)_ZglPipCtor;
    pipClsCfg.dtor = (void*)_ZglPipDtor;

    afxClassConfig qrypClsCfg = _AVX_QRYP_CLASS_CONFIG;
    qrypClsCfg.fixedSiz = sizeof(AFX_OBJECT(avxQueryPool));
    qrypClsCfg.ctor = (void*)_ZglQrypCtor;
    qrypClsCfg.dtor = (void*)_ZglQrypDtor;

    afxClassConfig doutClsCfg = _AVX_DOUT_CLASS_CONFIG;
    doutClsCfg.fixedSiz = sizeof(AFX_OBJ(afxDrawOutput));
    doutClsCfg.ctor = (void*)_ZglDoutCtorCb;
    doutClsCfg.dtor = (void*)_ZglDoutDtorCb;

    afxClassConfig dexuClsCfg = _AVX_DEXU_CLASS_CONFIG;
    dexuClsCfg.fixedSiz = sizeof(AFX_OBJ(afxDrawBridge));
    dexuClsCfg.ctor = (void*)_ZglDexuCtorCb;
    dexuClsCfg.dtor = (void*)_ZglDexuDtorCb;

    cfg2.bufClsCfg = &bufClsCfg;
    cfg2.rasClsCfg = &rasClsCfg;
    cfg2.canvClsCfg = &canvClsCfg;
    cfg2.sampClsCfg = &sampClsCfg;
    cfg2.ligaClsCfg = &ligaClsCfg;
    cfg2.vtxdClsCfg = &vtxdClsCfg;
    cfg2.shdClsCfg = &shdClsCfg;
    cfg2.pipClsCfg = &pipClsCfg;
    cfg2.qrypClsCfg = &qrypClsCfg;
    cfg2.doutClsCfg = &doutClsCfg;
    cfg2.dexuClsCfg = &dexuClsCfg;

    for (afxUnit i = 0; i < cfg->bridgeCnt; i++)
    {
        //bridgeCfgs[i].dctxClsCfg = &dctxClsCfg;
        //bridgeCfgs[i].dctxClsCfg = &dqueClsCfg;
    }

    if (_AVX_DSYS_CLASS_CONFIG.ctor(dsys, (void*[]) { icd, (void*)&cfg2, (void*)bridgeCfgs }, 0)) AfxThrowError();
    else
    {
        afxChain *classes = &dsys->m.ctx.classes;
#if 0
        afxClassConfig semClsConf = _AfxSemStdImplementation;
        semClsConf.fixedSiz = sizeof(AFX_OBJECT(afxSemaphore));
        semClsConf.ctor = (void*)_ZglSemCtorCb;
        semClsConf.dtor = (void*)_ZglSemDtorCb;
        AfxMountClass(&dsys->m.ctx.semaphores, NIL, classes, &semClsConf);
#endif
        afxClassConfig fencClsConf = _AVX_FENC_CLASS_CONFIG;
        fencClsConf.fixedSiz = sizeof(AFX_OBJECT(avxFence));
        fencClsConf.ctor = (void*)_ZglFencCtorCb;
        fencClsConf.dtor = (void*)_ZglFencDtorCb;
        AfxMountClass(&dsys->m.fencCls, NIL, classes, &fencClsConf);


        dsys->rasUniqueId = 0;
        dsys->smpUniqueId = 0;
        dsys->bufUniqueId = 0;
        dsys->pipUniqueId = 0;
        dsys->shdUniqueId = 0;
        dsys->vdecUniqueId = 0;
        dsys->canvUniqueId = 0;
        dsys->fencUniqueId = 0;
        dsys->qrypUniqueId = 0;

        afxUri uri;
        AfxMakeUri(&uri, 0, "//./z/video/sampleOutRgba.xsh.xml", 0);
        //AfxMakeUri(&uri, 0, "data/pipeline/rgbaToRgba.xsh.xml?yFlipped", 0);
        //AfxMakeUri(&uri, 0, "data/pipeline/rgbaToRgbaYFlippedBrokenLens.pip.xml", 0);
        //dsys->m.presentPip = AfxDrawSystemFetchPipeline(dsys, &uri);


        //dsys->presentRazr = AvxLoadPipeline(dsys, NIL, &uri);

        //AfxAssertObjects(1, &dsys->presentRazr, afxFcc_RAZR);

        avxSamplerInfo smpSpec = { 0 };
        smpSpec.magnify = avxTexelFilter_NEAREST;
        smpSpec.minify = avxTexelFilter_NEAREST;
        smpSpec.mipFlt = avxTexelFilter_NEAREST;
        smpSpec.uvw[0] = avxTexelWrap_REPEAT; // EDGE fucks this shit
        smpSpec.uvw[1] = avxTexelWrap_REPEAT; // EDGE fucks this shit
        smpSpec.uvw[2] = avxTexelWrap_REPEAT; // EDGE fucks this shit

        AvxDeclareSamplers(dsys, 1, &smpSpec, &dsys->presentSmp);
        AFX_ASSERT_OBJECTS(afxFcc_SAMP, 1, &dsys->presentSmp);
#if 0
        afxString tmpStr;
        AfxMakeString(&tmpStr, "a_xy", 0);
        const afxV2d tristrippedQuad2dPos[] =
        {
            { -1.0,  1.0 },
            { -1.0, -1.0 },
            {  1.0,  1.0 },
            {  1.0, -1.0 },
        };

        afxVertexBufferBlueprint vbub;
        AfxVertexBufferBlueprintReset(&vbub, 4);
        afxVertexSpec vtxAttrSpec = { 0 };
        vtxAttrSpec.secIdx = 0;
        vtxAttrSpec.semantic = &tmpStr;
        vtxAttrSpec.fmt = afxVertexFormat_V2D;
        vtxAttrSpec.usage = afxVertexUsage_POS;
        vtxAttrSpec.src = vtxPos;
        vtxAttrSpec.srcFmt = afxVertexFormat_V2D;
        vtxAttrSpec.srcStride = sizeof(afxV2d);
        AfxVertexBufferBlueprintAddAttributes(&vbub, 1, &vtxAttrSpec);
        AfxBuildVertexBuffers(dsys, 1, &dsys->m.presentVbuf, &vbub);
        AfxAssertObjects(1, &dsys->m.presentVbuf, afxFcc_VBUF);

        avxBufferInfo vbufSpec;
        vbufSpec.siz = sizeof(tristrippedQuad2dPos);
        vbufSpec.src = tristrippedQuad2dPos;
        vbufSpec.usage = avxBufferUsage_VERTEX;

        AvxAcquireBuffers(dsys, 1, &dsys->m.tristrippedQuad2dPosBuf, &vbufSpec);
        AfxAssertObjects(1, &dsys->m.tristrippedQuad2dPosBuf, afxFcc_BUF);

        dsys->m.presentFboGpuHandle = 0;
#endif

#if 0
        avxPipelineBlueprint razrCfg = { 0 };
        razrCfg.stageCnt = 2;
        razrCfg.cullMode = avxCullMode_BACK;
        razrCfg.fillMode = avxFillMode_FACE;
        razrCfg.primTop = avxTopology_TRI_STRIP;
        AvxAssemblePipelines(dsys, 1, &razrCfg, &dsys->presentRazr);
        AvxUplinkPipelineFunction(dsys->presentRazr, avxShaderType_VERTEX, AfxUri("//./z/video/uvOutTristripQuad.vsh"), NIL, NIL, NIL);
        AvxUplinkPipelineFunction(dsys->presentRazr, avxShaderType_FRAGMENT, AfxUri("//./z/video/sampleOutRgba2d.fsh"), NIL, NIL, NIL);
#endif

        // DEVICE FONT STUFF
#if 0
        afxUri uri2;
        AfxMakeUri(&uri2, "//./z/video/font-256.tga", 0);
        AvxLoadRasters(dsys, avxRasterUsage_RESAMPLE, NIL, 1, &uri2, &dsys->fntRas);

        avxVertexFetch const vinStreams[] =
        {
            {
                .instanceRate = 1,
                .slotIdx = 0
            }
        };

        avxVertexInput const vinAttrs[] =
        {
            {
                .location = 0,
                .streamIdx = 0,
                .offset = 0,
                .fmt = afxVertexFormat_V2D
            },
            {
                .location = 1,
                .streamIdx = 0,
                .offset = 8,
                .fmt = afxVertexFormat_V2D
            },
            {
                .location = 2,
                .streamIdx = 0,
                .offset = 16,
                .fmt = afxVertexFormat_V4B,
                .normalized = TRUE
            }
        };

        avxVertexInput vin = AfxDeclareVertexLayout(dsys, ARRAY_SIZE(vinStreams), vinStreams, ARRAY_SIZE(vinAttrs), vinAttrs);
        AFX_ASSERT_OBJECTS(afxFcc_VIN, 1, &vin);

        AfxMakeUri(&uri2, "//./z/video/font.xsh.xml?instanced", 0);
        dsys->fntRazr = AvxLoadPipeline(dsys, vin, &uri2);
        AfxAssertObjects(1, &dsys->fntRazr, afxFcc_RAZR);
        AfxDisposeObjects(1, &dsys->fntRazr);

        avxBufferInfo vboSpec = { 0 };
        vboSpec.flags = avxBufferFlag_W;
        vboSpec.usage = avxBufferUsage_VERTEX;
        vboSpec.src = NIL;
        vboSpec.bufCap = 2048;
        AvxAcquireBuffers(dsys, 1, &vboSpec, &dsys->fntDataBuf);
        AfxAssertObjects(1, &dsys->fntDataBuf, afxFcc_BUF);

        avxSamplerInfo smpCnf = { 0 };
        AfxDescribeDefaultSampler(&smpCnf);
        smpCnf.minFilter = avxTexelFilter_LINEAR;
        smpCnf.uvw[0] = avxTexelWrap_EDGE;
        smpCnf.uvw[1] = avxTexelWrap_EDGE;
        smpCnf.uvw[2] = avxTexelWrap_EDGE;
        AvxDeclareSamplers(dsys, 1, &smpCnf, &dsys->fntSamp);
        AfxAssertObjects(1, &dsys->fntSamp, afxFcc_SAMP);

        avxBufferInfo bufSpec = { 0 };
        bufSpec.siz = sizeof(akxViewConstants);
        bufSpec.usage = avxBufferUsage_UNIFORM;
        bufSpec.access = avxBufferFlag_W;
        bufSpec.src = NIL;
        AvxAcquireBuffers(dsys, 1, &bufSpec, &dsys->fntUnifBuf);
        AfxAssertObjects(1, &dsys->fntUnifBuf, afxFcc_BUF);
#endif
        //AFX_ASSERT(dsys->m.vmt);

        if (!err)
        {
            dsys->m.running = TRUE;
        }

        if (err)
            AfxDeregisterChainedClasses(&dsys->m.ctx.classes);

        if (err && _AVX_DSYS_CLASS_CONFIG.dtor(dsys))
            AfxThrowError();
    }

    wglMakeCurrentGDI(bkpHdc, bkpGlrc);

    if (err)
    {
        wglDeleteContextGDI(dsys->hPrimeRC);
        ReleaseDC(dsys->hPrimeWnd, dsys->hPrimeDC), DestroyWindow(dsys->hPrimeWnd);
    }

    return err;
}
