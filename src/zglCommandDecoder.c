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
// BIND RESOURCES

_ZGL void _DecodeCmdStampDebug(zglDpu* dpu, _avxCmd const* cmd)
{
    //
}

_ZGL void _DecodeCmdBindPipeline(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuBindPipeline(dpu, cmd->BindPipeline.pip, cmd->BindPipeline.vin, cmd->BindPipeline.dynamics);
}

_ZGL void _DecodeCmdBindShadersEXT(zglDpu* dpu, _avxCmd const* cmd)
{
    for (afxUnit i = 0; i < cmd->BindShadersEXT.cnt; i++)
    {
        DpuBindShadersEXT(dpu, cmd->BindShadersEXT.stages[i].stage, cmd->BindShadersEXT.stages[i].shd);
    }
}

_ZGL void _DecodeCmdBindBuffers(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuBindBuffers(dpu, cmd->BindBuffers.set, cmd->BindBuffers.pin, cmd->BindBuffers.cnt, cmd->BindBuffers.maps);
}

_ZGL void _DecodeCmdBindRasters(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuBindRasters(dpu, cmd->BindRasters.set, cmd->BindRasters.pin, cmd->BindRasters.cnt, cmd->BindRasters.rasters);
}

_ZGL void _DecodeCmdBindSamplers(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuBindSamplers(dpu, cmd->BindSamplers.set, cmd->BindSamplers.pin, cmd->BindSamplers.cnt, cmd->BindSamplers.samplers);
}

_ZGL void _DecodeCmdBindVertexBuffers(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuBindVertexBuffers(dpu, cmd->BindVertexBuffers.basePin, cmd->BindVertexBuffers.cnt, cmd->BindVertexBuffers.src);
}

_ZGL void _DecodeCmdBindIndexBuffer(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuBindIndexBuffer(dpu, cmd->BindIndexBuffer.buf, cmd->BindIndexBuffer.offset, cmd->BindIndexBuffer.range, cmd->BindIndexBuffer.idxSiz);
}

_ZGL void _DecodeCmdUseVertexInput(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuBindVertexInput(dpu, cmd->UseVertexInput.vin);
}

_ZGL void _DecodeCmdPushConstants(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuPushConstants(dpu, cmd->PushConstants.offset, cmd->PushConstants.siz, cmd->PushConstants.data);
}

///////////////////////////////////////////////////////////////////////////////////
// DO WORK

_ZGL void _DecodeCmdExecuteCommands(zglDpu* dpu, _avxCmd const* cmd);

_ZGL void _DecodeCmdDraw(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuDraw(dpu, &cmd->Draw.data);
}

_ZGL void _DecodeCmdDrawIndirect(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuDrawIndirect(dpu, cmd->DrawIndirect.buf, cmd->DrawIndirect.offset, cmd->DrawIndirect.drawCnt, cmd->DrawIndirect.stride);
}

_ZGL void _DecodeCmdDrawIndirectCount(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuDrawIndirectCount(dpu, cmd->DrawIndirectCount.buf, cmd->DrawIndirectCount.offset, cmd->DrawIndirectCount.cntBuf, cmd->DrawIndirectCount.cntBufOff, cmd->DrawIndirectCount.maxDrawCnt, cmd->DrawIndirectCount.stride);
}

_ZGL void _DecodeCmdDrawIndexed(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuDrawIndexed(dpu, &cmd->DrawIndexed.data);
}

_ZGL void _DecodeCmdDrawIndexedIndirect(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuDrawIndexedIndirect(dpu, cmd->DrawIndexedIndirect.buf, cmd->DrawIndexedIndirect.offset, cmd->DrawIndexedIndirect.drawCnt, cmd->DrawIndexedIndirect.stride);
}

_ZGL void _DecodeCmdDrawIndexedIndirectCount(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuDrawIndexedIndirectCount(dpu, cmd->DrawIndexedIndirectCount.buf, cmd->DrawIndexedIndirectCount.offset, cmd->DrawIndexedIndirectCount.cntBuf, cmd->DrawIndexedIndirectCount.cntBufOff, cmd->DrawIndexedIndirectCount.maxDrawCnt, cmd->DrawIndexedIndirectCount.stride);
}

