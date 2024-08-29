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

#define _AVX_DRAW_C
#define _AVX_DRAW_SYSTEM_C
#include "zglCommands.h"
#include "zglObjects.h"
#include "qwadro/sim/rendering/akxRenderer.h"

// RENDERING SCOPE

_ZGL void _DpuEndDrawScope(sglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    //_DpuBindAndSyncCanv(gl, FALSE, GL_READ_FRAMEBUFFER, 0);
    //_DpuBindAndSyncCanv(gl, FALSE, GL_DRAW_FRAMEBUFFER, 0);
    //gl->Flush(); _SglThrowErrorOccuried(); // flush was presenting/swapping without wglSwapBuffers.

    avxCanvas canv = dpu->activeRasterState.pass.canv;

    if (canv)
    {
        afxNat surCnt;
        AfxCountDrawBufferSlots(canv, &surCnt, NIL);

        if (surCnt)
        {
            AfxAssert(_ZGL_MAX_SURF_PER_CANV >= surCnt);
            afxRaster surfaces[_ZGL_MAX_SURF_PER_CANV];
            AfxGetDrawBuffers(canv, 0, surCnt, surfaces);

            for (afxNat i = 0; i < surCnt; i++)
            {
                afxRaster ras = surfaces[i];

                if (ras)
                {
                    AfxAssertObjects(1, &ras, afxFcc_RAS);
                    ras->updFlags |= ZGL_UPD_FLAG_HOST_FLUSH;
                }
            }
        }
    }
}

_ZGL void _DpuBeginDrawScope(sglDpu* dpu, avxCanvas canv, afxRect const* area, afxNat layerCnt, afxNat cCnt, avxDrawTarget const* c, avxDrawTarget const* d, avxDrawTarget const* s, afxBool defFbo)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    avxDrawTarget const* dt;
    
    //_DpuBindAndSyncCanv(gl, bindCanv, TRUE, GL_READ_FRAMEBUFFER, canv);
    _DpuBindAndSyncCanv(dpu, /*(dpu->activeRasterState.pass.canv != canv)*/TRUE, TRUE, GL_DRAW_FRAMEBUFFER, canv);
    //dpu->activeRasterState.pass.canv = canv;

    if (!canv)
    {
        if (defFbo)
        {
            dt = &c[0];

            switch (dt->loadOp)
            {
            case avxLoadOp_CLEAR:
            {
                afxReal const *color = dt->clearValue.color;
                gl->ClearColor(color[0], color[1], color[2], color[3]); _SglThrowErrorOccuried();
                gl->Clear(GL_COLOR_BUFFER_BIT); _SglThrowErrorOccuried();
                break;
            }
            case avxLoadOp_LOAD:
            {
                break;
            }
            case avxLoadOp_DONT_CARE:
            {
                afxWhd whd;
                AfxGetCanvasExtent(canv, whd);

                if ((area->x == 0) && (area->y == 0) && (area->w == whd[0]) && (area->h == whd[1]))
                {
                    gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 1, (GLenum[]) { GL_BACK_LEFT }); _SglThrowErrorOccuried();
                }
                else
                {
                    gl->InvalidateSubFramebuffer(GL_DRAW_FRAMEBUFFER, 1, (GLenum[]) { GL_BACK_LEFT }, area->x, area->y, area->w, area->h); _SglThrowErrorOccuried();
                }
                break;
            }
            }
        }
    }
    else
    {
        GLuint glHandle = canv->glHandle;
        AfxAssert(gl->IsFramebuffer(glHandle));

        afxNat colorCnt = cCnt;
        afxNat maxSurCnt;
        AfxCountDrawBufferSlots(canv, &maxSurCnt, NIL);
        colorCnt = AfxMin(colorCnt, maxSurCnt);

        afxNat enabledToDrawCnt = 0;
        GLenum enabledToDraw[_ZGL_MAX_SURF_PER_CANV];
        afxMask storeBitmask = NIL;

        for (afxNat i = 0; i < colorCnt; i++)
        {
            dt = i ? &c[i] : &c[0];

            switch (dt->storeOp)
            {
            case avxStoreOp_STORE:
            {
                enabledToDraw[enabledToDrawCnt] = GL_COLOR_ATTACHMENT0 + i;
                enabledToDrawCnt++;
                storeBitmask |= AFX_BIT(i);
                break;
            }
            case avxStoreOp_DISCARD:
            {
                enabledToDraw[enabledToDrawCnt] = GL_NONE;
                enabledToDrawCnt++;
                storeBitmask |= AFX_BIT(8 + i);
                break;
            }
            case avxStoreOp_DONT_CARE:
            default:
            {
                enabledToDraw[enabledToDrawCnt] = GL_NONE;
                enabledToDrawCnt++;
                storeBitmask |= AFX_BIT(16 + i);
                break;
            }
            }
        }

        if (canv->storeBitmask != storeBitmask)
        {
            canv->storeBitmask = storeBitmask; // cache it
            gl->DrawBuffers(enabledToDrawCnt, enabledToDraw); _SglThrowErrorOccuried();
        }
        
        afxNat enabledToDontLoadCnt = 0;
        GLenum enabledToDontLoad[_ZGL_MAX_SURF_PER_CANV];

        for (afxNat i = 0; i < colorCnt; i++)
        {
            dt = i ? &c[i] : &c[0];

            switch (dt->loadOp)
            {
            case avxLoadOp_CLEAR:
            {
                afxReal const *color = dt->clearValue.color;
                gl->ClearBufferfv(GL_COLOR, i, color); _SglThrowErrorOccuried();
                break;
            }
            case avxLoadOp_LOAD:
            {
                break;
            }
            case avxLoadOp_DONT_CARE:
            default:
            {
                enabledToDontLoad[enabledToDontLoadCnt] = GL_COLOR_ATTACHMENT0 + i;
                ++enabledToDontLoadCnt;
                break;
            }
            }
        }

        if (enabledToDontLoadCnt)
        {
            afxWhd whd;
            AfxGetCanvasExtent(canv, whd);

            if ((area->x == 0) && (area->y == 0) && (area->w == whd[0]) && (area->h == whd[1]))
            {
                gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, enabledToDontLoadCnt, enabledToDontLoad); _SglThrowErrorOccuried();
            }
            else
            {
                gl->InvalidateSubFramebuffer(GL_DRAW_FRAMEBUFFER, enabledToDontLoadCnt, enabledToDontLoad, area->x, area->y, area->w, area->h); _SglThrowErrorOccuried();
            }
        }

        afxNat dsSurIdx[2];
        AfxGetDepthBufferSlot(canv, &dsSurIdx[0]);
        afxBool combinedDs = (AfxGetStencilBufferSlot(canv, &dsSurIdx[1]) && (dsSurIdx[1] == dsSurIdx[0]));
        
        if (dsSurIdx[0] != AFX_INVALID_INDEX)
        {
            dt = d;

            if (dt)
            {
                if (dt->loadOp == avxLoadOp_CLEAR)
                {
                    if (!combinedDs)
                    {
                        gl->ClearBufferfv(GL_DEPTH, 0, &dt->clearValue.depth); _SglThrowErrorOccuried();
                    }
                    else
                    {
                        gl->ClearBufferfi(GL_DEPTH_STENCIL, 0, dt->clearValue.depth, dt->clearValue.stencil); _SglThrowErrorOccuried();
                    }
                }
            }
        }

        if ((dsSurIdx[1] != AFX_INVALID_INDEX) && !combinedDs)
        {
            dt = s;

            if (dt)
            {
                if (dt->loadOp == avxLoadOp_CLEAR)
                {
                    GLint sCv = dt->clearValue.stencil;
                    gl->ClearBufferiv(GL_STENCIL, 0, &sCv); _SglThrowErrorOccuried();
                }
            }
        }
    }
}

// STATE SETTING

_ZGL void _DpuBindPipeline(sglDpu* dpu, avxPipeline pip, avxVertexInput vin, afxFlags dynamics)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextPip = pip;

    if (!pip)
    {
        //dpu->nextVin = NIL;
    }
    else
    {
        dpu->nextVin = vin ? vin : pip->m.vin;

        dpu->nextXformState.primTop = pip->m.primTop;
        dpu->nextXformState.primRestartEnabled = pip->m.primRestartEnabled;
        dpu->nextXformState.depthClampEnabled = pip->m.depthClampEnabled;

        if ((dpu->nextXformState.cullMode = pip->m.cullMode))
        {
            dpu->nextXformState.cwFrontFacing = pip->m.frontFacingInverted;
        }
    }
}

