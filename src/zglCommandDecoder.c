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

_ZGL void _DecodeCmdStampDebug(zglDpu* dpu, avxCmd const* cmd)
{
    //
}

_ZGL void _DecodeCmdBindPipeline(zglDpu* dpu, avxCmd const* cmd)
{
    _DpuBindPipeline(dpu, cmd->BindPipeline.pip, cmd->BindPipeline.vin, cmd->BindPipeline.dynamics);
}

_ZGL void _DecodeCmdBindBuffers(zglDpu* dpu, avxCmd const* cmd)
{
    for (afxUnit i = 0; i < cmd->BindBuffers.cnt; i++)
        _DpuBindBuffers(dpu, cmd->BindBuffers.set, cmd->BindBuffers.baseIdx + i, 1, (afxBuffer*)&cmd->BindBuffers.items[i].buf, &cmd->BindBuffers.items[i].offset, &cmd->BindBuffers.items[i].range);
}

_ZGL void _DecodeCmdBindRasters(zglDpu* dpu, avxCmd const* cmd)
{
    for (afxUnit i = 0; i < cmd->BindRasters.cnt; i++)
        _DpuBindRasters(dpu, cmd->BindRasters.set, cmd->BindRasters.baseIdx + i, 1, (afxRaster*)&cmd->BindRasters.items[i].ras, &cmd->BindRasters.items[i].subIdx);
}

_ZGL void _DecodeCmdBindSamplers(zglDpu* dpu, avxCmd const* cmd)
{
    for (afxUnit i = 0; i < cmd->BindSamplers.cnt; i++)
        _DpuBindSamplers(dpu, cmd->BindSamplers.set, cmd->BindSamplers.baseIdx + i, 1, (avxSampler*)&cmd->BindSamplers.samplers[i]);
}

_ZGL void _DecodeCmdBindVertexSources(zglDpu* dpu, avxCmd const* cmd)
{
    for (afxUnit i = 0; i < cmd->BindVertexSources.slotCnt; i++)
    {
        zglBufferInfo info = { 0 };
        info.buf = cmd->BindVertexSources.items[i].buf;
        info.offset = cmd->BindVertexSources.items[i].offset;
        info.range = cmd->BindVertexSources.items[i].range;
        info.stride = cmd->BindVertexSources.items[i].stride;
        _DpuBindVertexSources(dpu, cmd->BindVertexSources.baseSlotIdx + i, 1, &info);
    }
}

_ZGL void _DecodeCmdBindIndexSource(zglDpu* dpu, avxCmd const* cmd)
{
    _DpuBindIndexSource(dpu, cmd->BindIndexSource.buf, cmd->BindIndexSource.offset, cmd->BindIndexSource.range, cmd->BindIndexSource.idxSiz);
}

_ZGL void _DecodeCmdBindVertexInput(zglDpu* dpu, avxCmd const* cmd)
{
    _DpuBindVertexInput(dpu, cmd->BindVertexInput.vin);
}

_ZGL void _DecodeCmdPushConstants(zglDpu* dpu, avxCmd const* cmd)
{
    _DpuPushConstants(dpu, cmd->PushConstants.offset, cmd->PushConstants.siz, cmd->PushConstants.data);
}

///////////////////////////////////////////////////////////////////////////////////
// DO WORK

_ZGL void _DecodeCmdExecuteCommands(zglDpu* dpu, avxCmd const* cmd);

_ZGL void _DecodeCmdDraw(zglDpu* dpu, avxCmd const* cmd)
{
    _DpuDraw(dpu, &cmd->Draw.data);
}

_ZGL void _DecodeCmdDrawIndirect(zglDpu* dpu, avxCmd const* cmd)
{
    _DpuDrawIndirect(dpu, cmd->DrawIndirect.buf, cmd->DrawIndirect.offset, cmd->DrawIndirect.drawCnt, cmd->DrawIndirect.stride);
}

