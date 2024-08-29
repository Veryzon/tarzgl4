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
    sglUpdateFlags  updFlags;
    GLuint          glHandle;
    //sglVertexInput  vertexInput;
};

AFX_OBJECT(avxVertexInput)
{
    AFX_OBJECT(_avxVertexInput) m;
    sglUpdateFlags  updFlags;
    GLuint          glHandle;
    sglVertexInputState bindings;
};

AFX_OBJECT(avxRasterizer)
{
    AFX_OBJECT(_avxRasterizer) m;
    sglUpdateFlags  updFlags;

};

AFX_OBJECT(avxQueryPool)
{
    AFX_OBJECT(_avxQueryPool) m;
    sglUpdateFlags  updFlags;
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
    sglUpdateFlags      updFlags;
    afxNat              glHandle;
    afxNat              glProgHandle;
    afxBool             compiled;
};

AFX_OBJECT(avxSampler)
{
    AFX_OBJECT(_avxSampler) m;
    sglUpdateFlags  updFlags;
    afxNat          glHandle;
};

AFX_OBJECT(afxFence)
{
    AFX_OBJECT(_afxFence) m;
    sglUpdateFlags  updFlags;
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

    sglUpdateFlags  updFlags;
    afxNat          glHandle;
    GLenum          glTarget;
    //GLenum          glUsage;
    GLbitfield      glAccess;
    //GLintptr        glMappedOff;
    //GLsizeiptr      glMappedSiz;
};

AFX_OBJECT(afxRaster)
{
    AFX_OBJECT(_avxRaster) m;
    sglUpdateFlags  updFlags;
    GLuint          glHandle;
    GLuint          glRboHandle; // for draw buffers
    GLint           glIntFmt;
    GLenum          glTarget, glFmt, glType;
};

AFX_OBJECT(avxCanvas)
{
    AFX_OBJECT(_avxCanvas) m;
    sglUpdateFlags  updFlags;
    afxNat          glHandle;
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

    afxNat              totalCmdCnt;
    afxNat              lastDrawCmdIdx;
    afxNat              drawCmdCnt;
    afxNat              lastPipBindCmdIdx;
    afxNat              pipBindCmdCnt;
    afxNat              lastVbufBindCmdIdx;
    afxNat              vbufBindCmdCnt;
    afxNat              lastIbufBindCmdIdx;
    afxNat              ibufBindCmdCnt;
    afxNat              lastBufBindCmdIdx;
    afxNat              bufBindCmdCnt;
    afxNat              lastTexBindCmdIdx;
    afxNat              texBindCmdCnt;
    afxNat              lastRasUpdCmdIdx;
    afxNat              rasUpdCmdCnt;
    afxNat              lastDepthUpdCmdIdx;
    afxNat              depthUpdCmdCnt;
    afxNat              lastVpCmdIdx; // viewport and/or scissor changes
    afxNat              vpUpdCmdCnt;
    afxNat              lastScisCmdIdx; // viewport and/or scissor changes
    afxNat              scisUpdCmdCnt;
    struct
    {
        struct
        {
            avxPipeline     next;
            avxPipeline     curr;
        } pip;
    } levelCaches[1];
};

ZGL void _SglDctxEnqueueDeletion(afxDrawContext dctx, afxNat exuIdx, afxNat type, afxSize gpuHandle);

ZGL afxError _DpuBindAndSyncSamp(sglDpu* dpu, afxNat glUnit, avxSampler samp);
ZGL afxError _DpuSyncShd(sglDpu* dpu, avxShader shd, avxShaderStage stage, glVmt const* gl);
ZGL afxError _DpuSurfSync(sglDpu* dpu, afxRasterSlot surf, glVmt const* gl); // must be used before texUpdate
ZGL afxError DpuBindAndSyncRas(sglDpu* dpu, afxNat glUnit, afxRaster tex);
ZGL afxError _SglTexSubImage(glVmt const* gl, GLenum glTarget, afxRasterRegion const* rgn, GLenum glFmt, GLenum glType, afxAddress const src);

ZGL afxError _DpuBindAndSyncVin(sglDpu* dpu, avxVertexInput vin, sglVertexInputState* nextVinBindings);
ZGL afxError _DpuBindAndSyncPip(sglDpu* dpu, afxBool bind, afxBool sync, avxPipeline pip);
ZGL afxError _DpuBindAndSyncRazr(sglDpu* dpu, avxRasterizer razr);
ZGL afxError _DpuBindAndResolveLiga(sglDpu* dpu, avxLigature legt, GLuint glHandle);
ZGL afxError _DpuBindAndSyncCanv(sglDpu* dpu, afxBool bind, afxBool sync, GLenum glTarget, avxCanvas canv);