_ZGL void _DpuBindRasterizer(sglDpu* dpu, avxRasterizer razr, avxVertexInput vin, afxFlags dynamics)
{
    afxError err = AFX_ERR_NONE;

    if (!razr)
    {
        dpu->nextRasterState.rasterizationDisabled = TRUE;
    }
    else
    {
        dpu->nextRasterState.rasterizationDisabled = FALSE;

        avxRasterizationConfig config;
        AfxDescribeRasterizerConfiguration(razr, &config);

        if ((dpu->nextRasterState.outCnt = config.colorOutCnt))
        {
            AfxCopy2(dpu->nextRasterState.outs, config.colorOuts, sizeof(config.colorOuts[0]), config.colorOutCnt);

            AfxColorCopy(dpu->nextRasterState.blendConstants, config.blendConstants);
            dpu->nextBlendConstUpd = 1;
        }

        if ((dpu->nextRasterState.depthBiasEnabled = config.depthBiasEnabled))
        {
            dpu->nextRasterState.depthBiasClamp = config.depthBiasClamp;
            dpu->nextRasterState.depthBiasConstFactor = config.depthBiasConstFactor;
            dpu->nextRasterState.depthBiasSlopeScale = config.depthBiasSlopeScale;
        }

        if ((dpu->nextRasterState.depthBoundsTestEnabled = config.depthBoundsTestEnabled))
            AfxV2dCopy(dpu->nextRasterState.depthBounds, config.depthBounds);

        if ((dpu->nextRasterState.depthTestEnabled = config.depthTestEnabled))
        {
            dpu->nextRasterState.depthCompareOp = config.depthCompareOp;
        }

        dpu->nextRasterState.depthWriteDisabled = config.depthWriteDisabled;

        dpu->nextRasterState.fillMode = config.fillMode;
        dpu->nextRasterState.lineWidth = config.lineWidth;

        if ((dpu->nextRasterState.minSampleShadingValue = config.minSampleShadingValue))
        {
            dpu->nextRasterState.sampleShadingEnabled = config.sampleShadingEnabled;
        }

        dpu->nextRasterState.msEnabled = config.msEnabled;

        dpu->nextRasterState.alphaToCoverageEnabled = config.alphaToCoverageEnabled;
        dpu->nextRasterState.alphaToOneEnabled = config.alphaToOneEnabled;

        if ((dpu->nextRasterState.logicOpEnabled = config.pixelLogicOpEnabled))
        {
            dpu->nextRasterState.logicOp = config.pixelLogicOp;
        }

        if ((dpu->nextRasterState.sampleCnt = config.sampleCnt))
        {
            AfxCopy2(dpu->nextRasterState.sampleMasks, config.sampleMasks, sizeof(config.sampleMasks), config.sampleCnt);
        }

        if ((dpu->nextRasterState.stencilTestEnabled = config.stencilTestEnabled))
        {
            dpu->nextRasterState.stencilBack = config.stencilBack;
            dpu->nextRasterState.stencilFront = config.stencilFront;
        }
    }

    dpu->nextRazr = razr;

    _DpuBindPipeline(dpu, AfxGetRasterizerPipeline(razr), vin, dynamics);
}

// RESOURCE BINDING

