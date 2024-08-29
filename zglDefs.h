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

#ifndef ZGL_DEFS_H
#define ZGL_DEFS_H

#define _CRT_SECURE_NO_WARNINGS 1
//#define WIN32_LEAN_AND_MEAN 1

#define COBJMACROS
#define INITGUID
#include <intrin.h>
#include <d3d9.h>
#include <d3d11.h>
#include <windows.h>
#include <dxgi1_2.h>

//#include "../src/e2coree/deps/gl/glcorearb.h"
//#include "../src/e2coree/deps/gl/wglext.h"

#include "gl/glad.h"
#include "gl/glad_wgl.h"

//#define DISABLE_SHARED_HARDWARE_CONTEXT

#define _AVX_DRAW_C
#define _AFX_CORE_C
#define _AFX_DEVICE_C
#define _AFX_CONTEXT_C

//#define _AFX_MODULE_C
#define _AVX_DRAW_SYSTEM_C
#define _AVX_DRAW_DEVICE_C
#define _AVX_DRAW_DEVICE_IMPL
#define _AVX_DRAW_BRIDGE_C
#define _AVX_DRAW_BRIDGE_IMPL
#define _AVX_DRAW_QUEUE_C
#define _AVX_DRAW_QUEUE_IMPL
#define _AVX_DRAW_CONTEXT_C
#define _AVX_DRAW_CONTEXT_IMPL
#define _AVX_DRAW_OUTPUT_C
#define _AVX_DRAW_OUTPUT_IMPL
#define _AVX_DRAW_INPUT_C
#define _AVX_DRAW_INPUT_IMPL
#define _AVX_PIPELINE_C
#define _AVX_PIPELINE_IMPL
#define _AVX_BUFFER_C
#define _AVX_BUFFER_IMPL
#define _AVX_RASTER_C
#define _AVX_RASTER_IMPL
#define _AVX_RASTERIZER_C
#define _AVX_RASTERIZER_IMPL
#define _AVX_VERTEX_INPUT_C
#define _AVX_VERTEX_INPUT_IMPL
#define _AVX_CMD_BUFFER_C
#define _AVX_CMD_BUFFER_IMPL
#define _AVX_SAMPLER_C
#define _AVX_SAMPLER_IMPL
#define _AVX_SHADER_C
#define _AVX_SHADER_IMPL
#define _AVX_CANVAS_C
#define _AVX_CANVAS_IMPL
#define _AVX_LIGATURE_C
#define _AVX_BIND_SCHEMA_IMPL
#define _AFX_FENCE_C
#define _AFX_FENCE_IMPL
#define _AFX_SEMAPHORE_C
#define _AFX_SEMAPHORE_IMPL
#define _AVX_QUERY_POOL_C
#define _AVX_QUERY_POOL_IMPL

//#define _AUX_UX_C
//#define _AUX_OVERLAY_IMPL
//#define _AUX_OVERLAY_C

#include "qwadro/afx/dev/AvxImplKit.h"
#include "qwadro/ux/afxScript.h"
#include "zglUtils.h"

#define _ZGL_MAX_DOUT_PER_PRESENTATION 4
#define _ZGL_MAX_DSCR_PER_SUBMISSION 4
#define _ZGL_MAX_OBJ_PER_STREAMING 4
#define _ZGL_MAX_DQUE_PER_DCTX 4
#define _ZGL_MAX_SWAPCHAIN_CAP 3
#define _ZGL_MAX_SURF_PER_DOUT 3
#define _ZGL_MAX_LEGO_PER_BIND 4
#define _ZGL_MAX_ENTRY_PER_LEGO 10 
#define _ZGL_MAX_STAGE_PER_PIP 8
//#define _ZGL_MAX_VBO_PER_BIND 8

#define ZGL_MAX_VERTEX_ATTRIBS 8
#define ZGL_MAX_VERTEX_ATTRIB_BINDINGS 8
#define ZGL_MAX_VERTEX_ATTRIB_STRIDE 4096
#define ZGL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET 4096

#define ZGL_MIN_MAP_BUFFER_ALIGNMENT 64

#define ZGL_MAX_VIEWPORTS 8
#define ZGL_VIEWPORT_BOUNDS_RANGE 0
#define ZGL_MAX_VIEWPORT_DIMS 0
#define ZGL_MAX_CANVAS_WIDTH AFX_N16_MAX
#define ZGL_MAX_CANVAS_HEIGHT AFX_N16_MAX
#define ZGL_MAX_CANVAS_LAYERS AFX_N16_MAX
#define ZGL_DEPTH_RANGE


#define ZGL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 80
#define ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT (ZGL_MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1)

#define _ZGL_MAX_VAO_PER_TIME 8
//#define _ZGL_MAX_VP_PER_SET 8
//#define _ZGL_MAX_SCISSOR_PER_SET 8
//#define _ZGL_MAX_INSTREAM_PER_SET 8
//#define _ZGL_MAX_RASTER_SURF_PER_CANV 8
#define ZGL_MAX_COLOR_ATTACHMENTS 8
#define _ZGL_MAX_SURF_PER_CANV ZGL_MAX_COLOR_ATTACHMENTS + 2

#define FORCE_GL_GENERIC_FUNCS 1 // we also attempt to use specific functions as a medium of hinting for the driver.
#define ZGL_DONT_USE_ROBUST_ACCESS 1

typedef enum sglUpdateFlags
{
    ZGL_UPD_FLAG_DEVICE_FLUSH   = AFX_BIT(0), // flush from host to device
    ZGL_UPD_FLAG_HOST_FLUSH     = AFX_BIT(1), // flush from device to host
    ZGL_UPD_FLAG_DEVICE_INST    = AFX_BIT(2), // (re)instantiate on device
    ZGL_UPD_FLAG_HOST_INST      = AFX_BIT(3), // (re)instantiate on host
    
    ZGL_UPD_FLAG_HOST           = (ZGL_UPD_FLAG_HOST_INST | ZGL_UPD_FLAG_HOST_FLUSH),
    ZGL_UPD_FLAG_DEVICE         = (ZGL_UPD_FLAG_DEVICE_INST | ZGL_UPD_FLAG_DEVICE_FLUSH),
} sglUpdateFlags;

AFX_DEFINE_UNION(sglLegoData) // A GPUBindGroupEntry describes a single resource to be bound in a GPUBindGroup, and has the following members:
{
    struct
    {
        afxBuffer           buf; // The GPUBuffer to bind.
        afxNat              offset; // The offset, in bytes, from the beginning of buffer to the beginning of the range exposed to the shader by the buffer binding.
        afxNat              range; // The size, in bytes, of the buffer binding. If undefined, specifies the range starting at offset and ending at the end of buffer.
    };
    struct
    {
        avxSampler          smp;
        afxRaster          tex;
    };
};

typedef struct _DrawPipelineResource
{
    avxShaderStage stages;
    avxShaderResType resType;
    //afxShaerBaseResType baseType;
    //VkAccessFlags access;
    afxNat8 set;
    afxNat8 binding;
    afxNat8 location;
    afxNat8 inputAttachmentIndex;
    afxNat8 vecSize;
    afxNat8 arraySize;
    afxNat offset;
    afxNat size;
    afxChar name[16];
} _DrawPipelineResource;

typedef enum sglBindFlags
{
    sglBindFlag_BIND = AFX_BIT(0),
    sglBindFlag_SYNC = AFX_BIT(1),
    sglBindFlag_KEEP = AFX_BIT(2)
}sglBindFlags;

#endif//ZGL_DEFS_H
