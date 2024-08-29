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


#include "zglCommands.h"
#include "zglObjects.h"
#include "qwadro/afxQwadro.h"
#include "qwadro/draw/pipe/avxDrawOps.h"

////////////////////////////////////////////////////////////////////////////////
// BIND RESOURCES

_ZGL void _DecodeCmdBindPipeline(sglDpu* dpu, _sglCmdPipeline *cmd)
{
    _DpuBindPipeline(dpu, cmd->pip, cmd->vin, cmd->dynamics);
}

_ZGL void _DecodeCmdBindRasterizer(sglDpu* dpu, _sglCmdRasterizer *cmd)
{
    _DpuBindRasterizer(dpu, cmd->razr, cmd->vin, cmd->dynamics);
}

_ZGL void _DecodeCmdBindBuffers(sglDpu* dpu, _sglCmdBindBuffers const *cmd)
{
    _DpuBindBuffers(dpu, cmd->set, cmd->first, cmd->cnt, (afxBuffer*)cmd->buf, cmd->offset, cmd->range);
}

_ZGL void _DecodeCmdBindRasters(sglDpu* dpu, _sglCmdBindRasters const *cmd)
{
    _DpuBindRasters(dpu, cmd->set, cmd->first, cmd->cnt, (afxRaster*)cmd->tex, cmd->subras);
}

_ZGL void _DecodeCmdBindSamplers(sglDpu* dpu, _sglCmdBindSamplers const *cmd)
{
    _DpuBindSamplers(dpu, cmd->set, cmd->first, cmd->cnt, (avxSampler*)cmd->smp);
}

_ZGL void _DecodeCmdBindVertexSources(sglDpu* dpu, _sglCmdVertexSources const *cmd)
{
    _DpuBindVertexSources(dpu, cmd->first, cmd->cnt, &cmd->info[0]);
}

_ZGL void _DecodeCmdBindIndexSource(sglDpu* dpu, _sglCmdBufferRange const *cmd)
{
    _DpuBindIndexSource(dpu, cmd->buf, cmd->offset, cmd->range, cmd->stride);
}

_ZGL void _DecodeCmdBindVertexInput(sglDpu* dpu, _sglCmdVertexInput *cmd)
{
    _DpuBindVertexInput(dpu, cmd->vin);
}

///////////////////////////////////////////////////////////////////////////////////
// DO WORK

_ZGL void _DecodeCmdExecuteCommands(sglDpu* dpu, _sglCmdExecCmds const* cmd);

_ZGL void _DecodeCmdDraw(sglDpu* dpu, _sglCmdDraw const *cmd)
{
    _DpuDraw(dpu, &cmd->data);
}

_ZGL void _DecodeCmdDrawIndirect(sglDpu* dpu, _sglCmdDrawIndirect const *cmd)
{
    _DpuDrawIndirect(dpu, cmd->buf, cmd->offset, cmd->drawCnt, cmd->stride);
}

_ZGL void _DecodeCmdDrawIndirectCount(sglDpu* dpu, _sglCmdDrawIndirectCnt const *cmd)
{
    _DpuDrawIndirectCount(dpu, cmd->buf, cmd->offset, cmd->cntBuf, cmd->cntBufOff, cmd->maxDrawCnt, cmd->stride);
}

_ZGL void _DecodeCmdDrawIndexed(sglDpu* dpu, _sglCmdDrawIndexed const* cmd)
{
    _DpuDrawIndexed(dpu, &cmd->data);
}

_ZGL void _DecodeCmdDrawIndexedIndirect(sglDpu* dpu, _sglCmdDrawIndexedIndirect const* cmd)
{
    _DpuDrawIndexedIndirect(dpu, cmd->buf, cmd->offset, cmd->drawCnt, cmd->stride);
}

_ZGL void _DecodeCmdDrawIndexedIndirectCount(sglDpu* dpu, _sglCmdDrawIndexedIndirectCnt const* cmd)
{
    _DpuDrawIndexedIndirectCount(dpu, cmd->buf, cmd->offset, cmd->cntBuf, cmd->cntBufOff, cmd->maxDrawCnt, cmd->stride);
}

_ZGL void _DecodeCmdDispatch(sglDpu* dpu, _sglCmdDispatch const *cmd)
{
    _DpuDispatch(dpu, cmd->data.x, cmd->data.y, cmd->data.z);
}

_ZGL void _DecodeCmdDispatchIndirect(sglDpu* dpu, _sglCmdDispatchIndirect const *cmd)
{
    _DpuDispatchIndirect(dpu, cmd->buf, cmd->offset);
}