_ZGL void _DpuBindBuffers(sglDpu* dpu, afxNat set, afxNat first, afxNat cnt, afxBuffer buffers[], afxNat const offsets[], afxNat const ranges[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssertRange(_ZGL_MAX_ENTRY_PER_LEGO, first, cnt);
    AfxAssertRange(_ZGL_MAX_LEGO_PER_BIND, set, 1);

    for (afxNat i = 0; i < cnt; i++)
    {
        afxNat entryIdx = first + i;
        afxBuffer buf = buffers[i];
        afxNat32 offset = offsets[i];
        afxNat32 range = ranges[i];

        dpu->nextResBind[set][entryIdx].buf = buf;
        dpu->nextResBind[set][entryIdx].offset = offset;
        dpu->nextResBind[set][entryIdx].range = range;
        dpu->nextResBindUpdMask[set] |= AFX_BIT(entryIdx);
    }
}

_ZGL void _DpuBindRasters(sglDpu* dpu, afxNat set, afxNat first, afxNat cnt, afxRaster rasters[], afxNat const subras[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssertRange(_ZGL_MAX_ENTRY_PER_LEGO, first, cnt);
    AfxAssertRange(_ZGL_MAX_LEGO_PER_BIND, set, 1);

    for (afxNat i = 0; i < cnt; i++)
    {
        afxNat entryIdx = first + i;
        afxRaster ras = rasters[i];
        dpu->nextResBind[set][entryIdx].ras = ras;
        dpu->nextResBind[set][entryIdx].subrasIdx = subras[i];
        dpu->nextResBindUpdMask[set] |= AFX_BIT(entryIdx);
    }
}

_ZGL void _DpuBindSamplers(sglDpu* dpu, afxNat set, afxNat first, afxNat cnt, avxSampler samplers[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssertRange(_ZGL_MAX_ENTRY_PER_LEGO, first, cnt);
    AfxAssertRange(_ZGL_MAX_LEGO_PER_BIND, set, 1);

    for (afxNat i = 0; i < cnt; i++)
    {
        afxNat entryIdx = first + i;
        avxSampler smp = samplers[i];
        dpu->nextResBind[set][entryIdx].smp = smp;
        dpu->nextResBindUpdMask[set] |= AFX_BIT(entryIdx);
    }
}

_ZGL void _SglFlushResourcingStateChanges(sglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    // BIND RESOURCES (TEXTURES, SAMPLERS AND BUFFERS)

    for (afxNat i = 0; i < dpu->activePip->m.liga->m.setCnt; i++)
    {
        afxNat set = dpu->activePip->m.liga->m.sets[i].set;
        afxMask updMask = dpu->nextResBindUpdMask[set];

        avxLigature lego = dpu->activePip->m.liga;
        AfxAssert(lego);
        AfxAssertObjects(1, &lego, afxFcc_BSCH);

        for (afxNat j = 0; j < lego->m.sets[i].entryCnt; j++)
        {
            afxNat resIdx = lego->m.sets[i].baseEntry + j;
            avxLigatureEntry const *entry = &lego->m.totalEntries[resIdx];
            AfxAssert(entry->type);
            afxNat binding = entry->binding;
            afxNat glUnit = (set * _ZGL_MAX_ENTRY_PER_LEGO) + binding;
            afxBool reqUpd = FALSE, reqUpd2 = FALSE;
            GLuint glHandle = 0, glHandle2 = 0;
            afxSize bufSiz = 0;

            switch (entry->type)
            {
            case avxShaderResType_CONSTANT_BUFFER:
            case avxShaderResType_STORAGE_BUFFER:
            {
                afxNat offset = dpu->nextResBind[set][binding].offset;
                afxNat range = dpu->nextResBind[set][binding].range;
                afxBuffer buf = dpu->nextResBind[set][binding].buf;
                afxBool rebind = FALSE;
                GLenum glTarget = GL_INVALID_ENUM;

                if (entry->type == avxShaderResType_CONSTANT_BUFFER)
                    glTarget = GL_UNIFORM_BUFFER;
                else if (entry->type == avxShaderResType_STORAGE_BUFFER)
                    glTarget = GL_SHADER_STORAGE_BUFFER;
                else AfxThrowError();

                //if (updMask & AFX_BIT(j))
                {
                    if ((dpu->activeResBind[set][binding].buf != buf) ||
                        (dpu->activeResBind[set][binding].offset != offset) ||
                        (dpu->activeResBind[set][binding].range != range)
                        )
                    {
                        dpu->activeResBind[set][binding].buf = buf;
                        dpu->activeResBind[set][binding].offset = offset;
                        dpu->activeResBind[set][binding].range = range;
                        rebind = TRUE;
                    }
                }

                if (!buf)
                {
                    gl->BindBufferBase(glTarget, glUnit, 0); _SglThrowErrorOccuried();
                }
                else
                {
                    DpuBindAndSyncBuf(dpu, glTarget, buf);
                    bufSiz = AfxGetBufferCapacity(buf, 0);

                    if (offset || range)
                    {
                        AfxAssert(range);
                        AfxAssertRange(bufSiz, offset, range);
                        gl->BindBufferRange(glTarget, glUnit, buf->glHandle, offset, AFX_ALIGNED_SIZEOF(range, 16)); _SglThrowErrorOccuried();
                    }
                    else
                    {
                        gl->BindBufferBase(glTarget, glUnit, buf->glHandle); _SglThrowErrorOccuried();
                    }
                }
                break;
            }
            case avxShaderResType_SAMPLER:
            case avxShaderResType_SAMPLED_IMAGE:
            case avxShaderResType_COMBINED_IMAGE_SAMPLER:
            {
                avxSampler samp = dpu->nextResBind[set][binding].smp;
                afxRaster ras = dpu->nextResBind[set][binding].ras;
                afxBool rebindRas = FALSE;
                afxBool rebindSamp = FALSE;

                //if (updMask & AFX_BIT(j))
                {
                    if (dpu->activeResBind[set][binding].ras != ras)
                    {
                        dpu->activeResBind[set][binding].ras = ras;
                        rebindRas = TRUE;
                    }

                    if (dpu->activeResBind[set][binding].smp != samp)
                    {
                        dpu->activeResBind[set][binding].smp = samp;
                        rebindSamp = TRUE;
                    }
                }

                if (entry->type == avxShaderResType_SAMPLED_IMAGE || entry->type == avxShaderResType_COMBINED_IMAGE_SAMPLER)
                {
                    if (!ras)
                    {
                        gl->ActiveTexture(GL_TEXTURE0 + glUnit); _SglThrowErrorOccuried();
                        gl->BindTexture(GL_TEXTURE_2D, 0); _SglThrowErrorOccuried();
                    }
                    else
                    {
                        DpuBindAndSyncRas(dpu, glUnit, ras);
                        gl->ActiveTexture(GL_TEXTURE0 + glUnit); _SglThrowErrorOccuried();
                        gl->BindTexture(ras->glTarget, ras->glHandle); _SglThrowErrorOccuried();
                    }
                }

                if (entry->type == avxShaderResType_SAMPLER || entry->type == avxShaderResType_COMBINED_IMAGE_SAMPLER)
                {
                    if (!samp)
                    {
                        gl->BindSampler(glUnit, 0); _SglThrowErrorOccuried();
                    }
                    else
                    {
                        _DpuBindAndSyncSamp(dpu, glUnit, samp);
                        gl->BindSampler(glUnit, samp->glHandle); _SglThrowErrorOccuried();
                    }
                }
                break;
            }
            default:
            {
                AfxLogError("");
            }
            }
        }
        dpu->nextResBindUpdMask[set] = NIL;
    }
}

_ZGL void _DpuDraw(sglDpu* dpu, avxDrawIndirectCmd const* data)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    afxNat cnt;

    _DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    dpu->activePip = dpu->nextPip;

    if (!dpu->activePip) AfxThrowError();
    else
    {
        dpu->activeVin = dpu->nextVin;
        _DpuBindAndSyncVin(dpu, dpu->activeVin, &dpu->nextVinBindings);

        SglFlushXformStateChanges(dpu);
        SglFlushRasterizationStateChanges(dpu);
        _SglFlushResourcingStateChanges(dpu);

        //if (dpu->nextVtxInStreamUpdMask || dpu->nextVtxInAttribUpdMask || dpu->flushIbb)
            //_SglFlushVertexInputStateChanges(dpu, FALSE);

        //AfxAssert(!dpu->nextVinBindingsMask);
        //AfxAssert(!dpu->nextVtxInAttribUpdMask);
        //AfxAssert(!dpu->flushIbb);

        AfxAssert(dpu->activePip);
        afxNat32 vtxCnt = data->vtxCnt;
        AfxAssert(vtxCnt);
        //AfxAssert(cmd->instCnt);
        GLenum top = AfxToGlTopology(dpu->activeXformState.primTop);

        afxNat firstInst = data->firstInst;
        afxNat firstVtx = data->firstVtx;
        afxNat instCnt = data->instCnt;

#if FORCE_GL_GENERIC_FUNCS
        AfxAssert(gl->DrawArraysInstancedBaseInstance);
        gl->DrawArraysInstancedBaseInstance(top, firstVtx, vtxCnt, instCnt, firstInst); _SglThrowErrorOccuried();
#else
        if (instCnt)
        {
            if (firstInst)
            {
                AfxAssert(gl->DrawArraysInstancedBaseInstance);
                gl->DrawArraysInstancedBaseInstance(top, firstVtx, vtxCnt, instCnt, firstInst); _SglThrowErrorOccuried();
            }
            else
            {
                gl->DrawArraysInstanced(top, firstVtx, vtxCnt, instCnt); _SglThrowErrorOccuried();
            }
        }
        else
        {
            gl->DrawArrays(top, firstVtx, vtxCnt); _SglThrowErrorOccuried();
        }
#endif
        dpu->numOfFedVertices += vtxCnt;
        dpu->numOfFedInstances += instCnt;

        //AfxLogEcho("Geometry drawn. vtxCnt %u, instCnt %u, firstVtx %u, firstInst %u", vtxCnt, instCnt, firstVtx, firstInst);
    }
}

_ZGL void _DpuDrawIndexed(sglDpu* dpu, avxDrawIndexedIndirectCmd const* data)
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

    _DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    dpu->activePip = dpu->nextPip;

    if (!dpu->activePip) AfxThrowError();
    else
    {
        dpu->activeVin = dpu->nextVin;
        _DpuBindAndSyncVin(dpu, dpu->activeVin, &dpu->nextVinBindings);


        SglFlushXformStateChanges(dpu);
        SglFlushRasterizationStateChanges(dpu);
        _SglFlushResourcingStateChanges(dpu);

        //if (dpu->nextVtxInStreamUpdMask || dpu->nextVtxInAttribUpdMask || dpu->flushIbb)
            //_SglFlushVertexInputStateChanges(dpu, TRUE);

        //AfxAssert(!dpu->nextVinBindingsMask);
        //AfxAssert(!dpu->nextVtxInAttribUpdMask);
        //AfxAssert(!dpu->flushIbb);

        AfxAssertObjects(1, &dpu->activeVin->bindings.idxSrcBuf, afxFcc_BUF);
        //AfxAssert(dpu->state.vertexBindingCnt);

        afxNat idxCnt = data->idxCnt;
        AfxAssert(idxCnt);
        //AfxAssert(cmd->instCnt);

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

        GLint vtxOff2 = data->vtxOff;
        afxNat32 firstIdx = data->firstIdx;
        afxNat32 instCnt = data->instCnt;
        afxNat32 firstInst = data->firstInst;
        afxSize dataOff = (idxBaseOff + (idxSiz * firstIdx));

        GLenum top = AfxToGlTopology(dpu->activeXformState.primTop);

        glVmt const* gl = &dpu->gl;

#if FORCE_GL_GENERIC_FUNCS
        AfxAssert(gl->DrawElementsInstancedBaseVertexBaseInstance);
        gl->DrawElementsInstancedBaseVertexBaseInstance(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, instCnt, vtxOff2, firstInst); _SglThrowErrorOccuried();
#else
        if (instCnt)
        {
            if (firstInst)
            {
                AfxAssert(gl->DrawElementsInstancedBaseVertexBaseInstance);
                gl->DrawElementsInstancedBaseVertexBaseInstance(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, instCnt, vtxOff2, firstInst); _SglThrowErrorOccuried();
            }
            else
            {
                if (vtxOff2)
                {
                    gl->DrawElementsInstancedBaseVertex(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, instCnt, vtxOff2); _SglThrowErrorOccuried();
                }
                else
                {
                    gl->DrawElementsInstanced(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, instCnt); _SglThrowErrorOccuried();
                }
            }
        }
        else
        {
            if (vtxOff2)
            {
                gl->DrawElementsBaseVertex(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, vtxOff2); _SglThrowErrorOccuried();
            }
            else
            {
                gl->DrawElements(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff); _SglThrowErrorOccuried();
            }
        }
#endif

        //dpu->numOfFedVertices += cmd->idxCnt;
        dpu->numOfFedIndices += idxCnt;
        dpu->numOfFedInstances += instCnt;

        //AfxLogEcho("Indexed geometry drawn. idxCnt %u, instCnt %u, baseIdx %u, vtxOff %u, baseInst %u", idxCnt, instCnt, firstIdx, vtxOff, firstInst);
    }
}

_ZGL void _DpuDrawIndirect(sglDpu* dpu, afxBuffer buf, afxNat32 offset, afxNat32 drawCnt, afxNat32 stride)
{
    afxError err = AFX_ERR_NONE;

    _DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    dpu->activePip = dpu->nextPip;

    if (!dpu->activePip) AfxThrowError();
    else
    {
        dpu->activeVin = dpu->nextVin;
        _DpuBindAndSyncVin(dpu, dpu->activeVin, &dpu->nextVinBindings);

        SglFlushXformStateChanges(dpu);
        SglFlushRasterizationStateChanges(dpu);
        _SglFlushResourcingStateChanges(dpu);

        glVmt const* gl = &dpu->gl;

        GLenum top = AfxToGlTopology(dpu->activeXformState.primTop);
        void* offPtr = (void*)offset;

        AfxAssertObjects(1, &buf, afxFcc_BUF);
        DpuBindAndSyncBuf(dpu, GL_DRAW_INDIRECT_BUFFER, buf);

#if FORCE_GL_GENERIC_FUNCS
        AfxAssert(gl->MultiDrawArraysIndirect);
        gl->MultiDrawArraysIndirect(top, offPtr, drawCnt, stride); _SglThrowErrorOccuried();
#else
        if (1 >= drawCnt)
        {
            gl->DrawArraysIndirect(top, offPtr); _SglThrowErrorOccuried();
        }
        else
        {
            AfxAssert(gl->MultiDrawArraysIndirect);
            gl->MultiDrawArraysIndirect(top, offPtr, drawCnt, stride); _SglThrowErrorOccuried();
        }
#endif
    }
}

_ZGL void _DpuDrawIndirectCount(sglDpu* dpu, afxBuffer buf, afxNat32 offset, afxBuffer cntBuf, afxNat32 cntBufOff, afxNat32 maxDrawCnt, afxNat32 stride)
{
    afxError err = AFX_ERR_NONE;

    _DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    dpu->activePip = dpu->nextPip;

    if (!dpu->activePip) AfxThrowError();
    else
    {
        dpu->activeVin = dpu->nextVin;
        _DpuBindAndSyncVin(dpu, dpu->activeVin, &dpu->nextVinBindings);

        SglFlushXformStateChanges(dpu);
        SglFlushRasterizationStateChanges(dpu);
        _SglFlushResourcingStateChanges(dpu);

        glVmt const* gl = &dpu->gl;

        GLenum top = AfxToGlTopology(dpu->activeXformState.primTop);
        void* offPtr = (void*)offset;

        AfxAssertObjects(1, &buf, afxFcc_BUF);
        DpuBindAndSyncBuf(dpu, GL_DRAW_INDIRECT_BUFFER, buf);
        AfxAssertObjects(1, &cntBuf, afxFcc_BUF);
        DpuBindAndSyncBuf(dpu, GL_PARAMETER_BUFFER, cntBuf);

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

        AfxAssert(gl->MultiDrawArraysIndirectCount);
        gl->MultiDrawArraysIndirectCount(top, offPtr, cntBufOff, maxDrawCnt, stride); _SglThrowErrorOccuried();
    }
}

_ZGL void _DpuDrawIndexedIndirect(sglDpu* dpu, afxBuffer buf, afxNat32 offset, afxNat32 drawCnt, afxNat32 stride)
{
    afxError err = AFX_ERR_NONE;

    _DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    dpu->activePip = dpu->nextPip;

    if (!dpu->activePip) AfxThrowError();
    else
    {
        dpu->activeVin = dpu->nextVin;
        _DpuBindAndSyncVin(dpu, dpu->activeVin, &dpu->nextVinBindings);

        SglFlushXformStateChanges(dpu);
        SglFlushRasterizationStateChanges(dpu);
        _SglFlushResourcingStateChanges(dpu);

        glVmt const* gl = &dpu->gl;

        static const GLenum idxSizGl[] =
        {
            0,
            GL_UNSIGNED_BYTE,
            GL_UNSIGNED_SHORT,
            0,
            GL_UNSIGNED_INT
        };
        afxSize idxSiz = dpu->activeVin->bindings.idxSrcSiz;
        GLenum top = AfxToGlTopology(dpu->activeXformState.primTop);
        void* offPtr = (void*)offset;

        AfxAssertObjects(1, &buf, afxFcc_BUF);
        DpuBindAndSyncBuf(dpu, GL_DRAW_INDIRECT_BUFFER, buf);

#if FORCE_GL_GENERIC_FUNCS
        AfxAssert(gl->MultiDrawElementsIndirect);
        gl->MultiDrawElementsIndirect(top, idxSizGl[idxSiz], offPtr, drawCnt, stride); _SglThrowErrorOccuried();
#else
        if (1 >= drawCnt)
        {
            gl->DrawElementsIndirect(top, idxSizGl[idxSiz], offPtr); _SglThrowErrorOccuried();
        }
        else
        {
            AfxAssert(gl->MultiDrawElementsIndirect);
            gl->MultiDrawElementsIndirect(top, idxSizGl[idxSiz], offPtr, drawCnt, stride); _SglThrowErrorOccuried();
        }
#endif
    }
}

_ZGL void _DpuDrawIndexedIndirectCount(sglDpu* dpu, afxBuffer buf, afxNat32 offset, afxBuffer cntBuf, afxNat32 cntBufOff, afxNat32 maxDrawCnt, afxNat32 stride)
{
    afxError err = AFX_ERR_NONE;

    _DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    dpu->activePip = dpu->nextPip;

    if (!dpu->activePip) AfxThrowError();
    else
    {
        dpu->activeVin = dpu->nextVin;
        _DpuBindAndSyncVin(dpu, dpu->activeVin, &dpu->nextVinBindings);

        SglFlushXformStateChanges(dpu);
        SglFlushRasterizationStateChanges(dpu);
        _SglFlushResourcingStateChanges(dpu);

        glVmt const* gl = &dpu->gl;

        static const GLenum idxSizGl[] =
        {
            0,
            GL_UNSIGNED_BYTE,
            GL_UNSIGNED_SHORT,
            0,
            GL_UNSIGNED_INT
        };
        afxSize idxSiz = dpu->activeVin->bindings.idxSrcSiz;
        GLenum top = AfxToGlTopology(dpu->activeXformState.primTop);
        void* offPtr = (void*)offset;

        AfxAssertObjects(1, &buf, afxFcc_BUF);
        DpuBindAndSyncBuf(dpu, GL_DRAW_INDIRECT_BUFFER, buf);
        AfxAssertObjects(1, &cntBuf, afxFcc_BUF);
        DpuBindAndSyncBuf(dpu, GL_PARAMETER_BUFFER, cntBuf);

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

        AfxAssert(gl->MultiDrawElementsIndirectCount);
        gl->MultiDrawElementsIndirectCount(top, idxSizGl[idxSiz], offPtr, cntBufOff, maxDrawCnt, stride); _SglThrowErrorOccuried();
    }
}

_ZGL void _DpuDispatch(sglDpu* dpu, afxNat grpCntX, afxNat grpCntY, afxNat grpCntZ)
{
    afxError err = AFX_ERR_NONE;

    _DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    dpu->activePip = dpu->nextPip;

    if (!dpu->activePip) AfxThrowError();
    else
    {
        _SglFlushResourcingStateChanges(dpu);

        glVmt const* gl = &dpu->gl;

        AfxAssert(gl->DispatchCompute);
        gl->DispatchCompute(grpCntX, grpCntY, grpCntZ); _SglThrowErrorOccuried();
    }
}

_ZGL void _DpuDispatchIndirect(sglDpu* dpu, afxBuffer buf, afxNat32 offset)
{
    afxError err = AFX_ERR_NONE;

    _DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    dpu->activePip = dpu->nextPip;

    if (!dpu->activePip) AfxThrowError();
    else
    {
        _SglFlushResourcingStateChanges(dpu);

        glVmt const* gl = &dpu->gl;

        AfxAssertObjects(1, &buf, afxFcc_BUF);
        DpuBindAndSyncBuf(dpu, GL_DISPATCH_INDIRECT_BUFFER, buf);

        AfxAssert(gl->DispatchComputeIndirect);
        gl->DispatchComputeIndirect(offset); _SglThrowErrorOccuried();
    }
}

_ZGL void _DpuCmdUpdateUniformVector(sglDpu* dpu, _sglCmdUniformVectorEXT const* cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    GLuint loc = gl->GetUniformLocation(dpu->activePip->glHandle, cmd->name);
    gl->Uniform4fv(loc, cmd->cnt, cmd->v);
}

_ZGL void _DpuCmdUpdateUniformMatrix(sglDpu* dpu, _sglCmdUniformMatrixEXT const* cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    GLuint loc = gl->GetUniformLocation(dpu->activePip->glHandle, cmd->name);
    //gl->UniformMatrix4fv(loc, cmd->cnt, GL_FALSE, cmd->m);
    AfxThrowError();
}

// QUEUE STUFF

_ZGL afxError _DpuExecSubmCallback(sglDpu* dpu, afxDrawBridge dexu, afxNat queIdx, _sglQueueingWork* subm)
{
    afxError err = AFX_ERR_NONE;
    subm->subm.f(dpu, dexu, queIdx, subm->subm.udd);
    return err;
}

_ZGL afxError _DpuExecSubm(sglDpu* dpu, afxDrawBridge dexu, afxNat queIdx, _sglQueueingExecute* subm)
{
    afxError err = AFX_ERR_NONE;
    //AfxAssertObject(dexu, afxFcc_DEXU);
    //glVmt const* gl = &thdIdd->wglVmt;

    for (afxNat i = 0; i < subm->cmdbCnt; i++)
    {
        avxCmdb cmdb = subm->cmdbs[i];
        AfxAssertObjects(1, &cmdb, afxFcc_CMDB);

        if (cmdb->m.state == avxCmdbState_PENDING)
        {
            AfxAssert(cmdb->m.state == avxCmdbState_PENDING);

            _sglCmd * cmdHdr;
            AfxChainForEveryLinkageB2F(&cmdb->commands, _sglCmd, script, cmdHdr)
            {
                if (cmdHdr->id == NIL/*ZGL_CMD_END*/)
                    break;

                if (cmdb->m.state != avxCmdbState_PENDING)
                {
                    AfxThrowError();
                    break;
                }

                AfxAssertRange(ZGL_CMD_TOTAL, cmdHdr->id, 1);

                // This shit breaks CPU prediction. Just ignore it for now.

                if (err)
                {
                    int a = 0;
                }

                _SglDecodeCmdVmt.f[cmdHdr->id](dpu, cmdHdr);
            }

            if (!err)
            {
                cmdb->m.state = avxCmdbState_EXECUTABLE;
            }

            if (err || cmdb->m.disposable)
            {
                AfxAssert(cmdb->m.portIdx == dpu->portIdx);
                cmdb->m.state = avxCmdbState_INVALID;
                afxDrawQueue dque = AfxGetParent(cmdb);
                AfxAssertObjects(1, &dque, afxFcc_DQUE);

                afxNat poolIdx = cmdb->m.poolIdx;

                AfxEnterSlockExclusive(&dque->m.cmdbReqLock);

                if (AfxPushQueue(&dque->m.cmdbRecycQue, &cmdb))
                {
                    AfxReleaseObjects(1, (void**)&cmdb);
                }

                AfxExitSlockExclusive(&dque->m.cmdbReqLock);
            }
        }
        AfxDecAtom32(&cmdb->m.submCnt);
    }
    return err;
}

_ZGL afxError _DpuExecuteSubmMmap(sglDpu* dpu, afxDrawBridge dexu, afxNat queIdx, _sglQueueingMmap* subm)
{
    afxError err = AFX_ERR_NONE;
    
    if (_DpuRemapBuf(dpu, subm->buf, subm->off, subm->ran, subm->flags, subm->placeholder))
        AfxThrowError();

    AfxDecAtom32(&subm->buf->m.pendingRemap);

    return err;
}

_ZGL afxError _DpuExecSubmTransfer(sglDpu* dpu, afxDrawBridge dexu, afxNat queIdx, _sglQueueingTransfer* subm)
{
    afxError err = AFX_ERR_NONE;
    //AfxAssertObject(dexu, afxFcc_DEXU);

    switch (subm->srcFcc)
    {
    case NIL:
    {
        switch (subm->dstFcc)
        {
        case afxFcc_BUF: // raw to buf
            if (_DpuLoadBuf2(dpu, subm->data.buf, subm->data.src, subm->itemCnt, subm->items.bufIo)) AfxThrowError();
            break;
        case afxFcc_RAS: // raw to ras
            if (_DpuLoadRas(dpu, subm->img.ras, subm->img.src, subm->itemCnt, subm->items.rasIo)) AfxThrowError();
            break;
        default: AfxThrowError(); break;
        }
        break;
    }
    case afxFcc_BUF:
    {
        switch (subm->dstFcc)
        {
        case NIL: // buf to raw
            if (_DpuStoreBuf2(dpu, subm->data.buf, subm->data.dst, subm->itemCnt, subm->items.bufIo)) AfxThrowError();
            break;
        case afxFcc_RAS:
            if (_DpuUnpackRas(dpu, subm->img.ras, subm->img.buf, subm->itemCnt, subm->items.rasIo)) AfxThrowError();
            break;
        case afxFcc_IOB: // buf to iob
            if (_DpuOutputBuf2(dpu, subm->data.buf, subm->data.iob, subm->itemCnt, subm->items.bufIo)) AfxThrowError();
            break;
        default: AfxThrowError(); break;
        }
        break;
    }
    case afxFcc_RAS:
    {
        switch (subm->dstFcc)
        {
        case NIL: // ras to raw
            if (_DpuStoreRas(dpu, subm->img.ras, subm->img.dst, subm->itemCnt, subm->items.rasIo)) AfxThrowError();
            break;
        case afxFcc_BUF:
            if (_DpuPackRas(dpu, subm->img.ras, subm->img.buf, subm->itemCnt, subm->items.rasIo)) AfxThrowError();
            break;
        case afxFcc_IOB: // ras to iob
            if (_DpuOutputRas(dpu, subm->img.ras, subm->img.iob, subm->itemCnt, subm->items.rasIo)) AfxThrowError();
            break;
        default: AfxThrowError(); break;
        }
        break;
    }
    case afxFcc_IOB:
    {
        switch (subm->dstFcc)
        {
        case afxFcc_BUF: // iob to buf
            if (_DpuInputBuf2(dpu, subm->data.buf, subm->data.iob, subm->itemCnt, subm->items.bufIo)) AfxThrowError();
            break;
        case afxFcc_RAS: // iob to ras
            if (_DpuInputRas(dpu, subm->img.ras, subm->img.iob, subm->itemCnt, subm->items.rasIo)) AfxThrowError();
            break;
        default: AfxThrowError(); break;
        }
        break;
    }
    default: AfxThrowError(); break;
    }
    return err;
}

#if 0
_ZGL afxError _DpuExecSubmStamp(sglDpu* dpu, afxDrawBridge dexu, afxNat queIdx, _sglQueueingStamp* subm)
{
    afxError err = AFX_ERR_NONE;

    for (afxNat itemIdx = 0; itemIdx < subm->itemCnt; itemIdx++)
    {
        afxDrawOutput dout = subm->items[itemIdx].dout;
        AfxAssertObjects(1, &dout, afxFcc_DOUT);
        afxNat outBufIdx = subm->items[itemIdx].bufIdx;

        afxDrawContext dctx = dpu->activeDctx;
        AfxAssertObjects(1, &dctx, afxFcc_DCTX);

        afxWhd whd;
        avxCanvas canv;
        AfxGetDrawOutputCanvases(dout, outBufIdx, 1, &canv);
        AfxGetCanvasExtent(canv, whd);

        afxRect area = { 0 };
        area.extent[0] = whd[0];
        area.extent[1] = whd[1];
        avxDrawTarget const dt = { .loadOp = avxLoadOp_LOAD,.storeOp = avxStoreOp_STORE };
        _DpuBeginDrawScope(dpu, canv, &area, whd[2], 1, &dt, NIL, NIL, FALSE);

        afxViewport vp;
        vp.extent[0] = whd[0];
        vp.extent[1] = whd[1];
        vp.depth[1] = 1;
        _DpuSetViewports(dpu, 0, 1, &vp);

        _DpuBindRasterizer(dpu, dctx->fntRazr, NIL, NIL);

        akxViewConstants vc;
        AfxM4dReset(vc.v);
        AfxComputeOffcenterOrthographicMatrix(vc.p, 0, vp.extent[0], 0, vp.extent[1], -1.f, 1.f, &AVX_CLIP_SPACE_OPENGL);
        //AfxComputeBasicOrthographicMatrix(vc.p, vp.extent[0] / vp.extent[1], 1.0, 3.0, &AVX_CLIP_SPACE_OPENGL);
        DpuBufRw(dpu, dctx->fntUnifBuf, 0, sizeof(vc), FALSE, &vc);

        _DpuBindBuffers(dpu, 0, 0, 1, &dctx->fntUnifBuf, (afxNat const[]) {0}, (afxNat const[]) { 0 });

        _DpuBindSamplers(dpu, 0, 1, 1, &dctx->fntSamp);
        _DpuBindRasters(dpu, 0, 1, 1, &dctx->fntRas, NIL);

        afxReal x = subm->origin[0];
        afxReal y = subm->origin[1];
        afxNat numchar = subm->caption.str.len;
        afxReal r = 1, g = 1, b = 1;
        afxReal x2 = x;

        afxReal* verts;
        _DpuRemapBuf(dpu, dctx->fntDataBuf, 0, AfxGetBufferCapacity(dctx->fntDataBuf, 0), NIL, (void**)&verts);
        //afxReal* verts = (void*)dctx->fntDataBuf->m.bytemap;// AfxMapBuffer(dctx->fntDataBuf, 0, 2048, afxBufferAccess_W, TRUE); // TODO map directly
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

        _DpuRemapBuf(dpu, dctx->fntDataBuf, 0, 0, NIL, NIL);
        //AfxUnmapBuffer(dctx->fntDataBuf, FALSE);

        sglBufferInfo bufi = { 0 };
        bufi.buf = dctx->fntDataBuf;
        bufi.stride = 20;
        _DpuBindVertexSources(dpu, 0, 1, &bufi);
        
        avxDrawIndirectCmd dic;
        dic.vtxCnt = 4;
        dic.firstInst = 0;
        dic.firstVtx = 0;
        dic.instCnt = numchar;
        _DpuDraw(dpu, &dic);

        _DpuEndDrawScope(dpu);
        AfxDecAtom32(&dout->submCnt);
    }

    return err;
}
#endif

_ZGL void* _AfxDqueRequestArenaSpace(afxDrawBridge dexu, afxNat queIdx, afxNat siz)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);
    afxDrawQueue dque = dexu->m.queues[queIdx];
    //AfxEnterSlockExclusive(&dque->m.workArenaSlock);
    void *block = AfxAllocateArena(&dque->m.workArena, siz);

    if (!block)
        AfxThrowError();

    //AfxExitSlockExclusive(&dque->m.workArenaSlock);
    return block;
}

