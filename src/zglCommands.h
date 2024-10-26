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

AFX_DEFINE_STRUCT(_zglQueueing)
{
    afxUnit      siz;
    afxLinkage  chain;
    afxUnit      submNo; // this submission number ordinal (B2F)
    afxUnit      reqSubmNo; // required submission num ordinal (need be executed before this). Usually submissions of resource benefiting of fetch priority.
    afxTime     pushTime; // submission (into input) time
    afxTime     pullTime; // fecth (by queue) time
    afxTime     complTime; // completation time
    afxError    (*exec)(zglDpu*, afxDrawBridge, afxUnit queIdx, _zglQueueing*);
};

AFX_DEFINE_STRUCT(_zglQueueingExecute)
{
    _zglQueueing            hdr;
    afxSemaphore            wait;
    afxSemaphore            signal;
    afxFence                fence;
    afxUnit                  cmdbCnt;
    avxCmdb                 cmdbs[];
};

AFX_DEFINE_STRUCT(_zglQueueingWork)
{
    _zglQueueing            hdr;
    avxQueueOp              subm;
    void*                   data[];
};

AFX_DEFINE_STRUCT(_zglQueueingTransfer)
{
    _zglQueueing            hdr;

    afxSemaphore        wait;
    afxSemaphore        signal;
    afxFence            fence;

    afxFcc              srcFcc;
    afxFcc              dstFcc;
    afxCodec            codec;
    afxUnit              decSiz;

    union
    {
        struct
        {
            afxRaster       ras;
            //afxRasterIo     op;
            union
            {
                void*       dst;
                void const* src;
                afxBuffer   buf;
                afxStream   iob;
            };
        }                   img;
        struct
        {
            afxBuffer       buf;
            //afxBufferIo     op;
            union
            {
                void*       dst;
                void const* src;
                afxStream   iob;
            };
        }                   data;
    };

    afxUnit                  itemCnt;
    union
    {
        afxRasterIo         rasIo[];
        afxBufferIo         bufIo[];
    }                       items;
};

AFX_DEFINE_STRUCT(_zglQueueingPresent)
{
    _zglQueueing            hdr;
    afxSemaphore            wait;
    afxUnit                  itemCnt;
    struct
    {
        afxDrawOutput       dout;
        afxUnit              bufIdx;
    }                       items[];
};

AFX_DEFINE_STRUCT(_zglQueueingMmap)
{
    _zglQueueing            hdr;
    afxBuffer               buf;
    afxSize                 off;
    afxUnit                  ran;
    afxFlags                flags;
    void**                  placeholder;
};

AFX_DEFINE_STRUCT(_zglCmd)
{
    afxLinkage          script;
    afxUnit              id;
    afxUnit              siz;
};

