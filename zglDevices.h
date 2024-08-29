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

#ifndef ZGL_DEVICES_H
#define ZGL_DEVICES_H

#include "zglDefs.h"

AFX_DEFINE_STRUCT(sglVertexInputState)
{
    struct
    {
        afxBuffer       buf;
        afxNat32        offset;
        afxNat32        range;
        afxNat32        stride;
    }                   sources[ZGL_MAX_VERTEX_ATTRIB_BINDINGS];
    /*
    struct
    {
        afxNat          srcIdx;
        afxNat32        stride;
        afxBool         instance;
    }                   streams[ZGL_MAX_VERTEX_ATTRIB_BINDINGS];
    afxNat              streamCnt;
    struct
    {
        afxNat          location;
        afxVertexFormat fmt;
        afxNat          srcIdx;
        afxNat32        offset;
    }                   attrs[ZGL_MAX_VERTEX_ATTRIBS];
    afxNat              attrCnt;
    */
    afxBuffer           idxSrcBuf;
    afxNat32            idxSrcOff;
    afxNat32            idxSrcRange;
    afxNat32            idxSrcSiz;


    // used by queue
    afxMask             streamUpdMask;
    afxNat              streamUpdCnt;
    afxMask             attribUpdMask;
    afxNat              attribUpdCnt;
    afxBool             iboUpdReq;
};

typedef struct sglXfrmState
{
    afxNat              vpCnt; /// 0
    afxViewport         vps[ZGL_MAX_VIEWPORTS];

    avxTopology     primTop; /// is a option defining the primitive topology. /// avxTopology_TRI_LIST
    afxBool             primRestartEnabled; /// controls whether a special vertex index value (0xFF, 0xFFFF, 0xFFFFFFFF) is treated as restarting the assembly of primitives. /// FALSE
    afxNat              patchControlPoints; /// is the number of control points per patch.

    afxBool             depthClampEnabled; /// controls whether to clamp the fragment's depth values as described in Depth Test. /// FALSE

    avxCullMode         cullMode; /// is the triangle facing direction used for primitive culling. /// avxCullMode_BACK
    afxBool             cwFrontFacing; /// If this member is TRUE, a triangle will be considered front-facing if its vertices are clockwise. /// FALSE (CCW)

} sglXfrmState;

typedef struct sglRasterizerState
{
    afxBool             rasterizationDisabled; /// controls whether primitives are discarded immediately before the rasterization stage. /// FALSE

    struct
    {
        avxCanvas       canv;
        afxRect         area;
        afxNat32        layerCnt;
        afxNat32        rasterCnt;
        avxDrawTarget   rasters[ZGL_MAX_COLOR_ATTACHMENTS];
        avxDrawTarget   ds[2];
        afxBool         defFbo;
    }                   pass; // always active pass

    avxFillMode         fillMode; /// is the triangle rendering mode. /// avxFillMode_SOLID
    afxReal             lineWidth; /// is the width of rasterized line segments. /// 1.f    

    afxBool             depthBiasEnabled; /// controls whether to bias fragment depth values. /// FALSE
    afxReal             depthBiasSlopeScale; /// is a scalar factor applied to a fragment's slope in depth bias calculations. /// 0.f
    afxReal             depthBiasConstFactor; /// is a scalar factor controlling the constant depth value added to each fragment. /// 0.f
    afxReal             depthBiasClamp; /// is the maximum (or minimum) depth bias of a fragment. /// 0.f

    afxBool             msEnabled; /// If enabld, multisample rasterization will be used. FALSE
    afxNat              sampleCnt; /// is a value specifying the number of samples used in rasterization. /// 0
    afxMask             sampleMasks[32]; /// an array of sample mask values used in the sample mask test. /// [ 1, ]
    afxBool             sampleShadingEnabled; /// used to enable Sample Shading. /// FALSE
    afxReal             minSampleShadingValue; /// specifies a minimum fraction of sample shading if sampleShadingEnable is set to TRUE. /// 0.f
    afxBool             alphaToCoverageEnabled; /// controls whether a temporary coverage value is generated based on the alpha component of the fragment's first color output. /// FALSE
    afxBool             alphaToOneEnabled; /// controls whether the alpha component of the fragment's first color output is replaced with one. /// FALSE

    afxPixelFormat      dsFmt; /// is the format of depth/stencil surface this pipeline will be compatible with.

    afxBool             depthTestEnabled; /// controls whether depth testing is enabled. /// FALSE
    avxCompareOp        depthCompareOp; /// is a value specifying the comparison operator to use in the Depth Comparison step of the depth test. /// avxCompareOp_LESS
    afxBool             depthWriteDisabled; /// controls whether depth writes are enabled when depthTestEnable is TRUE. Depth writes are always disabled when depthTestEnable is FALSE. /// FALSE
    afxBool             depthBoundsTestEnabled; /// controls whether depth bounds testing is enabled. /// FALSE
    afxV2d              depthBounds; /// is the minimum depth bound used in the depth bounds test. /// [ min, max ]
    afxBool             stencilTestEnabled; /// FALSE
    avxStencilConfig    stencilFront; /// is the configuration values controlling the corresponding parameters of the stencil test.
    avxStencilConfig    stencilBack; /// is the configuration controlling the corresponding parameters of the stencil test.


    afxNat              areaCnt; /// 0
    afxRect             areaRects[ZGL_MAX_VIEWPORTS];
    afxNat              scisCnt; /// 0
    afxRect             scisRects[ZGL_MAX_VIEWPORTS];

    afxNat              outCnt;
    avxColorOutputChannel   outs[8];
    afxBool             anyBlendEnabled;
    afxReal             blendConstants[4]; /// [ 0, 0, 0, 1 ]
    afxBool             logicOpEnabled; /// FALSE
    avxLogicOp          logicOp; /// avxLogicOp_NOP
} sglRasterizerState;


