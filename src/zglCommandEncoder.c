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

_ZGL afxCmdId _SglEncodeCmdCommand(avxCmdb cmdb, afxUnit id, afxUnit siz, _zglCmd *cmd)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &cmdb, afxFcc_CMDB);
    cmd->id = id;
    AfxAssertRange(ZGL_CMD_TOTAL, cmd->id, 1);

    if (err)
    {
        int a = 0;
    }

    cmd->siz = siz;
    return AfxPushLinkage(&cmd->script, &cmdb->commands);
}

///////////////////////////////////////////

_ZGL afxCmdId FlushNextCachedPipelineBinding(avxCmdb cmdb, afxUnit segment)
{
    afxError err = AFX_ERR_NONE;
    AfxAssert(cmdb->m.state == avxCmdbState_RECORDING);
    avxPipeline curr = cmdb->levelCaches[segment].pip.curr;
    avxPipeline next = cmdb->levelCaches[segment].pip.next;

    if ((curr != next) && next)
    {
        cmdb->levelCaches[segment].pip.curr = next;
        cmdb->levelCaches[segment].pip.next = NIL;

        _zglCmdPipeline *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
        AfxAssert(cmd);
        cmd->segment = segment;
        cmd->pip = next;
        return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, BindPipeline) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
    }
    return 0;
}

_ZGL afxCmdId FlushCachedDrawStuff(avxCmdb cmdb, afxUnit segment)
{
    afxError err = AFX_ERR_NONE;
    AfxAssert(cmdb->m.state == avxCmdbState_RECORDING);

    return FlushNextCachedPipelineBinding(cmdb, segment);
}

_ZGL afxCmdId FlushCachedDrawIndexedStuff(avxCmdb cmdb, afxUnit segment)
{
    afxError err = AFX_ERR_NONE;
    AfxAssert(cmdb->m.state == avxCmdbState_RECORDING);
    return FlushCachedDrawStuff(cmdb, segment);
}

_ZGL afxCmdId FlushCachedStuff(avxCmdb cmdb, afxUnit segment)
{
    afxError err = AFX_ERR_NONE;
    AfxAssert(cmdb->m.state == avxCmdbState_RECORDING);
    return FlushCachedDrawIndexedStuff(cmdb, segment);
}

////////////////////////////////////////////////////////////////////////////////
// BIND RESOURCES

_ZGL afxCmdId _EncodeCmdBindPipeline(avxCmdb cmdb, afxUnit segment, avxPipeline pip, avxVertexInput vin, afxFlags dynamics)
{
    afxError err = AFX_ERR_NONE;
#if 0
    avxPipeline next = cmdb->levelCaches[line].pip.next;

    if (next != pip)
    {
        afxCmdId cmdId = FlushNextCachedPipelineBinding(cmdb, line);
        cmdb->levelCaches[line].pip.next = pip;
        return cmdId;
    }
#else
    cmdb->scisUpdCmdCnt;
    _zglCmdPipeline *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->segment = segment;
    cmd->pip = pip;
    cmd->vin = vin;
    cmd->dynamics = dynamics;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, BindPipeline) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
#endif
    return 0;
}