#if 0
typedef enum afxDrawCmdId
{
    ZGL_CMD_END = offsetof(afxCmd, EndOfScript) / sizeof(void*),
    ZGL_CMD_EXECUTE_COMMANDS = offsetof(afxCmd, ExecuteCommands) / sizeof(void*),
    ZGL_CMD_BEGIN_SYNTHESIS = offsetof(afxCmd, BeginSynthesis) / sizeof(void*),
    ZGL_CMD_NEXT_PASS = offsetof(afxCmd, NextPass) / sizeof(void*),
    ZGL_CMD_FINISH_SYNTHESIS = offsetof(afxCmd, FinishSynthesis) / sizeof(void*),
    
    ZGL_CMD_BIND_BUFFERS = offsetof(afxCmd, BindBuffers) / sizeof(void*),
    ZGL_CMD_BIND_RASTERS = offsetof(afxCmd, BindRasters) / sizeof(void*),
    
    ZGL_CMD_BIND_PIPELINE = offsetof(afxCmd, BindPipeline) / sizeof(void*),

    ZGL_CMD_READJUST_VIEWPORTS = offsetof(afxCmd, ReadjustViewports) / sizeof(void*),
    ZGL_CMD_RESET_VIEWPORTS = offsetof(afxCmd, ResetViewports) / sizeof(void*),    
    ZGL_CMD_READJUST_SCISSORS = offsetof(afxCmd, ReadjustScissors) / sizeof(void*),
    ZGL_CMD_RESET_SCISSORS = offsetof(afxCmd, ResetScissors) / sizeof(void*),
    ZGL_CMD_READJUST_AREAS = offsetof(afxCmd, ReadjustAreas) / sizeof(void*),
    ZGL_CMD_RESET_AREAS = offsetof(afxCmd, ResetAreas) / sizeof(void*),

    ZGL_CMD_BIND_VERTEX_SOURCES = offsetof(afxCmd, BindVertexSources) / sizeof(void*),
    //ZGL_CMD_RESET_VERTEX_STREAMS = offsetof(afxCmd, ResetVertexStreams) / sizeof(void*),
    //ZGL_CMD_RESET_VERTEX_ATTRIBUTES = offsetof(afxCmd, ResetVertexAttributes) / sizeof(void*),
    ZGL_CMD_BIND_INDEX_SOURCE = offsetof(afxCmd, BindIndexSource) / sizeof(void*),
    ZGL_CMD_SET_PRIMITIVE_TOPOLOGY = offsetof(afxCmd, SetPrimitiveTopology) / sizeof(void*),
    
    ZGL_CMD_DRAW = offsetof(afxCmd, Draw) / sizeof(void*),
    ZGL_CMD_DRAW_INDEXED = offsetof(afxCmd, DrawIndexed) / sizeof(void*),

    ZGL_CMD_DISABLE_RASTERIZATION = offsetof(afxCmd, DisableRasterization) / sizeof(void*),
    ZGL_CMD_SWITCH_FRONT_FACE = offsetof(afxCmd, SwitchFrontFace) / sizeof(void*),
    ZGL_CMD_SET_CULL_MODE = offsetof(afxCmd, SetCullMode) / sizeof(void*),
    ZGL_CMD_ENABLE_DEPTH_BIAS = offsetof(afxCmd, EnableDepthBias) / sizeof(void*),
    ZGL_CMD_SET_DEPTH_BIAS = offsetof(afxCmd, SetDepthBias) / sizeof(void*),
    ZGL_CMD_SET_LINE_WIDTH = offsetof(afxCmd, SetLineWidth) / sizeof(void*),

    ZGL_CMD_ENABLE_DEPTH_BOUNDS_TEST = offsetof(afxCmd, EnableDepthBoundsTest) / sizeof(void*),
    ZGL_CMD_SET_DEPTH_BOUNDS = offsetof(afxCmd, SetDepthBounds) / sizeof(void*),
    ZGL_CMD_ENABLE_STENCIL_TEST = offsetof(afxCmd, EnableStencilTest) / sizeof(void*),
    ZGL_CMD_SET_STENCIL_COMPARE_MASK = offsetof(afxCmd, SetStencilCompareMask) / sizeof(void*),
    ZGL_CMD_SET_STENCIL_WRITE_MASK = offsetof(afxCmd, SetStencilWriteMask) / sizeof(void*),
    ZGL_CMD_SET_STENCIL_REFERENCE = offsetof(afxCmd, SetStencilReference) / sizeof(void*),
    ZGL_CMD_ENABLE_DEPTH_TEST = offsetof(afxCmd, EnableDepthTest) / sizeof(void*),
    ZGL_CMD_SET_DEPTH_COMPARE_OP = offsetof(afxCmd, SetDepthCompareOp) / sizeof(void*),
    ZGL_CMD_ENABLE_DEPTH_WRITE = offsetof(afxCmd, EnableDepthWrite) / sizeof(void*),
    
    ZGL_CMD_SET_BLEND_CONSTANTS = offsetof(afxCmd, SetBlendConstants) / sizeof(void*),

    ZGL_CMD_PACK_RASTER = offsetof(afxCmd, PackRaster) / sizeof(void*),
    ZGL_CMD_UNPACK_RASTER = offsetof(afxCmd, UnpackRaster) / sizeof(void*),
    ZGL_CMD_COPY_RASTER = offsetof(afxCmd, CopyRaster) / sizeof(void*),
    ZGL_CMD_REGENERATE_MIPMAPS = offsetof(afxCmd, RegenerateMipmaps) / sizeof(void*),
    ZGL_CMD_FLIP_RASTER = offsetof(afxCmd, FlipRaster) / sizeof(void*),
    ZGL_CMD_SWIZZLE_RASTER = offsetof(afxCmd, SwizzleRaster) / sizeof(void*),

    ZGL_CMD_TOTAL = offsetof(afxCmd, Total) / sizeof(void*),
} afxDrawCmdId;
#else
#define ZGL_CMD_TOTAL offsetof(afxCmd, Total) / sizeof(void*)
#endif

