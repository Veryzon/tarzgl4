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

AFX_DEFINE_STRUCT(_sglQueueing)
{
    afxNat      siz;
    afxLinkage  chain;
    afxNat      submNo; // this submission number ordinal (B2F)
    afxNat      reqSubmNo; // required submission num ordinal (need be executed before this). Usually submissions of resource benefiting of fetch priority.
    afxTime     pushTime; // submission (into input) time
    afxTime     pullTime; // fecth (by queue) time
    afxTime     complTime; // completation time
    afxError    (*exec)(sglDpu*, afxDrawBridge, afxNat queIdx, _sglQueueing*);
};

AFX_DEFINE_STRUCT(_sglQueueingExecute)
{
    _sglQueueing            hdr;
    afxSemaphore            wait;
    afxSemaphore            signal;
    afxFence                fence;
    afxNat                  cmdbCnt;
    avxCmdb                 cmdbs[];
};

AFX_DEFINE_STRUCT(_sglQueueingWork)
{
    _sglQueueing            hdr;
    avxQueueOp              subm;
    void*                   data[];
};

AFX_DEFINE_STRUCT(_sglQueueingTransfer)
{
    _sglQueueing            hdr;

    afxSemaphore        wait;
    afxSemaphore        signal;
    afxFence            fence;

    afxFcc              srcFcc;
    afxFcc              dstFcc;
    afxCodec            codec;
    afxNat              decSiz;

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

    afxNat                  itemCnt;
    union
    {
        afxRasterIo         rasIo[];
        afxBufferIo         bufIo[];
    }                       items;
};

AFX_DEFINE_STRUCT(_sglQueueingPresent)
{
    _sglQueueing            hdr;
    afxSemaphore            wait;
    afxNat                  itemCnt;
    struct
    {
        afxDrawOutput       dout;
        afxNat              bufIdx;
    }                       items[];
};

AFX_DEFINE_STRUCT(_sglQueueingMmap)
{
    _sglQueueing            hdr;
    afxBuffer               buf;
    afxSize                 off;
    afxNat                  ran;
    afxFlags                flags;
    void**                  placeholder;
};