_ZGL void _DecodeCmdDrawIndirectCount(zglDpu* dpu, avxCmd const* cmd)
{
    _DpuDrawIndirectCount(dpu, cmd->DrawIndirectCount.buf, cmd->DrawIndirectCount.offset, cmd->DrawIndirectCount.cntBuf, cmd->DrawIndirectCount.cntBufOff, cmd->DrawIndirectCount.maxDrawCnt, cmd->DrawIndirectCount.stride);
}

_ZGL void _DecodeCmdDrawIndexed(zglDpu* dpu, avxCmd const* cmd)
{
    _DpuDrawIndexed(dpu, &cmd->DrawIndexed.data);
}

_ZGL void _DecodeCmdDrawIndexedIndirect(zglDpu* dpu, avxCmd const* cmd)
{
    _DpuDrawIndexedIndirect(dpu, cmd->DrawIndexedIndirect.buf, cmd->DrawIndexedIndirect.offset, cmd->DrawIndexedIndirect.drawCnt, cmd->DrawIndexedIndirect.stride);
}

_ZGL void _DecodeCmdDrawIndexedIndirectCount(zglDpu* dpu, avxCmd const* cmd)
{
    _DpuDrawIndexedIndirectCount(dpu, cmd->DrawIndexedIndirectCount.buf, cmd->DrawIndexedIndirectCount.offset, cmd->DrawIndexedIndirectCount.cntBuf, cmd->DrawIndexedIndirectCount.cntBufOff, cmd->DrawIndexedIndirectCount.maxDrawCnt, cmd->DrawIndexedIndirectCount.stride);
}

_ZGL void _DecodeCmdDispatch(zglDpu* dpu, avxCmd const* cmd)
{
    _DpuDispatch(dpu, cmd->Dispatch.data.x, cmd->Dispatch.data.y, cmd->Dispatch.data.z);
}

_ZGL void _DecodeCmdDispatchIndirect(zglDpu* dpu, avxCmd const* cmd)
{
    _DpuDispatchIndirect(dpu, cmd->DispatchIndirect.buf, cmd->DispatchIndirect.offset);
}

////////////////////////////////////////////////////////////////////////////////
// BUFFER OPERATIONS

_ZGL void _DecodeCmdBufCpy(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    DpuBufCpy(dpu, cmd->CopyBuffer.src, cmd->CopyBuffer.dst, cmd->CopyBuffer.opCnt, cmd->CopyBuffer.ops);
}

_ZGL void _DecodeCmdBufClear(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    DpuBufSet(dpu, cmd->ClearBuffer.buf, cmd->ClearBuffer.offset, cmd->ClearBuffer.range, 0);
}

_ZGL void _DecodeCmdBufFill(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    DpuBufSet(dpu, cmd->FillBuffer.buf, cmd->FillBuffer.offset, cmd->FillBuffer.range, cmd->FillBuffer.value);
}

_ZGL void _DecodeCmdBufUpdate(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    DpuBufRw(dpu, cmd->UpdateBuffer.buf, cmd->UpdateBuffer.offset, cmd->UpdateBuffer.range, FALSE, (void*)&cmd->UpdateBuffer.data[0]);
}

_ZGL void _DecodeCmdRasCopy(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    _DpuCopyRas(dpu, cmd->CopyRaster.src, cmd->CopyRaster.dst, cmd->CopyRaster.opCnt, cmd->CopyRaster.ops);
}

_ZGL void _DecodeCmdRasPack(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    _DpuPackRasToBuf(dpu, cmd->PackRaster.ras, cmd->PackRaster.buf, cmd->PackRaster.opCnt, cmd->PackRaster.ops);
}

_ZGL void _DecodeCmdRasUnpack(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    _DpuUnpackRasFromBuf(dpu, cmd->UnpackRaster.ras, cmd->UnpackRaster.buf, cmd->UnpackRaster.opCnt, cmd->UnpackRaster.ops);
}