AFX_DEFINE_UNION(zglDecodeVmt)
{
    afxCmd  cmd;
    void(*f[ZGL_CMD_TOTAL])(zglDpu* dpu, _zglCmd *cmd);
};

AFX_DEFINE_STRUCT(_zglCmdEcho)
{
    _zglCmd                     cmd;
    afxLinkage                  echoLnk;
    afxM4d                      m;
    afxString                 caption;
    afxChar            AFX_SIMD strBuf[];
};

AFX_DEFINE_STRUCT(_zglCmdEchoFmt)
{
    _zglCmd                     cmd;
    afxM4d                      m;
    afxString                 caption;
    afxChar            AFX_SIMD strBuf[];
};

AFX_DEFINE_STRUCT(_zglCmdDraw)
{
    _zglCmd                     cmd;
    avxDrawIndirectCmd          data;
};

AFX_DEFINE_STRUCT(_zglCmdDrawIndirect)
{
    _zglCmd                     cmd;
    afxBuffer buf;
    afxUnit32 offset;
    afxUnit32 drawCnt;
    afxUnit32 stride;
};

AFX_DEFINE_STRUCT(_zglCmdDrawIndirectCnt)
{
    _zglCmd                     cmd;
    afxBuffer buf;
    afxUnit32 offset;
    afxBuffer cntBuf;
    afxUnit32 cntBufOff;
    afxUnit32 maxDrawCnt;
    afxUnit32 stride;
};

AFX_DEFINE_STRUCT(_zglCmdDrawIndexed)
{
    _zglCmd                     cmd;
    avxDrawIndexedIndirectCmd   data;
};

AFX_DEFINE_STRUCT(_zglCmdDrawIndexedIndirect)
{
    _zglCmd                     cmd;
    afxBuffer buf;
    afxUnit32 offset;
    afxUnit32 drawCnt;
    afxUnit32 stride;
};

AFX_DEFINE_STRUCT(_zglCmdDrawIndexedIndirectCnt)
{
    _zglCmd                     cmd;
    afxBuffer buf;
    afxUnit32 offset;
    afxBuffer cntBuf;
    afxUnit32 cntBufOff;
    afxUnit32 maxDrawCnt;
    afxUnit32 stride;
};

AFX_DEFINE_STRUCT(_zglCmdDispatch)
{
    _zglCmd                     cmd;
    avxDispatchIndirectCmd      data;
};

AFX_DEFINE_STRUCT(_zglCmdDispatchIndirect)
{
    _zglCmd                     cmd;
    afxBuffer buf;
    afxUnit32 offset;
};

AFX_DEFINE_STRUCT(_zglCmdBeginSynthesis)
{
    _zglCmd cmd;
    afxBool defFbo;
    afxLinkage canvOpsLnk;
    avxCanvas canv;
    afxRect area;
    afxUnit32 baseLayer;
    afxUnit32 layerCnt;
    avxDrawTarget depth;
    avxDrawTarget stencil;
    afxUnit32 rasterCnt;
    avxDrawTarget AFX_SIMD rasters[];
};

AFX_DEFINE_STRUCT(_zglCmdBindRasters)
{
    _zglCmd cmd;
    afxUnit32 set;
    afxUnit32 first, cnt;
    afxRaster AFX_SIMD tex[_ZGL_MAX_ENTRY_PER_LEGO];
    afxUnit subras[_ZGL_MAX_ENTRY_PER_LEGO];
};

AFX_DEFINE_STRUCT(_zglCmdBindSamplers)
{
    _zglCmd cmd;
    afxUnit32 set;
    afxUnit32 first, cnt;
    avxSampler AFX_SIMD smp[_ZGL_MAX_ENTRY_PER_LEGO];
};

AFX_DEFINE_STRUCT(_zglCmdBindBuffers)
{
    _zglCmd cmd;
    afxUnit32 set;
    afxUnit32 first, cnt;
    afxBuffer AFX_SIMD buf[_ZGL_MAX_ENTRY_PER_LEGO];
    afxUnit32 AFX_SIMD offset[_ZGL_MAX_ENTRY_PER_LEGO];
    afxUnit32 AFX_SIMD range[_ZGL_MAX_ENTRY_PER_LEGO];
};

AFX_DEFINE_STRUCT(_zglCmdPushConstants)
{
    _zglCmd cmd;
    afxUnit32 offset;
    afxUnit32 siz;
    afxByte data[];
};