typedef struct
{
    glVmt                   gl;
    afxNat                  exuIdx;
    afxNat                  portIdx; // exuIdx
    afxDrawContext          activeDctx;

    afxSize                 numOfFedVertices, numOfFedIndices, numOfFedInstances;
 
    sglXfrmState            activeXformState, nextXformState;
    sglRasterizerState      activeRasterState, nextRasterState;

    avxRasterizer           activeRazr, nextRazr;
    avxPipeline             activePip, nextPip;
    avxVertexInput          activeVin, nextVin;
    sglVertexInputState     nextVinBindings;

    union
    {
        struct
        {
            afxBuffer       buf; // The GPUBuffer to bind.
            afxNat32        offset; // The offset, in bytes, from the beginning of buffer to the beginning of the range exposed to the shader by the buffer binding.
            afxNat32        range; // The size, in bytes, of the buffer binding. If undefined, specifies the range starting at offset and ending at the end of buffer.
            
        };
        struct
        {
            avxSampler      smp;
            afxRaster      ras;
            afxNat          subrasIdx;
        };
    }                       activeResBind[_ZGL_MAX_LEGO_PER_BIND][_ZGL_MAX_ENTRY_PER_LEGO], nextResBind[_ZGL_MAX_LEGO_PER_BIND][_ZGL_MAX_ENTRY_PER_LEGO];
    afxMask                 nextResBindUpdMask[_ZGL_MAX_LEGO_PER_BIND];

    afxBuffer               activeUnpakPbo;
    afxBuffer               activePakPbo;

    afxMask                 nextVinBindingsMask;
    afxNat                  nextVinBindingsCnt;
    afxMask                 nextVinAttribUpdMask;
    afxNat                  nextVinAttribUpdCnt;

    afxMask                 nextBlendConstUpd;
    afxMask                 nextViewportUpdMask;
    afxNat                  nextViewportUpdCnt;
    afxMask                 nextScissorUpdMask;
    afxNat                  nextScissorUpdCnt;
    afxMask                 nextAreaUpdMask;
    afxNat                  nextAreaUpdCnt;
    afxBool                 scissorTestEnabled;

    afxBool                 flushPip, flushPass, flushSr;
    
    afxBool                 flushDepthTest, flushDepthBoundsTest, flushStencilTest, flushDepthBias;
    afxBool                 flushRasterizer, flushPrimCulling;
    afxBool                 flushMultisample, flushSampleShading, flushAlphaToCover, flushAlphaToOne;
    afxBool                 flushColorBlend, flushLogicOp, flushColorWriteMask;

    //afxNat                  flushVbBase, flushVbCnt;
    afxNat                  flushVpBase, flushVpCnt;
    afxNat                  flushSrBase, flushSrCnt;
    //afxNat                  flushRbBase[_ZGL_MAX_LEGO_PER_BIND], flushRbCnt[_ZGL_MAX_LEGO_PER_BIND];

    
    afxBool                 running;

    afxBool         instanced;

    // submission stuff
    afxBool         submissionSuspended;
    GLuint          tmpFbo[_ZGL_MAX_SWAPCHAIN_CAP];
    afxNat          activeTmpFboIdx;
    GLuint          vao[_ZGL_MAX_VAO_PER_TIME];
    afxNat          activeVaoIdx;

    GLuint          emptyVao;
    GLuint          fboOpSrc;
    afxRaster       fboOpSrcAnnex;
    GLuint          fboOpDst;
    afxRaster       fboOpDstAnnex;
} sglDpu;