_ZGL afxCmdId _EncodeCmdPushConstants(avxCmdb cmdb, afxUnit offset, afxUnit siz, void const* data)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdPushConstants *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (siz * sizeof(cmd->data[0])));
    AfxAssert(cmd);

    cmd->offset = offset;
    cmd->siz = siz;
    AfxCopy(cmd->data, data, siz);

    return _SglEncodeCmdCommand(cmdb, offsetof(afxCmd, PushConstants) / sizeof(void*), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdBindBuffers(avxCmdb cmdb, afxUnit set, afxUnit first, afxUnit cnt, afxBuffer buf[], afxUnit offset[], afxUnit range[])
{
    afxError err = AFX_ERR_NONE;
    _zglCmdBindBuffers *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->set = set;
    cmd->first = first;
    cmd->cnt = cnt;

    for (afxUnit i = 0; i < cnt; i++)
    {
        cmd->buf[i] = buf ? buf[i] : 0;
        cmd->offset[i] = offset ? offset[i] : 0;
        cmd->range[i] = range ? range[i] : 0;
    }
    return _SglEncodeCmdCommand(cmdb, offsetof(afxCmd, BindBuffers) / sizeof(void*), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdBindRasters(avxCmdb cmdb, afxUnit set, afxUnit first, afxUnit cnt, afxRaster tex[], afxUnit const subras[])
{
    afxError err = AFX_ERR_NONE;
    _zglCmdBindRasters *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->set = set;
    cmd->first = first;
    cmd->cnt = cnt;

    for (afxUnit i = 0; i < cnt; i++)
    {
        cmd->tex[i] = tex ? tex[i] : 0;
        cmd->subras[i] = subras ? subras[i] : 0;
    }

    return _SglEncodeCmdCommand(cmdb, offsetof(afxCmd, BindRasters) / sizeof(void*), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdBindSamplers(avxCmdb cmdb, afxUnit set, afxUnit first, afxUnit cnt, avxSampler smp[])
{
    afxError err = AFX_ERR_NONE;
    _zglCmdBindSamplers *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->set = set;
    cmd->first = first;
    cmd->cnt = cnt;

    for (afxUnit i = 0; i < cnt; i++)
    {
        cmd->smp[i] = smp ? smp[i] : 0;
    }

    return _SglEncodeCmdCommand(cmdb, offsetof(afxCmd, BindSamplers) / sizeof(void*), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _SglEncodeCmdBindVertexSources(avxCmdb cmdb, afxUnit baseSlot, afxUnit cnt, afxBuffer buf[], afxUnit32 const offset[], afxUnit32 const range[], afxUnit32 const stride[])
{
    afxError err = AFX_ERR_NONE;
    _zglCmdVertexSources *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (cnt * sizeof(cmd->info[0])));
    AfxAssert(cmd);
    cmd->first = baseSlot;
    cmd->cnt = cnt;

    for (afxUnit i = 0; i < cnt; i++)
    {
        zglBufferInfo* info = &cmd->info[i];
        info->buf = buf ? buf[i] : NIL;
        info->offset = offset ? offset[i] : 0;
        info->range = range ? range[i] : 0;
        info->stride = stride ? stride[i] : 0;

    }
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, xfrm.BindVertexSources) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdBindIndexSource(avxCmdb cmdb, afxBuffer buf, afxUnit32 offset, afxUnit32 range, afxUnit32 idxSiz)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdBufferRange *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->buf = buf;
    cmd->offset = offset;
    cmd->range = range;
    cmd->stride = idxSiz;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, xfrm.BindIndexSource) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdBindVertexInput(avxCmdb cmdb, avxVertexInput vin)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdVertexInput *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->vin = vin;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, xfrm.BindVertexInput) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

////////////////////////////////////////////////////////////////////////////////
// DO WORK

_ZGL afxCmdId _EncodeCmdExecuteCommands(avxCmdb cmdb, afxUnit cnt, avxCmdb subsets[])
{
    afxError err = AFX_ERR_NONE;
    _zglCmdExecCmds *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->cnt = cnt;

    for (afxUnit i = 0; i < cnt; i++)
        cmd->subsets[i] = subsets[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, ExecuteCommands) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDraw(avxCmdb cmdb, afxUnit32 vtxCnt, afxUnit32 instCnt, afxUnit32 firstVtx, afxUnit32 firstInst)
{
    afxError err = AFX_ERR_NONE;
    FlushCachedDrawStuff(cmdb, 0);

    _zglCmdDraw *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->data.vtxCnt = vtxCnt;
    cmd->data.instCnt = instCnt;
    cmd->data.firstVtx = firstVtx;
    cmd->data.firstInst = firstInst;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, Draw) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDrawIndirect(avxCmdb cmdb, afxBuffer buf, afxUnit32 offset, afxUnit32 drawCnt, afxUnit32 stride)
{
    afxError err = AFX_ERR_NONE;
    FlushCachedDrawStuff(cmdb, 0);

    _zglCmdDrawIndirect *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->buf = buf;
    cmd->offset = offset;
    cmd->drawCnt = drawCnt;
    cmd->stride = stride;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, DrawIndirect) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDrawIndirectCount(avxCmdb cmdb, afxBuffer buf, afxUnit32 offset, afxBuffer cntBuf, afxUnit32 cntBufOff, afxUnit32 maxDrawCnt, afxUnit32 stride)
{
    afxError err = AFX_ERR_NONE;
    FlushCachedDrawStuff(cmdb, 0);

    _zglCmdDrawIndirectCnt *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->buf = buf;
    cmd->offset = offset;
    cmd->cntBuf = cntBuf;
    cmd->cntBufOff = cntBufOff;
    cmd->maxDrawCnt = maxDrawCnt;
    cmd->stride = stride;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, DrawIndirectCount) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDrawIndexed(avxCmdb cmdb, afxUnit32 idxCnt, afxUnit32 instCnt, afxUnit32 firstIdx, afxUnit32 vtxOff, afxUnit32 firstInst)
{
    afxError err = AFX_ERR_NONE;
    FlushCachedDrawIndexedStuff(cmdb, 0);

    _zglCmdDrawIndexed *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->data.idxCnt = idxCnt;
    cmd->data.instCnt = instCnt;
    cmd->data.firstIdx = firstIdx;
    cmd->data.vtxOff = vtxOff;
    cmd->data.firstInst = firstInst;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, DrawIndexed) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDrawIndexedIndirect(avxCmdb cmdb, afxBuffer buf, afxUnit32 offset, afxUnit32 drawCnt, afxUnit32 stride)
{
    afxError err = AFX_ERR_NONE;
    FlushCachedDrawIndexedStuff(cmdb, 0);

    _zglCmdDrawIndexedIndirect *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->buf = buf;
    cmd->offset = offset;
    cmd->drawCnt = drawCnt;
    cmd->stride = stride;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, DrawIndexedIndirect) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDrawIndexedIndirectCount(avxCmdb cmdb, afxBuffer buf, afxUnit32 offset, afxBuffer cntBuf, afxUnit32 cntBufOff, afxUnit32 maxDrawCnt, afxUnit32 stride)
{
    afxError err = AFX_ERR_NONE;
    FlushCachedDrawIndexedStuff(cmdb, 0);

    _zglCmdDrawIndexedIndirectCnt *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->buf = buf;
    cmd->offset = offset;
    cmd->cntBuf = cntBuf;
    cmd->cntBufOff = cntBufOff;
    cmd->maxDrawCnt = maxDrawCnt;
    cmd->stride = stride;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, DrawIndexedIndirectCount) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDispatch(avxCmdb cmdb, afxUnit grpCntX, afxUnit grpCntY, afxUnit grpCntZ)
{
    afxError err = AFX_ERR_NONE;
    FlushCachedDrawStuff(cmdb, 0);

    _zglCmdDispatch *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->data.x = grpCntX;
    cmd->data.y = grpCntY;
    cmd->data.z = grpCntZ;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, Dispatch) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDispatchIndirect(avxCmdb cmdb, afxBuffer buf, afxUnit32 offset)
{
    afxError err = AFX_ERR_NONE;
    FlushCachedDrawStuff(cmdb, 0);

    _zglCmdDispatchIndirect *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->buf = buf;
    cmd->offset = offset;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, DispatchIndirect) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

////////////////////////////////////////////////////////////////////////////////
// BUFFER OPERATIONS

_ZGL afxCmdId _SglEncodeCmdBufCpy(avxCmdb cmdb, afxBuffer src, afxBuffer dst, afxUnit opCnt, afxBufferCopy const ops[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &cmdb, afxFcc_CMDB);
    AfxAssertObjects(1, &src, afxFcc_BUF);
    AfxAssertObjects(1, &dst, afxFcc_BUF);
    AfxAssert(opCnt);
    AfxAssert(ops);

    _zglCmdBufCpy *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (opCnt * sizeof(cmd->ops[0])));
    AfxAssert(cmd);
    cmd->src = src;
    cmd->dst = dst;
    cmd->opCnt = opCnt;

    AfxAssert(8 > opCnt);

    for (afxUnit i = 0; i < opCnt; i++)
        cmd->ops[i] = ops[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, buf.cpy) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _SglEncodeCmdBufSet(avxCmdb cmdb, afxBuffer buf, afxUnit offset, afxUnit range, afxUnit data)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &cmdb, afxFcc_CMDB);
    AfxAssertObjects(1, &buf, afxFcc_BUF);
    AfxAssert(range);

    _zglCmdBufSet *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->buf = buf;
    cmd->offset = offset;
    cmd->range = range;
    cmd->data = data;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, buf.set) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _SglEncodeCmdBufRw(avxCmdb cmdb, afxBuffer buf, afxUnit offset, afxUnit range, afxBool toHost, void* data)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &cmdb, afxFcc_CMDB);
    AfxAssertObjects(1, &buf, afxFcc_BUF);
    AfxAssert(range);
    AfxAssert(data);
    afxCmdId rslt;
    _zglCmdBufRw *cmd;

    if (!toHost)
    {
        cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + range);
        AfxAssert(cmd);
        cmd->toHost = !!toHost;
        cmd->buf = buf;
        cmd->offset = offset;
        cmd->range = range;

        AfxCopy2(cmd->src, data, 1, range);

        rslt = _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, buf.rw) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
    }
    else
    {
        cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
        AfxAssert(cmd);
        cmd->toHost = !!toHost;
        cmd->buf = buf;
        cmd->offset = offset;
        cmd->range = range;
        cmd->dst = data;

        rslt = _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, buf.rw) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
    }
    return rslt;
}