AFX_DEFINE_STRUCT(zglBufferInfo)
{
    afxBuffer buf;
    afxUnit32 offset;
    afxUnit32 range;
    afxUnit32 stride;
};

AFX_DEFINE_STRUCT(_zglCmdVertexSources)
{
    _zglCmd cmd;
    afxUnit32 first, cnt;
    zglBufferInfo AFX_SIMD info[];
};

AFX_DEFINE_STRUCT(_zglCmdVertexStreams)
{
    _zglCmd                     cmd;
    afxUnit cnt;
    struct
    {
        afxUnit  srcIdx;
        afxUnit  stride;
        afxBool instance;
    }   AFX_SIMD streams[];
};

AFX_DEFINE_STRUCT(_zglCmdVertexAttributes)
{
    _zglCmd                     cmd;    
    afxUnit cnt;
    struct
    {
        afxUnit location;
        afxVertexFormat fmt;
        afxUnit srcIdx;
        afxUnit32 offset;
    }   AFX_SIMD attrs[];
};

AFX_DEFINE_STRUCT(_zglCmdDebugScope)
{
    _zglCmd             cmd;
    afxBool             open;
    afxColor            color;
    afxString2048       name;
};

AFX_DEFINE_STRUCT(_zglCmdBufferRange)
{
    _zglCmd                     cmd;
    afxBuffer                       buf;
    afxUnit32                        offset;
    afxUnit32                        range;
    afxUnit32                        stride;
};

AFX_DEFINE_STRUCT(_zglCmdRasterizer)
{
    _zglCmd                     cmd;
    avxRasterizer               razr;
    avxVertexInput              vin;
    afxFlags                    dynamics;
};

AFX_DEFINE_STRUCT(_zglCmdPipeline)
{
    _zglCmd                     cmd;
    afxUnit                      segment;
    avxPipeline                 pip;
    avxVertexInput              vin;
    afxFlags                    dynamics;
};

AFX_DEFINE_STRUCT(_zglCmdVertexInput)
{
    _zglCmd                     cmd;
    avxVertexInput              vin;
};

AFX_DEFINE_STRUCT(_zglCmdBool)
{
    _zglCmd                     cmd;
    afxBool                         value;
};

AFX_DEFINE_STRUCT(_zglCmdReal)
{
    _zglCmd                     cmd;
    afxReal                         value;
};

AFX_DEFINE_STRUCT(_zglCmdReal2)
{
    _zglCmd                     cmd;
    afxReal                         value[2];
};

AFX_DEFINE_STRUCT(_zglCmdReal3)
{
    _zglCmd                     cmd;
    afxReal                         value[3];
};

AFX_DEFINE_STRUCT(_zglCmdReal4)
{
    _zglCmd                     cmd;
    afxReal                         value[4];
};

AFX_DEFINE_STRUCT(_zglCmdPackRasterRegions)
{
    _zglCmd         cmd;
    afxBool         unpack;
    afxRaster ras;
    afxBuffer buf;
    afxUnit opCnt;
    afxRasterIo AFX_SIMD ops[];
};

AFX_DEFINE_STRUCT(_zglCmdRwRasterRegions)
{
    _zglCmd         cmd;
    afxBool         down;
    afxRaster ras;
    afxStream ios;
    afxCodec cdc;
    afxUnit opCnt;
    afxRasterIo AFX_SIMD ops[];
};

AFX_DEFINE_STRUCT(_zglCmdCopyRasterRegions)
{
    _zglCmd         cmd;
    afxRaster           src;
    afxRaster           dst;
    afxUnit              opCnt;
    afxRasterCopy AFX_SIMD ops[];
};

AFX_DEFINE_STRUCT(_zglCmdRegenerateMipmaps)
{
    _zglCmd             cmd;
    afxRaster           ras;
    afxUnit              baseLod;
    afxUnit              lodCnt;
};

AFX_DEFINE_STRUCT(_zglCmdSwizzleRasterRegions)
{
    _zglCmd             cmd;
    afxRaster           ras;
    avxColorSwizzle     a;
    avxColorSwizzle     b;
    afxUnit              rgnCnt;
    afxRasterRegion AFX_SIMD rgn[];
};