_ZGL void _AfxDqueRecycleArenaSpace(afxDrawBridge dexu, afxNat queIdx, void *block, afxNat siz)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);
    afxDrawQueue dque = dexu->m.queues[queIdx];
    //AfxEnterSlockExclusive(&dque->m.workArenaSlock);
    AfxAssert(block);
    AfxRecycleArena(&dque->m.workArena, block, siz);
    //AfxExitSlockExclusive(&dque->m.workArenaSlock);
}

_ZGL afxBool _DexuProcCb(afxDrawBridge dexu, afxThread thr)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);
    AfxAssertObjects(1, &thr, afxFcc_THR);

    afxNat portIdx = dexu->m.portIdx;
    sglDpu *dpu = &dexu->dpu;
        
    AfxAssert(portIdx == dexu->m.portIdx);

    for (afxNat queIdx = 0; queIdx < dexu->m.queCnt; queIdx++)
    {
        afxDrawQueue dque = dexu->m.queues[queIdx];

        if (AfxTryLockMutex(&dque->m.workChnMtx))
        {
            afxBool ctxEntered = FALSE;
            afxDrawContext dctx = AfxGetDrawBridgeContext(dexu);

            if (!ctxEntered)
            {
                HGLRC rc = wglGetCurrentContextSIG();

                if (rc == dexu->hRC) ctxEntered = TRUE;
                else
                {
                    if (!(ctxEntered = !!wglMakeContextCurrentSIG(dexu->hDC, dexu->hDC, dexu->hRC)))
                    {
                        AfxThrowError();
                        AfxLogError("DPU %u failed to take active context.", portIdx);
                    }
                }

                if (ctxEntered)
                    dpu->activeDctx = dctx;
            }

            if (ctxEntered)
            {
                dpu->numOfFedVertices = 0;
                dpu->numOfFedIndices = 0;
                dpu->numOfFedInstances = 0;

                afxDrawQueue dque = dexu->m.queues[queIdx];

                _sglQueueing* subm;
                AfxChainForEveryLinkageB2F(&dque->m.workChn, _sglQueueing, chain, subm)
                {
                    AfxGetTime(&subm->pullTime);
                    subm->exec(dpu, dexu, queIdx, subm);
                    AfxGetTime(&subm->complTime);

                    AfxPopLinkage(&subm->chain);
                    _AfxDqueRecycleArenaSpace(dexu, queIdx, subm, subm->siz);
                }
            }

            AfxUnlockMutex(&dque->m.workChnMtx);
            AfxSignalCondition(&dque->m.idleCnd);
        }
    }
    return TRUE;
}