////////////////////////////////////////////////////////////////////////////////
// RASTER OPERATIONS

_ZGL afxCmdId _SglEncodeCmdRasCopy(avxCmdb cmdb, afxRaster src, afxRaster dst, afxUnit opCnt, afxRasterCopy const ops[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssert(cmdb->m.state == avxCmdbState_RECORDING);

    _zglCmdCopyRasterRegions *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (opCnt * sizeof(cmd->ops[0])));
    AfxAssert(cmd);
    cmd->src = src;
    cmd->dst = dst;
    cmd->opCnt = opCnt;
    AfxAssert(8 > opCnt);

    for (afxUnit i = 0; i < opCnt; i++)
        cmd->ops[i] = ops[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, ras.cpy) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _SglEncodeCmdRasPack(avxCmdb cmdb, afxRaster ras, afxBuffer buf, afxUnit opCnt, afxRasterIo const ops[], afxBool unpack)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &cmdb, afxFcc_CMDB);
    AfxAssertObjects(1, &ras, afxFcc_RAS);
    AfxAssertObjects(1, &buf, afxFcc_BUF);
    AfxAssert(opCnt);
    AfxAssert(ops);

    _zglCmdPackRasterRegions *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (opCnt * sizeof(cmd->ops[0])));
    AfxAssert(cmd);
    cmd->unpack = !!unpack;
    cmd->ras = ras;
    cmd->buf = buf;
    cmd->opCnt = opCnt;
    AfxAssert(8 > opCnt);

    for (afxUnit i = 0; i < opCnt; i++)
        cmd->ops[i] = ops[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, ras.pak) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _SglEncodeCmdRasSubsample(avxCmdb cmdb, afxRaster ras, afxUnit baseLod, afxUnit lodCnt)
{
    afxError err = AFX_ERR_NONE;
    AfxAssert(cmdb->m.state == avxCmdbState_RECORDING);

    _zglCmdRegenerateMipmaps *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->ras = ras;
    cmd->baseLod = baseLod;
    cmd->lodCnt = lodCnt;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, ras.mip) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _SglEncodeCmdRasSwizzle(avxCmdb cmdb, afxRaster ras, avxColorSwizzle a, avxColorSwizzle b, afxUnit rgnCnt, afxRasterRegion const rgn[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssert(cmdb->m.state == avxCmdbState_RECORDING);

    _zglCmdSwizzleRasterRegions *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (rgnCnt * sizeof(cmd->rgn[0])));
    AfxAssert(cmd);
    cmd->ras = ras;
    cmd->a = a;
    cmd->b = b;
    cmd->rgnCnt = rgnCnt;
    AfxAssert(8 > rgnCnt);

    for (afxUnit i = 0; i < rgnCnt; i++)
        cmd->rgn[i] = rgn[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, ras.swizzle) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _SglEncodeCmdRasXform(avxCmdb cmdb, afxRaster ras, afxM4d const m, afxUnit rgnCnt, afxRasterRegion const rgn[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssert(cmdb->m.state == avxCmdbState_RECORDING);

    _zglCmdFlipRasterRegions *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (rgnCnt * sizeof(cmd->rgn[0])));
    AfxAssert(cmd);
    cmd->ras = ras;
    AfxM4dCopy(cmd->m, m);
    cmd->rgnCnt = rgnCnt;
    AfxAssert(8 > rgnCnt);

    for (afxUnit i = 0; i < rgnCnt; i++)
        cmd->rgn[i] = rgn[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, ras.xform) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

////////////////////////////////////////////////////////////////////////////////
// VERTEX TRANSFORMATION OPERATIONS

_ZGL afxCmdId _EncodeCmdAdjustViewports(avxCmdb cmdb, afxUnit32 first, afxUnit32 cnt, afxViewport const vp[])
{
    afxError err = AFX_ERR_NONE;
    _zglCmdViewport *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (cnt * sizeof(cmd->vp[0])));
    AfxAssert(cmd);
    cmd->first = first;
    cmd->cnt = cnt;

    for (afxUnit i = 0; i < cnt; i++)
        cmd->vp[i] = vp[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, xfrm.AdjustViewports) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetPrimitiveTopology(avxCmdb cmdb, avxTopology topology)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdNat *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = topology;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, xfrm.SetPrimitiveTopology) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSwitchFrontFace(avxCmdb cmdb, afxBool cw)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdBool *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = cw;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, xfrm.SwitchFrontFace) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetCullMode(avxCmdb cmdb, avxCullMode mode)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdNat *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = mode;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, xfrm.SetCullMode) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