AFX_DEFINE_STRUCT(_zglCmdFlipRasterRegions)
{
    _zglCmd             cmd;
    afxRaster           ras;
    afxM4d              m;
    afxUnit              rgnCnt;
    afxRasterRegion AFX_SIMD rgn[];
};

AFX_DEFINE_STRUCT(_zglCmdBufIo)
{
    _zglCmd         cmd;
    afxBuffer       buf;
    afxBool         export;
    afxStream       io;
    afxUnit          opCnt;
    afxBufferIo AFX_SIMD ops[];
};

AFX_DEFINE_STRUCT(_zglCmdBufRw)
{
    _zglCmd         cmd;
    afxBuffer       buf;
    afxUnit          offset;
    afxUnit          range;
    afxBool         toHost;
    union
    {
        void*       dst;
        afxByte AFX_SIMD src[];
    };
};

AFX_DEFINE_STRUCT(_zglCmdBufCpy)
{
    _zglCmd         cmd;
    afxBuffer src;
    afxBuffer dst;
    afxUnit opCnt;
    afxBufferCopy AFX_SIMD ops[];
};

AFX_DEFINE_STRUCT(_zglCmdBufSet)
{
    _zglCmd         cmd;
    afxBuffer buf;
    afxUnit  offset;
    afxUnit  range;
    afxUnit data;
};

AFX_DEFINE_STRUCT(_zglCmdNat)
{
    _zglCmd                     cmd;
    afxUnit                          value;
};

AFX_DEFINE_STRUCT(_zglCmdBitmaskNat32)
{
    _zglCmd                     cmd;
    afxMask                         mask;
    afxUnit32                        value;
};

AFX_DEFINE_STRUCT(_zglCmdScissor)
{
    _zglCmd cmd;
    afxUnit32 first, cnt;
    afxRect AFX_SIMD rect[];
};

AFX_DEFINE_STRUCT(_zglCmdViewport)
{
    _zglCmd cmd;
    afxUnit32 first, cnt;
    afxViewport AFX_SIMD vp[];
};

AFX_DEFINE_STRUCT(_zglCmdNextPass)
{
    _zglCmd                     cmd;
    afxBool32                       useAuxScripts;
};

AFX_DEFINE_STRUCT(_zglCmdExecCmds)
{
    _zglCmd                     cmd;
    afxUnit                          cnt;
    avxCmdb AFX_SIMD subsets[8];
};

AFX_DEFINE_STRUCT(_zglCmdCopyTex)
{
    _zglCmd                     cmd;
    afxRaster                      dst;
    afxRaster                      src;
    afxUnit32                        rgnCnt;
    afxRasterRegion AFX_SIMD rgn[16];
};

AFX_DEFINE_STRUCT(_zglCmdUniformVectorEXT)
{
    _zglCmd                     cmd;
    afxChar const*                  name;
    afxUnit                          cnt;
    afxReal AFX_SIMD v[4];
};

AFX_DEFINE_STRUCT(_zglCmdUniformMatrixEXT)
{
    _zglCmd                     cmd;
    afxChar const*                  name;
    afxUnit                          cnt;
    afxReal AFX_SIMD m[4][4];
};


// vkCmdBindDescriptorSets - Binds descriptor sets to a command buffer

ZGL afxError _DpuBindAndSyncSamp(zglDpu* dpu, afxUnit glUnit, avxSampler samp);
ZGL afxError _DpuSyncShd(zglDpu* dpu, avxShader shd, avxShaderStage stage, glVmt const* gl);
ZGL afxError _DpuSurfSync(zglDpu* dpu, afxRasterSlot surf, glVmt const* gl); // must be used before texUpdate
ZGL afxError DpuBindAndSyncRas(zglDpu* dpu, afxUnit glUnit, afxRaster tex);
ZGL afxError _SglTexSubImage(glVmt const* gl, GLenum glTarget, afxRasterRegion const* rgn, GLenum glFmt, GLenum glType, afxAddress const src);
ZGL afxError _SglCompressedTexSubImage(glVmt const* gl, GLenum glTarget, afxRasterRegion const* rgn, GLenum glFmt, GLenum glType, afxUnit compressedSiz, afxAddress const src);


ZGL afxError _DpuBindVertexInput(zglDpu* dpu, avxVertexInput vin);
ZGL afxError _DpuBindPipeline(zglDpu* dpu, avxPipeline pip, avxVertexInput vin, afxFlags dynamics);
ZGL afxError _DpuBindAndResolveLiga(zglDpu* dpu, avxLigature legt, GLuint glHandle);
ZGL afxError _DpuBindAndSyncCanv(zglDpu* dpu, GLenum glTarget, avxCanvas canv);


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

