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

_ZGL void _DecodeCmdBindPipeline(zglDpu* dpu, _zglCmdPipeline *cmd)
{
    _DpuBindPipeline(dpu, cmd->pip, cmd->vin, cmd->dynamics);
}

_ZGL void _DecodeCmdBindBuffers(zglDpu* dpu, _zglCmdBindBuffers const *cmd)
{
    _DpuBindBuffers(dpu, cmd->set, cmd->first, cmd->cnt, (afxBuffer*)cmd->buf, cmd->offset, cmd->range);
}

_ZGL void _DecodeCmdBindRasters(zglDpu* dpu, _zglCmdBindRasters const *cmd)
{
    _DpuBindRasters(dpu, cmd->set, cmd->first, cmd->cnt, (afxRaster*)cmd->tex, cmd->subras);
}

_ZGL void _DecodeCmdBindSamplers(zglDpu* dpu, _zglCmdBindSamplers const *cmd)
{
    _DpuBindSamplers(dpu, cmd->set, cmd->first, cmd->cnt, (avxSampler*)cmd->smp);
}

_ZGL void _DecodeCmdBindVertexSources(zglDpu* dpu, _zglCmdVertexSources const *cmd)
{
    _DpuBindVertexSources(dpu, cmd->first, cmd->cnt, &cmd->info[0]);
}

_ZGL void _DecodeCmdBindIndexSource(zglDpu* dpu, _zglCmdBufferRange const *cmd)
{
    _DpuBindIndexSource(dpu, cmd->buf, cmd->offset, cmd->range, cmd->stride);
}

_ZGL void _DecodeCmdBindVertexInput(zglDpu* dpu, _zglCmdVertexInput *cmd)
{
    _DpuBindVertexInput(dpu, cmd->vin);
}

_ZGL void _DecodeCmdPushConstants(zglDpu* dpu, _zglCmdPushConstants *cmd)
{
    _DpuPushConstants(dpu, cmd->offset, cmd->siz, cmd->data);
}

///////////////////////////////////////////////////////////////////////////////////
// DO WORK

_ZGL void _DecodeCmdExecuteCommands(zglDpu* dpu, _zglCmdExecCmds const* cmd);

_ZGL void _DecodeCmdDraw(zglDpu* dpu, _zglCmdDraw const *cmd)
{
    _DpuDraw(dpu, &cmd->data);
}

_ZGL void _DecodeCmdDrawIndirect(zglDpu* dpu, _zglCmdDrawIndirect const *cmd)
{
    _DpuDrawIndirect(dpu, cmd->buf, cmd->offset, cmd->drawCnt, cmd->stride);
}

_ZGL void _DecodeCmdDrawIndirectCount(zglDpu* dpu, _zglCmdDrawIndirectCnt const *cmd)
{
    _DpuDrawIndirectCount(dpu, cmd->buf, cmd->offset, cmd->cntBuf, cmd->cntBufOff, cmd->maxDrawCnt, cmd->stride);
}

_ZGL void _DecodeCmdDrawIndexed(zglDpu* dpu, _zglCmdDrawIndexed const* cmd)
{
    _DpuDrawIndexed(dpu, &cmd->data);
}

_ZGL void _DecodeCmdDrawIndexedIndirect(zglDpu* dpu, _zglCmdDrawIndexedIndirect const* cmd)
{
    _DpuDrawIndexedIndirect(dpu, cmd->buf, cmd->offset, cmd->drawCnt, cmd->stride);
}

_ZGL void _DecodeCmdDrawIndexedIndirectCount(zglDpu* dpu, _zglCmdDrawIndexedIndirectCnt const* cmd)
{
    _DpuDrawIndexedIndirectCount(dpu, cmd->buf, cmd->offset, cmd->cntBuf, cmd->cntBufOff, cmd->maxDrawCnt, cmd->stride);
}

_ZGL void _DecodeCmdDispatch(zglDpu* dpu, _zglCmdDispatch const *cmd)
{
    _DpuDispatch(dpu, cmd->data.x, cmd->data.y, cmd->data.z);
}

_ZGL void _DecodeCmdDispatchIndirect(zglDpu* dpu, _zglCmdDispatchIndirect const *cmd)
{
    _DpuDispatchIndirect(dpu, cmd->buf, cmd->offset);
}

////////////////////////////////////////////////////////////////////////////////
// BUFFER OPERATIONS

