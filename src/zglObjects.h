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

#ifndef ZGL_OBJECTS_H
#define ZGL_OBJECTS_H

#include "zglDevices.h"

AFX_OBJECT(avxPipeline)
{
    AFX_OBJECT(_avxPipeline) m;
    zglUpdateFlags  updFlags;
    GLuint          glHandle;
    //zglVertexInput  vertexInput;
};

AFX_OBJECT(avxVertexInput)
{
    AFX_OBJECT(_avxVertexInput) m;
    zglUpdateFlags  updFlags;
    GLuint          glHandle;
    zglVertexInputState bindings;
};

AFX_OBJECT(avxQueryPool)
{
    AFX_OBJECT(_avxQueryPool) m;
    zglUpdateFlags  updFlags;
    GLuint         *glHandle; // * cap
    GLenum          glTarget;
};

AFX_OBJECT(avxLigature)
{
    AFX_OBJECT(_avxLigature) m;
    int a;
};

AFX_OBJECT(avxShader)
{
    AFX_OBJECT(_avxShader) m;
    zglUpdateFlags      updFlags;
    afxUnit              glHandle;
    afxUnit              glProgHandle;
    afxBool             compiled;
};

AFX_OBJECT(avxSampler)
{
    AFX_OBJECT(_avxSampler) m;
    zglUpdateFlags  updFlags;
    afxUnit          glHandle;
};

AFX_OBJECT(afxFence)
{
    AFX_OBJECT(_afxFence) m;
    zglUpdateFlags  updFlags;
    GLsync          glHandle;
};

AFX_OBJECT(afxSemaphore)
{
    AFX_OBJECT(_afxSemaphore) m;
};

AFX_OBJECT(afxBuffer)
{
    AFX_OBJECT(_avxBuffer) m;
    // idd

    zglUpdateFlags  updFlags;
    afxUnit          glHandle;
    afxUnit          glTexHandle; // if GL_TEXTURE_BUFFER, it would be a GL texture handle used to bind the buffer.
    // Since Vulkan doesn't share OpenGL's notion that this is some kind of texture, the descriptor's data is a VkBufferView, not a VkImage or VkImageView. Specifically, VkWriteDescriptorSet::pTexelBufferView.
    GLenum          glTexIntFmt;
    GLenum          glTarget;
    //GLenum          glUsage;
    GLbitfield      glAccess;
    //GLintptr        glMappedOff;
    //GLsizeiptr      glMappedSiz;
};

AFX_OBJECT(afxRaster)
{
    AFX_OBJECT(_avxRaster) m;
    zglUpdateFlags  updFlags;
    GLuint          glHandle;
    GLuint          glRboHandle; // for draw buffers when raster is created with afxRasterUsage_DRAW only
    GLint           glIntFmt;
    GLenum          glTarget, glFmt, glType;
};

AFX_OBJECT(avxCanvas)
{
    AFX_OBJECT(_avxCanvas) m;
    zglUpdateFlags  updFlags;
    afxUnit          glHandle;
    afxMask         storeBitmask;
};

AFX_OBJECT(avxCmdb)
{
    AFX_OBJECT(_avxCmdb) m;
    afxChain                    commands;
    afxChain                    echoes;
    afxChain                    canvOps;
    afxChain            drawCalls;
    afxChain            stateChanges;
    afxChain            pipBinds;
    afxChain            vbufBinds;
    afxChain            ibufBinds;
    afxChain            bufBinds;
    afxChain            texBinds;

    afxUnit              totalCmdCnt;
    afxUnit              lastDrawCmdIdx;
    afxUnit              drawCmdCnt;
    afxUnit              lastPipBindCmdIdx;
    afxUnit              pipBindCmdCnt;
    afxUnit              lastVbufBindCmdIdx;
    afxUnit              vbufBindCmdCnt;
    afxUnit              lastIbufBindCmdIdx;
    afxUnit              ibufBindCmdCnt;
    afxUnit              lastBufBindCmdIdx;
    afxUnit              bufBindCmdCnt;
    afxUnit              lastTexBindCmdIdx;
    afxUnit              texBindCmdCnt;
    afxUnit              lastRasUpdCmdIdx;
    afxUnit              rasUpdCmdCnt;
    afxUnit              lastDepthUpdCmdIdx;
    afxUnit              depthUpdCmdCnt;
    afxUnit              lastVpCmdIdx; // viewport and/or scissor changes
    afxUnit              vpUpdCmdCnt;
    afxUnit              lastScisCmdIdx; // viewport and/or scissor changes
    afxUnit              scisUpdCmdCnt;
    struct
    {
        struct
        {
            avxPipeline     next;
            avxPipeline     curr;
        } pip;
    } levelCaches[1];
};

ZGL void _SglDctxEnqueueDeletion(afxDrawContext dctx, afxUnit exuIdx, afxUnit type, afxSize gpuHandle);