ZGL afxError _DpuPresentDout(zglDpu* dpu, afxDrawOutput dout, afxUnit outBufIdx);

//ZGL afxSize _AfxMeasureTextureRegion(afxRaster tex, afxRasterRegion const *rgn);

ZGL afxError _SglDdevOpenCb(afxDrawDevice ddev, afxDrawContext dctx, void** args, afxUnit invokeNo);
ZGL afxError _SglDdevCloseCb(afxDrawDevice ddev, afxDrawContext dctx);

//ZGL afxBool _SglDqueVmtSubmitCb(afxDrawContext dctx, afxDrawBridge dexu, afxDrawSubmissionSpecification const *spec, afxUnit *submNo);
ZGL afxBool _DexuProcCb(afxDrawBridge dexu, afxThread thr);


ZGL afxCmd _SglEncodeCmdVmt;
ZGL afxError _SglBindFboAttachment(glVmt const* gl, GLenum glTarget, GLenum glAttachment, GLenum texTarget, GLuint texHandle, GLint level, GLuint layer, GLuint z);

ZGL afxError _SglWaitFenc(afxBool waitAll, afxUnit64 timeout, afxUnit cnt, afxFence const fences[]);
ZGL afxError _SglResetFenc(afxUnit cnt, afxFence const fences[]);

ZGL afxCmdId _SglEncodeCmdCommand(avxCmdb cmdb, afxUnit id, afxUnit siz, _zglCmd *cmd);

ZGL afxError _SglDqueEnqueueMmapCb(afxDrawQueue dque, afxBuffer buf, afxSize off, afxUnit ran, afxFlags flags, void** placeholder);
ZGL afxError _SglDqueRequestSubmCb(afxDrawQueue dque, afxFence fenc, afxUnit cnt, avxQueueOp const req[]);

ZGL afxClassConfig const _SglDctxMgrCfg;
ZGL afxClassConfig const _SglFencMgrCfg;
ZGL afxClassConfig const _SglSemMgrCfg;

ZGL void _SglCopyTexSubImage(zglDpu* dpu, GLenum glDstTarget, GLenum glSrcTarget, GLuint glSrcHandle, afxUnit opCnt, afxRasterCopy const ops[]);
ZGL void _SglUnpackTexSubImage(zglDpu* dpu, GLenum glTarget, afxUnit bpp, GLenum glFmt, GLenum glType, afxUnit opCnt, afxRasterIo const ops[]);

ZGL void DpuBufRw(zglDpu* dpu, afxBuffer buf, afxUnit offset, afxUnit range, afxBool toHost, void* srcOrDst);
ZGL void DpuBufCpy(zglDpu* dpu, afxBuffer src, afxBuffer dst, afxUnit opCnt, afxBufferCopy const ops[]);
ZGL void DpuBufSet(zglDpu* dpu, afxBuffer buf, afxUnit offset, afxUnit range, afxUnit data);

ZGL void _DecodeCmdSwitchFrontFace(zglDpu* dpu, _zglCmdBool const *cmd);
ZGL void _DecodeCmdSetCullMode(zglDpu* dpu, _zglCmdNat const *cmd);
ZGL void _DecodeCmdSetPrimitiveTopology(zglDpu* dpu, _zglCmdNat const *cmd);
ZGL void _DecodeCmdSetViewports(zglDpu* dpu, _zglCmdViewport const *cmd);
ZGL void _DecodeCmdBindVertexSources(zglDpu* dpu, _zglCmdVertexSources const *cmd);
ZGL void _DecodeCmdBindIndexSource(zglDpu* dpu, _zglCmdBufferRange const *cmd);
ZGL void _DecodeCmdBindVertexInput(zglDpu* dpu, _zglCmdVertexInput *cmd);

ZGL void _DpuBindIndexSource(zglDpu* dpu, afxBuffer buf, afxUnit32 offset, afxUnit32 range, afxUnit32 stride);
ZGL void _DpuBindVertexSources(zglDpu* dpu, afxUnit first, afxUnit cnt, zglBufferInfo const src[]);
ZGL void _DpuSetViewports(zglDpu* dpu, afxUnit first, afxUnit cnt, afxViewport const vp[]);