_ZGL afxNat _SglDexuRequestSubmCb(afxDrawBridge dexu, afxFence fenc, afxNat cnt, avxQueueOp const req[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);
    AfxAssert(cnt);
    AfxAssert(req);
    afxNat queIdx2 = AFX_INVALID_INDEX;
    afxBool queued = FALSE;
    afxNat iterFailCnt = 0;

    do
    {
        for (afxNat queIdx = 0; queIdx < dexu->m.queCnt; queIdx++)
        {
            afxDrawQueue dque = dexu->m.queues[queIdx];

            if (AfxTryLockMutex(&dque->m.workChnMtx))
            {
                for (afxNat i = 0; i < cnt; i++)
                {
                    _sglQueueingWork* subm;
                    afxNat queuingSiz = 0;
                    
                    switch (req[i].submType)
                    {
                    case 0:
                    {
                        queuingSiz = sizeof(*subm);
                        subm = _AfxDqueRequestArenaSpace(dexu, queIdx, queuingSiz);
                        *subm = (_sglQueueingWork) { 0 };
                        subm->hdr.siz = queuingSiz;
                        AfxGetTime(&subm->hdr.pushTime);
                        subm->hdr.exec = (void*)_DpuExecSubmCallback;
                        subm->subm.f = req[i].f;
                        subm->subm.udd = req[i].udd;
                        break;
                    }
                    case avxQueueOp_MAPPING:
                    {
                        queuingSiz = sizeof(*subm) + sizeof(((avxQueueOpData*)0)->map);
                        subm = _AfxDqueRequestArenaSpace(dexu, queIdx, queuingSiz);
                        *subm = (_sglQueueingWork) { 0 };
                        subm->hdr.siz = queuingSiz;
                        AfxGetTime(&subm->hdr.pushTime);
                        subm->hdr.exec = (void*)_DpuExecuteSubmMmap;
                        avxQueueOpData* data = &subm->data[0];
                        data->map = req[i].data->map;
                        
                        for (afxNat j = 0; j < data->present.bufCnt; j++)
                        {
                            AfxIncAtom32(&data->map.buf->m.pendingRemap);
                        }
                        break;
                    }
                    default:
                    {
                        AfxThrowError();
                        break;
                    }
                    };

                    if (!err)
                        AfxPushLinkage(&subm->hdr.chain, &dque->m.workChn);
                }

                AfxUnlockMutex(&dque->m.workChnMtx);
                queued = TRUE;
                queIdx2 = queIdx;
                break;
            }
        }

        if (queued)
        {
            dexu->schedCnt += 1;
            AfxSignalCondition2(&dexu->schedCnd);
            break;
        }
        else AfxYield();

    } while (1);

    return queIdx2;
}

