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

_ZGL afxCmdId _SglEncodeCmdCommand(avxCmdb cmdb, afxNat id, afxNat siz, _sglCmd *cmd)
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

_ZGL afxCmdId FlushNextCachedPipelineBinding(avxCmdb cmdb, afxNat segment)
{
    afxError err = AFX_ERR_NONE;
    AfxAssert(cmdb->m.state == avxCmdbState_RECORDING);
    avxPipeline curr = cmdb->levelCaches[segment].pip.curr;
    avxPipeline next = cmdb->levelCaches[segment].pip.next;

    if ((curr != next) && next)
    {
        cmdb->levelCaches[segment].pip.curr = next;
        cmdb->levelCaches[segment].pip.next = NIL;

        _sglCmdPipeline *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
        AfxAssert(cmd);
        cmd->segment = segment;
        cmd->pip = next;
        return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, BindPipeline) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
    }
    return 0;
}

_ZGL afxCmdId FlushCachedDrawStuff(avxCmdb cmdb, afxNat segment)
{
    afxError err = AFX_ERR_NONE;
    AfxAssert(cmdb->m.state == avxCmdbState_RECORDING);

    return FlushNextCachedPipelineBinding(cmdb, segment);
}

_ZGL afxCmdId FlushCachedDrawIndexedStuff(avxCmdb cmdb, afxNat segment)
{
    afxError err = AFX_ERR_NONE;
    AfxAssert(cmdb->m.state == avxCmdbState_RECORDING);
    return FlushCachedDrawStuff(cmdb, segment);
}

_ZGL afxCmdId FlushCachedStuff(avxCmdb cmdb, afxNat segment)
{
    afxError err = AFX_ERR_NONE;
    AfxAssert(cmdb->m.state == avxCmdbState_RECORDING);
    return FlushCachedDrawIndexedStuff(cmdb, segment);
}

////////////////////////////////////////////////////////////////////////////////
// BIND RESOURCES

_ZGL afxCmdId _EncodeCmdBindPipeline(avxCmdb cmdb, afxNat segment, avxPipeline pip, avxVertexInput vin, afxFlags dynamics)
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

    _sglCmdPipeline *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->segment = segment;
    cmd->pip = pip;
    cmd->vin = vin;
    cmd->dynamics = dynamics;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, BindPipeline) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
#endif
    return 0;
}

_ZGL afxCmdId _EncodeCmdBindRasterizer(avxCmdb cmdb, avxRasterizer razr, avxVertexInput vin, afxFlags dynamics)
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

    _sglCmdRasterizer *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->razr = razr;
    cmd->vin = vin;
    cmd->dynamics = dynamics;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, BindRasterizer) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
#endif
    return 0;
}