_ZGL void _DecodeCmdRasSubsample(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    afxRaster ras = cmd->SubsampleRaster.ras;
    AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &ras);
    afxUnit lodCnt = cmd->SubsampleRaster.lodCnt;
    afxUnit baseLod = cmd->SubsampleRaster.baseLod;
    AFX_ASSERT(lodCnt == 1);
    AFX_ASSERT(baseLod == 0);

    afxUnit rasLodCnt = ras->m.lodCnt;

    DpuBindAndSyncRas(dpu, ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT, ras);
    gl->GenerateMipmap(ras->glTarget); _ZglThrowErrorOccuried();
    DpuBindAndSyncRas(dpu, ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT, NIL);

    AFX_ASSERT_RANGE(rasLodCnt, baseLod, lodCnt);
    AfxThrowError();
}

////////////////////////////////////////////////////////////////////////////////
// RASTER OPERATIONS

////////////////////////////////////////////////////////////////////////////////
// VERTEX TRANSFORMATION OPERATIONS

_ZGL void _DecodeCmdSetViewports(zglDpu* dpu, avxCmd const* cmd)
{
    _DpuSetViewports(dpu, cmd->AdjustViewports.baseIdx, cmd->AdjustViewports.cnt, &cmd->AdjustViewports.viewports[0]);
}

_ZGL void _DecodeCmdSetPrimitiveTopology(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextTs.primTop = cmd->SetPrimitiveTopology.topology;
}

_ZGL void _DecodeCmdSwitchFrontFace(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextTs.cwFrontFacing = cmd->SwitchFrontFace.cw;
}

_ZGL void _DecodeCmdSetCullMode(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextTs.cullMode = cmd->SetCullMode.mode;
}

////////////////////////////////////////////////////////////////////////////////
// RASTERIZATION OPERATIONS

_ZGL void _DecodeCmdCommenceDrawScope(zglDpu* dpu, avxCmd const* cmd)
{
    _DpuCommenceDrawScope(dpu, cmd->CommenceDrawScope.canv, &cmd->CommenceDrawScope.area, cmd->CommenceDrawScope.baseLayer, cmd->CommenceDrawScope.layerCnt, cmd->CommenceDrawScope.targetCnt, &cmd->CommenceDrawScope.targets[0], cmd->CommenceDrawScope.hasD ? &cmd->CommenceDrawScope.depth : NIL, cmd->CommenceDrawScope.hasS ? &cmd->CommenceDrawScope.stencil : NIL, FALSE);
}

_ZGL void _DecodeCmdConcludeDrawScope(zglDpu* dpu, avxCmd const* cmd)
{
    (void)cmd;
    _DpuConcludeDrawScope(dpu);
}

_ZGL void _DecodeCmdNextPass(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err;
    AfxThrowError();
}

_ZGL void _DecodeCmdDisableRasterization(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRs.rasterizationDisabled = cmd->DisableRasterization.disable;
}

_ZGL void _DecodeCmdEnableDepthBias(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRs.depthBiasEnabled = cmd->EnableDepthBias.enable;
}

_ZGL void _DecodeCmdSetDepthBias(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRs.depthBiasConstFactor = cmd->SetDepthBias.constFactor;
    dpu->nextRs.depthBiasClamp = cmd->SetDepthBias.clamp;
    dpu->nextRs.depthBiasSlopeScale = cmd->SetDepthBias.slopeFactor;
}

_ZGL void _DecodeCmdSetLineWidth(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRs.lineWidth = cmd->SetLineWidth.lineWidth;
}

_ZGL void _DecodeCmdEnableStencilTest(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRs.stencilTestEnabled = cmd->EnableStencilTest.enable;
}

_ZGL void _DecodeCmdSetStencilCompareMask(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_RANGE((AFX_BIT(0) | AFX_BIT(1)), AFX_BIT(0), cmd->SetStencilCompareMask.faceMask);

    if (cmd->SetStencilCompareMask.faceMask & AFX_BIT(0))
        dpu->nextRs.stencilFront.compareMask = cmd->SetStencilCompareMask.compareMask;

    if (cmd->SetStencilCompareMask.faceMask & AFX_BIT(1))
        dpu->nextRs.stencilBack.compareMask = cmd->SetStencilCompareMask.compareMask;
}