struct _afxDdevIdd
{
    WNDCLASSEX      wndClss;
    afxModule       oglDll;
    afxNat          oglVerMajor, oglVerMinor, oglVerPatch;

    afxModule       d3d9Dll;
    HRESULT (WINAPI*Direct3DCreate9Ex)(UINT SDKVersion, IDirect3D9Ex **ppD3D);
    afxBool         useDxgiSwapchain;
    afxBool         hasDxInterop1;
    afxBool         hasDxInterop2;
};

AFX_OBJECT(afxDrawBridge)
{
    AFX_OBJ(_avxDrawBridge) m;
    sglDpu                  dpu;
    HWND                    hWnd;
    HDC                     hDC;
    int                     dcPxlFmt;
    PIXELFORMATDESCRIPTOR   dcPfd;
    HGLRC                   hRC;
    afxNat                  verMajor, verMinor, verPatch;
    afxString               subsysName;
    afxString               subsysVer;

    afxThread               worker;
    afxCondition            schedCnd; // signaled when some task is scheduled to this bridge.
    afxMutex                schedCndMtx;
    afxBool                 schedCnt;

    afxInterlockedQueue     deletionQueue;
};

AFX_OBJECT(afxDrawQueue)
{
    AFX_OBJECT(_avxDrawQueue) m;
};

AFX_OBJECT(afxDrawContext)
{
    AFX_OBJECT(_avxDrawContext) m;
    // presentation stuff, only on unit 0.
    afxBool presentationSuspended;
    //GLuint presentFboGpuHandle;
    avxRasterizer presentRazr;
    avxSampler  presentSmp;

    avxRasterizer fntRazr;
    avxSampler  fntSamp;
    afxRaster   fntRas;
    afxBuffer   fntDataBuf;
    afxBuffer   fntUnifBuf;

    //afxBuffer tristrippedQuad2dPosBuf;
    HWND                    hPrimeWnd;
    HDC                     hPrimeDC;
    int                     primeDcPixFmt;
    PIXELFORMATDESCRIPTOR   primeDcPfd;
    HGLRC                   hPrimeRC;
    glVmt                   gl;
};

AFX_OBJECT(afxDrawOutput)
{
    AFX_OBJECT(_avxDrawOutput) m;

    HINSTANCE               hInst;
    HWND                    hWnd;
    afxBool                 isWpp;
    HDC                     hDC;
    int                     dcPixFmt;
    PIXELFORMATDESCRIPTOR   dcPfd;
    HGLRC                   hSwapRC; // only swaps it. This is needed because hGLRC are strictly bound to a pixel format.
    GLuint                  swapFbo;

    struct
    {
        ID3D11Device*   d3d11Dev;
        HANDLE          d3d11DevForGl;
        DXGI_FORMAT mSwapChainFormat;
        UINT mSwapChainFlags;
        afxBool mFirstSwap;
        IDXGISwapChain *mSwapChain;
        IDXGISwapChain1 *mSwapChain1;
        GLuint mFramebufferID;
        GLuint mColorRenderbufferID;
        HANDLE mRenderbufferBufferHandle;
        GLuint mTextureID;
        HANDLE mTextureHandle;
        afxInt mSwapInterval;
        afxInt mOrientation;
    } dxgi;
    struct
    {
        IDirect3D9Ex*       d3d9Ex;
        IDirect3DDevice9Ex* d3d9DevEx;
        HANDLE              d3d9DevExForGl;
        IDirect3DSwapChain9*d3d9Sw;
        IDirect3DTexture9* sharedTexture;
        IDirect3DSurface9* pSurface;
    } d3dsw9;
};