_ZGL void _DecodeCmdBufCpy(zglDpu* dpu, _zglCmdBufCpy const* cmd)
{
    afxError err = AFX_ERR_NONE;
    DpuBufCpy(dpu, cmd->src, cmd->dst, cmd->opCnt, cmd->ops);
}

_ZGL void _DecodeCmdBufSet(zglDpu* dpu, _zglCmdBufSet const* cmd)
{
    afxError err = AFX_ERR_NONE;
    DpuBufSet(dpu, cmd->buf, cmd->offset, cmd->range, cmd->data);
}

_ZGL void _DecodeCmdBufRw(zglDpu* dpu, _zglCmdBufRw const* cmd)
{
    afxError err = AFX_ERR_NONE;
    DpuBufRw(dpu, cmd->buf, cmd->offset, cmd->range, cmd->toHost, cmd->toHost ? cmd->dst : (void*)cmd->src);
}

////////////////////////////////////////////////////////////////////////////////
// RASTER OPERATIONS

////////////////////////////////////////////////////////////////////////////////
// VERTEX TRANSFORMATION OPERATIONS

_ZGL void _DecodeCmdSetViewports(zglDpu* dpu, _zglCmdViewport const *cmd)
{
    _DpuSetViewports(dpu, cmd->first, cmd->cnt, &cmd->vp[0]);
}

_ZGL void _DecodeCmdSetPrimitiveTopology(zglDpu* dpu, _zglCmdNat const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextTs.primTop = cmd->value;
}

_ZGL void _DecodeCmdSwitchFrontFace(zglDpu* dpu, _zglCmdBool const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextTs.cwFrontFacing = cmd->value;
}

_ZGL void _DecodeCmdSetCullMode(zglDpu* dpu, _zglCmdNat const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextTs.cullMode = cmd->value;
}

////////////////////////////////////////////////////////////////////////////////
// RASTERIZATION OPERATIONS

_ZGL void _DecodeCmdCommenceDrawScope(zglDpu* dpu, _zglCmdBeginSynthesis const *cmd)
{
    _DpuCommenceDrawScope(dpu, cmd->canv, &cmd->area, cmd->baseLayer, cmd->layerCnt, cmd->rasterCnt, &cmd->rasters[0], &cmd->depth, &cmd->stencil, cmd->defFbo);
}

_ZGL void _DecodeCmdConcludeDrawScope(zglDpu* dpu, _zglCmd const *cmd)
{
    (void)cmd;
    _DpuConcludeDrawScope(dpu);
}

_ZGL void _DecodeCmdNextPass(zglDpu* dpu, _zglCmd const *cmd)
{

}

_ZGL void _DecodeCmdDisableRasterization(zglDpu* dpu, _zglCmdBool const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRs.rasterizationDisabled = cmd->value;
}

_ZGL void _DecodeCmdEnableDepthBias(zglDpu* dpu, _zglCmdBool const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRs.depthBiasEnabled = cmd->value;
}

_ZGL void _DecodeCmdSetDepthBias(zglDpu* dpu, _zglCmdReal3 const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRs.depthBiasConstFactor = cmd->value[0];
    dpu->nextRs.depthBiasClamp = cmd->value[1];
    dpu->nextRs.depthBiasSlopeScale = cmd->value[2];
}

_ZGL void _DecodeCmdSetLineWidth(zglDpu* dpu, _zglCmdReal const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRs.lineWidth = cmd->value;
}

_ZGL void _DecodeCmdEnableStencilTest(zglDpu* dpu, _zglCmdBool const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRs.stencilTestEnabled = cmd->value;
}

_ZGL void _DecodeCmdSetStencilCompareMask(zglDpu* dpu, _zglCmdBitmaskNat32 const *cmd)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertRange((AFX_BIT(0) | AFX_BIT(1)), AFX_BIT(0), cmd->mask);

    if (cmd->mask & AFX_BIT(0))
        dpu->nextRs.stencilFront.compareMask = cmd->value;

    if (cmd->mask & AFX_BIT(1))
        dpu->nextRs.stencilBack.compareMask = cmd->value;
}

_ZGL void _DecodeCmdSetStencilWriteMask(zglDpu* dpu, _zglCmdBitmaskNat32 const *cmd)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertRange((AFX_BIT(0) | AFX_BIT(1)), AFX_BIT(0), cmd->mask);

    if (cmd->mask & AFX_BIT(0))
        dpu->nextRs.stencilFront.writeMask = cmd->value;

    if (cmd->mask & AFX_BIT(1))
        dpu->nextRs.stencilBack.writeMask = cmd->value;
}