_ZGL afxNat _SglDexuEnqueueExecuteCb(afxDrawBridge dexu, avxSubmission const* ctrl, afxNat cnt, avxCmdb cmdbs[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);
    AfxAssert(cnt);
    AfxAssert(cmdbs);
    afxNat queIdx2 = AFX_INVALID_INDEX;
    afxBool queued = FALSE;

    do
    {
        for (afxNat queIdx = 0; queIdx < dexu->m.queCnt; queIdx++)
        {
            afxDrawQueue dque = dexu->m.queues[queIdx];

            if (AfxTryLockMutex(&dque->m.workChnMtx))
            {
                _sglQueueingExecute* subm;
                afxNat queuingSiz = sizeof(*subm) + (cnt * sizeof(subm->cmdbs[0]));
                subm = _AfxDqueRequestArenaSpace(dexu, queIdx, queuingSiz);
                *subm = (_sglQueueingExecute) { 0 };
                subm->hdr.siz = queuingSiz;
                AfxGetTime(&subm->hdr.pushTime);
                subm->hdr.exec = (void*)_DpuExecSubm;
                subm->cmdbCnt = cnt;

                for (afxNat i = 0; i < cnt; i++)
                {
                    avxCmdb cmdb = cmdbs[i];
                    AfxAssertObjects(1, &cmdb, afxFcc_CMDB);

                    subm->cmdbs[i] = cmdb;
                    AfxIncAtom32(&subm->cmdbs[i]->m.submCnt);
                    subm->cmdbs[i]->m.state = avxCmdbState_PENDING;
                }

                AfxPushLinkage(&subm->hdr.chain, &dque->m.workChn);
                AfxUnlockMutex(&dque->m.workChnMtx);
                queued = TRUE;
                queIdx2 = queIdx;
                break;
            }
        }

        if (queued)
        {
            dexu->schedCnt += 1;
            AfxSignalCondition2(&dexu->schedCnd);
            break;
        }
        else AfxYield();

    } while (1);

    return queIdx2;
}