////////////////////////////////////////////////////////////////////////////////
// RASTERIZATION OPERATIONS

_ZGL afxCmdId _EncodeCmdCommenceDrawScope(avxCmdb cmdb, avxDrawScope const *state)
{
    afxError err = AFX_ERR_NONE;

    afxUnit rasCnt = state->rasterCnt;

    _zglCmdBeginSynthesis *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (rasCnt * sizeof(cmd->rasters[0])));
    AfxAssert(cmd);

    AfxCopyRect(&cmd->area, &state->area);
    cmd->layerCnt = state->layerCnt;
    cmd->rasterCnt = state->rasterCnt;
    cmd->canv = state->canv;
    cmd->defFbo = FALSE;

    for (afxUnit i = 0; i < rasCnt; i++)
        cmd->rasters[i] = state->rasters[i];

    if (state->depth)
        cmd->depth = *state->depth;
    else
        cmd->depth = (avxDrawTarget) { 0 };

    if (state->stencil)
        cmd->stencil = *state->stencil;
    else
        cmd->stencil = (avxDrawTarget) { 0 };

    cmdb->m.inRenderPass = TRUE;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.CommenceDrawScope) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdConcludeDrawScope(avxCmdb cmdb)
{
    afxError err = AFX_ERR_NONE;
    _zglCmd *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmdb->m.inRenderPass = FALSE;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.ConcludeDrawScope) / sizeof(void*)), sizeof(cmd), cmd);
}