AFX_DEFINE_STRUCT(_sglCmd)
{
    afxLinkage          script;
    afxNat              id;
    afxNat              siz;
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

AFX_DEFINE_UNION(sglDecodeVmt)
{
    afxCmd  cmd;
    void(*f[ZGL_CMD_TOTAL])(sglDpu* dpu, _sglCmd *cmd);
};

AFX_DEFINE_STRUCT(_sglCmdEcho)
{
    _sglCmd                     cmd;
    afxLinkage                  echoLnk;
    afxM4d                      m;
    afxString                 caption;
    afxChar            AFX_SIMD strBuf[];
};

AFX_DEFINE_STRUCT(_sglCmdEchoFmt)
{
    _sglCmd                     cmd;
    afxM4d                      m;
    afxString                 caption;
    afxChar            AFX_SIMD strBuf[];
};

AFX_DEFINE_STRUCT(_sglCmdDraw)
{
    _sglCmd                     cmd;
    avxDrawIndirectCmd          data;
};

AFX_DEFINE_STRUCT(_sglCmdDrawIndirect)
{
    _sglCmd                     cmd;
    afxBuffer buf;
    afxNat32 offset;
    afxNat32 drawCnt;
    afxNat32 stride;
};

AFX_DEFINE_STRUCT(_sglCmdDrawIndirectCnt)
{
    _sglCmd                     cmd;
    afxBuffer buf;
    afxNat32 offset;
    afxBuffer cntBuf;
    afxNat32 cntBufOff;
    afxNat32 maxDrawCnt;
    afxNat32 stride;
};

AFX_DEFINE_STRUCT(_sglCmdDrawIndexed)
{
    _sglCmd                     cmd;
    avxDrawIndexedIndirectCmd   data;
};

AFX_DEFINE_STRUCT(_sglCmdDrawIndexedIndirect)
{
    _sglCmd                     cmd;
    afxBuffer buf;
    afxNat32 offset;
    afxNat32 drawCnt;
    afxNat32 stride;
};

AFX_DEFINE_STRUCT(_sglCmdDrawIndexedIndirectCnt)
{
    _sglCmd                     cmd;
    afxBuffer buf;
    afxNat32 offset;
    afxBuffer cntBuf;
    afxNat32 cntBufOff;
    afxNat32 maxDrawCnt;
    afxNat32 stride;
};

AFX_DEFINE_STRUCT(_sglCmdDispatch)
{
    _sglCmd                     cmd;
    avxDispatchIndirectCmd      data;
};

AFX_DEFINE_STRUCT(_sglCmdDispatchIndirect)
{
    _sglCmd                     cmd;
    afxBuffer buf;
    afxNat32 offset;
};

AFX_DEFINE_STRUCT(_sglCmdBeginSynthesis)
{
    _sglCmd cmd;
    afxBool defFbo;
    afxLinkage canvOpsLnk;
    avxCanvas canv;
    afxRect area;
    afxNat32 layerCnt;
    avxDrawTarget depth;
    avxDrawTarget stencil;
    afxNat32 rasterCnt;
    avxDrawTarget AFX_SIMD rasters[];
};

AFX_DEFINE_STRUCT(_sglCmdBindRasters)
{
    _sglCmd cmd;
    afxNat32 set;
    afxNat32 first, cnt;
    afxRaster AFX_SIMD tex[_ZGL_MAX_ENTRY_PER_LEGO];
    afxNat subras[_ZGL_MAX_ENTRY_PER_LEGO];
};

AFX_DEFINE_STRUCT(_sglCmdBindSamplers)
{
    _sglCmd cmd;
    afxNat32 set;
    afxNat32 first, cnt;
    avxSampler AFX_SIMD smp[_ZGL_MAX_ENTRY_PER_LEGO];
};

AFX_DEFINE_STRUCT(_sglCmdBindBuffers)
{
    _sglCmd cmd;
    afxNat32 set;
    afxNat32 first, cnt;
    afxBuffer AFX_SIMD buf[_ZGL_MAX_ENTRY_PER_LEGO];
    afxNat32 AFX_SIMD offset[_ZGL_MAX_ENTRY_PER_LEGO];
    afxNat32 AFX_SIMD range[_ZGL_MAX_ENTRY_PER_LEGO];
};

AFX_DEFINE_STRUCT(sglBufferInfo)
{
    afxBuffer buf;
    afxNat32 offset;
    afxNat32 range;
    afxNat32 stride;
};

AFX_DEFINE_STRUCT(_sglCmdVertexSources)
{
    _sglCmd cmd;
    afxNat32 first, cnt;
    sglBufferInfo AFX_SIMD info[];
};

AFX_DEFINE_STRUCT(_sglCmdVertexStreams)
{
    _sglCmd                     cmd;
    afxNat cnt;
    struct
    {
        afxNat  srcIdx;
        afxNat  stride;
        afxBool instance;
    }   AFX_SIMD streams[];
};

AFX_DEFINE_STRUCT(_sglCmdVertexAttributes)
{
    _sglCmd                     cmd;    
    afxNat cnt;
    struct
    {
        afxNat location;
        afxVertexFormat fmt;
        afxNat srcIdx;
        afxNat32 offset;
    }   AFX_SIMD attrs[];
};

AFX_DEFINE_STRUCT(_sglCmdDebugScope)
{
    _sglCmd             cmd;
    afxBool             open;
    afxColor            color;
    afxString2048       name;
};

AFX_DEFINE_STRUCT(_sglCmdBufferRange)
{
    _sglCmd                     cmd;
    afxBuffer                       buf;
    afxNat32                        offset;
    afxNat32                        range;
    afxNat32                        stride;
};

AFX_DEFINE_STRUCT(_sglCmdRasterizer)
{
    _sglCmd                     cmd;
    avxRasterizer               razr;
    avxVertexInput              vin;
    afxFlags                    dynamics;
};

AFX_DEFINE_STRUCT(_sglCmdPipeline)
{
    _sglCmd                     cmd;
    afxNat                      segment;
    avxPipeline                 pip;
    avxVertexInput              vin;
    afxFlags                    dynamics;
};

AFX_DEFINE_STRUCT(_sglCmdVertexInput)
{
    _sglCmd                     cmd;
    avxVertexInput              vin;
};

AFX_DEFINE_STRUCT(_sglCmdBool)
{
    _sglCmd                     cmd;
    afxBool                         value;
};

AFX_DEFINE_STRUCT(_sglCmdReal)
{
    _sglCmd                     cmd;
    afxReal                         value;
};

AFX_DEFINE_STRUCT(_sglCmdReal2)
{
    _sglCmd                     cmd;
    afxReal                         value[2];
};

AFX_DEFINE_STRUCT(_sglCmdReal3)
{
    _sglCmd                     cmd;
    afxReal                         value[3];
};

AFX_DEFINE_STRUCT(_sglCmdReal4)
{
    _sglCmd                     cmd;
    afxReal                         value[4];
};

AFX_DEFINE_STRUCT(_sglCmdPackRasterRegions)
{
    _sglCmd         cmd;
    afxBool         unpack;
    afxRaster ras;
    afxBuffer buf;
    afxNat opCnt;
    afxRasterIo AFX_SIMD ops[];
};

AFX_DEFINE_STRUCT(_sglCmdRwRasterRegions)
{
    _sglCmd         cmd;
    afxBool         down;
    afxRaster ras;
    afxStream ios;
    afxCodec cdc;
    afxNat opCnt;
    afxRasterIo AFX_SIMD ops[];
};

AFX_DEFINE_STRUCT(_sglCmdCopyRasterRegions)
{
    _sglCmd         cmd;
    afxRaster           src;
    afxRaster           dst;
    afxNat              opCnt;
    afxRasterCopy AFX_SIMD ops[];
};

AFX_DEFINE_STRUCT(_sglCmdRegenerateMipmaps)
{
    _sglCmd             cmd;
    afxRaster           ras;
    afxNat              baseLod;
    afxNat              lodCnt;
};

AFX_DEFINE_STRUCT(_sglCmdSwizzleRasterRegions)
{
    _sglCmd             cmd;
    afxRaster           ras;
    avxColorSwizzle     a;
    avxColorSwizzle     b;
    afxNat              rgnCnt;
    afxRasterRegion AFX_SIMD rgn[];
};

AFX_DEFINE_STRUCT(_sglCmdFlipRasterRegions)
{
    _sglCmd             cmd;
    afxRaster           ras;
    afxM4d              m;
    afxNat              rgnCnt;
    afxRasterRegion AFX_SIMD rgn[];
};

AFX_DEFINE_STRUCT(_sglCmdBufIo)
{
    _sglCmd         cmd;
    afxBuffer       buf;
    afxBool         export;
    afxStream       io;
    afxNat          opCnt;
    afxBufferIo AFX_SIMD ops[];
};

AFX_DEFINE_STRUCT(_sglCmdBufRw)
{
    _sglCmd         cmd;
    afxBuffer       buf;
    afxNat          offset;
    afxNat          range;
    afxBool         toHost;
    union
    {
        void*       dst;
        afxByte AFX_SIMD src[];
    };
};

AFX_DEFINE_STRUCT(_sglCmdBufCpy)
{
    _sglCmd         cmd;
    afxBuffer src;
    afxBuffer dst;
    afxNat opCnt;
    afxBufferCopy AFX_SIMD ops[];
};

AFX_DEFINE_STRUCT(_sglCmdBufSet)
{
    _sglCmd         cmd;
    afxBuffer buf;
    afxNat  offset;
    afxNat  range;
    afxNat data;
};

AFX_DEFINE_STRUCT(_sglCmdNat)
{
    _sglCmd                     cmd;
    afxNat                          value;
};

AFX_DEFINE_STRUCT(_sglCmdBitmaskNat32)
{
    _sglCmd                     cmd;
    afxMask                         mask;
    afxNat32                        value;
};

AFX_DEFINE_STRUCT(_sglCmdScissor)
{
    _sglCmd cmd;
    afxNat32 first, cnt;
    afxRect AFX_SIMD rect[];
};

AFX_DEFINE_STRUCT(_sglCmdViewport)
{
    _sglCmd cmd;
    afxNat32 first, cnt;
    afxViewport AFX_SIMD vp[];
};

AFX_DEFINE_STRUCT(_sglCmdNextPass)
{
    _sglCmd                     cmd;
    afxBool32                       useAuxScripts;
};

AFX_DEFINE_STRUCT(_sglCmdExecCmds)
{
    _sglCmd                     cmd;
    afxNat                          cnt;
    avxCmdb AFX_SIMD subsets[8];
};

AFX_DEFINE_STRUCT(_sglCmdCopyTex)
{
    _sglCmd                     cmd;
    afxRaster                      dst;
    afxRaster                      src;
    afxNat32                        rgnCnt;
    afxRasterRegion AFX_SIMD rgn[16];
};

AFX_DEFINE_STRUCT(_sglCmdUniformVectorEXT)
{
    _sglCmd                     cmd;
    afxChar const*                  name;
    afxNat                          cnt;
    afxReal AFX_SIMD v[4];
};

AFX_DEFINE_STRUCT(_sglCmdUniformMatrixEXT)
{
    _sglCmd                     cmd;
    afxChar const*                  name;
    afxNat                          cnt;
    afxReal AFX_SIMD m[4][4];
};


// vkCmdBindDescriptorSets - Binds descriptor sets to a command buffer

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

ZGL afxError _DpuPresentDout(sglDpu* dpu, afxDrawOutput dout, afxNat outBufIdx);

//ZGL afxSize _AfxMeasureTextureRegion(afxRaster tex, afxRasterRegion const *rgn);

ZGL afxError _SglDdevOpenCb(afxDrawDevice ddev, afxDrawContext dctx, void** args, afxNat invokeNo);
ZGL afxError _SglDdevCloseCb(afxDrawDevice ddev, afxDrawContext dctx);

//ZGL afxBool _SglDqueVmtSubmitCb(afxDrawContext dctx, afxDrawBridge dexu, afxDrawSubmissionSpecification const *spec, afxNat *submNo);
ZGL afxBool _DexuProcCb(afxDrawBridge dexu, afxThread thr);

ZGL afxClassConfig const _SglDctxMgrCfg;

ZGL afxError _SglDoutVmtFlushCb(afxDrawOutput dout, afxTime timeout);

ZGL afxError _SglDdevOpenDinCb(afxDrawDevice ddev, afxDrawInput din, afxDrawInputConfig const* cfg);
ZGL afxError _SglDdevCloseDoutCb(afxDrawDevice ddev, afxDrawOutput dout);
ZGL afxError _SglDdevOpenDoutCb(afxDrawDevice ddev, afxDrawOutput dout, afxDrawOutputConfig const* config);
ZGL afxError _SglDinFreeAllBuffers(afxDrawInput din);

ZGL afxCmd _SglEncodeCmdVmt;
ZGL void _SglBindFboAttachment(glVmt const* gl, GLenum glTarget, GLenum glAttachment, GLenum texTarget, GLuint texHandle, GLint level, GLuint layer, GLuint z);

ZGL afxError _SglWaitFenc(afxBool waitAll, afxNat64 timeout, afxNat cnt, afxFence const fences[]);
ZGL afxError _SglResetFenc(afxNat cnt, afxFence const fences[]);

ZGL afxCmdId _SglEncodeCmdCommand(avxCmdb cmdb, afxNat id, afxNat siz, _sglCmd *cmd);

ZGL afxClassConfig const _SglFencMgrCfg;
ZGL afxClassConfig const _SglSemMgrCfg;

ZGL afxResult _SglDdevIoctl(afxDrawDevice ddev, afxNat reqCode, va_list va);

ZGL afxError _SglActivateDout(sglDpu* dpu, afxDrawOutput dout);

ZGL afxNat _SglDexuEnqueueMmapCb(afxDrawBridge dexu, afxBuffer buf, afxSize off, afxNat ran, afxFlags flags, void** placeholder);

ZGL void _SglCopyTexSubImage(sglDpu* dpu, GLenum glDstTarget, GLenum glSrcTarget, GLuint glSrcHandle, afxNat opCnt, afxRasterCopy const ops[]);
ZGL void _SglUnpackTexSubImage(sglDpu* dpu, GLenum glTarget, afxNat bpp, GLenum glFmt, GLenum glType, afxNat opCnt, afxRasterIo const ops[]);

ZGL void DpuBufRw(sglDpu* dpu, afxBuffer buf, afxNat offset, afxNat range, afxBool toHost, void* srcOrDst);
ZGL void DpuBufCpy(sglDpu* dpu, afxBuffer src, afxBuffer dst, afxNat opCnt, afxBufferCopy const ops[]);
ZGL void DpuBufSet(sglDpu* dpu, afxBuffer buf, afxNat offset, afxNat range, afxNat data);

ZGL void _DecodeCmdSwitchFrontFace(sglDpu* dpu, _sglCmdBool const *cmd);
ZGL void _DecodeCmdSetCullMode(sglDpu* dpu, _sglCmdNat const *cmd);
ZGL void _DecodeCmdSetPrimitiveTopology(sglDpu* dpu, _sglCmdNat const *cmd);
ZGL void _DecodeCmdSetViewports(sglDpu* dpu, _sglCmdViewport const *cmd);
ZGL void _DecodeCmdBindVertexSources(sglDpu* dpu, _sglCmdVertexSources const *cmd);
ZGL void _DecodeCmdBindIndexSource(sglDpu* dpu, _sglCmdBufferRange const *cmd);
ZGL void _DecodeCmdBindVertexInput(sglDpu* dpu, _sglCmdVertexInput *cmd);

ZGL void _DpuBindIndexSource(sglDpu* dpu, afxBuffer buf, afxNat32 offset, afxNat32 range, afxNat32 stride);
ZGL void _DpuBindVertexInput(sglDpu* dpu, avxVertexInput vin);
ZGL void _DpuBindVertexSources(sglDpu* dpu, afxNat first, afxNat cnt, sglBufferInfo const src[]);
ZGL void _DpuSetViewports(sglDpu* dpu, afxNat first, afxNat cnt, afxViewport const vp[]);

ZGL void _DecodeCmdDisableRasterization(sglDpu* dpu, _sglCmdBool const *cmd);
ZGL void _DecodeCmdEnableDepthBias(sglDpu* dpu, _sglCmdBool const *cmd);
ZGL void _DecodeCmdSetDepthBias(sglDpu* dpu, _sglCmdReal3 const *cmd);
ZGL void _DecodeCmdSetLineWidth(sglDpu* dpu, _sglCmdReal const *cmd);
ZGL void _DecodeCmdEnableStencilTest(sglDpu* dpu, _sglCmdBool const *cmd);
ZGL void _DecodeCmdSetStencilCompareMask(sglDpu* dpu, _sglCmdBitmaskNat32 const *cmd);
ZGL void _DecodeCmdSetStencilWriteMask(sglDpu* dpu, _sglCmdBitmaskNat32 const *cmd);
ZGL void _DecodeCmdSetStencilReference(sglDpu* dpu, _sglCmdBitmaskNat32 const *cmd);
ZGL void _DecodeCmdEnableDepthTest(sglDpu* dpu, _sglCmdBool const *cmd);
ZGL void _DecodeCmdSetDepthCompareOp(sglDpu* dpu, _sglCmdNat const *cmd);
ZGL void _DecodeCmdDisableDepthWrite(sglDpu* dpu, _sglCmdBool const *cmd);
ZGL void _DecodeCmdSetBlendConstants(sglDpu* dpu, _sglCmdReal4 const *cmd);
ZGL void _DecodeCmdSetScissors(sglDpu* dpu, _sglCmdScissor const *cmd);
ZGL void _DecodeCmdSetCurtains(sglDpu* dpu, _sglCmdScissor const *cmd);

ZGL void _DecodeCmdRasSubsample(sglDpu* dpu, _sglCmdRegenerateMipmaps const* cmd);
ZGL void _DecodeCmdRasPack(sglDpu* dpu, _sglCmdPackRasterRegions const* cmd);
ZGL void _DecodeCmdRasCopy(sglDpu* dpu, _sglCmdCopyRasterRegions const* cmd);

ZGL void _DecodeCmdBufCpy(sglDpu* dpu, _sglCmdBufCpy const* cmd);
ZGL void _DecodeCmdBufSet(sglDpu* dpu, _sglCmdBufSet const* cmd);
ZGL void _DecodeCmdBufRw(sglDpu* dpu, _sglCmdBufRw const* cmd);

ZGL void SglFlushXformStateChanges(sglDpu* dpu);
ZGL void SglFlushRasterizationStateChanges(sglDpu* dpu);

ZGL void _DecodeCmdNextPass(sglDpu* dpu, _sglCmd const *cmd);
ZGL void _DecodeCmdEndDrawScope(sglDpu* dpu, _sglCmd const *cmd);
ZGL void _DecodeCmdBeginDrawScope(sglDpu* dpu, _sglCmdBeginSynthesis const *cmd);

ZGL void _DecodeCmdBindSamplers(sglDpu* dpu, _sglCmdBindSamplers const *cmd);
ZGL void _DecodeCmdBindRasters(sglDpu* dpu, _sglCmdBindRasters const *cmd);
ZGL void _DecodeCmdBindBuffers(sglDpu* dpu, _sglCmdBindBuffers const *cmd);

ZGL void _DecodeCmdBindPipeline(sglDpu* dpu, _sglCmdPipeline *cmd);
ZGL void _DecodeCmdBindRasterizer(sglDpu* dpu, _sglCmdRasterizer *cmd);


ZGL void _DecodeCmdDraw(sglDpu* dpu, _sglCmdDraw const *cmd);
ZGL void _DecodeCmdDrawIndexed(sglDpu* dpu, _sglCmdDrawIndexed const* cmd);

ZGL void _DpuBeginDrawScope(sglDpu* dpu, avxCanvas canv, afxRect const* area, afxNat layerCnt, afxNat cCnt, avxDrawTarget const* c, avxDrawTarget const* d, avxDrawTarget const* s, afxBool defFbo);
ZGL void _DpuEndDrawScope(sglDpu* dpu);

ZGL void _DpuExecuteCommands(sglDpu* dpu, afxNat cnt, avxCmdb streams[]);
ZGL void _DpuDraw(sglDpu* dpu, avxDrawIndirectCmd const* data);
ZGL void _DpuDrawIndirect(sglDpu* dpu, afxBuffer buf, afxNat32 offset, afxNat32 drawCnt, afxNat32 stride);
ZGL void _DpuDrawIndirectCount(sglDpu* dpu, afxBuffer buf, afxNat32 offset, afxBuffer cntBuf, afxNat32 cntBufOff, afxNat32 maxDrawCnt, afxNat32 stride);
ZGL void _DpuDrawIndexed(sglDpu* dpu, avxDrawIndexedIndirectCmd const* data);
ZGL void _DpuDrawIndexedIndirect(sglDpu* dpu, afxBuffer buf, afxNat32 offset, afxNat32 drawCnt, afxNat32 stride);
ZGL void _DpuDrawIndexedIndirectCount(sglDpu* dpu, afxBuffer buf, afxNat32 offset, afxBuffer cntBuf, afxNat32 cntBufOff, afxNat32 maxDrawCnt, afxNat32 stride);
ZGL void _DpuDispatch(sglDpu* dpu, afxNat grpCntX, afxNat grpCntY, afxNat grpCntZ);
ZGL void _DpuDispatchIndirect(sglDpu* dpu, afxBuffer buf, afxNat32 offset);

ZGL void _DpuBindRasters(sglDpu* dpu, afxNat set, afxNat first, afxNat cnt, afxRaster rasters[], afxNat const subras[]);
ZGL void _DpuBindSamplers(sglDpu* dpu, afxNat set, afxNat first, afxNat cnt, avxSampler samplers[]);
ZGL void _DpuBindBuffers(sglDpu* dpu, afxNat set, afxNat first, afxNat cnt, afxBuffer buffers[], afxNat const offsets[], afxNat const ranges[]);

ZGL void _DpuBindPipeline(sglDpu* dpu, avxPipeline pip, avxVertexInput vin, afxFlags dynamics);
ZGL void _DpuBindRasterizer(sglDpu* dpu, avxRasterizer razr, avxVertexInput vin, afxFlags dynamics);

ZGL sglDecodeVmt _SglDecodeCmdVmt;

#endif//ZGL_COMMANDS_H