_ZGL afxNat _SglDexuEnqueueTransferCb(afxDrawBridge dexu, avxTransference const* ctrl, afxNat opCnt, void const* ops)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);
    AfxAssert(opCnt);
    AfxAssert(ops);
    afxNat queIdx2 = AFX_INVALID_INDEX;
    afxBool queued = FALSE;

    do
    {
        for (afxNat queIdx = 0; queIdx < dexu->m.queCnt; queIdx++)
        {
            afxDrawQueue dque = dexu->m.queues[queIdx];

            if (AfxTryLockMutex(&dque->m.workChnMtx))
            {
                _sglQueueingTransfer* subm;
                afxNat queuingSiz = sizeof(*subm) + (opCnt * AfxMax(sizeof(subm->items.bufIo[0]), sizeof(subm->items.rasIo[0])));
                subm = _AfxDqueRequestArenaSpace(dexu, queIdx, queuingSiz);
                *subm = (_sglQueueingTransfer) { 0 };
                subm->hdr.siz = queuingSiz;
                AfxGetTime(&subm->hdr.pushTime);
                subm->hdr.exec = (void*)_DpuExecSubmTransfer;

                subm->srcFcc = ctrl->srcFcc;
                subm->dstFcc = ctrl->dstFcc;
                subm->codec = ctrl->codec;
                subm->decSiz = ctrl->decSiz;

                subm->itemCnt = opCnt;

                for (afxNat i = 0; i < opCnt; i++)
                {
                    if (ctrl->srcFcc == afxFcc_RAS || ctrl->dstFcc == afxFcc_RAS)
                    {
                        AfxCopy(&subm->img, &ctrl->img, sizeof(ctrl->img));
                        afxRasterIo const* riops = ops;
                        subm->items.rasIo[i] = riops[i];
                    }
                    else
                    {
                        AfxAssert((ctrl->srcFcc == afxFcc_BUF || ctrl->dstFcc == afxFcc_BUF));
                        AfxCopy(&subm->data, &ctrl->data, sizeof(ctrl->data));
                        afxBufferIo const* biops = ops;
                        subm->items.bufIo[i] = biops[i];
                    }
                }

                AfxPushLinkage(&subm->hdr.chain, &dque->m.workChn);
                AfxUnlockMutex(&dque->m.workChnMtx);
                queued = TRUE;
                queIdx2 = queIdx;
                break;
            }
        }

        if (queued)
        {
            dexu->schedCnt += 1;
            AfxSignalCondition2(&dexu->schedCnd);
            break;
        }
        else AfxYield();

    } while (1);

    return queIdx2;
}

_ZGL afxNat _SglDexuEnqueueMmapCb(afxDrawBridge dexu, afxBuffer buf, afxSize off, afxNat ran, afxFlags flags, void** placeholder)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);
    afxNat queIdx2 = AFX_INVALID_INDEX;
    afxBool queued = FALSE;

    do
    {
        for (afxNat queIdx = 0; queIdx < dexu->m.queCnt; queIdx++)
        {
            afxDrawQueue dque = dexu->m.queues[queIdx];

            if (AfxTryLockMutex(&dque->m.workChnMtx))
            {
                _sglQueueingMmap* subm;
                afxNat queuingSiz = sizeof(*subm);
                subm = _AfxDqueRequestArenaSpace(dexu, queIdx, queuingSiz);
                *subm = (_sglQueueingMmap) { 0 };
                subm->hdr.siz = queuingSiz;
                subm->hdr.exec = (void*)_DpuExecuteSubmMmap;
                AfxGetTime(&subm->hdr.pushTime);

                subm->buf = buf;
                subm->off = off;
                subm->ran = ran;
                subm->flags = flags;
                subm->placeholder = placeholder;

                AfxIncAtom32(&buf->m.pendingRemap);

                AfxPushLinkage(&subm->hdr.chain, &dque->m.workChn);
                AfxUnlockMutex(&dque->m.workChnMtx);
                queued = TRUE;
                queIdx2 = queIdx;
                break;
            }
        }

        if (queued)
        {
            dexu->schedCnt += 1;
            AfxSignalCondition2(&dexu->schedCnd);
            break;
        }
        else AfxYield();

    } while (1);

    return queIdx2;
}

_ZGL afxInt DexuThrProc(afxDrawBridge dexu)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);

    sglDpu* dpu = &dexu->dpu;
    glVmt const* gl = &dpu->gl;

    wglMakeContextCurrentSIG(dexu->hDC, dexu->hDC, dexu->hRC);

    AfxMakeString(&dexu->subsysName, 0, (afxChar const*)gl->GetString(GL_RENDERER), 0); _SglThrowErrorOccuried();
    AfxMakeString(&dexu->subsysVer, 0, (afxChar const*)gl->GetString(GL_VERSION), 0); _SglThrowErrorOccuried();

    AfxDbgLogf(6, NIL, "SIGMA GL/2 Draw Execution Unit %.03u\n\t%.*s %.*s", dexu->m.portIdx, AfxPushString(&dexu->subsysName), AfxPushString(&dexu->subsysVer));

#if 0
    AfxLogAssistence("%s", dpu->wgl.GetExtensionsStringARB(dpu->dc));
#endif

    dpu->exuIdx = dexu->m.exuIdx;
    dpu->portIdx = dexu->m.portIdx;

    // RESET DEFAULT STATE TO OPENGL DEFAULTS

    dpu->activeRasterState.depthCompareOp = avxCompareOp_LESS;
    dpu->activeRasterState.depthWriteDisabled = FALSE;
    dpu->activeRasterState.depthBounds[0] = 0.f;
    dpu->activeRasterState.depthBounds[1] = 1.f;
    dpu->activeRasterState.stencilFront.compareOp = avxCompareOp_ALWAYS;
    dpu->activeRasterState.stencilFront.reference = 0;
    dpu->activeRasterState.stencilFront.compareMask = (dpu->activeRasterState.stencilFront.writeMask = 0xFFFFFFFF);
    dpu->activeRasterState.stencilFront.failOp = (dpu->activeRasterState.stencilFront.passOp = (dpu->activeRasterState.stencilFront.depthFailOp = avxStencilOp_KEEP));
    dpu->activeRasterState.stencilBack.compareOp = avxCompareOp_ALWAYS;
    dpu->activeRasterState.stencilBack.reference = 0;
    dpu->activeRasterState.stencilBack.compareMask = (dpu->activeRasterState.stencilBack.writeMask = 0xFFFFFFFF);
    dpu->activeRasterState.stencilBack.failOp = (dpu->activeRasterState.stencilBack.passOp = (dpu->activeRasterState.stencilBack.depthFailOp = avxStencilOp_KEEP));
    dpu->activeRasterState.depthBiasConstFactor = 0.f;
    dpu->activeRasterState.depthBiasSlopeScale = 0.f;

    for (afxNat i = 0; i < 8; i++)
    {
        dpu->activeRasterState.outs[i].blendConfig.aBlendOp = avxBlendOp_ADD;
        dpu->activeRasterState.outs[i].blendConfig.rgbBlendOp = avxBlendOp_ADD;
        dpu->activeRasterState.outs[i].blendConfig.aSrcFactor = avxBlendFactor_ONE;
        dpu->activeRasterState.outs[i].blendConfig.rgbSrcFactor = avxBlendFactor_ONE;
        dpu->activeRasterState.outs[i].blendConfig.aDstFactor = avxBlendFactor_ZERO;
        dpu->activeRasterState.outs[i].blendConfig.rgbDstFactor = avxBlendFactor_ZERO;
        dpu->activeRasterState.outs[i].discardMask = NIL;
    }

    // In Qwadro specification, it is enabled by default. As SIGMA GL/2 is only bound to Qwadro, actually there is not way of disable it.
    gl->Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS); _SglThrowErrorOccuried();
    //gl->ClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE); _SglThrowErrorOccuried();// set screen origin to top to bottm, and set depth to [ 0, 1 ]