AFX_OBJECT(afxDrawInput)
{
    AFX_OBJECT(_avxDrawInput) m;
};

AFX_DEFINE_STRUCT(_sglDeleteGlRes)
{
    union
    {
        GLuint  gpuHandle;
        void*   gpuHandlePtr;
    };
    GLuint  type; // 0 buf, 1 tex, 2 surf, 3 canv, 4 smp, 5 pip, 6 shd, 7 shd (separate) program
};

ZGL void _SglDctxEnqueueDeletion(afxDrawContext dctx, afxNat exuIdx, afxNat type, afxSize gpuHandle);

ZGL afxError _DpuPresentDout(sglDpu* dpu, afxDrawOutput dout, afxNat outBufIdx);

ZGL afxError _SglDdevOpenCb(afxDrawDevice ddev, afxDrawContext dctx, void** args, afxNat invokeNo);
ZGL afxError _SglDdevCloseCb(afxDrawDevice ddev, afxDrawContext dctx);

ZGL afxError _SglOpenDexuCb(afxDrawDevice ddev, afxDrawBridge dexu, afxDrawBridgeConfig const* cfg);

//ZGL afxBool _SglDqueVmtSubmitCb(afxDrawContext dctx, afxDrawBridge dexu, afxDrawSubmissionSpecification const *spec, afxNat *submNo);
ZGL afxBool _DexuProcCb(afxDrawBridge dexu, afxThread thr);

ZGL afxClassConfig const _SglDctxMgrCfg;

ZGL afxError _SglDdevOpenDinCb(afxDrawDevice ddev, afxDrawInput din, afxDrawInputConfig const* cfg);
ZGL afxError _SglDdevCloseDoutCb(afxDrawDevice ddev, afxDrawOutput dout);
ZGL afxError _SglDdevOpenDoutCb(afxDrawDevice ddev, afxDrawOutput dout, afxDrawOutputConfig const* config);
ZGL afxError _SglDinFreeAllBuffers(afxDrawInput din);

ZGL afxResult _SglDdevIoctl(afxDrawDevice ddev, afxNat reqCode, va_list va);

ZGL afxError _SglActivateDout(sglDpu* dpu, afxDrawOutput dout);

ZGL afxError _SglRelinkDoutCb(afxDrawDevice ddev, afxDrawContext dctx, afxNat cnt, afxDrawOutput dout[]);

ZGL afxError _SglDinCtorCb(afxDrawInput din, void** args, afxNat invokeNo);
ZGL afxError _SglDoutCtorCb(afxDrawOutput dout, void** args, afxNat invokeNo);
ZGL afxError _SglDoutDtorCb(afxDrawOutput dout);

ZGL afxError _SglDctxDtorCb(afxDrawContext dctx);
ZGL afxError _SglDctxCtorCb(afxDrawContext dctx, void** args, afxNat invokeNo);

ZGL afxError _SglDexuDtorCb(afxDrawBridge dexu);
ZGL afxError _SglDexuCtorCb(afxDrawBridge dexu, void** args, afxNat invokeNo);

ZGL afxError _SglDexuStartCb(afxDrawBridge dexu);
ZGL afxError _SglDexuStopCb(afxDrawBridge dexu);

ZGL afxError _SglCreateHwSurface(LPWNDCLASSEXA cls, HWND* phWnd, HDC* phDC, int* pPixFmt, PIXELFORMATDESCRIPTOR* pPfd);
ZGL afxError _SglCreateHwContext(afxModule oglDll, HDC hDC, HGLRC hShareCtx, HGLRC* phGLRC, glVmt* gl);
ZGL afxError _SglProcessDeletionQueue(glVmt const* gl, afxInterlockedQueue* ique);
ZGL void    _SglDctxEnqueueDeletion(afxDrawContext dctx, afxNat exuIdx, afxNat type, afxSize gpuHandle);

#endif//ZGL_DEVICES_H
