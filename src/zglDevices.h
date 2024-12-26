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

AFX_DEFINE_STRUCT(zglVertexInputState)
{
    struct
    {
        GLuint          bufGpuHandle;
        afxBuffer       buf;
        afxUnit32        offset;
        afxUnit32        range;
        afxUnit32        stride;
    }                   sources[ZGL_MAX_VERTEX_ATTRIB_BINDINGS];
    // used by queue
    afxMask             sourcesUpdMask;
    afxUnit              sourcesUpdCnt;

    GLuint              idxSrcGpuHandle;
    afxBuffer           idxSrcBuf;
    afxUnit32            idxSrcOff;
    afxUnit32            idxSrcRange;
    afxUnit32            idxSrcSiz;
    afxBool             iboUpdReq;
};

typedef struct zglXfrmState
{
    afxUnit              vpCnt; /// 0
    afxViewport         vps[ZGL_MAX_VIEWPORTS];
    afxMask             vpsUpdMask;

    avxTopology         primTop; /// is a option defining the primitive topology. /// avxTopology_TRI_LIST
    afxBool             primRestartEnabled; /// controls whether a special vertex index value (0xFF, 0xFFFF, 0xFFFFFFFF) is treated as restarting the assembly of primitives. /// FALSE
    afxUnit              patchControlPoints; /// is the number of control points per patch.

    afxBool             depthClampEnabled; /// controls whether to clamp the fragment's depth values as described in Depth Test. /// FALSE

    avxCullMode         cullMode; /// is the triangle facing direction used for primitive culling. /// avxCullMode_BACK
    afxBool             cwFrontFacing; /// If this member is TRUE, a triangle will be considered front-facing if its vertices are clockwise. /// FALSE (CCW)

} zglXfrmState;

typedef struct zglRasterizerState
{
    avxCanvas       canv;
    GLuint          canvGpuHandle;
    afxBool         canvIsDefFbo;
    afxRect         drawArea;
    afxUnit32        baseLayer;
    afxUnit32        layerCnt;
    afxUnit32        colDtCnt;
    avxDrawTarget   colDts[ZGL_MAX_COLOR_ATTACHMENTS];
    avxDrawTarget   dsDts[2];

    afxBool         rasterizationDisabled; /// controls whether primitives are discarded immediately before the rasterization stage. /// FALSE
    avxFillMode         fillMode; /// is the triangle rendering mode. /// avxFillMode_SOLID
    afxReal             lineWidth; /// is the width of rasterized line segments. /// 1.f    

    afxBool             depthBiasEnabled; /// controls whether to bias fragment depth values. /// FALSE
    afxReal             depthBiasSlopeScale; /// is a scalar factor applied to a fragment's slope in depth bias calculations. /// 0.f
    afxReal             depthBiasConstFactor; /// is a scalar factor controlling the constant depth value added to each fragment. /// 0.f
    afxReal             depthBiasClamp; /// is the maximum (or minimum) depth bias of a fragment. /// 0.f

    afxBool             msEnabled; /// If enabld, multisample rasterization will be used. FALSE
    afxUnit             sampleLvl; /// is a value specifying the number of samples used in rasterization. /// 0
    afxMask             sampleMasks[32]; /// an array of sample mask values used in the sample mask test. /// [ 1, ]
    afxBool             sampleShadingEnabled; /// used to enable Sample Shading. /// FALSE
    afxReal             minSampleShadingValue; /// specifies a minimum fraction of sample shading if sampleShadingEnable is set to TRUE. /// 0.f
    afxBool             alphaToCoverageEnabled; /// controls whether a temporary coverage value is generated based on the alpha component of the fragment's first color output. /// FALSE
    afxBool             alphaToOneEnabled; /// controls whether the alpha component of the fragment's first color output is replaced with one. /// FALSE

    avxFormat           dsFmt; /// is the format of depth/stencil surface this pipeline will be compatible with.

    afxBool             depthTestEnabled; /// controls whether depth testing is enabled. /// FALSE
    avxCompareOp        depthCompareOp; /// is a value specifying the comparison operator to use in the Depth Comparison step of the depth test. /// avxCompareOp_LESS
    afxBool             depthWriteDisabled; /// controls whether depth writes are enabled when depthTestEnable is TRUE. Depth writes are always disabled when depthTestEnable is FALSE. /// FALSE
    afxBool             depthBoundsTestEnabled; /// controls whether depth bounds testing is enabled. /// FALSE
    afxV2d              depthBounds; /// is the minimum depth bound used in the depth bounds test. /// [ min, max ]
    afxBool             stencilTestEnabled; /// FALSE
    avxStencilInfo  stencilFront; /// is the configuration values controlling the corresponding parameters of the stencil test.
    avxStencilInfo  stencilBack; /// is the configuration controlling the corresponding parameters of the stencil test.

    afxUnit              scisCnt; // must be the same as viewport
    afxRect             scisRects[ZGL_MAX_VIEWPORTS];
    afxUnit32            scisUpdMask;
    afxUnit              curtainCnt; // must be the same as viewport
    afxRect             curtainRects[ZGL_MAX_VIEWPORTS];
    afxUnit32            curtainUpdMask;

    afxUnit              outCnt;
    avxColorOutput  outs[8];
    afxMask             outUpdMask;

    afxBool             anyBlendEnabled;
    afxReal             blendConstants[4]; /// [ 0, 0, 0, 1 ]
    afxBool             logicOpEnabled; /// FALSE
    avxLogicOp          logicOp; /// avxLogicOp_NOP
} zglRasterizerState;