_ZGL afxCmdId _EncodeCmdBindBuffers(avxCmdb cmdb, afxNat set, afxNat first, afxNat cnt, afxBuffer buf[], afxNat offset[], afxNat range[])
{
    afxError err = AFX_ERR_NONE;
    _sglCmdBindBuffers *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->set = set;
    cmd->first = first;
    cmd->cnt = cnt;

    for (afxNat i = 0; i < cnt; i++)
    {
        cmd->buf[i] = buf ? buf[i] : 0;
        cmd->offset[i] = offset ? offset[i] : 0;
        cmd->range[i] = range ? range[i] : 0;
    }
    return _SglEncodeCmdCommand(cmdb, offsetof(afxCmd, BindBuffers) / sizeof(void*), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdBindRasters(avxCmdb cmdb, afxNat set, afxNat first, afxNat cnt, afxRaster tex[], afxNat const subras[])
{
    afxError err = AFX_ERR_NONE;
    _sglCmdBindRasters *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->set = set;
    cmd->first = first;
    cmd->cnt = cnt;

    for (afxNat i = 0; i < cnt; i++)
    {
        cmd->tex[i] = tex ? tex[i] : 0;
        cmd->subras[i] = subras ? subras[i] : 0;
    }

    return _SglEncodeCmdCommand(cmdb, offsetof(afxCmd, BindRasters) / sizeof(void*), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdBindSamplers(avxCmdb cmdb, afxNat set, afxNat first, afxNat cnt, avxSampler smp[])
{
    afxError err = AFX_ERR_NONE;
    _sglCmdBindSamplers *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->set = set;
    cmd->first = first;
    cmd->cnt = cnt;

    for (afxNat i = 0; i < cnt; i++)
    {
        cmd->smp[i] = smp ? smp[i] : 0;
    }

    return _SglEncodeCmdCommand(cmdb, offsetof(afxCmd, BindSamplers) / sizeof(void*), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _SglEncodeCmdBindVertexSources(avxCmdb cmdb, afxNat baseSlot, afxNat cnt, afxBuffer buf[], afxNat32 const offset[], afxNat32 const range[], afxNat32 const stride[])
{
    afxError err = AFX_ERR_NONE;
    _sglCmdVertexSources *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (cnt * sizeof(cmd->info[0])));
    AfxAssert(cmd);
    cmd->first = baseSlot;
    cmd->cnt = cnt;

    for (afxNat i = 0; i < cnt; i++)
    {
        sglBufferInfo* info = &cmd->info[i];
        info->buf = buf ? buf[i] : NIL;
        info->offset = offset ? offset[i] : 0;
        info->range = range ? range[i] : 0;
        info->stride = stride ? stride[i] : 0;

    }
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, xfrm.BindVertexSources) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdBindIndexSource(avxCmdb cmdb, afxBuffer buf, afxNat32 offset, afxNat32 range, afxNat32 idxSiz)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdBufferRange *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
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
    _sglCmdVertexInput *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->vin = vin;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, xfrm.BindVertexInput) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

////////////////////////////////////////////////////////////////////////////////
// DO WORK

_ZGL afxCmdId _EncodeCmdExecuteCommands(avxCmdb cmdb, afxNat cnt, avxCmdb subsets[])
{
    afxError err = AFX_ERR_NONE;
    _sglCmdExecCmds *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->cnt = cnt;

    for (afxNat i = 0; i < cnt; i++)
        cmd->subsets[i] = subsets[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, ExecuteCommands) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDraw(avxCmdb cmdb, afxNat32 vtxCnt, afxNat32 instCnt, afxNat32 firstVtx, afxNat32 firstInst)
{
    afxError err = AFX_ERR_NONE;
    FlushCachedDrawStuff(cmdb, 0);

    _sglCmdDraw *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->data.vtxCnt = vtxCnt;
    cmd->data.instCnt = instCnt;
    cmd->data.firstVtx = firstVtx;
    cmd->data.firstInst = firstInst;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, Draw) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDrawIndirect(avxCmdb cmdb, afxBuffer buf, afxNat32 offset, afxNat32 drawCnt, afxNat32 stride)
{
    afxError err = AFX_ERR_NONE;
    FlushCachedDrawStuff(cmdb, 0);

    _sglCmdDrawIndirect *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->buf = buf;
    cmd->offset = offset;
    cmd->drawCnt = drawCnt;
    cmd->stride = stride;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, DrawIndirect) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDrawIndirectCount(avxCmdb cmdb, afxBuffer buf, afxNat32 offset, afxBuffer cntBuf, afxNat32 cntBufOff, afxNat32 maxDrawCnt, afxNat32 stride)
{
    afxError err = AFX_ERR_NONE;
    FlushCachedDrawStuff(cmdb, 0);

    _sglCmdDrawIndirectCnt *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->buf = buf;
    cmd->offset = offset;
    cmd->cntBuf = cntBuf;
    cmd->cntBufOff = cntBufOff;
    cmd->maxDrawCnt = maxDrawCnt;
    cmd->stride = stride;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, DrawIndirectCount) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDrawIndexed(avxCmdb cmdb, afxNat32 idxCnt, afxNat32 instCnt, afxNat32 firstIdx, afxNat32 vtxOff, afxNat32 firstInst)
{
    afxError err = AFX_ERR_NONE;
    FlushCachedDrawIndexedStuff(cmdb, 0);

    _sglCmdDrawIndexed *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->data.idxCnt = idxCnt;
    cmd->data.instCnt = instCnt;
    cmd->data.firstIdx = firstIdx;
    cmd->data.vtxOff = vtxOff;
    cmd->data.firstInst = firstInst;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, DrawIndexed) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDrawIndexedIndirect(avxCmdb cmdb, afxBuffer buf, afxNat32 offset, afxNat32 drawCnt, afxNat32 stride)
{
    afxError err = AFX_ERR_NONE;
    FlushCachedDrawIndexedStuff(cmdb, 0);

    _sglCmdDrawIndexedIndirect *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->buf = buf;
    cmd->offset = offset;
    cmd->drawCnt = drawCnt;
    cmd->stride = stride;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, DrawIndexedIndirect) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDrawIndexedIndirectCount(avxCmdb cmdb, afxBuffer buf, afxNat32 offset, afxBuffer cntBuf, afxNat32 cntBufOff, afxNat32 maxDrawCnt, afxNat32 stride)
{
    afxError err = AFX_ERR_NONE;
    FlushCachedDrawIndexedStuff(cmdb, 0);

    _sglCmdDrawIndexedIndirectCnt *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->buf = buf;
    cmd->offset = offset;
    cmd->cntBuf = cntBuf;
    cmd->cntBufOff = cntBufOff;
    cmd->maxDrawCnt = maxDrawCnt;
    cmd->stride = stride;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, DrawIndexedIndirectCount) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDispatch(avxCmdb cmdb, afxNat grpCntX, afxNat grpCntY, afxNat grpCntZ)
{
    afxError err = AFX_ERR_NONE;
    FlushCachedDrawStuff(cmdb, 0);

    _sglCmdDispatch *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->data.x = grpCntX;
    cmd->data.y = grpCntY;
    cmd->data.z = grpCntZ;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, Dispatch) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDispatchIndirect(avxCmdb cmdb, afxBuffer buf, afxNat32 offset)
{
    afxError err = AFX_ERR_NONE;
    FlushCachedDrawStuff(cmdb, 0);

    _sglCmdDispatchIndirect *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->buf = buf;
    cmd->offset = offset;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, DispatchIndirect) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

////////////////////////////////////////////////////////////////////////////////
// BUFFER OPERATIONS

_ZGL afxCmdId _SglEncodeCmdBufCpy(avxCmdb cmdb, afxBuffer src, afxBuffer dst, afxNat opCnt, afxBufferCopy const ops[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &cmdb, afxFcc_CMDB);
    AfxAssertObjects(1, &src, afxFcc_BUF);
    AfxAssertObjects(1, &dst, afxFcc_BUF);
    AfxAssert(opCnt);
    AfxAssert(ops);

    _sglCmdBufCpy *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (opCnt * sizeof(cmd->ops[0])));
    AfxAssert(cmd);
    cmd->src = src;
    cmd->dst = dst;
    cmd->opCnt = opCnt;

    AfxAssert(8 > opCnt);

    for (afxNat i = 0; i < opCnt; i++)
        cmd->ops[i] = ops[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, buf.cpy) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _SglEncodeCmdBufSet(avxCmdb cmdb, afxBuffer buf, afxNat offset, afxNat range, afxNat data)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &cmdb, afxFcc_CMDB);
    AfxAssertObjects(1, &buf, afxFcc_BUF);
    AfxAssert(range);

    _sglCmdBufSet *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->buf = buf;
    cmd->offset = offset;
    cmd->range = range;
    cmd->data = data;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, buf.set) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _SglEncodeCmdBufRw(avxCmdb cmdb, afxBuffer buf, afxNat offset, afxNat range, afxBool toHost, void* data)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &cmdb, afxFcc_CMDB);
    AfxAssertObjects(1, &buf, afxFcc_BUF);
    AfxAssert(range);
    AfxAssert(data);
    afxCmdId rslt;
    _sglCmdBufRw *cmd;

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

_ZGL afxCmdId _SglEncodeCmdRasCopy(avxCmdb cmdb, afxRaster src, afxRaster dst, afxNat opCnt, afxRasterCopy const ops[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssert(cmdb->m.state == avxCmdbState_RECORDING);

    _sglCmdCopyRasterRegions *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (opCnt * sizeof(cmd->ops[0])));
    AfxAssert(cmd);
    cmd->src = src;
    cmd->dst = dst;
    cmd->opCnt = opCnt;
    AfxAssert(8 > opCnt);

    for (afxNat i = 0; i < opCnt; i++)
        cmd->ops[i] = ops[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, ras.cpy) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _SglEncodeCmdRasPack(avxCmdb cmdb, afxRaster ras, afxBuffer buf, afxNat opCnt, afxRasterIo const ops[], afxBool unpack)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &cmdb, afxFcc_CMDB);
    AfxAssertObjects(1, &ras, afxFcc_RAS);
    AfxAssertObjects(1, &buf, afxFcc_BUF);
    AfxAssert(opCnt);
    AfxAssert(ops);

    _sglCmdPackRasterRegions *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (opCnt * sizeof(cmd->ops[0])));
    AfxAssert(cmd);
    cmd->unpack = !!unpack;
    cmd->ras = ras;
    cmd->buf = buf;
    cmd->opCnt = opCnt;
    AfxAssert(8 > opCnt);

    for (afxNat i = 0; i < opCnt; i++)
        cmd->ops[i] = ops[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, ras.pak) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _SglEncodeCmdRasSubsample(avxCmdb cmdb, afxRaster ras, afxNat baseLod, afxNat lodCnt)
{
    afxError err = AFX_ERR_NONE;
    AfxAssert(cmdb->m.state == avxCmdbState_RECORDING);

    _sglCmdRegenerateMipmaps *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->ras = ras;
    cmd->baseLod = baseLod;
    cmd->lodCnt = lodCnt;

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, ras.mip) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _SglEncodeCmdRasSwizzle(avxCmdb cmdb, afxRaster ras, avxColorSwizzle a, avxColorSwizzle b, afxNat rgnCnt, afxRasterRegion const rgn[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssert(cmdb->m.state == avxCmdbState_RECORDING);

    _sglCmdSwizzleRasterRegions *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (rgnCnt * sizeof(cmd->rgn[0])));
    AfxAssert(cmd);
    cmd->ras = ras;
    cmd->a = a;
    cmd->b = b;
    cmd->rgnCnt = rgnCnt;
    AfxAssert(8 > rgnCnt);

    for (afxNat i = 0; i < rgnCnt; i++)
        cmd->rgn[i] = rgn[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, ras.swizzle) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _SglEncodeCmdRasXform(avxCmdb cmdb, afxRaster ras, afxM4d const m, afxNat rgnCnt, afxRasterRegion const rgn[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssert(cmdb->m.state == avxCmdbState_RECORDING);

    _sglCmdFlipRasterRegions *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (rgnCnt * sizeof(cmd->rgn[0])));
    AfxAssert(cmd);
    cmd->ras = ras;
    AfxM4dCopy(cmd->m, m);
    cmd->rgnCnt = rgnCnt;
    AfxAssert(8 > rgnCnt);

    for (afxNat i = 0; i < rgnCnt; i++)
        cmd->rgn[i] = rgn[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, ras.xform) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

////////////////////////////////////////////////////////////////////////////////
// VERTEX TRANSFORMATION OPERATIONS

_ZGL afxCmdId _EncodeCmdAdjustViewports(avxCmdb cmdb, afxNat32 first, afxNat32 cnt, afxViewport const vp[])
{
    afxError err = AFX_ERR_NONE;
    _sglCmdViewport *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (cnt * sizeof(cmd->vp[0])));
    AfxAssert(cmd);
    cmd->first = first;
    cmd->cnt = cnt;

    for (afxNat i = 0; i < cnt; i++)
        cmd->vp[i] = vp[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, xfrm.AdjustViewports) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetPrimitiveTopology(avxCmdb cmdb, avxTopology topology)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdNat *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = topology;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, xfrm.SetPrimitiveTopology) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSwitchFrontFace(avxCmdb cmdb, afxBool cw)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdBool *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = cw;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, xfrm.SwitchFrontFace) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetCullMode(avxCmdb cmdb, avxCullMode mode)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdNat *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = mode;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, xfrm.SetCullMode) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