ZGL afxError DpuBindAndSyncBuf(sglDpu* dpu, GLenum glTarget, afxBuffer buf);
ZGL afxError _DpuRemapBuf(sglDpu* dpu, afxBuffer buf, afxSize offset, afxNat range, afxFlags flags, void** placeholder);
ZGL afxError _DpuStoreBuf(sglDpu* dpu, afxBuffer buf, afxSize offset, afxNat range, afxByte* dst);
ZGL afxError _DpuLoadBuf(sglDpu* dpu, afxBuffer buf, afxSize offset, afxNat range, afxByte const* src);
ZGL afxError _DpuOutputBuf(sglDpu* dpu, afxBuffer buf, afxSize offset, afxNat range, afxStream out, afxSize at);
ZGL afxError _DpuInputBuf(sglDpu* dpu, afxBuffer buf, afxSize offset, afxNat range, afxStream in, afxSize at);

ZGL afxError _DpuLoadBuf2(sglDpu* dpu, afxBuffer buf, afxByte const* src, afxNat opCnt, afxBufferIo const* ops);
ZGL afxError _DpuStoreBuf2(sglDpu* dpu, afxBuffer buf, afxByte* dst, afxNat opCnt, afxBufferIo const* ops);
ZGL afxError _DpuOutputBuf2(sglDpu* dpu, afxBuffer buf, afxStream out, afxNat opCnt, afxBufferIo const* ops);
ZGL afxError _DpuInputBuf2(sglDpu* dpu, afxBuffer buf, afxStream in, afxNat opCnt, afxBufferIo const* ops);

ZGL afxError _DpuLoadRas(sglDpu* dpu, afxRaster ras, afxByte const* src, afxNat opCnt, afxRasterIo const* ops);
ZGL afxError _DpuStoreRas(sglDpu* dpu, afxRaster ras, afxByte* dst, afxNat opCnt, afxRasterIo const* ops);
ZGL afxError _DpuPackRas(sglDpu* dpu, afxRaster ras, afxBuffer buf, afxNat opCnt, afxRasterIo const* ops);
ZGL afxError _DpuUnpackRas(sglDpu* dpu, afxRaster ras, afxBuffer buf, afxNat opCnt, afxRasterIo const* ops);
ZGL afxError _DpuOutputRas(sglDpu* dpu, afxRaster ras, afxStream out, afxNat opCnt, afxRasterIo const* ops);
ZGL afxError _DpuInputRas(sglDpu* dpu, afxRaster ras, afxStream in, afxNat opCnt, afxRasterIo const* ops);

ZGL afxError _DpuCopyRas(sglDpu* dpu, afxRaster src, afxRaster dst, afxNat opCnt, afxRasterCopy const ops[]);

ZGL void _SglBindFboAttachment(glVmt const* gl, GLenum glTarget, GLenum glAttachment, GLenum texTarget, GLuint texHandle, GLint level, GLuint layer, GLuint z);

ZGL afxError _SglWaitFenc(afxBool waitAll, afxNat64 timeout, afxNat cnt, afxFence const fences[]);
ZGL afxError _SglResetFenc(afxNat cnt, afxFence const fences[]);

ZGL afxClassConfig const _SglFencMgrCfg;
ZGL afxClassConfig const _SglSemMgrCfg;

ZGL afxClassConfig _SglCmdbMgrCfg;

ZGL afxError _SglPipCtor(avxPipeline pip, void** args, afxNat invokeNo);
ZGL afxError _SglPipDtor(avxPipeline pip);
ZGL afxError _SglRazrDtor(avxRasterizer razr);
ZGL afxError _SglRazrCtor(avxRasterizer razr, void** args, afxNat invokeNo);
ZGL afxError _SglShdCtor(avxShader shd, void** args, afxNat invokeNo);
ZGL afxError _SglShdDtor(avxShader shd);
ZGL afxError _SglVinCtor(avxVertexInput vin, void** args, afxNat invokeNo);
ZGL afxError _SglVinDtor(avxVertexInput vin);
ZGL afxError _SglCanvCtor(avxCanvas canv, void** args, afxNat invokeNo);
ZGL afxError _SglCanvDtor(avxCanvas canv);
ZGL afxError _SglRasCtor(afxRaster ras, void** args, afxNat invokeNo);
ZGL afxError _SglRasDtor(afxRaster ras);
ZGL afxError _BufCtorCb(afxBuffer buf, void** args, afxNat invokeNo);
ZGL afxError _BufDtorCb(afxBuffer buf);
ZGL afxError _SglSampCtor(avxSampler samp, void** args, afxNat invokeNo);
ZGL afxError _SglSampDtor(avxSampler samp);
ZGL afxError _SglLigaCtor(avxLigature liga, void** args, afxNat invokeNo);
ZGL afxError _SglLigaDtor(avxLigature liga);
ZGL afxError _SglQrypCtor(avxQueryPool qryp, void** args, afxNat invokeNo);
ZGL afxError _SglQrypDtor(avxQueryPool qryp);

#endif//ZGL_OBJECTS_H