_ZGL afxCmdId _EncodeCmdNextPass(avxCmdb cmdb, afxBool useAuxStreams)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdNextPass *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->useAuxScripts = !!useAuxStreams;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.NextPass) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDisableRasterization(avxCmdb cmdb, afxBool disable)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdBool *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = disable;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.DisableRasterization) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdEnableDepthBias(avxCmdb cmdb, afxBool enable)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdBool *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = enable;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.EnableDepthBias) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetDepthBias(avxCmdb cmdb, afxReal constFactor, afxReal clamp, afxReal slopeFactor)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdReal3 *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value[0] = constFactor;
    cmd->value[1] = clamp;
    cmd->value[2] = slopeFactor;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.SetDepthBias) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetLineWidth(avxCmdb cmdb, afxReal lineWidth)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdReal *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = lineWidth;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.SetLineWidth) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdEnableStencilTest(avxCmdb cmdb, afxBool enable)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdBool *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = enable;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.EnableStencilTest) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetStencilCompareMask(avxCmdb cmdb, afxMask faceMask, afxUnit32 compareMask)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdBitmaskNat32 *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->mask = faceMask;
    cmd->value = compareMask;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.SetStencilCompareMask) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetStencilWriteMask(avxCmdb cmdb, afxMask faceMask, afxUnit32 writeMask)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdBitmaskNat32 *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->mask = faceMask;
    cmd->value = writeMask;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.SetStencilWriteMask) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetStencilReference(avxCmdb cmdb, afxMask faceMask, afxUnit32 reference)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdBitmaskNat32 *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->mask = faceMask;
    cmd->value = reference;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.SetStencilReference) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdEnableDepthTest(avxCmdb cmdb, afxBool enable)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdBool *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = enable;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.EnableDepthTest) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetDepthCompareOp(avxCmdb cmdb, avxCompareOp op)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdNat *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = op;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.SetDepthCompareOp) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDisableDepthWrite(avxCmdb cmdb, afxBool enable)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdBool *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = enable;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.DisableDepthWrite) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetBlendConstants(avxCmdb cmdb, afxV4d const blendConstants)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdReal4 *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    AfxV4dCopy(cmd->value, blendConstants);
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.SetBlendConstants) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdAdjustScissors(avxCmdb cmdb, afxUnit32 first, afxUnit32 cnt, afxRect const rect[])
{
    afxError err = AFX_ERR_NONE;
    _zglCmdScissor *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (cnt * sizeof(cmd->rect[0])));
    AfxAssert(cmd);
    cmd->first = first;
    cmd->cnt = cnt;

    for (afxUnit i = 0; i < cnt; i++)
        cmd->rect[i] = rect[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.AdjustScissors) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdAdjustCurtains(avxCmdb cmdb, afxBool exclusive, afxUnit32 first, afxUnit32 cnt, afxRect const rect[])
{
    afxError err = AFX_ERR_NONE;
    _zglCmdScissor *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (cnt * sizeof(cmd->rect[0])));
    AfxAssert(cmd);
    cmd->first = first;
    cmd->cnt = cnt;

    for (afxUnit i = 0; i < cnt; i++)
        cmd->rect[i] = rect[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.AdjustCurtains) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdEnableDepthBoundsTest(avxCmdb cmdb, afxBool enable)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdBool *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = enable;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.EnableDepthBoundsTest) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetDepthBounds(avxCmdb cmdb, afxReal const bounds[2])
{
    afxError err = AFX_ERR_NONE;
    _zglCmdReal2 *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    AfxV2dCopy(cmd->value, bounds);
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.SetDepthBounds) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdPushDebugScope(avxCmdb cmdb, afxString const* name, afxColor const color)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdDebugScope *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);

    cmd->open = TRUE;

    if (color)
        AfxColorCopy(cmd->color, color);
    else
        AfxColorReset(cmd->color);

    AfxMakeString2048(&cmd->name, name);

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, PushDebugScope) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdPopDebugScope(avxCmdb cmdb)
{
    afxError err = AFX_ERR_NONE;
    _zglCmdDebugScope *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);

    cmd->open = FALSE;
    AfxColorReset(cmd->color);
    AfxMakeString2048(&cmd->name, NIL);

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, PopDebugScope) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