ZGL afxError DpuBindAndSyncBuf(zglDpu* dpu, GLenum glTarget, afxBuffer buf);
ZGL afxError _DpuRemapBuf(zglDpu* dpu, afxBuffer buf, afxSize offset, afxUnit range, afxFlags flags, void** placeholder);
ZGL afxError _DpuStoreBuf(zglDpu* dpu, afxBuffer buf, afxSize offset, afxUnit range, afxByte* dst);
ZGL afxError _DpuLoadBuf(zglDpu* dpu, afxBuffer buf, afxSize offset, afxUnit range, afxByte const* src);
ZGL afxError _DpuOutputBuf(zglDpu* dpu, afxBuffer buf, afxSize offset, afxUnit range, afxStream out, afxSize at);
ZGL afxError _DpuInputBuf(zglDpu* dpu, afxBuffer buf, afxSize offset, afxUnit range, afxStream in, afxSize at);

ZGL afxError _DpuLoadBuf2(zglDpu* dpu, afxBuffer buf, afxByte const* src, afxUnit opCnt, afxBufferIo const* ops);
ZGL afxError _DpuStoreBuf2(zglDpu* dpu, afxBuffer buf, afxByte* dst, afxUnit opCnt, afxBufferIo const* ops);
ZGL afxError _DpuOutputBuf2(zglDpu* dpu, afxBuffer buf, afxStream out, afxUnit opCnt, afxBufferIo const* ops);
ZGL afxError _DpuInputBuf2(zglDpu* dpu, afxBuffer buf, afxStream in, afxUnit opCnt, afxBufferIo const* ops);

ZGL afxError _DpuUnpackRasFromMem(zglDpu* dpu, afxRaster ras, afxByte const* src, afxUnit opCnt, afxRasterIo const* ops);
ZGL afxError _DpuPackRasToMem(zglDpu* dpu, afxRaster ras, afxByte* dst, afxUnit opCnt, afxRasterIo const* ops);
ZGL afxError _DpuPackRasToBuf(zglDpu* dpu, afxRaster ras, afxBuffer buf, afxUnit opCnt, afxRasterIo const* ops);
ZGL afxError _DpuUnpackRasFromBuf(zglDpu* dpu, afxRaster ras, afxBuffer buf, afxUnit opCnt, afxRasterIo const* ops);
ZGL afxError _DpuPackRasToIob(zglDpu* dpu, afxRaster ras, afxStream out, afxUnit opCnt, afxRasterIo const* ops);
ZGL afxError _DpuUnpackRasFromIob(zglDpu* dpu, afxRaster ras, afxStream in, afxUnit opCnt, afxRasterIo const* ops);

ZGL afxError _DpuCopyRas(zglDpu* dpu, afxRaster src, afxRaster dst, afxUnit opCnt, afxRasterCopy const ops[]);

ZGL afxError _SglWaitFenc(afxBool waitAll, afxUnit64 timeout, afxUnit cnt, afxFence const fences[]);
ZGL afxError _SglResetFenc(afxUnit cnt, afxFence const fences[]);

ZGL afxClassConfig const _SglFencMgrCfg;
ZGL afxClassConfig const _SglSemMgrCfg;

ZGL afxClassConfig _SglCmdbMgrCfg;

ZGL afxError _SglPipCtor(avxPipeline pip, void** args, afxUnit invokeNo);
ZGL afxError _SglPipDtor(avxPipeline pip);
ZGL afxError _SglRazrDtor(avxRasterizer razr);
ZGL afxError _SglRazrCtor(avxRasterizer razr, void** args, afxUnit invokeNo);
ZGL afxError _SglShdCtor(avxShader shd, void** args, afxUnit invokeNo);
ZGL afxError _SglShdDtor(avxShader shd);
ZGL afxError _SglVinCtor(avxVertexInput vin, void** args, afxUnit invokeNo);
ZGL afxError _SglVinDtor(avxVertexInput vin);
ZGL afxError _SglCanvCtor(avxCanvas canv, void** args, afxUnit invokeNo);
ZGL afxError _SglCanvDtor(avxCanvas canv);
ZGL afxError _SglRasCtor(afxRaster ras, void** args, afxUnit invokeNo);
ZGL afxError _SglRasDtor(afxRaster ras);
ZGL afxError _BufCtorCb(afxBuffer buf, void** args, afxUnit invokeNo);
ZGL afxError _BufDtorCb(afxBuffer buf);
ZGL afxError _SglSampCtor(avxSampler samp, void** args, afxUnit invokeNo);
ZGL afxError _SglSampDtor(avxSampler samp);
ZGL afxError _SglLigaCtor(avxLigature liga, void** args, afxUnit invokeNo);
ZGL afxError _SglLigaDtor(avxLigature liga);
ZGL afxError _SglQrypCtor(avxQueryPool qryp, void** args, afxUnit invokeNo);
ZGL afxError _SglQrypDtor(avxQueryPool qryp);

#endif//ZGL_OBJECTS_H