////////////////////////////////////////////////////////////////////////////////
// RASTERIZATION OPERATIONS

_ZGL afxCmdId _EncodeCmdBeginDrawScope(avxCmdb cmdb, avxSynthesisConfig const *state)
{
    afxError err = AFX_ERR_NONE;

    afxNat rasCnt = state->rasterCnt;

    _sglCmdBeginSynthesis *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (rasCnt * sizeof(cmd->rasters[0])));
    AfxAssert(cmd);

    AfxCopyRect(&cmd->area, &state->area);
    cmd->layerCnt = state->layerCnt;
    cmd->rasterCnt = state->rasterCnt;
    cmd->canv = state->canv;
    cmd->defFbo = FALSE;

    for (afxNat i = 0; i < rasCnt; i++)
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

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.BeginDrawScope) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdEndDrawScope(avxCmdb cmdb)
{
    afxError err = AFX_ERR_NONE;
    _sglCmd *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmdb->m.inRenderPass = FALSE;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.EndDrawScope) / sizeof(void*)), sizeof(cmd), cmd);
}

_ZGL afxCmdId _EncodeCmdNextPass(avxCmdb cmdb, afxBool useAuxStreams)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdNextPass *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->useAuxScripts = !!useAuxStreams;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.NextPass) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDisableRasterization(avxCmdb cmdb, afxBool disable)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdBool *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = disable;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.DisableRasterization) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdEnableDepthBias(avxCmdb cmdb, afxBool enable)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdBool *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = enable;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.EnableDepthBias) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetDepthBias(avxCmdb cmdb, afxReal constFactor, afxReal clamp, afxReal slopeFactor)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdReal3 *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value[0] = constFactor;
    cmd->value[1] = clamp;
    cmd->value[2] = slopeFactor;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.SetDepthBias) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetLineWidth(avxCmdb cmdb, afxReal lineWidth)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdReal *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = lineWidth;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.SetLineWidth) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdEnableStencilTest(avxCmdb cmdb, afxBool enable)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdBool *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = enable;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.EnableStencilTest) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetStencilCompareMask(avxCmdb cmdb, afxMask faceMask, afxNat32 compareMask)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdBitmaskNat32 *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->mask = faceMask;
    cmd->value = compareMask;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.SetStencilCompareMask) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetStencilWriteMask(avxCmdb cmdb, afxMask faceMask, afxNat32 writeMask)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdBitmaskNat32 *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->mask = faceMask;
    cmd->value = writeMask;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.SetStencilWriteMask) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetStencilReference(avxCmdb cmdb, afxMask faceMask, afxNat32 reference)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdBitmaskNat32 *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->mask = faceMask;
    cmd->value = reference;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.SetStencilReference) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdEnableDepthTest(avxCmdb cmdb, afxBool enable)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdBool *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = enable;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.EnableDepthTest) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetDepthCompareOp(avxCmdb cmdb, avxCompareOp op)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdNat *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = op;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.SetDepthCompareOp) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdDisableDepthWrite(avxCmdb cmdb, afxBool enable)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdBool *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = enable;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.DisableDepthWrite) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetBlendConstants(avxCmdb cmdb, afxV4d const blendConstants)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdReal4 *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    AfxV4dCopy(cmd->value, blendConstants);
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.SetBlendConstants) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdAdjustScissors(avxCmdb cmdb, afxNat32 first, afxNat32 cnt, afxRect const rect[])
{
    afxError err = AFX_ERR_NONE;
    _sglCmdScissor *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (cnt * sizeof(cmd->rect[0])));
    AfxAssert(cmd);
    cmd->first = first;
    cmd->cnt = cnt;

    for (afxNat i = 0; i < cnt; i++)
        cmd->rect[i] = rect[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.AdjustScissors) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdAdjustCurtains(avxCmdb cmdb, afxBool exclusive, afxNat32 first, afxNat32 cnt, afxRect const rect[])
{
    afxError err = AFX_ERR_NONE;
    _sglCmdScissor *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd) + (cnt * sizeof(cmd->rect[0])));
    AfxAssert(cmd);
    cmd->first = first;
    cmd->cnt = cnt;

    for (afxNat i = 0; i < cnt; i++)
        cmd->rect[i] = rect[i];

    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.AdjustCurtains) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdEnableDepthBoundsTest(avxCmdb cmdb, afxBool enable)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdBool *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    cmd->value = enable;
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.EnableDepthBoundsTest) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdSetDepthBounds(avxCmdb cmdb, afxReal const bounds[2])
{
    afxError err = AFX_ERR_NONE;
    _sglCmdReal2 *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
    AfxAssert(cmd);
    AfxV2dCopy(cmd->value, bounds);
    return _SglEncodeCmdCommand(cmdb, (offsetof(afxCmd, razr.SetDepthBounds) / sizeof(void*)), sizeof(cmd), &cmd->cmd);
}

_ZGL afxCmdId _EncodeCmdPushDebugScope(avxCmdb cmdb, afxString const* name, afxColor const color)
{
    afxError err = AFX_ERR_NONE;
    _sglCmdDebugScope *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
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
    _sglCmdDebugScope *cmd = AfxAllocateArena(&cmdb->m.cmdArena, sizeof(*cmd));
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
    .BindRasterizer = _EncodeCmdBindRasterizer,

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

    .razr.BeginDrawScope = _EncodeCmdBeginDrawScope,
    .razr.NextPass = _EncodeCmdNextPass,
    .razr.EndDrawScope = _EncodeCmdEndDrawScope,

    .PushDebugScope = _EncodeCmdPushDebugScope,
    .PopDebugScope = _EncodeCmdPopDebugScope
};