_ZGL void _DecodeCmdSetStencilWriteMask(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_RANGE((AFX_BIT(0) | AFX_BIT(1)), AFX_BIT(0), cmd->SetStencilWriteMask.faceMask);

    if (cmd->SetStencilWriteMask.faceMask & AFX_BIT(0))
        dpu->nextRs.stencilFront.writeMask = cmd->SetStencilWriteMask.writeMask;

    if (cmd->SetStencilWriteMask.faceMask & AFX_BIT(1))
        dpu->nextRs.stencilBack.writeMask = cmd->SetStencilWriteMask.writeMask;
}

_ZGL void _DecodeCmdSetStencilReference(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_RANGE((AFX_BIT(0) | AFX_BIT(1)), AFX_BIT(0), cmd->SetStencilReference.faceMask);

    if (cmd->SetStencilReference.faceMask & AFX_BIT(0))
        dpu->nextRs.stencilFront.reference = cmd->SetStencilReference.reference;

    if (cmd->SetStencilReference.faceMask & AFX_BIT(1))
        dpu->nextRs.stencilBack.reference = cmd->SetStencilReference.reference;
}

_ZGL void _DecodeCmdEnableDepthTest(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRs.depthTestEnabled = cmd->EnableDepthTest.enable;
}

_ZGL void _DecodeCmdSetDepthCompareOp(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRs.depthCompareOp = cmd->SetDepthCompareOp.compareOp;
}

_ZGL void _DecodeCmdDisableDepthWrite(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRs.depthWriteDisabled = cmd->DisableDepthWrite.disable;
}

_ZGL void _DecodeCmdSetBlendConstants(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    AfxV4dCopy(dpu->nextRs.blendConstants, cmd->SetBlendConstants.blendContants);
    //dpu->nextBlendConstUpd = TRUE;
}

_ZGL void _DecodeCmdSetScissors(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    afxUnit first = cmd->AdjustScissors.baseIdx;
    afxUnit cnt = cmd->AdjustScissors.cnt;
    AFX_ASSERT_RANGE(ZGL_MAX_VIEWPORTS, first, cnt);

    for (afxUnit i = 0; i < cnt; i++)
    {
        dpu->nextRs.scisRects[first + i] = cmd->AdjustScissors.rects[i];
        dpu->nextRs.scisUpdMask |= AFX_BIT(first + i);
    }
}

_ZGL void _DecodeCmdSetCurtains(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    afxUnit first = cmd->AdjustCurtains.baseIdx;
    afxUnit cnt = cmd->AdjustCurtains.cnt;
    AFX_ASSERT_RANGE(ZGL_MAX_VIEWPORTS, first, cnt);

    for (afxUnit i = 0; i < cnt; i++)
    {
        dpu->nextRs.curtainRects[first + i] = cmd->AdjustCurtains.rects[i];
        dpu->nextRs.curtainUpdMask |= AFX_BIT(first + i);
    }
}

_ZGL void _DecodeCmdPushDebugScope(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;    
    gl->PushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, AfxGetColorAsRgba8(cmd->PushDebugScope.color), cmd->PushDebugScope.label.str.len, cmd->PushDebugScope.label.str.start); _ZglThrowErrorOccuried();
}

_ZGL void _DecodeCmdPopDebugScope(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    gl->PopDebugGroup(); _ZglThrowErrorOccuried();
}

_ZGL void _DecodeCmdExecuteCommands(zglDpu* dpu, avxCmd const* cmd)
{
    afxError err = AFX_ERR_NONE;

    for (afxUnit i = 0; i < cmd->ExecuteCommands.cnt; i++)
        _AvxDpuRollContext(dpu, cmd->ExecuteCommands.contexts[i]);
}

_ZGL avxCmdList const cmdDevmt =
{
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

    .BindVertexInput = (void*)_DecodeCmdBindVertexInput,
    .BindVertexSources = (void*)_DecodeCmdBindVertexSources,
    .BindIndexSource = (void*)_DecodeCmdBindIndexSource,
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
    .AdjustCurtains = (void*)_DecodeCmdSetCurtains,
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
    .ClearBuffer = (void*)_DecodeCmdBufClear,
    .UpdateBuffer = (void*)_DecodeCmdBufUpdate
};