_ZGL void _DecodeCmdDispatch(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuDispatch(dpu, cmd->Dispatch.data.w, cmd->Dispatch.data.h, cmd->Dispatch.data.d);
}

_ZGL void _DecodeCmdDispatchIndirect(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuDispatchIndirect(dpu, cmd->DispatchIndirect.buf, cmd->DispatchIndirect.offset);
}

////////////////////////////////////////////////////////////////////////////////
// BUFFER OPERATIONS

_ZGL void _DecodeCmdBufCpy(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    DpuCopyBuffer(dpu, cmd->CopyBuffer.src, cmd->CopyBuffer.dst, cmd->CopyBuffer.opCnt, cmd->CopyBuffer.ops);
}

_ZGL void _DecodeCmdBufFill(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    DpuFillBuffer(dpu, cmd->FillBuffer.buf, cmd->FillBuffer.offset, cmd->FillBuffer.range, cmd->FillBuffer.value);
}

_ZGL void _DecodeCmdBufUpdate(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    avxBufferIo iop = { 0 };
    iop.srcStride = 1;
    iop.dstStride = 1;
    iop.rowCnt = cmd->UpdateBuffer.range;
    iop.dstOffset = cmd->UpdateBuffer.offset;
    DpuUpdateBuffer(dpu, cmd->UpdateBuffer.buf, cmd->UpdateBuffer.data, 1, &iop);
}

_ZGL void _DecodeCmdRasCopy(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    DpuCopyRaster(dpu, cmd->CopyRaster.src, cmd->CopyRaster.dst, cmd->CopyRaster.opCnt, cmd->CopyRaster.ops);
}

_ZGL void _DecodeCmdRasPack(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    DpuPackRaster(dpu, cmd->PackRaster.ras, cmd->PackRaster.buf, cmd->PackRaster.opCnt, cmd->PackRaster.ops);
}

_ZGL void _DecodeCmdRasUnpack(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    DpuUnpackRaster(dpu, cmd->UnpackRaster.ras, cmd->UnpackRaster.buf, cmd->UnpackRaster.opCnt, cmd->UnpackRaster.ops);
}

_ZGL void _DecodeCmdRegenerateMipmapsSIGMA(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

    afxFlags flags = cmd->RegenerateMipmapsSIGMA.flags;
    afxUnit rasCnt = cmd->RegenerateMipmapsSIGMA.rasCnt;

    for (afxUnit i = 0; i < rasCnt; i++)
    {
        avxRaster ras = cmd->RegenerateMipmapsSIGMA.rasters[i];
        if (!ras) continue;
        AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &ras);
        AFX_ASSERT(flags == 0);

        afxUnit rasLodCnt = ras->m.mipCnt;

        DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, TRUE);
        gl->GenerateMipmap(ras->glTarget); _ZglThrowErrorOccuried();
        DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, NIL, TRUE);

        AfxThrowError();
    }
}

_ZGL void _DecodeCmdRasResolve(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;    
    _ZglDpuResolveRaster(dpu, cmd->ResolveRaster.src, cmd->ResolveRaster.dst, cmd->ResolveRaster.opCnt, cmd->ResolveRaster.ops);
}

_ZGL void _DecodeCmdRasBlit(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    _ZglDpuBlitRaster(dpu, cmd->BlitRaster.src, cmd->BlitRaster.dst, cmd->BlitRaster.opCnt, cmd->BlitRaster.ops, cmd->BlitRaster.flt);
}

_ZGL void _DecodeCmdRasClear(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    _ZglDpuClearRaster(dpu, cmd->ClearRaster.ras, &cmd->ClearRaster.value, cmd->ClearRaster.baseLod, cmd->ClearRaster.lodCnt, cmd->ClearRaster.baseLayer, cmd->ClearRaster.layerCnt);
}

////////////////////////////////////////////////////////////////////////////////
// RASTER OPERATIONS

////////////////////////////////////////////////////////////////////////////////
// VERTEX TRANSFORMATION OPERATIONS