////////////////////////////////////////////////////////////////////////////////
// BUFFER OPERATIONS

_ZGL void _DecodeCmdBufCpy(sglDpu* dpu, _sglCmdBufCpy const* cmd)
{
    afxError err = AFX_ERR_NONE;
    DpuBufCpy(dpu, cmd->src, cmd->dst, cmd->opCnt, cmd->ops);
}

_ZGL void _DecodeCmdBufSet(sglDpu* dpu, _sglCmdBufSet const* cmd)
{
    afxError err = AFX_ERR_NONE;
    DpuBufSet(dpu, cmd->buf, cmd->offset, cmd->range, cmd->data);
}

_ZGL void _DecodeCmdBufRw(sglDpu* dpu, _sglCmdBufRw const* cmd)
{
    afxError err = AFX_ERR_NONE;
    DpuBufRw(dpu, cmd->buf, cmd->offset, cmd->range, cmd->toHost, cmd->toHost ? cmd->dst : (void*)cmd->src);
}

////////////////////////////////////////////////////////////////////////////////
// RASTER OPERATIONS

////////////////////////////////////////////////////////////////////////////////
// VERTEX TRANSFORMATION OPERATIONS

_ZGL void _DecodeCmdSetViewports(sglDpu* dpu, _sglCmdViewport const *cmd)
{
    _DpuSetViewports(dpu, cmd->first, cmd->cnt, &cmd->vp[0]);
}

_ZGL void _DecodeCmdSetPrimitiveTopology(sglDpu* dpu, _sglCmdNat const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextXformState.primTop = cmd->value;
}

_ZGL void _DecodeCmdSwitchFrontFace(sglDpu* dpu, _sglCmdBool const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextXformState.cwFrontFacing = cmd->value;
}

_ZGL void _DecodeCmdSetCullMode(sglDpu* dpu, _sglCmdNat const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextXformState.cullMode = cmd->value;
}

////////////////////////////////////////////////////////////////////////////////
// RASTERIZATION OPERATIONS

_ZGL void _DecodeCmdBeginDrawScope(sglDpu* dpu, _sglCmdBeginSynthesis const *cmd)
{
    _DpuBeginDrawScope(dpu, cmd->canv, &cmd->area, cmd->layerCnt, cmd->rasterCnt, &cmd->rasters[0], &cmd->depth, &cmd->stencil, cmd->defFbo);
}

_ZGL void _DecodeCmdEndDrawScope(sglDpu* dpu, _sglCmd const *cmd)
{
    (void)cmd;
    _DpuEndDrawScope(dpu);
}

_ZGL void _DecodeCmdNextPass(sglDpu* dpu, _sglCmd const *cmd)
{

}

_ZGL void _DecodeCmdDisableRasterization(sglDpu* dpu, _sglCmdBool const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRasterState.rasterizationDisabled = cmd->value;
}

_ZGL void _DecodeCmdEnableDepthBias(sglDpu* dpu, _sglCmdBool const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRasterState.depthBiasEnabled = cmd->value;
}

_ZGL void _DecodeCmdSetDepthBias(sglDpu* dpu, _sglCmdReal3 const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRasterState.depthBiasConstFactor = cmd->value[0];
    dpu->nextRasterState.depthBiasClamp = cmd->value[1];
    dpu->nextRasterState.depthBiasSlopeScale = cmd->value[2];
}

_ZGL void _DecodeCmdSetLineWidth(sglDpu* dpu, _sglCmdReal const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRasterState.lineWidth = cmd->value;
}

_ZGL void _DecodeCmdEnableStencilTest(sglDpu* dpu, _sglCmdBool const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRasterState.stencilTestEnabled = cmd->value;
}

_ZGL void _DecodeCmdSetStencilCompareMask(sglDpu* dpu, _sglCmdBitmaskNat32 const *cmd)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertRange((AFX_BIT(0) | AFX_BIT(1)), AFX_BIT(0), cmd->mask);

    if (cmd->mask & AFX_BIT(0))
        dpu->nextRasterState.stencilFront.compareMask = cmd->value;

    if (cmd->mask & AFX_BIT(1))
        dpu->nextRasterState.stencilBack.compareMask = cmd->value;
}

_ZGL void _DecodeCmdSetStencilWriteMask(sglDpu* dpu, _sglCmdBitmaskNat32 const *cmd)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertRange((AFX_BIT(0) | AFX_BIT(1)), AFX_BIT(0), cmd->mask);

    if (cmd->mask & AFX_BIT(0))
        dpu->nextRasterState.stencilFront.writeMask = cmd->value;

    if (cmd->mask & AFX_BIT(1))
        dpu->nextRasterState.stencilBack.writeMask = cmd->value;
}

