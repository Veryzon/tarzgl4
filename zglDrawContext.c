﻿/*
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
#include <Windows.h>

#include "zglCommands.h"
#include "zglObjects.h"

#include "qwadro/afxQwadro.h"

_ZGL afxError _SglDctxDtorCb(afxDrawContext dctx)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dctx, afxFcc_DCTX);

    AfxDisconnectDrawContext(dctx, FALSE, FALSE);
    AfxWaitForDrawContext(dctx);

    AfxReleaseObjects(1, &dctx->presentRazr);
    AfxReleaseObjects(1, &dctx->presentSmp);
    
    AfxExhaustChainedClasses(&dctx->m.ctx.classes);

    AfxWaitForDrawContext(dctx);

    if (_AvxDctxStdImplementation.dtor(dctx))
        AfxThrowError();

    AfxCleanUpChainedClasses(&dctx->m.ctx.classes);
#if 0
    AfxDeregisterClass(&dctx->m.razrMgr);
    AfxDeregisterClass(&dctx->m.pipMgr);
    AfxDeregisterClass(&dctx->m.shaders);
    AfxDeregisterClass(&dctx->m.vinputs);
    AfxDeregisterClass(&dctx->m.canvases);
    AfxDeregisterClass(&dctx->m.rasters);
    AfxDeregisterClass(&dctx->m.buffers);
    AfxDeregisterClass(&dctx->m.samplers);
    AfxDeregisterClass(&dctx->m.schemas);
    AfxDeregisterClass(&dctx->m.queries);
    AfxDeregisterClass(&dctx->m.ctx.fences);
    AfxDeregisterClass(&dctx->m.ctx.semaphores);
#endif
    dctx->m.exuCnt = 0;

    wglDeleteContextSIG(dctx->hPrimeRC);
    ReleaseDC(dctx->hPrimeWnd, dctx->hPrimeDC);
    DestroyWindow(dctx->hPrimeWnd);

    return err;
}

_ZGL afxError _SglDctxCtorCb(afxDrawContext dctx, void** args, afxNat invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dctx, afxFcc_DCTX);

    afxDrawDevice ddev = args[0];
    AfxAssertObjects(1, &ddev, afxFcc_DDEV);
    afxDrawContextConfig const *cfg = (afxDrawContextConfig const *)(args[1]) + invokeNo;

    if (_SglCreateHwSurface(&ddev->idd->wndClss, &dctx->hPrimeWnd, &dctx->hPrimeDC, &dctx->primeDcPixFmt, &dctx->primeDcPfd)) AfxThrowError();
    else
    {
        HDC bkpHdc = wglGetCurrentDrawDCSIG();
        HGLRC bkpGlrc = wglGetCurrentContextSIG();

        HGLRC hShareRC = NIL;

        if (_SglCreateHwContext(ddev->idd->oglDll, dctx->hPrimeDC, hShareRC, &dctx->hPrimeRC, &dctx->gl)) AfxThrowError();
        else
        {
            if (_AvxDctxStdImplementation.ctor(dctx, (void*[]) { ddev, (void*)cfg }, 0)) AfxThrowError();
            else
            {
                afxChain *classes = &dctx->m.ctx.classes;
                afxClassConfig tmpClsConf;

                tmpClsConf = _SglSemMgrCfg;
                AfxRegisterClass(&dctx->m.ctx.semaphores, NIL, classes, &tmpClsConf);

                tmpClsConf = _SglFencMgrCfg;
                AfxRegisterClass(&dctx->m.ctx.fences, NIL, classes, &tmpClsConf);

                tmpClsConf = _AvxBufStdImplementation;
                tmpClsConf.fixedSiz = sizeof(AFX_OBJECT(afxBuffer));
                tmpClsConf.ctor = (void*)_BufCtorCb;
                tmpClsConf.dtor = (void*)_BufDtorCb;
                AfxRegisterClass(&dctx->m.bufCls, NIL, classes, &tmpClsConf);

                tmpClsConf = _AvxRasStdImplementation;
                tmpClsConf.fixedSiz = sizeof(AFX_OBJECT(afxRaster));
                tmpClsConf.ctor = (void*)_SglRasCtor;
                tmpClsConf.dtor = (void*)_SglRasDtor;
                AfxRegisterClass(&dctx->m.rasCls, NIL, classes, &tmpClsConf);

                tmpClsConf = _AvxCanvStdImplementation;
                tmpClsConf.fixedSiz = sizeof(AFX_OBJECT(avxCanvas));
                tmpClsConf.ctor = (void*)_SglCanvCtor;
                tmpClsConf.dtor = (void*)_SglCanvDtor;
                AfxRegisterClass(&dctx->m.canvCls, NIL, classes, &tmpClsConf);

                tmpClsConf = _AvxSampStdImplementation;
                tmpClsConf.fixedSiz = sizeof(AFX_OBJECT(avxSampler));
                tmpClsConf.ctor = (void*)_SglSampCtor;
                tmpClsConf.dtor = (void*)_SglSampDtor;
                AfxRegisterClass(&dctx->m.sampCls, NIL, classes, &tmpClsConf);

                tmpClsConf = _AvxLigaStdImplementation;
                tmpClsConf.fixedSiz = sizeof(AFX_OBJECT(avxLigature));
                tmpClsConf.ctor = (void*)_SglLigaCtor;
                tmpClsConf.dtor = (void*)_SglLigaDtor;
                AfxRegisterClass(&dctx->m.ligaCls, NIL, classes, &tmpClsConf);

                tmpClsConf = _AvxVinStdImplementation;
                tmpClsConf.fixedSiz = sizeof(AFX_OBJECT(avxVertexInput));
                tmpClsConf.ctor = (void*)_SglVinCtor;
                tmpClsConf.dtor = (void*)_SglVinDtor;
                AfxRegisterClass(&dctx->m.vinCls, NIL, classes, &tmpClsConf);

                tmpClsConf = _AvxShdStdImplementation;
                tmpClsConf.fixedSiz = sizeof(AFX_OBJECT(avxShader));
                tmpClsConf.ctor = (void*)_SglShdCtor;
                tmpClsConf.dtor = (void*)_SglShdDtor;
                AfxRegisterClass(&dctx->m.shadCls, NIL, classes, &tmpClsConf);

                tmpClsConf = _AvxPipStdImplementation;
                tmpClsConf.fixedSiz = sizeof(AFX_OBJECT(avxPipeline));
                tmpClsConf.ctor = (void*)_SglPipCtor;
                tmpClsConf.dtor = (void*)_SglPipDtor;
                AfxRegisterClass(&dctx->m.pipCls, NIL, classes, &tmpClsConf);

                tmpClsConf = _AvxRazrStdImplementation;
                tmpClsConf.fixedSiz = sizeof(AFX_OBJECT(avxRasterizer));
                tmpClsConf.dtor = (void*)_SglRazrDtor;
                tmpClsConf.ctor = (void*)_SglRazrCtor;
                AfxRegisterClass(&dctx->m.razrCls, NIL, classes, &tmpClsConf);

                tmpClsConf = _AvxQrypStdImplementation;
                tmpClsConf.fixedSiz = sizeof(AFX_OBJECT(avxQueryPool));
                tmpClsConf.ctor = (void*)_SglQrypCtor;
                tmpClsConf.dtor = (void*)_SglQrypDtor;
                AfxRegisterClass(&dctx->m.qrypCls, NIL, classes, &tmpClsConf);

                afxUri uri;
                AfxMakeUri(&uri, 0, "//./z/video/sampleOutRgba.xsh.xml", 0);
                //AfxMakeUri(&uri, 0, "data/pipeline/rgbaToRgba.xsh.xml?yFlipped", 0);
                //AfxMakeUri(&uri, 0, "data/pipeline/rgbaToRgbaYFlippedBrokenLens.pip.xml", 0);
                //dctx->m.presentPip = AfxDrawContextFetchPipeline(dctx, &uri);


                //dctx->presentRazr = AfxLoadRasterizerFromXsh(dctx, NIL, &uri);

                //AfxAssertObjects(1, &dctx->presentRazr, afxFcc_RAZR);

                avxSamplerConfig smpSpec = { 0 };
                smpSpec.magFilter = avxTexelFilter_POINT;
                smpSpec.minFilter = avxTexelFilter_POINT;
                smpSpec.mipFilter = avxTexelFilter_POINT;
                smpSpec.uvw[0] = avxTexelAddress_REPEAT; // EDGE fucks this shit
                smpSpec.uvw[1] = avxTexelAddress_REPEAT; // EDGE fucks this shit
                smpSpec.uvw[2] = avxTexelAddress_REPEAT; // EDGE fucks this shit

                AfxAcquireSamplers(dctx, 1, &smpSpec, &dctx->presentSmp);
                AfxAssertObjects(1, &dctx->presentSmp, afxFcc_SAMP);
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
                vtxAttrSpec.usage = akxVertexUsage_POS;
                vtxAttrSpec.src = vtxPos;
                vtxAttrSpec.srcFmt = afxVertexFormat_V2D;
                vtxAttrSpec.srcStride = sizeof(afxV2d);
                AfxVertexBufferBlueprintAddAttributes(&vbub, 1, &vtxAttrSpec);
                AfxBuildVertexBuffers(dctx, 1, &dctx->m.presentVbuf, &vbub);
                AfxAssertObjects(1, &dctx->m.presentVbuf, afxFcc_VBUF);

                afxBufferSpecification vbufSpec;
                vbufSpec.siz = sizeof(tristrippedQuad2dPos);
                vbufSpec.src = tristrippedQuad2dPos;
                vbufSpec.usage = afxBufferUsage_VERTEX;

                AfxAcquireBuffers(dctx, 1, &dctx->m.tristrippedQuad2dPosBuf, &vbufSpec);
                AfxAssertObjects(1, &dctx->m.tristrippedQuad2dPosBuf, afxFcc_BUF);

                dctx->m.presentFboGpuHandle = 0;
#endif

                avxRasterizationConfig razrCfg = { 0 };
                razrCfg.xfmrPipb.cullMode = avxCullMode_BACK;
                razrCfg.fillMode = avxFillMode_SOLID;
                razrCfg.xfmrPipb.primTop = avxTopology_TRI_STRIP;
                AfxMakeUri(&razrCfg.xfmrPipb.shdUri[0], 0, "../system/video/uvOutTristripQuad.vsh", 0);
                AfxMakeUri(&razrCfg.xfmrPipb.shdUri[1], 0, "../system/video/sampleOutRgba2d.fsh", 0);
                razrCfg.xfmrPipb.shdStage[0] = avxShaderStage_VERTEX;
                razrCfg.xfmrPipb.shdStage[1] = avxShaderStage_FRAGMENT;
                razrCfg.xfmrPipb.shdCnt = 2;

                AfxAssembleRasterizers(dctx, 1, &razrCfg, &dctx->presentRazr);
                // DEVICE FONT STUFF
#if 0
                afxUri uri2;
                AfxMakeUri(&uri2, "system/video/font-256.tga", 0);
                AfxLoadRasters(dctx, afxRasterUsage_SAMPLING, NIL, 1, &uri2, &dctx->fntRas);

                avxVertexInputStream const vinStreams[] =
                {
                    {
                        .instanceRate = 1,
                        .slotIdx = 0
                    }
                };

                avxVertexInputAttr const vinAttrs[] =
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

                avxVertexInput vin = AfxAcquireVertexInput(dctx, AFX_COUNTOF(vinStreams), vinStreams, AFX_COUNTOF(vinAttrs), vinAttrs);
                AfxAssertObjects(1, &vin, afxFcc_VIN);

                AfxMakeUri(&uri2, "system/video/font.xsh.xml?instanced", 0);
                dctx->fntRazr = AfxLoadRasterizerFromXsh(dctx, vin, &uri2);
                AfxAssertObjects(1, &dctx->fntRazr, afxFcc_RAZR);
                AfxReleaseObjects(1, &dctx->fntRazr);

                afxBufferSpecification vboSpec = { 0 };
                vboSpec.access = afxBufferAccess_W;
                vboSpec.usage = afxBufferUsage_VERTEX;
                vboSpec.src = NIL;
                vboSpec.siz = 2048;
                AfxAcquireBuffers(dctx, 1, &vboSpec, &dctx->fntDataBuf);
                AfxAssertObjects(1, &dctx->fntDataBuf, afxFcc_BUF);

                avxSamplerConfig smpCnf = { 0 };
                AfxDescribeDefaultSampler(&smpCnf);
                smpCnf.minFilter = avxTexelFilter_LINEAR;
                smpCnf.uvw[0] = avxTexelAddress_CLAMP;
                smpCnf.uvw[1] = avxTexelAddress_CLAMP;
                smpCnf.uvw[2] = avxTexelAddress_CLAMP;
                AfxAcquireSamplers(dctx, 1, &smpCnf, &dctx->fntSamp);
                AfxAssertObjects(1, &dctx->fntSamp, afxFcc_SAMP);

                afxBufferSpecification bufSpec = { 0 };
                bufSpec.siz = sizeof(akxViewConstants);
                bufSpec.usage = afxBufferUsage_UNIFORM;
                bufSpec.access = afxBufferAccess_W;
                bufSpec.src = NIL;
                AfxAcquireBuffers(dctx, 1, &bufSpec, &dctx->fntUnifBuf);
                AfxAssertObjects(1, &dctx->fntUnifBuf, afxFcc_BUF);
#endif
                //AfxAssert(dctx->m.vmt);

                if (!err)
                {
                    dctx->m.running = TRUE;
                }
                if (err)
                    AfxCleanUpChainedClasses(&dctx->m.ctx.classes);

                if (err && _AvxDctxStdImplementation.dtor(dctx))
                    AfxThrowError();
            }

            if (err)
                wglDeleteContextSIG(dctx->hPrimeRC);
        }

        if (err)
            ReleaseDC(dctx->hPrimeWnd, dctx->hPrimeDC), DestroyWindow(dctx->hPrimeWnd);

        wglMakeContextCurrentSIG(bkpHdc, bkpHdc, bkpGlrc);
    }
    return err;
}