_ZGL void _DecodeCmdSetViewports(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuSetViewports(dpu, cmd->AdjustViewports.baseIdx, cmd->AdjustViewports.cnt, &cmd->AdjustViewports.viewports[0]);
}

_ZGL void _DecodeCmdSetPrimitiveTopology(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextPrimTop = cmd->SetPrimitiveTopology.topology;
}

_ZGL void _DecodeCmdSwitchFrontFace(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextFrontFaceCw = cmd->SwitchFrontFace.cw;
}

_ZGL void _DecodeCmdSetCullMode(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextCullMode = cmd->SetCullMode.mode;
}

////////////////////////////////////////////////////////////////////////////////
// RASTERIZATION OPERATIONS

_ZGL void _DecodeCmdCommenceDrawScope(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuCommenceDrawScope(dpu, 
        cmd->CommenceDrawScope.flags,
        cmd->CommenceDrawScope.canv, 
        &cmd->CommenceDrawScope.area, 
        cmd->CommenceDrawScope.targetCnt, 
        &cmd->CommenceDrawScope.targets[0], 
        /*cmd->CommenceDrawScope.hasD ? &cmd->CommenceDrawScope.depth : NIL, */&cmd->CommenceDrawScope.ds[0],
        /*cmd->CommenceDrawScope.hasS ? &cmd->CommenceDrawScope.stencil : NIL, */&cmd->CommenceDrawScope.ds[1],
        &cmd->CommenceDrawScope.dbgTag,
        FALSE);
}

_ZGL void _DecodeCmdConcludeDrawScope(zglDpu* dpu, _avxCmd const* cmd)
{
    (void)cmd;
    DpuConcludeDrawScope(dpu);
}

_ZGL void _DecodeCmdNextPass(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err;
    DpuNextPass(dpu);
}

_ZGL void _DecodeCmdClearCanvas(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err;
    _ZglDpuClearCanvas(dpu, cmd->ClearCanvas.bufCnt, cmd->ClearCanvas.bins, cmd->ClearCanvas.values, cmd->ClearCanvas.areaCnt, cmd->ClearCanvas.areas);
}

_ZGL void _DecodeCmdDisableRasterization(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRasterizationDisabled = cmd->DisableRasterization.disable;
}

_ZGL void _DecodeCmdEnableDepthBias(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextDepthBiasEnabled = cmd->EnableDepthBias.enable;
}

_ZGL void _DecodeCmdSetDepthBias(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextDepthBiasConstFactor = cmd->SetDepthBias.constFactor;
    dpu->nextDepthBiasClamp = cmd->SetDepthBias.clamp;
    dpu->nextDepthBiasSlopeScale = cmd->SetDepthBias.slopeFactor;
}

_ZGL void _DecodeCmdSetLineWidth(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextLineWidth = cmd->SetLineWidth.lineWidth;
}

_ZGL void _DecodeCmdEnableStencilTest(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextStencilTestEnabled = cmd->EnableStencilTest.enable;
}

_ZGL void _DecodeCmdSetStencilCompareMask(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_RANGE((AFX_BITMASK(0) | AFX_BITMASK(1)), AFX_BITMASK(0), cmd->SetStencilCompareMask.faceMask);

    if (cmd->SetStencilCompareMask.faceMask & AFX_BITMASK(0))
        dpu->nextStencilFront.compareMask = cmd->SetStencilCompareMask.compareMask;

    if (cmd->SetStencilCompareMask.faceMask & AFX_BITMASK(1))
        dpu->nextStencilBack.compareMask = cmd->SetStencilCompareMask.compareMask;
}

_ZGL void _DecodeCmdSetStencilWriteMask(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_RANGE((AFX_BITMASK(0) | AFX_BITMASK(1)), AFX_BITMASK(0), cmd->SetStencilWriteMask.faceMask);

    if (cmd->SetStencilWriteMask.faceMask & AFX_BITMASK(0))
        dpu->nextStencilFront.writeMask = cmd->SetStencilWriteMask.writeMask;

    if (cmd->SetStencilWriteMask.faceMask & AFX_BITMASK(1))
        dpu->nextStencilBack.writeMask = cmd->SetStencilWriteMask.writeMask;
}