_ZGL void _DecodeCmdSetStencilReference(zglDpu* dpu, _zglCmdBitmaskNat32 const *cmd)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertRange((AFX_BIT(0) | AFX_BIT(1)), AFX_BIT(0), cmd->mask);

    if (cmd->mask & AFX_BIT(0))
        dpu->nextRs.stencilFront.reference = cmd->value;

    if (cmd->mask & AFX_BIT(1))
        dpu->nextRs.stencilBack.reference = cmd->value;
}

_ZGL void _DecodeCmdEnableDepthTest(zglDpu* dpu, _zglCmdBool const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRs.depthTestEnabled = cmd->value;
}

_ZGL void _DecodeCmdSetDepthCompareOp(zglDpu* dpu, _zglCmdNat const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRs.depthCompareOp = cmd->value;
}

_ZGL void _DecodeCmdDisableDepthWrite(zglDpu* dpu, _zglCmdBool const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRs.depthWriteDisabled = cmd->value;
}

_ZGL void _DecodeCmdSetBlendConstants(zglDpu* dpu, _zglCmdReal4 const *cmd)
{
    afxError err = AFX_ERR_NONE;
    AfxV4dCopy(dpu->nextRs.blendConstants, cmd->value);
    //dpu->nextBlendConstUpd = TRUE;
}

_ZGL void _DecodeCmdSetScissors(zglDpu* dpu, _zglCmdScissor const *cmd)
{
    afxError err = AFX_ERR_NONE;
    afxUnit first = cmd->first;
    afxUnit cnt = cmd->cnt;
    AfxAssertRange(ZGL_MAX_VIEWPORTS, first, cnt);

    for (afxUnit i = 0; i < cnt; i++)
        dpu->nextRs.scisRects[first + i] = cmd->rect[i], dpu->nextRs.scisUpdMask |= AFX_BIT(first + i);
}

_ZGL void _DecodeCmdSetCurtains(zglDpu* dpu, _zglCmdScissor const *cmd)
{
    afxError err = AFX_ERR_NONE;
    afxUnit first = cmd->first;
    afxUnit cnt = cmd->cnt;
    AfxAssertRange(ZGL_MAX_VIEWPORTS, first, cnt);

    for (afxUnit i = 0; i < cnt; i++)
        dpu->nextRs.curtainRects[first + i] = cmd->rect[i], dpu->nextRs.curtainUpdMask |= AFX_BIT(first + i);
}

_ZGL void _DecodeCmdPushDebugScope(zglDpu* dpu, _zglCmdDebugScope const *cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;    
    gl->PushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, AfxGetColorAsRgba8(cmd->color), cmd->name.str.len, cmd->name.str.start); _SglThrowErrorOccuried();
}

_ZGL void _DecodeCmdPopDebugScope(zglDpu* dpu, _zglCmdDebugScope const *cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    gl->PopDebugGroup(); _SglThrowErrorOccuried();
}

/////

