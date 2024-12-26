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

#ifndef ZGL_COMMANDS_H
#define ZGL_COMMANDS_H

#include "zglDevices.h"

// vkCmdBindDescriptorSets - Binds descriptor sets to a command buffer

ZGL afxError _DpuBindAndSyncSamp(zglDpu* dpu, afxUnit glUnit, avxSampler samp);
ZGL afxError _DpuSyncShd(zglDpu* dpu, avxShader shd, avxShaderStage stage, glVmt const* gl);
ZGL afxError _DpuSurfSync(zglDpu* dpu, afxRasterSlot surf, glVmt const* gl); // must be used before texUpdate
ZGL afxError DpuBindAndSyncRas(zglDpu* dpu, afxUnit glUnit, afxRaster tex);
ZGL afxError _ZglTexSubImage(glVmt const* gl, GLenum glTarget, afxRasterRegion const* rgn, GLenum glFmt, GLenum glType, afxAddress const src);
ZGL afxError _ZglCompressedTexSubImage(glVmt const* gl, GLenum glTarget, afxRasterRegion const* rgn, GLenum glFmt, GLenum glType, afxUnit compressedSiz, afxAddress const src);


ZGL afxError _DpuBindVertexInput(zglDpu* dpu, avxVertexDecl vin);
ZGL afxError _DpuBindPipeline(zglDpu* dpu, avxPipeline pip, avxVertexDecl vin, afxFlags dynamics);
ZGL afxError _DpuBindAndResolveLiga(zglDpu* dpu, avxLigature legt, GLuint glHandle);
ZGL afxError _DpuBindAndSyncCanv(zglDpu* dpu, GLenum glTarget, avxCanvas canv);


ZGL afxError DpuBindAndSyncBuf(zglDpu* dpu, GLenum glTarget, afxBuffer buf);
ZGL afxError _DpuRemapBuf(zglDpu* dpu, afxBuffer buf, afxSize offset, afxUnit range, afxFlags flags, void** placeholder);
ZGL afxError _DpuStoreBuf(zglDpu* dpu, afxBuffer buf, afxSize offset, afxUnit range, afxByte* dst);
ZGL afxError _DpuLoadBuf(zglDpu* dpu, afxBuffer buf, afxSize offset, afxUnit range, afxByte const* src);
ZGL afxError _DpuOutputBuf(zglDpu* dpu, afxBuffer buf, afxSize offset, afxUnit range, afxStream out, afxSize at);
ZGL afxError _DpuInputBuf(zglDpu* dpu, afxBuffer buf, afxSize offset, afxUnit range, afxStream in, afxSize at);

ZGL void DpuBufRw(zglDpu* dpu, afxBuffer buf, afxUnit offset, afxUnit range, afxBool toHost, void* srcOrDst);
ZGL void DpuBufCpy(zglDpu* dpu, afxBuffer src, afxBuffer dst, afxUnit opCnt, afxBufferCopy const ops[]);
ZGL void DpuBufSet(zglDpu* dpu, afxBuffer buf, afxUnit offset, afxUnit range, afxUnit data);

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

ZGL afxError _DpuPresentDout(zglDpu* dpu, afxDrawOutput dout, afxUnit outBufIdx);

ZGL afxError _ZglBindFboAttachment(glVmt const* gl, GLenum glTarget, GLenum glAttachment, GLenum texTarget, GLuint texHandle, GLint level, GLuint layer, GLuint z);

ZGL afxError _ZglWaitFenc(afxBool waitAll, afxUnit64 timeout, afxUnit cnt, afxFence const fences[]);
ZGL afxError _ZglResetFenc(afxUnit cnt, afxFence const fences[]);

ZGL void _ZglCopyTexSubImage(zglDpu* dpu, GLenum glDstTarget, GLenum glSrcTarget, GLuint glSrcHandle, afxUnit opCnt, afxRasterCopy const ops[]);
ZGL void _ZglUnpackTexSubImage(zglDpu* dpu, GLenum glTarget, afxUnit bpp, GLenum glFmt, GLenum glType, afxUnit opCnt, afxRasterIo const ops[]);

AFX_DEFINE_STRUCT(zglBufferInfo)
{
    afxBuffer buf;
    afxUnit offset;
    afxUnit range;
    afxUnit stride;
};

ZGL void _DpuBindIndexSource(zglDpu* dpu, afxBuffer buf, afxUnit32 offset, afxUnit32 range, afxUnit32 stride);
ZGL void _DpuBindVertexSources(zglDpu* dpu, afxUnit first, afxUnit cnt, zglBufferInfo const src[]);
ZGL void _DpuSetViewports(zglDpu* dpu, afxUnit first, afxUnit cnt, afxViewport const vp[]);

ZGL void _ZglFlushTsChanges(zglDpu* dpu);
ZGL void _ZglFlushRsChanges(zglDpu* dpu);

ZGL void _DpuCommenceDrawScope(zglDpu* dpu, avxCanvas canv, afxRect const* area, afxUnit baseLayer, afxUnit layerCnt, afxUnit cCnt, avxDrawTarget const* c, avxDrawTarget const* d, avxDrawTarget const* s, afxBool defFbo);
ZGL void _DpuConcludeDrawScope(zglDpu* dpu);

ZGL void _DpuDraw(zglDpu* dpu, avxDrawIndirectCmd const* data);
ZGL void _DpuDrawIndirect(zglDpu* dpu, afxBuffer buf, afxUnit32 offset, afxUnit32 drawCnt, afxUnit32 stride);
ZGL void _DpuDrawIndirectCount(zglDpu* dpu, afxBuffer buf, afxUnit32 offset, afxBuffer cntBuf, afxUnit32 cntBufOff, afxUnit32 maxDrawCnt, afxUnit32 stride);
ZGL void _DpuDrawIndexed(zglDpu* dpu, avxDrawIndexedIndirectCmd const* data);
ZGL void _DpuDrawIndexedIndirect(zglDpu* dpu, afxBuffer buf, afxUnit32 offset, afxUnit32 drawCnt, afxUnit32 stride);
ZGL void _DpuDrawIndexedIndirectCount(zglDpu* dpu, afxBuffer buf, afxUnit32 offset, afxBuffer cntBuf, afxUnit32 cntBufOff, afxUnit32 maxDrawCnt, afxUnit32 stride);
ZGL void _DpuDispatch(zglDpu* dpu, afxUnit grpCntX, afxUnit grpCntY, afxUnit grpCntZ);
ZGL void _DpuDispatchIndirect(zglDpu* dpu, afxBuffer buf, afxUnit32 offset);

ZGL void _DpuBindRasters(zglDpu* dpu, afxUnit set, afxUnit first, afxUnit cnt, afxRaster rasters[], afxUnit const subras[]);
ZGL void _DpuBindSamplers(zglDpu* dpu, afxUnit set, afxUnit first, afxUnit cnt, avxSampler samplers[]);
ZGL void _DpuBindBuffers(zglDpu* dpu, afxUnit set, afxUnit first, afxUnit cnt, afxBuffer buffers[], afxUnit const offsets[], afxUnit const ranges[]);

ZGL void _DpuPushConstants(zglDpu* dpu, afxUnit32 offset, afxUnit32 siz, void const* data);

ZGL avxCmdList const cmdDevmt;

ZGL void _DpuEndOfCommands(zglDpu* dpu);

#endif//ZGL_COMMANDS_H