_ZGL void _DecodeCmdSetStencilReference(sglDpu* dpu, _sglCmdBitmaskNat32 const *cmd)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertRange((AFX_BIT(0) | AFX_BIT(1)), AFX_BIT(0), cmd->mask);

    if (cmd->mask & AFX_BIT(0))
        dpu->nextRasterState.stencilFront.reference = cmd->value;

    if (cmd->mask & AFX_BIT(1))
        dpu->nextRasterState.stencilBack.reference = cmd->value;
}

_ZGL void _DecodeCmdEnableDepthTest(sglDpu* dpu, _sglCmdBool const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRasterState.depthTestEnabled = cmd->value;
}

_ZGL void _DecodeCmdSetDepthCompareOp(sglDpu* dpu, _sglCmdNat const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRasterState.depthCompareOp = cmd->value;
}

_ZGL void _DecodeCmdDisableDepthWrite(sglDpu* dpu, _sglCmdBool const *cmd)
{
    afxError err = AFX_ERR_NONE;
    dpu->nextRasterState.depthWriteDisabled = cmd->value;
}

_ZGL void _DecodeCmdSetBlendConstants(sglDpu* dpu, _sglCmdReal4 const *cmd)
{
    afxError err = AFX_ERR_NONE;
    AfxV4dCopy(dpu->nextRasterState.blendConstants, cmd->value);
    dpu->nextBlendConstUpd = TRUE;
}

_ZGL void _DecodeCmdSetScissors(sglDpu* dpu, _sglCmdScissor const *cmd)
{
    afxError err = AFX_ERR_NONE;
    afxNat first = cmd->first;
    afxNat cnt = cmd->cnt;
    AfxAssertRange(ZGL_MAX_VIEWPORTS, first, cnt);

    for (afxNat i = 0; i < cnt; i++)
        dpu->nextRasterState.scisRects[first + i] = cmd->rect[i], dpu->nextScissorUpdMask |= AFX_BIT(first + i);

    dpu->nextScissorUpdCnt = AfxMax(dpu->nextScissorUpdCnt, cnt);
}

_ZGL void _DecodeCmdSetCurtains(sglDpu* dpu, _sglCmdScissor const *cmd)
{
    afxError err = AFX_ERR_NONE;
    afxNat first = cmd->first;
    afxNat cnt = cmd->cnt;
    AfxAssertRange(ZGL_MAX_VIEWPORTS, first, cnt);

    for (afxNat i = 0; i < cnt; i++)
        dpu->nextRasterState.areaRects[first + i] = cmd->rect[i], dpu->nextAreaUpdMask |= AFX_BIT(first + i);

    dpu->nextAreaUpdCnt = AfxMax(dpu->nextAreaUpdCnt, cnt);
}

_ZGL void _DecodeCmdPushDebugScope(sglDpu* dpu, _sglCmdDebugScope const *cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;    
    gl->PushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, AfxGetColorAsRgba8(cmd->color), cmd->name.str.len, cmd->name.str.start); _SglThrowErrorOccuried();
}

_ZGL void _DecodeCmdPopDebugScope(sglDpu* dpu, _sglCmdDebugScope const *cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    gl->PopDebugGroup(); _SglThrowErrorOccuried();
}

/////

sglDecodeVmt _SglDecodeCmdVmt =
{
    .cmd.BindPipeline = (void*)_DecodeCmdBindPipeline,
    .cmd.BindRasterizer = (void*)_DecodeCmdBindRasterizer,
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

    .cmd.razr.BeginDrawScope = (void*)_DecodeCmdBeginDrawScope,
    .cmd.razr.EndDrawScope = (void*)_DecodeCmdEndDrawScope,
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

_ZGL void _DecodeCmdExecuteCommands(sglDpu* dpu, _sglCmdExecCmds const* cmd)
{
    afxError err = AFX_ERR_NONE;

    for (afxNat i = 0; i < cmd->cnt; i++)
    {
        avxCmdb cmdb = cmd->subsets[i];
        AfxAssertObjects(1, &cmdb, afxFcc_CMDB);
        AfxAssert(cmdb->m.state == avxCmdbState_PENDING);

        if (cmdb->m.state == avxCmdbState_PENDING)
        {
            _sglCmd *cmdHdr;
            AfxChainForEveryLinkageB2F(&cmdb->commands, _sglCmd, script, cmdHdr)
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
                AfxAssert(cmdb->m.portIdx == dpu->portIdx);
                cmdb->m.state = avxCmdbState_INVALID;
            }
        }
    }
}