zglDecodeVmt _SglDecodeCmdVmt =
{
    .cmd.BindPipeline = (void*)_DecodeCmdBindPipeline,
    .cmd.BindBuffers = (void*)_DecodeCmdBindBuffers,
    .cmd.BindRasters = (void*)_DecodeCmdBindRasters,
    .cmd.BindSamplers = (void*)_DecodeCmdBindSamplers,
    .cmd.ExecuteCommands = (void*)_DecodeCmdExecuteCommands,

    .cmd.Draw = (void*)_DecodeCmdDraw,
    .cmd.DrawIndirect = (void*)_DecodeCmdDrawIndirect,
    .cmd.DrawIndirectCount = (void*)_DecodeCmdDrawIndirectCount,
    .cmd.DrawIndexed = (void*)_DecodeCmdDrawIndexed,
    .cmd.DrawIndexedIndirect = (void*)_DecodeCmdDrawIndexedIndirect,
    .cmd.DrawIndexedIndirectCount = (void*)_DecodeCmdDrawIndexedIndirectCount,
    .cmd.Dispatch = (void*)_DecodeCmdDispatch,
    .cmd.DispatchIndirect = (void*)_DecodeCmdDispatchIndirect,

    .cmd.xfrm.BindVertexInput = (void*)_DecodeCmdBindVertexInput,
    .cmd.xfrm.BindVertexSources = (void*)_DecodeCmdBindVertexSources,
    .cmd.xfrm.BindIndexSource = (void*)_DecodeCmdBindIndexSource,
    .cmd.xfrm.SetPrimitiveTopology = (void*)_DecodeCmdSetPrimitiveTopology,
    .cmd.xfrm.AdjustViewports = (void*)_DecodeCmdSetViewports,
    .cmd.xfrm.SwitchFrontFace = (void*)_DecodeCmdSwitchFrontFace,
    .cmd.xfrm.SetCullMode = (void*)_DecodeCmdSetCullMode,

    .cmd.razr.CommenceDrawScope = (void*)_DecodeCmdCommenceDrawScope,
    .cmd.razr.ConcludeDrawScope = (void*)_DecodeCmdConcludeDrawScope,
    .cmd.razr.NextPass = (void*)_DecodeCmdNextPass,

    .cmd.razr.DisableRasterization = (void*)_DecodeCmdDisableRasterization,
    .cmd.razr.EnableDepthBias = (void*)_DecodeCmdEnableDepthBias,
    .cmd.razr.SetDepthBias = (void*)_DecodeCmdSetDepthBias,
    .cmd.razr.SetLineWidth = (void*)_DecodeCmdSetLineWidth,
    .cmd.razr.AdjustScissors = (void*)_DecodeCmdSetScissors,
    .cmd.razr.AdjustCurtains = (void*)_DecodeCmdSetCurtains,
    .cmd.razr.EnableDepthBoundsTest = NIL,
    .cmd.razr.SetDepthBounds = NIL,
    .cmd.razr.EnableStencilTest = (void*)_DecodeCmdEnableStencilTest,
    .cmd.razr.SetStencilCompareMask = (void*)_DecodeCmdSetStencilCompareMask,
    .cmd.razr.SetStencilReference = (void*)_DecodeCmdSetStencilReference,
    .cmd.razr.SetStencilWriteMask = (void*)_DecodeCmdSetStencilWriteMask,
    .cmd.razr.EnableDepthTest = (void*)_DecodeCmdEnableDepthTest,
    .cmd.razr.SetDepthCompareOp = (void*)_DecodeCmdSetDepthCompareOp,
    .cmd.razr.DisableDepthWrite = (void*)_DecodeCmdDisableDepthWrite,
    .cmd.razr.SetBlendConstants = (void*)_DecodeCmdSetBlendConstants,

    .cmd.ras.xform = (void*)NIL,
    .cmd.ras.swizzle = (void*)NIL,
    .cmd.ras.mip = (void*)_DecodeCmdRasSubsample,
    .cmd.ras.cpy = (void*)_DecodeCmdRasCopy,
    .cmd.ras.pak = (void*)_DecodeCmdRasPack,

    .cmd.buf.cpy = (void*)_DecodeCmdBufCpy,
    .cmd.buf.set = (void*)_DecodeCmdBufSet,
    .cmd.buf.rw = (void*)_DecodeCmdBufRw
};

_ZGL void _DecodeCmdExecuteCommands(zglDpu* dpu, _zglCmdExecCmds const* cmd)
{
    afxError err = AFX_ERR_NONE;

    for (afxUnit i = 0; i < cmd->cnt; i++)
    {
        avxCmdb cmdb = cmd->subsets[i];
        AfxAssertObjects(1, &cmdb, afxFcc_CMDB);
        AfxAssert(cmdb->m.state == avxCmdbState_PENDING);

        if (cmdb->m.state == avxCmdbState_PENDING)
        {
            _zglCmd *cmdHdr;
            AfxChainForEveryLinkageB2F(&cmdb->commands, _zglCmd, script, cmdHdr)
            {
                if (cmdHdr->id == NIL/*ZGL_CMD_END*/)
                    break;

                if (cmdb->m.state != avxCmdbState_PENDING)
                {
                    AfxThrowError();
                    break;
                }

                _SglDecodeCmdVmt.f[cmdHdr->id](dpu, cmdHdr);
            }

            if (!err)
            {
                cmdb->m.state = avxCmdbState_EXECUTABLE;
            }

            if (err || cmdb->m.disposable)
            {
                AfxAssert(cmdb->m.portId == dpu->portId);
                cmdb->m.state = avxCmdbState_INVALID;
            }
        }
    }
}