typedef struct
{
    avxDpu                  m;
    glVmt                   gl;
    //afxUnit                  exuIdx;
    //afxUnit                  portId; // exuIdx
    //afxDrawSystem          activeDsys;

    //afxSize                 numOfFedVertices, numOfFedIndices, numOfFedInstances;

    //afxBool                 running;

    //afxBool         instanced;

    zglXfrmState            activeTs, nextTs;
    zglRasterizerState      activeRs, nextRs;

    avxLigature             activeLiga, nextLiga;
    avxPipeline             activePip, nextPip;
    GLuint                  activePipGpuHandle;
    avxVertexDecl          activeVin, nextVin;
    GLuint                  activeVinGpuHandle;
    zglVertexInputState     nextVinBindings;

    union
    {
        struct
        {
            afxBuffer       buf; // The GPUBuffer to bind.
            afxUnit32        offset; // The offset, in bytes, from the beginning of buffer to the beginning of the range exposed to the shader by the buffer binding.
            afxUnit32        range; // The size, in bytes, of the buffer binding. If undefined, specifies the range starting at offset and ending at the end of buffer.
            
        };
        struct
        {
            avxSampler      smp;
            afxRaster       ras;
            afxUnit          subrasIdx;
        };
    }                       activeLs[_ZGL_MAX_LEGO_PER_BIND][_ZGL_MAX_ENTRY_PER_LEGO],
                            nextLs[_ZGL_MAX_LEGO_PER_BIND][_ZGL_MAX_ENTRY_PER_LEGO];
    afxMask                 nextLsUpdMask[_ZGL_MAX_LEGO_PER_BIND];


    // submission stuff
    afxBool         submissionSuspended;

    afxUnit          texPackUnitIdx;
    afxUnit          texUnpackUnitIdx;
    GLuint          dstPboGpuHandle; // GL_PIXEL_PACK_BUFFER
    GLuint          srcPboGpuHandle; // GL_PIXEL_UNPACK_BUFFER

    GLuint          copyDstBufGpuHandle; // GL_COPY_WRITE_BUFFER 
    GLuint          copySrcBufGpuHandle; // GL_COPY_READ_BUFFER 

    GLuint          tmpFbo[_ZGL_MAX_SWAPCHAIN_CAP];
    afxUnit          activeTmpFboIdx;

    GLuint          emptyVao;
    GLuint          fboOpSrc;
    afxRaster       fboOpSrcAnnex;
    GLuint          fboOpDst;
    afxRaster       fboOpDstAnnex;

    GLuint          pushConstUbo;
    afxUnit          pushConstUboIdx;
    afxByte*        pushConstMappedMem;

    GLuint          timeElapsedQueryIdActive;
} zglDpu;

#if 0
struct _afxDdevIdd
{
    WNDCLASSEX      wndClss;
    afxModule       oglDll;
    afxUnit          oglVerMajor, oglVerMinor, oglVerPatch;

    afxModule       d3d9Dll;
    HRESULT (WINAPI*Direct3DCreate9Ex)(UINT SDKVersion, IDirect3D9Ex **ppD3D);
    afxBool         useDxgiSwapchain;
    afxBool         hasDxInterop1;
    afxBool         hasDxInterop2;
};
#endif

AFX_OBJECT(afxDrawDevice)
{
    AFX_OBJ(_avxDrawDevice) m;
    WNDCLASSEX      wndClss;
    afxModule       oglDll;
    afxUnit         oglVerMajor, oglVerMinor, oglVerPatch;

    afxModule       d3d9Dll;
    HRESULT(WINAPI*Direct3DCreate9Ex)(UINT SDKVersion, IDirect3D9Ex **ppD3D);
    afxBool         useDxgiSwapchain;
    afxBool         hasDxInterop1;
    afxBool         hasDxInterop2;
};