_ZGL void _DecodeCmdSetStencilReference(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_RANGE((AFX_BITMASK(0) | AFX_BITMASK(1)), AFX_BITMASK(0), cmd->SetStencilReference.faceMask);

    if (cmd->SetStencilReference.faceMask & AFX_BITMASK(0))
        dpu->nextStencilFront.reference = cmd->SetStencilReference.reference;

    if (cmd->SetStencilReference.faceMask & AFX_BITMASK(1))
        dpu->nextStencilBack.reference = cmd->SetStencilReference.reference;
}

_ZGL void _DecodeCmdEnableDepthTest(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextDepthTestEnabled = cmd->EnableDepthTest.enable;
}

_ZGL void _DecodeCmdSetDepthCompareOp(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextDepthCompareOp = cmd->SetDepthCompareOp.compareOp;
}

_ZGL void _DecodeCmdDisableDepthWrite(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextDepthWriteDisabled = cmd->DisableDepthWrite.disable;
}

_ZGL void _DecodeCmdSetBlendConstants(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    AfxV4dCopy(dpu->nextBlendConstants, cmd->SetBlendConstants.blendContants);
    //dpu->nextBlendConstUpd = TRUE;
}

_ZGL void _DecodeCmdSetScissors(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    afxUnit first = cmd->AdjustScissors.baseIdx;
    afxUnit cnt = cmd->AdjustScissors.cnt;
    AFX_ASSERT_RANGE(ZGL_MAX_VIEWPORTS, first, cnt);

    for (afxUnit i = 0; i < cnt; i++)
    {
        dpu->nextScisRects[first + i] = cmd->AdjustScissors.rects[i];
        dpu->nextScisUpdMask |= AFX_BITMASK(first + i);
    }
}

_ZGL void _DecodeCmdSetCurtains(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    afxUnit first = cmd->AdjustCurtainsSIGMA.baseIdx;
    afxUnit cnt = cmd->AdjustCurtainsSIGMA.cnt;
    AFX_ASSERT_RANGE(ZGL_MAX_VIEWPORTS, first, cnt);

    for (afxUnit i = 0; i < cnt; i++)
    {
        dpu->nextCurtainRects[first + i] = cmd->AdjustCurtainsSIGMA.rects[i];
        dpu->nextCurtainUpdMask |= AFX_BITMASK(first + i);
    }
}

_ZGL void _DecodeCmdPushDebugScope(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;    
    DpuPushDebugScope(dpu, cmd->PushDebugScope.color, &cmd->PushDebugScope.label.s);
}

_ZGL void _DecodeCmdMarkDebugStep(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    DpuMarkDebugStep(dpu, cmd->MarkDebugStep.color, &cmd->MarkDebugStep.label.s);
}

_ZGL void _DecodeCmdPopDebugScope(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    DpuPopDebugScope(dpu);
}

_ZGL void _DecodeCmdExecuteCommands(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;

    for (afxUnit i = 0; i < cmd->ExecuteCommands.cnt; i++)
    {
        afxDrawContext dctx = cmd->ExecuteCommands.contexts[i].dctx;
        afxUnit batchId = cmd->ExecuteCommands.contexts[i].batchId;
        AFX_ASSERT_OBJECTS(afxFcc_DCTX, 1, &dctx);
        _AvxDpuRollContext(&dpu->m, dctx, batchId);
    }
}

_ZGL void _DecodeCmdPipelineBarrier(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    _DpuPlacePipelineBarrier(dpu, cmd->PipelineBarrier.dstStage, cmd->PipelineBarrier.dstAccess);
}