#if _AFX_DEBUG
    gl->Enable(GL_DEBUG_OUTPUT);
    gl->Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    gl->DebugMessageCallback(_glDbgMsgCb, NIL);
    //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

    gl->GenVertexArrays(1, &dpu->emptyVao); _SglThrowErrorOccuried();
    gl->BindVertexArray(dpu->emptyVao); _SglThrowErrorOccuried();
    gl->BindVertexArray(0); _SglThrowErrorOccuried();

    gl->GenFramebuffers(1, &dpu->fboOpSrc); _SglThrowErrorOccuried();
    gl->BindFramebuffer(GL_FRAMEBUFFER, dpu->fboOpSrc); _SglThrowErrorOccuried();
    gl->GenFramebuffers(1, &dpu->fboOpDst); _SglThrowErrorOccuried();
    gl->BindFramebuffer(GL_FRAMEBUFFER, dpu->fboOpDst); _SglThrowErrorOccuried();
    gl->BindFramebuffer(GL_FRAMEBUFFER, 0); _SglThrowErrorOccuried();

    dpu->fboOpSrcAnnex = NIL;
    dpu->fboOpDstAnnex = NIL;

    dpu->instanced = TRUE;
    dpu->running = TRUE;

    while (1)
    {
        AfxLockMutex(&dexu->schedCndMtx);

        while (!dexu->schedCnt)
            AfxWaitCondition(&dexu->schedCnd, &dexu->schedCndMtx);

        dexu->schedCnt = 0;

        afxThread thr;
        AfxGetThread(&thr);
        AfxAssert(thr == dexu->worker);
        _DexuProcCb(dexu, dexu->worker);

        _SglProcessDeletionQueue(gl, &dexu->deletionQueue);

        AfxUnlockMutex(&dexu->schedCndMtx);
    }

    gl->DeleteVertexArrays(1, &dpu->emptyVao); _SglThrowErrorOccuried();
    gl->DeleteFramebuffers(1, &dpu->fboOpSrc); _SglThrowErrorOccuried();
    gl->DeleteFramebuffers(1, &dpu->fboOpDst); _SglThrowErrorOccuried();

    wglMakeContextCurrentSIG(NIL, NIL, NIL);

    return 0;
}

_ZGL afxError _SglDexuStartCb(afxDrawBridge dexu)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);

    if (!AfxIsThreadRunning(dexu->worker))
    {
        AfxRunThread(dexu->worker, (void*)DexuThrProc, dexu);
    }
    return err;
}

_ZGL afxError _SglDexuStopCb(afxDrawBridge dexu)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);

    afxInt exitCode = 0;
    do
    {
        AfxRequestThreadInterruption(dexu->worker);
        AfxWaitForDrawContext(dexu->m.dctx);
    } while (!AfxWaitForThreadExit(dexu->worker, &exitCode));
    
    return err;
}

_ZGL afxError _SglDexuPingCb(afxDrawBridge dexu, afxNat queIdx)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);
    AfxSignalCondition(&dexu->schedCnd);
    return err;
}

_ZGL afxError _SglDexuCtorCb(afxDrawBridge dexu, void** args, afxNat invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);

    afxDrawDevice ddev = args[0];
    AfxAssertObjects(1, &ddev, afxFcc_DDEV);
    afxDrawContext dctx = args[1];
    AfxAssertObjects(1, &dctx, afxFcc_DCTX);
    afxDrawBridgeConfig const *cfg = ((afxDrawBridgeConfig const *)args[2]);
    AfxAssert(cfg);
    afxNat exuIdx = *(afxNat*)(args[3]);

    if (_AvxDexuStdImplementation.ctor(dexu, args, invokeNo)) AfxThrowError();
    else
    {
        for (afxNat i = 0; i < dexu->m.queCnt; i++)
        {
            afxDrawQueue dque = dexu->m.queues[i];
            AfxDeregisterClass(&dque->m.cmdbCls);
            afxClassConfig tmpClsCfg = _SglCmdbMgrCfg;
            tmpClsCfg.fixedSiz = sizeof(AFX_OBJECT(avxCmdb)),
            AfxRegisterClass(&dque->m.cmdbCls, NIL, &dque->m.classes, &tmpClsCfg);
        }

        AfxZero(&dexu->dpu, sizeof(dexu->dpu));

        if (_SglCreateHwSurface(&ddev->idd->wndClss, &dexu->hWnd, &dexu->hDC, &dexu->dcPxlFmt, &dexu->dcPfd)) AfxThrowError();
        else
        {
            HDC bkpHdc = wglGetCurrentDrawDCSIG();
            HGLRC bkpGlrc = wglGetCurrentContextSIG();

            HGLRC hShareRC = dctx->hPrimeRC;

            if (_SglCreateHwContext(ddev->idd->oglDll, dexu->hDC, hShareRC, &dexu->hRC, &dexu->dpu.gl)) AfxThrowError();
            else
            {
                dexu->m.submCb = _SglDexuRequestSubmCb;
                dexu->m.submitCb = _SglDexuEnqueueExecuteCb;
                dexu->m.transferCb = _SglDexuEnqueueTransferCb;
                dexu->m.pingCb = _SglDexuPingCb;

                afxThreadConfig thrCfg = { 0 };
                //thrCfg.procCb = DrawThreadProc;
                thrCfg.purpose = afxThreadPurpose_DRAW;
                thrCfg.udd[0] = dexu;
                AfxAcquireThreads(AfxHere(), &thrCfg, 1, &dexu->worker);

                AfxDeployInterlockedQueue(&dexu->deletionQueue, sizeof(_sglDeleteGlRes), 32);

                AfxSetUpMutex(&dexu->schedCndMtx, AFX_MTX_PLAIN);
                AfxSetUpCondition(&dexu->schedCnd);
                dexu->schedCnt = 0;

                _SglDexuStartCb(dexu);

                if (err)
                    wglDeleteContextSIG(dexu->hRC);
            }

            if (err)
                ReleaseDC(dexu->hWnd, dexu->hDC), DestroyWindow(dexu->hWnd);

            wglMakeContextCurrentSIG(bkpHdc, bkpHdc, bkpGlrc);
        }

        if (err)
            _AvxDexuStdImplementation.dtor(dexu);
    }
    return err;
}

_ZGL afxError _SglDexuDtorCb(afxDrawBridge dexu)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);

    afxDrawDevice ddev = AfxGetDrawBridgeDevice(dexu);

    afxInt exitCode;
    AfxWaitForThreadExit(dexu->worker, &exitCode);
    AfxReleaseObjects(1, &dexu->worker);

    AfxWaitForDrawContext(dexu->m.dctx);
    AfxWaitForDrawContext(dexu->m.dctx); // yes, two times.
    AfxExhaustChainedClasses(&dexu->m.classes);
    AfxWaitForDrawContext(dexu->m.dctx);

    AfxCleanUpChainedClasses(&dexu->m.classes);
    _AvxDexuStdImplementation.dtor(dexu);

    wglDeleteContextSIG(dexu->hRC);
    ReleaseDC(dexu->hWnd, dexu->hDC);
    DestroyWindow(dexu->hWnd);

    AfxCleanUpCondition(&dexu->schedCnd);
    AfxCleanUpMutex(&dexu->schedCndMtx);

    AfxDismantleInterlockedQueue(&dexu->deletionQueue);

    return err;
}