AFX_OBJECT(afxDrawBridge)
{
    AFX_OBJ(_avxDrawBridge) m;
    zglDpu                  dpu;
    HWND                    hWnd;
    HDC                     hDC;
    int                     dcPxlFmt;
    PIXELFORMATDESCRIPTOR   dcPfd;
    HGLRC                   hRC;
    afxUnit                 verMajor, verMinor, verPatch;
    afxString               subsysName;
    afxString               subsysVer;

    afxInterlockedQueue     deletionQueue;

    afxClassExtension vinExt;
    afxClassExtension pipExt;
    afxClassExtension canvExt;
};

AFX_OBJECT(afxDrawSystem)
{
    AFX_OBJECT(_avxDrawSystem) m;
    // presentation stuff, only on unit 0.
    afxBool presentationSuspended;
    //GLuint presentFboGpuHandle;
    avxPipeline presentRazr;
    avxSampler  presentSmp;

    avxPipeline fntRazr;
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
    GLuint*                 swapFbo;
    afxBool8*               swapFboReady;

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

AFX_DEFINE_STRUCT(_zglDeleteGlRes)
{
    union
    {
        GLuint  gpuHandle;
        void*   gpuHandlePtr;
    };
    GLuint  type; // 0 buf, 1 tex, 2 surf, 3 canv, 4 smp, 5 pip, 6 shd, 7 shd (separate) program
};

ZGL void _ZglDsysEnqueueDeletion(afxDrawSystem dsys, afxUnit exuIdx, afxUnit type, afxSize gpuHandle);

ZGL afxError _DpuPresentDout(zglDpu* dpu, afxDrawOutput dout, afxUnit outBufIdx);

ZGL afxError _ZglDdevOpenCb(afxDrawDevice ddev, afxDrawSystem dsys, void** args, afxUnit invokeNo);
ZGL afxError _ZglDdevCloseCb(afxDrawDevice ddev, afxDrawSystem dsys);

ZGL afxError _ZglOpenDexuCb(afxDrawDevice ddev, afxDrawBridge dexu, afxDrawBridgeConfig const* cfg);

//ZGL afxBool _ZglDqueVmtSubmitCb(afxDrawSystem dsys, afxDrawBridge dexu, afxDrawSubmissionSpecification const *spec, afxUnit *submNo);
ZGL afxBool _DexuProcCb(afxDrawBridge dexu, afxThread thr);

ZGL afxClassConfig const _ZglDsysMgrCfg;

ZGL afxError _ZglDinCtorCb(afxDrawInput din, void** args, afxUnit invokeNo);
ZGL afxError _ZglDoutCtorCb(afxDrawOutput dout, void** args, afxUnit invokeNo);
ZGL afxError _ZglDoutDtorCb(afxDrawOutput dout);

ZGL afxError _ZglDsysDtorCb(afxDrawSystem dsys);
ZGL afxError _ZglDsysCtorCb(afxDrawSystem dsys, void** args, afxUnit invokeNo);

ZGL afxError _ZglDexuDtorCb(afxDrawBridge dexu);
ZGL afxError _ZglDexuCtorCb(afxDrawBridge dexu, void** args, afxUnit invokeNo);

ZGL afxError _ZglCreateHwSurface(LPWNDCLASSEXA cls, HWND* phWnd, HDC* phDC, int* pPixFmt, PIXELFORMATDESCRIPTOR* pPfd);
ZGL afxError _ZglCreateHwContext(afxModule oglDll, HDC hDC, HGLRC hShareCtx, HGLRC* phGLRC, glVmt* gl);
ZGL afxError _ZglProcessDeletionQueue(glVmt const* gl, afxInterlockedQueue* ique);
ZGL void    _ZglDsysEnqueueDeletion(afxDrawSystem dsys, afxUnit exuIdx, afxUnit type, afxSize gpuHandle);


_ZGL afxError _ZglSemCtorCb(afxSemaphore sem, void** args, afxUnit invokeNo);
_ZGL afxError _ZglSemDtorCb(afxSemaphore sem);
_ZGL afxError _ZglFencCtorCb(afxFence fenc, void** args, afxUnit invokeNo);
_ZGL afxError _ZglFencDtorCb(afxFence fenc);

#endif//ZGL_DEVICES_H
