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

AFX_OBJECT(avxVertexDecl)
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

ZGL void _ZglDsysEnqueueDeletion(afxDrawSystem dsys, afxUnit exuIdx, afxUnit type, afxSize gpuHandle);


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

ZGL afxError _ZglWaitFenc(afxBool waitAll, afxUnit64 timeout, afxUnit cnt, afxFence const fences[]);
ZGL afxError _ZglResetFenc(afxUnit cnt, afxFence const fences[]);

ZGL afxClassConfig const _ZglFencMgrCfg;
ZGL afxClassConfig const _ZglSemMgrCfg;

ZGL afxError _ZglPipCtor(avxPipeline pip, void** args, afxUnit invokeNo);
ZGL afxError _ZglPipDtor(avxPipeline pip);
ZGL afxError _ZglShdCtor(avxShader shd, void** args, afxUnit invokeNo);
ZGL afxError _ZglShdDtor(avxShader shd);
ZGL afxError _ZglVinCtor(avxVertexDecl vin, void** args, afxUnit invokeNo);
ZGL afxError _ZglVinDtor(avxVertexDecl vin);
ZGL afxError _ZglCanvCtor(avxCanvas canv, void** args, afxUnit invokeNo);
ZGL afxError _ZglCanvDtor(avxCanvas canv);
ZGL afxError _ZglRasCtor(afxRaster ras, void** args, afxUnit invokeNo);
ZGL afxError _ZglRasDtor(afxRaster ras);
ZGL afxError _BufCtorCb(afxBuffer buf, void** args, afxUnit invokeNo);
ZGL afxError _BufDtorCb(afxBuffer buf);
ZGL afxError _ZglSampCtor(avxSampler samp, void** args, afxUnit invokeNo);
ZGL afxError _ZglSampDtor(avxSampler samp);
ZGL afxError _ZglLigaCtor(avxLigature liga, void** args, afxUnit invokeNo);
ZGL afxError _ZglLigaDtor(avxLigature liga);
ZGL afxError _ZglQrypCtor(avxQueryPool qryp, void** args, afxUnit invokeNo);
ZGL afxError _ZglQrypDtor(avxQueryPool qryp);

ZGL afxError _ZglCmdbCtorCb(afxDrawContext dctx, void** args, afxUnit invokeNo);
ZGL afxError _ZglCmdbDtorCb(afxDrawContext dctx);



#endif//ZGL_OBJECTS_H