/////////////////////////

_ZGL afxError _SglCmdbEndCb(avxCmdb cmdb)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &cmdb, afxFcc_CMDB);
    FlushCachedStuff(cmdb, 0);
    return err;
}

_ZGL afxCmd _SglEncodeCmdVmt =
{
    .BindPipeline = _EncodeCmdBindPipeline,

    .Draw = _EncodeCmdDraw,
    .DrawIndirect = _EncodeCmdDrawIndirect,
    .DrawIndirectCount = _EncodeCmdDrawIndirectCount,

    .DrawIndexed = _EncodeCmdDrawIndexed,
    .DrawIndexedIndirect = _EncodeCmdDrawIndexedIndirect,
    .DrawIndexedIndirectCount = _EncodeCmdDrawIndexedIndirectCount,

    .Dispatch = _EncodeCmdDispatch,
    .DispatchIndirect = _EncodeCmdDispatchIndirect,

    .ExecuteCommands = _EncodeCmdExecuteCommands,
    .BindBuffers = _EncodeCmdBindBuffers,
    .BindRasters = _EncodeCmdBindRasters,
    .BindSamplers = _EncodeCmdBindSamplers,

    .buf.rw = _SglEncodeCmdBufRw,
    .buf.set = _SglEncodeCmdBufSet,
    .buf.cpy = _SglEncodeCmdBufCpy,

    .ras.swizzle = _SglEncodeCmdRasSwizzle,
    .ras.xform = _SglEncodeCmdRasXform,
    .ras.mip = _SglEncodeCmdRasSubsample,
    .ras.cpy = _SglEncodeCmdRasCopy,
    .ras.pak = _SglEncodeCmdRasPack,

    .xfrm.BindVertexInput = _EncodeCmdBindVertexInput,
    .xfrm.BindVertexSources = _SglEncodeCmdBindVertexSources,
    .xfrm.BindIndexSource = _EncodeCmdBindIndexSource,
    .xfrm.SetPrimitiveTopology = _EncodeCmdSetPrimitiveTopology,
    .xfrm.AdjustViewports = _EncodeCmdAdjustViewports,
    .xfrm.SwitchFrontFace = _EncodeCmdSwitchFrontFace,
    .xfrm.SetCullMode = _EncodeCmdSetCullMode,

    .razr.DisableRasterization = _EncodeCmdDisableRasterization,
    .razr.EnableDepthBias = _EncodeCmdEnableDepthBias,
    .razr.SetDepthBias = _EncodeCmdSetDepthBias,
    .razr.SetLineWidth = _EncodeCmdSetLineWidth,

    .razr.AdjustScissors = _EncodeCmdAdjustScissors,

    .razr.EnableDepthBoundsTest = _EncodeCmdEnableDepthBoundsTest,
    .razr.SetDepthBounds = _EncodeCmdSetDepthBounds,

    .razr.EnableStencilTest = _EncodeCmdEnableStencilTest,
    .razr.SetStencilCompareMask = _EncodeCmdSetStencilCompareMask,
    .razr.SetStencilWriteMask = _EncodeCmdSetStencilWriteMask,
    .razr.SetStencilReference = _EncodeCmdSetStencilReference,

    .razr.EnableDepthTest = _EncodeCmdEnableDepthTest,
    .razr.SetDepthCompareOp = _EncodeCmdSetDepthCompareOp,
    .razr.DisableDepthWrite = _EncodeCmdDisableDepthWrite,

    .razr.SetBlendConstants = _EncodeCmdSetBlendConstants,

    .razr.CommenceDrawScope = _EncodeCmdCommenceDrawScope,
    .razr.NextPass = _EncodeCmdNextPass,
    .razr.ConcludeDrawScope = _EncodeCmdConcludeDrawScope,

    .PushDebugScope = _EncodeCmdPushDebugScope,
    .PopDebugScope = _EncodeCmdPopDebugScope
};
