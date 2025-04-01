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

_ZGL void _DecodeCmdBindBuffers(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuBindBuffers(dpu, cmd->BindBuffers.set, cmd->BindBuffers.baseIdx, cmd->BindBuffers.cnt, cmd->BindBuffers.maps);
}

_ZGL void _DecodeCmdBindRasters(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuBindRasters(dpu, cmd->BindRasters.set, cmd->BindRasters.baseIdx, cmd->BindRasters.cnt, cmd->BindRasters.rasters);
}

_ZGL void _DecodeCmdBindSamplers(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuBindSamplers(dpu, cmd->BindSamplers.set, cmd->BindSamplers.baseIdx, cmd->BindSamplers.cnt, cmd->BindSamplers.samplers);
}

_ZGL void _DecodeCmdBindVertexBuffers(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuBindVertexBuffers(dpu, cmd->BindVertexBuffers.baseSlotIdx, cmd->BindVertexBuffers.cnt, cmd->BindVertexBuffers.src);
}

_ZGL void _DecodeCmdBindIndexBuffer(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuBindIndexBuffer(dpu, cmd->BindIndexBuffer.buf, cmd->BindIndexBuffer.offset, cmd->BindIndexBuffer.range, cmd->BindIndexBuffer.idxSiz);
}

_ZGL void _DecodeCmdBindVertexInput(zglDpu* dpu, _avxCmd const* cmd)
{
    DpuBindVertexInput(dpu, cmd->DeclareVertex.vin);
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
    DpuDispatch(dpu, cmd->Dispatch.data.x, cmd->Dispatch.data.y, cmd->Dispatch.data.z);
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

_ZGL void _DecodeCmdRasSubsample(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    avxRaster ras = cmd->SubsampleRaster.ras;
    AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &ras);
    afxUnit lodCnt = cmd->SubsampleRaster.lodCnt;
    afxUnit baseLod = cmd->SubsampleRaster.baseLod;
    AFX_ASSERT(lodCnt == 1);
    AFX_ASSERT(baseLod == 0);

    afxUnit rasLodCnt = ras->m.lodCnt;

    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, TRUE);
    gl->GenerateMipmap(ras->glTarget); _ZglThrowErrorOccuried();
    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, NIL, TRUE);

    AFX_ASSERT_RANGE(rasLodCnt, baseLod, lodCnt);
    AfxThrowError();
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
        cmd->CommenceDrawScope.canv, 
        &cmd->CommenceDrawScope.area, 
        cmd->CommenceDrawScope.baseLayer, 
        cmd->CommenceDrawScope.layerCnt, 
        cmd->CommenceDrawScope.targetCnt, 
        &cmd->CommenceDrawScope.targets[0], 
        /*cmd->CommenceDrawScope.hasD ? &cmd->CommenceDrawScope.depth : NIL, */&cmd->CommenceDrawScope.depth,
        /*cmd->CommenceDrawScope.hasS ? &cmd->CommenceDrawScope.stencil : NIL, */&cmd->CommenceDrawScope.stencil,
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
    AfxThrowError();
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
    AFX_ASSERT_RANGE((AFX_BIT(0) | AFX_BIT(1)), AFX_BIT(0), cmd->SetStencilCompareMask.faceMask);

    if (cmd->SetStencilCompareMask.faceMask & AFX_BIT(0))
        dpu->nextStencilFront.compareMask = cmd->SetStencilCompareMask.compareMask;

    if (cmd->SetStencilCompareMask.faceMask & AFX_BIT(1))
        dpu->nextStencilBack.compareMask = cmd->SetStencilCompareMask.compareMask;
}

_ZGL void _DecodeCmdSetStencilWriteMask(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_RANGE((AFX_BIT(0) | AFX_BIT(1)), AFX_BIT(0), cmd->SetStencilWriteMask.faceMask);

    if (cmd->SetStencilWriteMask.faceMask & AFX_BIT(0))
        dpu->nextStencilFront.writeMask = cmd->SetStencilWriteMask.writeMask;

    if (cmd->SetStencilWriteMask.faceMask & AFX_BIT(1))
        dpu->nextStencilBack.writeMask = cmd->SetStencilWriteMask.writeMask;
}

_ZGL void _DecodeCmdSetStencilReference(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_RANGE((AFX_BIT(0) | AFX_BIT(1)), AFX_BIT(0), cmd->SetStencilReference.faceMask);

    if (cmd->SetStencilReference.faceMask & AFX_BIT(0))
        dpu->nextStencilFront.reference = cmd->SetStencilReference.reference;

    if (cmd->SetStencilReference.faceMask & AFX_BIT(1))
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
        dpu->nextScisUpdMask |= AFX_BIT(first + i);
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
        dpu->nextCurtainUpdMask |= AFX_BIT(first + i);
    }
}

_ZGL void _DecodeCmdPushDebugScope(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;    
    DpuPushDebugScope(dpu, cmd->PushDebugScope.color, &cmd->PushDebugScope.label.str);
}

_ZGL void _DecodeCmdMarkDebugStep(zglDpu* dpu, _avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    DpuMarkDebugStep(dpu, cmd->MarkDebugStep.color, &cmd->MarkDebugStep.label.str);
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
        _AvxDpuRollContext(&dpu->m, cmd->ExecuteCommands.contexts[i]);
}

_ZGL _avxCmdLut const cmdDevmt =
{
    .PushDebugScope = (void*)_DecodeCmdPopDebugScope,
    .PopDebugScope = (void*)_DecodeCmdPopDebugScope,
    .MarkDebugStep = (void*)_DecodeCmdMarkDebugStep,

    .BindPipeline = (void*)_DecodeCmdBindPipeline,
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

    .DeclareVertex = (void*)_DecodeCmdBindVertexInput,
    .BindVertexBuffers = (void*)_DecodeCmdBindVertexBuffers,
    .BindIndexBuffer = (void*)_DecodeCmdBindIndexBuffer,
    .SetPrimitiveTopology = (void*)_DecodeCmdSetPrimitiveTopology,
    .AdjustViewports = (void*)_DecodeCmdSetViewports,
    .SwitchFrontFace = (void*)_DecodeCmdSwitchFrontFace,
    .SetCullMode = (void*)_DecodeCmdSetCullMode,

    .CommenceDrawScope = (void*)_DecodeCmdCommenceDrawScope,
    .ConcludeDrawScope = (void*)_DecodeCmdConcludeDrawScope,
    .NextPass = (void*)_DecodeCmdNextPass,

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

    .SubsampleRaster = (void*)_DecodeCmdRasSubsample,
    .CopyRaster = (void*)_DecodeCmdRasCopy,
    .PackRaster = (void*)_DecodeCmdRasPack,
    .UnpackRaster = (void*)_DecodeCmdRasUnpack,

    .CopyBuffer = (void*)_DecodeCmdBufCpy,
    .FillBuffer = (void*)_DecodeCmdBufFill,
    .UpdateBuffer = (void*)_DecodeCmdBufUpdate
};