_ZGL _avxCmdLut const cmdDevmt =
{
    .PushDebugScope = (void*)_DecodeCmdPopDebugScope,
    .PopDebugScope = (void*)_DecodeCmdPopDebugScope,
    .MarkDebugStep = (void*)_DecodeCmdMarkDebugStep,

    .BindPipeline = (void*)_DecodeCmdBindPipeline,
    .BindShadersEXT = (void*)_DecodeCmdBindShadersEXT,
    .BindBuffers = (void*)_DecodeCmdBindBuffers,
    .BindRasters = (void*)_DecodeCmdBindRasters,
    .BindSamplers = (void*)_DecodeCmdBindSamplers,
    .ExecuteCommands = (void*)_DecodeCmdExecuteCommands,

    .Draw = (void*)_DecodeCmdDraw,
    .DrawIndirect = (void*)_DecodeCmdDrawIndirect,
    .DrawIndirectCount = (void*)_DecodeCmdDrawIndirectCount,
    .DrawIndexed = (void*)_DecodeCmdDrawIndexed,
    .DrawIndexedIndirect = (void*)_DecodeCmdDrawIndexedIndirect,
    .DrawIndexedIndirectCount = (void*)_DecodeCmdDrawIndexedIndirectCount,
    .Dispatch = (void*)_DecodeCmdDispatch,
    .DispatchIndirect = (void*)_DecodeCmdDispatchIndirect,
    .PushConstants = (void*)_DecodeCmdPushConstants,

    .UseVertexInput = (void*)_DecodeCmdUseVertexInput,
    .BindVertexBuffers = (void*)_DecodeCmdBindVertexBuffers,
    .BindIndexBuffer = (void*)_DecodeCmdBindIndexBuffer,
    .SetPrimitiveTopology = (void*)_DecodeCmdSetPrimitiveTopology,
    .AdjustViewports = (void*)_DecodeCmdSetViewports,
    .SwitchFrontFace = (void*)_DecodeCmdSwitchFrontFace,
    .SetCullMode = (void*)_DecodeCmdSetCullMode,

    .CommenceDrawScope = (void*)_DecodeCmdCommenceDrawScope,
    .ConcludeDrawScope = (void*)_DecodeCmdConcludeDrawScope,
    .NextPass = (void*)_DecodeCmdNextPass,
    .ClearCanvas = (void*)_DecodeCmdClearCanvas,

    .DisableRasterization = (void*)_DecodeCmdDisableRasterization,
    .EnableDepthBias = (void*)_DecodeCmdEnableDepthBias,
    .SetDepthBias = (void*)_DecodeCmdSetDepthBias,
    .SetLineWidth = (void*)_DecodeCmdSetLineWidth,
    .AdjustScissors = (void*)_DecodeCmdSetScissors,
    .AdjustCurtainsSIGMA = (void*)_DecodeCmdSetCurtains,
    .EnableDepthBoundsTest = NIL,
    .SetDepthBounds = NIL,
    .EnableStencilTest = (void*)_DecodeCmdEnableStencilTest,
    .SetStencilCompareMask = (void*)_DecodeCmdSetStencilCompareMask,
    .SetStencilReference = (void*)_DecodeCmdSetStencilReference,
    .SetStencilWriteMask = (void*)_DecodeCmdSetStencilWriteMask,
    .EnableDepthTest = (void*)_DecodeCmdEnableDepthTest,
    .SetDepthCompareOp = (void*)_DecodeCmdSetDepthCompareOp,
    .DisableDepthWrite = (void*)_DecodeCmdDisableDepthWrite,
    .SetBlendConstants = (void*)_DecodeCmdSetBlendConstants,
    
    .RegenerateMipmapsSIGMA = (void*)_DecodeCmdRegenerateMipmapsSIGMA,
    .CopyRaster = (void*)_DecodeCmdRasCopy,
    .BlitRaster = (void*)_DecodeCmdRasBlit,
    .ResolveRaster = (void*)_DecodeCmdRasResolve,
    .ClearRaster = (void*)_DecodeCmdRasClear,
    .PackRaster = (void*)_DecodeCmdRasPack,
    .UnpackRaster = (void*)_DecodeCmdRasUnpack,

    .CopyBuffer = (void*)_DecodeCmdBufCpy,
    .FillBuffer = (void*)_DecodeCmdBufFill,
    .UpdateBuffer = (void*)_DecodeCmdBufUpdate,

    .PipelineBarrier = (void*)_DecodeCmdPipelineBarrier
};