ZGL void _DecodeCmdDisableRasterization(zglDpu* dpu, _zglCmdBool const *cmd);
ZGL void _DecodeCmdEnableDepthBias(zglDpu* dpu, _zglCmdBool const *cmd);
ZGL void _DecodeCmdSetDepthBias(zglDpu* dpu, _zglCmdReal3 const *cmd);
ZGL void _DecodeCmdSetLineWidth(zglDpu* dpu, _zglCmdReal const *cmd);
ZGL void _DecodeCmdEnableStencilTest(zglDpu* dpu, _zglCmdBool const *cmd);
ZGL void _DecodeCmdSetStencilCompareMask(zglDpu* dpu, _zglCmdBitmaskNat32 const *cmd);
ZGL void _DecodeCmdSetStencilWriteMask(zglDpu* dpu, _zglCmdBitmaskNat32 const *cmd);
ZGL void _DecodeCmdSetStencilReference(zglDpu* dpu, _zglCmdBitmaskNat32 const *cmd);
ZGL void _DecodeCmdEnableDepthTest(zglDpu* dpu, _zglCmdBool const *cmd);
ZGL void _DecodeCmdSetDepthCompareOp(zglDpu* dpu, _zglCmdNat const *cmd);
ZGL void _DecodeCmdDisableDepthWrite(zglDpu* dpu, _zglCmdBool const *cmd);
ZGL void _DecodeCmdSetBlendConstants(zglDpu* dpu, _zglCmdReal4 const *cmd);
ZGL void _DecodeCmdSetScissors(zglDpu* dpu, _zglCmdScissor const *cmd);
ZGL void _DecodeCmdSetCurtains(zglDpu* dpu, _zglCmdScissor const *cmd);

ZGL void _DecodeCmdRasSubsample(zglDpu* dpu, _zglCmdRegenerateMipmaps const* cmd);
ZGL void _DecodeCmdRasPack(zglDpu* dpu, _zglCmdPackRasterRegions const* cmd);
ZGL void _DecodeCmdRasCopy(zglDpu* dpu, _zglCmdCopyRasterRegions const* cmd);

ZGL void _DecodeCmdBufCpy(zglDpu* dpu, _zglCmdBufCpy const* cmd);
ZGL void _DecodeCmdBufSet(zglDpu* dpu, _zglCmdBufSet const* cmd);
ZGL void _DecodeCmdBufRw(zglDpu* dpu, _zglCmdBufRw const* cmd);

ZGL void _ZglFlushTsChanges(zglDpu* dpu);
ZGL void _ZglFlushRsChanges(zglDpu* dpu);

ZGL void _DecodeCmdNextPass(zglDpu* dpu, _zglCmd const *cmd);
ZGL void _DecodeCmdConcludeDrawScope(zglDpu* dpu, _zglCmd const *cmd);
ZGL void _DecodeCmdCommenceDrawScope(zglDpu* dpu, _zglCmdBeginSynthesis const *cmd);

ZGL void _DecodeCmdBindSamplers(zglDpu* dpu, _zglCmdBindSamplers const *cmd);
ZGL void _DecodeCmdBindRasters(zglDpu* dpu, _zglCmdBindRasters const *cmd);
ZGL void _DecodeCmdBindBuffers(zglDpu* dpu, _zglCmdBindBuffers const *cmd);

ZGL void _DecodeCmdBindPipeline(zglDpu* dpu, _zglCmdPipeline *cmd);
ZGL void _DecodeCmdBindRasterizer(zglDpu* dpu, _zglCmdRasterizer *cmd);


ZGL void _DecodeCmdDraw(zglDpu* dpu, _zglCmdDraw const *cmd);
ZGL void _DecodeCmdDrawIndexed(zglDpu* dpu, _zglCmdDrawIndexed const* cmd);

ZGL void _DpuCommenceDrawScope(zglDpu* dpu, avxCanvas canv, afxRect const* area, afxUnit baseLayer, afxUnit layerCnt, afxUnit cCnt, avxDrawTarget const* c, avxDrawTarget const* d, avxDrawTarget const* s, afxBool defFbo);
ZGL void _DpuConcludeDrawScope(zglDpu* dpu);

ZGL void _DpuExecuteCommands(zglDpu* dpu, afxUnit cnt, avxCmdb streams[]);
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

ZGL zglDecodeVmt _SglDecodeCmdVmt;

#endif//ZGL_COMMANDS_H
