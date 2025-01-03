﻿/*
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

#include "zglDefs.h"
#include "zglCommands.h"
#include "zglObjects.h"

/*
Well-defined sized internal formats glTexStorage

GL_R8	GL_RED	8
GL_R8_SNORM	GL_RED	s8
GL_R16	GL_RED	16
GL_R16_SNORM	GL_RED	s16
GL_RG8	GL_RG	8	8
GL_RG8_SNORM	GL_RG	s8	s8
GL_RG16	GL_RG	16	16
GL_RG16_SNORM	GL_RG	s16	s16
GL_R3_G3_B2	GL_RGB	3	3	2
GL_RGB4	GL_RGB	4	4	4
GL_RGB5	GL_RGB	5	5	5
GL_RGB8	GL_RGB	8	8	8
GL_RGB8_SNORM	GL_RGB	s8	s8	s8
GL_RGB10	GL_RGB	10	10	10
GL_RGB12	GL_RGB	12	12	12
GL_RGB16_SNORM	GL_RGB	16	16	16
GL_RGBA2	GL_RGB	2	2	2	2
GL_RGBA4	GL_RGB	4	4	4	4
GL_RGB5_A1	GL_RGBA	5	5	5	1
GL_RGBA8	GL_RGBA	8	8	8	8
GL_RGBA8_SNORM	GL_RGBA	s8	s8	s8	s8
GL_RGB10_A2	GL_RGBA	10	10	10	2
GL_RGB10_A2UI	GL_RGBA	ui10	ui10	ui10	ui2
GL_RGBA12	GL_RGBA	12	12	12	12
GL_RGBA16	GL_RGBA	16	16	16	16
GL_SRGB8	GL_RGB	8	8	8
GL_SRGB8_ALPHA8	GL_RGBA	8	8	8	8
GL_R16F	GL_RED	f16
GL_RG16F	GL_RG	f16	f16
GL_RGB16F	GL_RGB	f16	f16	f16
GL_RGBA16F	GL_RGBA	f16	f16	f16	f16
GL_R32F	GL_RED	f32
GL_RG32F	GL_RG	f32	f32
GL_RGB32F	GL_RGB	f32	f32	f32
GL_RGBA32F	GL_RGBA	f32	f32	f32	f32
GL_R11F_G11F_B10F	GL_RGB	f11	f11	f10
GL_RGB9_E5	GL_RGB	9	9	9		5
GL_R8I	GL_RED	i8
GL_R8UI	GL_RED	ui8
GL_R16I	GL_RED	i16
GL_R16UI	GL_RED	ui16
GL_R32I	GL_RED	i32
GL_R32UI	GL_RED	ui32
GL_RG8I	GL_RG	i8	i8
GL_RG8UI	GL_RG	ui8	ui8
GL_RG16I	GL_RG	i16	i16
GL_RG16UI	GL_RG	ui16	ui16
GL_RG32I	GL_RG	i32	i32
GL_RG32UI	GL_RG	ui32	ui32
GL_RGB8I	GL_RGB	i8	i8	i8
GL_RGB8UI	GL_RGB	ui8	ui8	ui8
GL_RGB16I	GL_RGB	i16	i16	i16
GL_RGB16UI	GL_RGB	ui16	ui16	ui16
GL_RGB32I	GL_RGB	i32	i32	i32
GL_RGB32UI	GL_RGB	ui32	ui32	ui32
GL_RGBA8I	GL_RGBA	i8	i8	i8	i8
GL_RGBA8UI	GL_RGBA	ui8	ui8	ui8	ui8
GL_RGBA16I	GL_RGBA	i16	i16	i16	i16
GL_RGBA16UI	GL_RGBA	ui16	ui16	ui16	ui16
GL_RGBA32I	GL_RGBA	i32	i32	i32	i32
GL_RGBA32UI	GL_RGBA	ui32	ui32	ui32	ui32

*/

#if 0
_ZGL void ZglToGlFormat(avxFormat pf, GLint *fmt, GLenum *layout, GLenum *type)
{
    static struct { GLint intFmt; GLenum layout, type; } const v[] =
    {
        {   NIL,                    NIL,                NIL                     },

        {   GL_R8,                  GL_RED,             GL_UNSIGNED_BYTE        },
        {   GL_RG8,                 GL_RG,              GL_UNSIGNED_BYTE        },
        {   GL_RGB8,                GL_RGB,             GL_UNSIGNED_BYTE        },
        {   GL_RGBA8,               GL_RGBA,            GL_UNSIGNED_BYTE        },

        {   GL_RG8,                 GL_RG,              GL_UNSIGNED_BYTE        },
        {   GL_RGB8,                GL_BGR,             GL_UNSIGNED_BYTE        },
        {   GL_RGBA8,               GL_BGRA,            GL_UNSIGNED_BYTE        },

        {   GL_R32F,                GL_RED,             GL_FLOAT                },
        {   GL_RG32F,               GL_RG,              GL_FLOAT                },
        {   GL_RGB32F,              GL_RGB,             GL_FLOAT                },
        {   GL_RGBA32F,             GL_RGBA,            GL_FLOAT                },

        {   GL_SRGB8,               GL_RGB,             GL_UNSIGNED_BYTE            },
        {   GL_SRGB8_ALPHA8,        GL_RGBA,            GL_UNSIGNED_BYTE            },
        {   GL_RGB9_E5,             GL_RGB,             GL_UNSIGNED_INT_5_9_9_9_REV },

        {   GL_RGBA4,               GL_RGBA,            GL_UNSIGNED_SHORT_4_4_4_4   },
        {   GL_RGB5_A1,             GL_RGBA,            GL_UNSIGNED_SHORT_5_5_5_1   },
        {   GL_RGB565,              GL_RGB,             GL_UNSIGNED_SHORT_5_6_5     },
        {   GL_RGB10_A2,            GL_RGBA,            GL_UNSIGNED_INT_10_10_10_2  },

        {   GL_STENCIL_INDEX8,      GL_STENCIL_INDEX,   GL_UNSIGNED_BYTE        },
        {   GL_DEPTH_COMPONENT16,   GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT       },
        {   GL_DEPTH_COMPONENT24,   GL_DEPTH_COMPONENT, GL_UNSIGNED_INT         },
        {   GL_DEPTH_COMPONENT32,   GL_DEPTH_COMPONENT, GL_UNSIGNED_INT         },
        {   GL_DEPTH_COMPONENT32F,  GL_DEPTH_COMPONENT, GL_FLOAT                },
        {   GL_DEPTH24_STENCIL8,    GL_DEPTH_STENCIL,   GL_UNSIGNED_INT_24_8    },
        {   GL_DEPTH32F_STENCIL8,   GL_DEPTH_STENCIL,   GL_UNSIGNED_INT_24_8    },

        {  GL_COMPRESSED_RGB_S3TC_DXT1_EXT , GL_COMPRESSED_RGB_S3TC_DXT1_EXT , GL_COMPRESSED_RGB_S3TC_DXT1_EXT  },
        {  GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT },
        {  GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT },
        {  GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT },
    };

    afxError err = NIL;
    AFX_ASSERT(v[avxFormat_D32F].intFmt == GL_DEPTH_COMPONENT32F);
    AFX_ASSERT(v[avxFormat_D32].intFmt == GL_DEPTH_COMPONENT32);
    AFX_ASSERT(v[avxFormat_D24].intFmt == GL_DEPTH_COMPONENT24);
    AFX_ASSERT(v[avxFormat_D24S8].intFmt == GL_DEPTH24_STENCIL8);
    AFX_ASSERT(v[avxFormat_D32FS8].intFmt == GL_DEPTH32F_STENCIL8);

    *fmt = v[pf].intFmt;
    *layout = v[pf].layout;
    *type = v[pf].type;
}
#endif

static struct { GLint intFmt; GLenum layout, type; } vf[avxFormat_TOTAL] = { 0 };
#define MAKE_FMT(fmt, glFmt, glLayout, glType) vf[fmt].intFmt = glFmt, vf[fmt].layout = glLayout, vf[fmt].type = glType

_ZGL void ZglToGlFormat2(avxFormat pf, GLint *fmt, GLenum *layout, GLenum *type)
{
    static afxBool inited = FALSE;

    if (!inited)
    {
        inited = TRUE;

        //                              inverse of name
        //                              as a vector
        MAKE_FMT(avxFormat_RG4un,       GL_RGBA4,           GL_RG,      GL_UNSIGNED_SHORT_4_4_4_4);
        MAKE_FMT(avxFormat_RGBA4un,     GL_RGBA4,           GL_RGBA,    GL_UNSIGNED_SHORT_4_4_4_4_REV);
        MAKE_FMT(avxFormat_BGRA4un,     GL_RGBA4,           GL_RGBA,    GL_UNSIGNED_SHORT_4_4_4_4);
        MAKE_FMT(avxFormat_R5G6B5un,    GL_RGB565,          GL_RGB,     GL_UNSIGNED_SHORT_5_6_5_REV);
        MAKE_FMT(avxFormat_B5G6R5un,    GL_RGB565,          GL_RGB,     GL_UNSIGNED_SHORT_5_6_5);
        MAKE_FMT(avxFormat_RGB5A1un,    GL_RGB5_A1,         GL_RGBA,    GL_UNSIGNED_SHORT_5_5_5_1);
        MAKE_FMT(avxFormat_BGR5A1un,    GL_RGB5_A1,         GL_BGRA,    GL_UNSIGNED_SHORT_1_5_5_5_REV);
        MAKE_FMT(avxFormat_A1RGB5un,    GL_RGB5_A1,         GL_RGBA,    GL_UNSIGNED_SHORT_5_5_5_1);
    
        MAKE_FMT(avxFormat_R8un,        GL_R8,              GL_RED,     GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_R8sn,        GL_R8_SNORM,        GL_RED,     GL_BYTE);
        MAKE_FMT(avxFormat_R8us,        GL_R8,              GL_RED,     GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_R8ss,        GL_R8,              GL_RED,     GL_BYTE);
        MAKE_FMT(avxFormat_R8u,         GL_R8UI,            GL_RED_INTEGER,GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_R8i,         GL_R8I,             GL_RED_INTEGER,GL_BYTE);
        MAKE_FMT(avxFormat_R8std,     GL_SRGB8,           GL_RED,     GL_UNSIGNED_BYTE);
        
        MAKE_FMT(avxFormat_RG8un,       GL_RG8,             GL_RG,      GL_UNSIGNED_SHORT);
        MAKE_FMT(avxFormat_RG8sn,       GL_RG8_SNORM,       GL_RG,      GL_SHORT);
        MAKE_FMT(avxFormat_RG8us,       GL_RG8,             GL_RG,      GL_UNSIGNED_SHORT);
        MAKE_FMT(avxFormat_RG8ss,       GL_RG8,             GL_RG,      GL_SHORT);
        MAKE_FMT(avxFormat_RG8u,        GL_RG8UI,           GL_RG_INTEGER,GL_UNSIGNED_SHORT);
        MAKE_FMT(avxFormat_RG8i,        GL_RG8I,            GL_RG_INTEGER,GL_SHORT);
        MAKE_FMT(avxFormat_RG8std,    GL_SRGB8,           GL_RG,      GL_UNSIGNED_SHORT);
        
        MAKE_FMT(avxFormat_RGB8un,      GL_RGB8,            GL_RGB,     GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_RGB8sn,      GL_RGB8_SNORM,      GL_RGB,     GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_RGB8us,      GL_RGB8,            GL_RGB,     GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_RGB8ss,      GL_RGB8,            GL_RGB,     GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_RGB8u,       GL_RGB8I,           GL_RGB_INTEGER, GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_RGB8i,       GL_RGB8UI,          GL_RGB_INTEGER, GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_RGB8std,   GL_SRGB8,           GL_RGB,     GL_UNSIGNED_BYTE);
        
        MAKE_FMT(avxFormat_BGR8un,      GL_RGB8,            GL_RGB,     GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_BGR8sn,      GL_RGB8_SNORM,      GL_RGB,     GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_BGR8us,      GL_RGB8,            GL_RGB,     GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_BGR8ss,      GL_RGB8,            GL_RGB,     GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_BGR8u,       GL_RGB8UI,          GL_RGB_INTEGER, GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_BGR8i,       GL_RGB8I,           GL_RGB_INTEGER, GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_BGR8std,   GL_SRGB8,           GL_RGBA,    GL_UNSIGNED_BYTE);
        
        MAKE_FMT(avxFormat_RGBA8un,     GL_RGBA8,           GL_RGBA,    GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_RGBA8sn,     GL_RGBA8_SNORM,     GL_RGBA,    GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_RGBA8us,     GL_RGBA8,           GL_RGBA,    GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_RGBA8ss,     GL_RGBA8,           GL_RGBA,    GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_RGBA8u,      GL_RGBA8UI,         GL_RGBA_INTEGER, GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_RGBA8i,      GL_RGBA8I,          GL_RGBA_INTEGER, GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_RGBA8std,  GL_SRGB8_ALPHA8,    GL_RGBA,    GL_UNSIGNED_BYTE);
        
        // 32-bit float
        MAKE_FMT(avxFormat_R32f,        GL_R32F,            GL_RED,     GL_FLOAT);
        MAKE_FMT(avxFormat_RG32f,       GL_RG32F,           GL_RG,      GL_FLOAT);
        MAKE_FMT(avxFormat_RGB32f,      GL_RGB32F,          GL_RGB,     GL_FLOAT);
        MAKE_FMT(avxFormat_RGBA32f,     GL_RGBA32F,         GL_RGBA,    GL_FLOAT);
        // 32-bit signed int
        MAKE_FMT(avxFormat_R32i,        GL_R32I,            GL_RED,     GL_INT);
        MAKE_FMT(avxFormat_RG32i,       GL_RG32I,           GL_RG,      GL_INT);
        MAKE_FMT(avxFormat_RGB32i,      GL_RGB32I,          GL_RGB,     GL_INT);
        MAKE_FMT(avxFormat_RGBA32i,     GL_RGBA32I,         GL_RGBA,    GL_INT);
        // 32-bit unsigned int
        MAKE_FMT(avxFormat_R32u,        GL_R32UI,           GL_RED,     GL_UNSIGNED_INT);
        MAKE_FMT(avxFormat_RG32u,       GL_RG32UI,          GL_RG,      GL_UNSIGNED_INT);
        MAKE_FMT(avxFormat_RGB32u,      GL_RGB32UI,         GL_RGB,     GL_UNSIGNED_INT);
        MAKE_FMT(avxFormat_RGBA32u,     GL_RGBA32UI,        GL_RGBA,    GL_UNSIGNED_INT);
        // 16-bit signed int
        MAKE_FMT(avxFormat_R16i,        GL_R16I,            GL_RED,     GL_SHORT);
        MAKE_FMT(avxFormat_RG16i,       GL_RG16I,           GL_RG,      GL_SHORT);
        MAKE_FMT(avxFormat_RGB16i,      GL_RGB16I,          GL_RGB,     GL_SHORT);
        MAKE_FMT(avxFormat_RGBA16i,     GL_RGBA16I,         GL_RGBA,    GL_SHORT);
        // 16-bit unsigned int
        MAKE_FMT(avxFormat_R16u,        GL_R16UI,           GL_RED,     GL_UNSIGNED_SHORT);
        MAKE_FMT(avxFormat_RG16u,       GL_RG16UI,          GL_RG,      GL_UNSIGNED_SHORT);
        MAKE_FMT(avxFormat_RGB16u,      GL_RGB16UI,         GL_RGB,     GL_UNSIGNED_SHORT);
        MAKE_FMT(avxFormat_RGBA16u,     GL_RGBA16UI,        GL_RGBA,    GL_UNSIGNED_SHORT);


        MAKE_FMT(avxFormat_BGRA8un,     GL_RGBA8,           GL_RGBA,    GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_BGRA8sn,     GL_RGBA8_SNORM,     GL_RGBA,    GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_BGRA8us,     GL_RGBA8,           GL_RGBA,    GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_BGRA8ss,     GL_RGBA8,           GL_RGBA,    GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_BGRA8u,      GL_RGBA8UI,         GL_RGBA_INTEGER, GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_BGRA8i,      GL_RGBA8I,          GL_RGBA_INTEGER, GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_BGRA8std,  GL_SRGB8_ALPHA8,    GL_RGBA,    GL_UNSIGNED_BYTE);

        MAKE_FMT(avxFormat_ABGR8un,     GL_RGBA8,           GL_RGBA,    GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_ABGR8sn,     GL_RGBA8_SNORM,     GL_RGBA,    GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_ABGR8us,     GL_RGBA8,           GL_RGBA,    GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_ABGR8ss,     GL_RGBA8,           GL_RGBA,    GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_ABGR8u,      GL_RGBA8UI,         GL_RGBA_INTEGER, GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_ABGR8i,      GL_RGBA8I,          GL_RGBA_INTEGER, GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_ABGR8std,  GL_SRGB8_ALPHA8,    GL_RGBA,    GL_UNSIGNED_BYTE);
        
        MAKE_FMT(avxFormat_A2RGB10un,   GL_RGB10_A2,        GL_BGRA,    GL_UNSIGNED_INT_2_10_10_10_REV);
        MAKE_FMT(avxFormat_A2RGB10sn,   GL_RGB10_A2,        GL_BGRA,    GL_UNSIGNED_INT_2_10_10_10_REV);
        MAKE_FMT(avxFormat_A2RGB10us,   GL_RGB10_A2,        GL_BGRA,    GL_UNSIGNED_INT_2_10_10_10_REV);
        MAKE_FMT(avxFormat_A2RGB10ss,   GL_RGB10_A2,        GL_BGRA,    GL_UNSIGNED_INT_2_10_10_10_REV);
        MAKE_FMT(avxFormat_A2RGB10u,    GL_RGB10_A2UI,      GL_BGRA_INTEGER,GL_UNSIGNED_INT_2_10_10_10_REV);
        MAKE_FMT(avxFormat_A2RGB10i,    GL_RGB10_A2,        GL_BGRA_INTEGER,GL_UNSIGNED_INT_2_10_10_10_REV);
        
        MAKE_FMT(avxFormat_A2BGR10un,   GL_RGB10_A2,        GL_RGBA,    GL_UNSIGNED_INT_2_10_10_10_REV);
        MAKE_FMT(avxFormat_A2BGR10sn,   GL_RGB10_A2,        GL_RGBA,    GL_UNSIGNED_INT_2_10_10_10_REV);
        MAKE_FMT(avxFormat_A2BGR10us,   GL_RGB10_A2,        GL_RGBA,    GL_UNSIGNED_INT_2_10_10_10_REV);
        MAKE_FMT(avxFormat_A2BGR10ss,   GL_RGB10_A2,        GL_RGBA,    GL_UNSIGNED_INT_2_10_10_10_REV);
        MAKE_FMT(avxFormat_A2BGR10u,    GL_RGB10_A2UI,      GL_RGBA_INTEGER,GL_UNSIGNED_INT_2_10_10_10_REV);
        MAKE_FMT(avxFormat_A2BGR10i,    GL_RGB10_A2,        GL_RGBA_INTEGER,GL_UNSIGNED_INT_2_10_10_10_REV);
    
        MAKE_FMT(avxFormat_B10GR11uf,   GL_R11F_G11F_B10F,  GL_RGBA,    GL_UNSIGNED_INT_10F_11F_11F_REV);
        MAKE_FMT(avxFormat_E5BGR9uf,    GL_RGB9_E5,         GL_RGBA,    GL_UNSIGNED_INT_5_9_9_9_REV);

        MAKE_FMT(avxFormat_D16un,       GL_DEPTH_COMPONENT16,   GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT);
        MAKE_FMT(avxFormat_X8D24,       GL_DEPTH_COMPONENT24,   GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
        MAKE_FMT(avxFormat_D32f,        GL_DEPTH_COMPONENT32F,  GL_DEPTH_COMPONENT, GL_FLOAT);
        MAKE_FMT(avxFormat_S8u,         GL_STENCIL_INDEX8,      GL_STENCIL_INDEX,   GL_UNSIGNED_BYTE);
        MAKE_FMT(avxFormat_D16S8,       GL_DEPTH24_STENCIL8,    GL_DEPTH_STENCIL,   GL_UNSIGNED_INT);
        MAKE_FMT(avxFormat_D24unS8u,    GL_DEPTH24_STENCIL8,    GL_DEPTH_STENCIL,   GL_UNSIGNED_INT_24_8);
        MAKE_FMT(avxFormat_D32fS8u,     GL_DEPTH32F_STENCIL8,   GL_DEPTH_STENCIL,   GL_FLOAT_32_UNSIGNED_INT_24_8_REV);
        
        MAKE_FMT(avxFormat_DXT1,        GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_NONE);
        MAKE_FMT(avxFormat_DXT1std,   GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, GL_NONE);
        MAKE_FMT(avxFormat_DXT1A,       GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_NONE);
        MAKE_FMT(avxFormat_DXT1Astd,  GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, GL_NONE);
        MAKE_FMT(avxFormat_DXT3,        GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_NONE);
        MAKE_FMT(avxFormat_DXT3std,   GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, GL_NONE);
        MAKE_FMT(avxFormat_DXT5,        GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_NONE);
        MAKE_FMT(avxFormat_DXT5std,   GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_NONE);
        MAKE_FMT(avxFormat_RGTC1,       GL_COMPRESSED_RED_RGTC1, GL_COMPRESSED_RED_RGTC1, GL_NONE);
        MAKE_FMT(avxFormat_RGTC1sn,     GL_COMPRESSED_SIGNED_RED_RGTC1, GL_COMPRESSED_SIGNED_RED_RGTC1, GL_NONE);
        MAKE_FMT(avxFormat_RGTC2,       GL_COMPRESSED_RG_RGTC2, GL_COMPRESSED_RG_RGTC2, GL_NONE);
        MAKE_FMT(avxFormat_RGTC2sn,     GL_COMPRESSED_SIGNED_RG_RGTC2, GL_COMPRESSED_SIGNED_RG_RGTC2, GL_NONE);
    }

    afxError err = NIL;
    AFX_ASSERT(vf[avxFormat_D32f].intFmt == GL_DEPTH_COMPONENT32F);
    //AFX_ASSERT(vf[avxFormat_D32f].intFmt == GL_DEPTH_COMPONENT32);
    AFX_ASSERT(vf[avxFormat_X8D24].intFmt == GL_DEPTH_COMPONENT24);
    AFX_ASSERT(vf[avxFormat_D24S8].intFmt == GL_DEPTH24_STENCIL8);
    AFX_ASSERT(vf[avxFormat_D32S8].intFmt == GL_DEPTH32F_STENCIL8);

    AFX_ASSERT(vf[avxFormat_RGBA4].intFmt == GL_RGBA4);

    *fmt = vf[pf].intFmt;
    *layout = vf[pf].layout;
    *type = vf[pf].type;
}

_ZGL GLenum ZglToGlImageDimensions(afxUnit d)
{
    static GLenum const v[] =
    {
        NIL,
        GL_TEXTURE_1D,
        GL_TEXTURE_2D,
        GL_TEXTURE_3D,
        GL_TEXTURE_CUBE_MAP,

        GL_INVALID_ENUM
    };

    return v[d];
}

_ZGL GLenum ZglToGlTexelFilterMode(avxTexelFilter tfm)
{
    static GLenum const v[] =
    {
        GL_NEAREST,
        GL_LINEAR,

        GL_INVALID_ENUM
    };

    return v[tfm];
}

_ZGL GLenum ZglToGlTexelFilterModeMipmapped(avxTexelFilter min, avxTexelFilter mip)
{
    static GLenum const nearest[] =
    {
        GL_NEAREST_MIPMAP_NEAREST, // - no filtering, sharp switching between mipmaps
        GL_NEAREST_MIPMAP_LINEAR, // - no filtering, smooth transition between mipmaps

        GL_INVALID_ENUM
    };

    static GLenum const linear[] =
    {
        GL_LINEAR_MIPMAP_NEAREST, //- filtering, sharp switching between mipmaps
        GL_LINEAR_MIPMAP_LINEAR,

        GL_INVALID_ENUM
    };

    return min == avxTexelFilter_LINEAR ? linear[mip] : nearest[mip];
}

_ZGL GLenum ZglToGlTexWrapMode(avxTexelAddress twm)
{
    static GLenum const v[] =
    {
        GL_REPEAT,
        GL_MIRRORED_REPEAT,
        GL_CLAMP_TO_EDGE,
        GL_CLAMP_TO_BORDER,
        GL_MIRROR_CLAMP_TO_EDGE,

        GL_INVALID_ENUM
    };

    return v[twm];
}

_ZGL GLenum ZglToGlColorSwizzle(avxColorSwizzle swizzle)
{
    static GLenum const v[] =
    {
        GL_RED,
        GL_GREEN,
        GL_BLUE,
        GL_ALPHA,
        GL_ZERO,
        GL_ONE,

        GL_INVALID_ENUM
    };
    return v[swizzle];
}

_ZGL void ZglToGlColorSwizzling(avxColorSwizzling const *swizzling, GLenum *arrayedSwizzles)
{
    arrayedSwizzles[0] = ZglToGlColorSwizzle(swizzling->r);
    arrayedSwizzles[1] = ZglToGlColorSwizzle(swizzling->g);
    arrayedSwizzles[2] = ZglToGlColorSwizzle(swizzling->b);
    arrayedSwizzles[3] = ZglToGlColorSwizzle(swizzling->a);
}

_ZGL void ZglDetermineGlTargetInternalFormatType(afxRaster ras, GLenum *target, GLint *intFmt, GLenum *fmt, GLenum *type)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT(target);
    AFX_ASSERT(intFmt);
    AFX_ASSERT(fmt);
    AFX_ASSERT(type);
    afxResult cubemap = AfxTestRasterFlags(ras, afxRasterFlag_CUBEMAP);
    AFX_ASSERT(ras->m.extent.w); // always have at least one dimension.
    
    if (AfxTestRasterFlags(ras, afxRasterFlag_1D)) // Y
    {
        if (ras->m.extent.d > 1)
        {
            *target = GL_TEXTURE_1D_ARRAY;
        }
        else
        {
            *target = GL_TEXTURE_1D;
        }
    }
    else
    {
        // 2D or 2D array?

        if (!AfxTestRasterFlags(ras, afxRasterFlag_3D))
        {
            if (cubemap)
            {
                if (ras->m.extent.d > 6)
                {
                    *target = GL_TEXTURE_CUBE_MAP_ARRAY;
                }
                else
                {
                    *target = GL_TEXTURE_CUBE_MAP;
                }
            }
            else
            {
                if (ras->m.extent.d > 1)
                {
                    *target = GL_TEXTURE_2D_ARRAY;
                }
                else
                {
                    *target = GL_TEXTURE_2D;
                }
            }
        }
        else
        {
            *target = GL_TEXTURE_3D;
        }
    }

    ZglToGlFormat2(ras->m.fmt, intFmt, fmt, type);
}

_ZGL GLenum AfxToGlFrontFace(avxFrontFace ff)
{
    static GLenum const v[] =
    {
        GL_CCW,
        GL_CW
    };

    return v[ff];
};

_ZGL GLenum AfxToGlCullMode(avxCullMode cm)
{
    afxError err;

    static GLenum const v[] =
    {
        NIL,
        GL_FRONT,
        GL_BACK,
        GL_FRONT_AND_BACK
    };
    AFX_ASSERT(v[avxCullMode_BACK] == GL_BACK);
    return v[cm];
};

_ZGL GLenum AfxToGlFillMode(avxFillMode pm)
{
    static GLenum const v[] =
    {
        GL_FILL,
        GL_LINE,
        GL_POINT
    };

    return v[pm];
};

_ZGL GLenum ZglToGlCompareOp(avxCompareOp co)
{
    static GLenum const v[] =
    {
        GL_NEVER,
        GL_LESS,
        GL_LEQUAL,
        GL_EQUAL,
        GL_NOTEQUAL,
        GL_GEQUAL,
        GL_GREATER,
        GL_ALWAYS
    };

    return v[co];
};

_ZGL GLuint AfxToGlShaderStage(avxShaderStage s)
{
    static GLuint const v[] =
    {
        NIL,
        GL_VERTEX_SHADER,
        GL_FRAGMENT_SHADER,
        GL_GEOMETRY_SHADER,
        GL_TESS_CONTROL_SHADER,
        GL_TESS_EVALUATION_SHADER,
        GL_COMPUTE_SHADER,
        NIL
    };

    return v[s];
};

_ZGL GLuint AfxToGlShaderStageBit(avxShaderStage s)
{
    static GLuint const v[] =
    {
        NIL,
        GL_VERTEX_SHADER_BIT,
        GL_FRAGMENT_SHADER_BIT,
        GL_GEOMETRY_SHADER_BIT,
        GL_TESS_CONTROL_SHADER_BIT,
        GL_TESS_EVALUATION_SHADER_BIT,
        GL_COMPUTE_SHADER_BIT,
        NIL
    };

    return v[s];
};

_ZGL GLenum AfxToGlTopology(avxTopology pm)
{
    static GLuint const v[] =
    {
        GL_POINTS,

        GL_LINES,
        GL_LINE_STRIP,

        GL_TRIANGLES,
        GL_TRIANGLE_STRIP,
        GL_TRIANGLE_FAN,

        GL_LINES_ADJACENCY,
        GL_LINE_STRIP_ADJACENCY,

        GL_TRIANGLES_ADJACENCY,
        GL_TRIANGLE_STRIP_ADJACENCY,

        GL_PATCHES
    };

    return v[pm];
};

_ZGL GLenum AfxToGlBufferUsage(afxBufferUsage usage)
{
    afxError err;
    AfxThrowError();
    static GLenum const v[] =
    {
        GL_COPY_READ_BUFFER,
        GL_COPY_WRITE_BUFFER,
        GL_PIXEL_PACK_BUFFER,
        GL_PIXEL_UNPACK_BUFFER,
        GL_UNIFORM_BUFFER,
        GL_SHADER_STORAGE_BUFFER,
        GL_ARRAY_BUFFER,
        GL_ELEMENT_ARRAY_BUFFER,
        GL_DRAW_INDIRECT_BUFFER
    };
    //errado
    return v[usage];
};

_ZGL GLenum AfxToGlStencilOp(avxStencilOp so)
{
    static GLenum const v[] =
    {
        GL_KEEP,
        GL_ZERO,
        GL_REPLACE,
        GL_INCR,
        GL_DECR,
        GL_INVERT,
        GL_INCR_WRAP,
        GL_DECR_WRAP
    };

    return v[so];
}

_ZGL GLenum AfxToGlBlendOp(avxBlendOp f)
{
    static GLenum const v[] =
    {
        GL_FUNC_ADD,
        GL_FUNC_SUBTRACT,
        GL_FUNC_REVERSE_SUBTRACT,
        GL_MIN,
        GL_MAX
    };

    return v[f];
}

_ZGL GLenum AfxToGlLogicOp(avxLogicOp f)
{
    afxError err;
    static GLenum const v[] =
    {
        GL_NOOP,
        GL_CLEAR,
        GL_SET,
        GL_COPY,
        GL_COPY_INVERTED,
        GL_INVERT,
        GL_AND,
        GL_NAND,
        GL_OR,
        GL_NOR,
        GL_XOR,
        GL_EQUIV,
        GL_AND_REVERSE,
        GL_AND_INVERTED,
        GL_OR_REVERSE,
        GL_OR_INVERTED
    };
    AFX_ASSERT(v[avxLogicOp_NOP] == GL_NOOP);
    return v[f];
}

_ZGL GLenum AfxToGlBlendFactor(avxBlendFactor f)
{
    static GLenum const v[] =
    {
        GL_ZERO,
        GL_ONE,
        GL_SRC_COLOR,
        GL_ONE_MINUS_SRC_COLOR,
        GL_DST_COLOR,
        GL_ONE_MINUS_DST_COLOR,
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA,
        GL_DST_ALPHA,
        GL_ONE_MINUS_DST_ALPHA,
        GL_CONSTANT_COLOR,
        GL_ONE_MINUS_CONSTANT_COLOR,
        GL_CONSTANT_ALPHA,
        GL_ONE_MINUS_CONSTANT_ALPHA,
        GL_SRC_ALPHA_SATURATE,
        GL_SRC1_COLOR,
        GL_ONE_MINUS_SRC1_COLOR,
        GL_SRC1_ALPHA,
        GL_ONE_MINUS_SRC1_ALPHA
    };

    return v[f];
}

#if 0
_ZGL void AfxToGlVertexFormat(afxVertexFormat fmt, GLint* siz, GLenum* typ, afxUnit32* stride)
{
    struct
    {
        GLint size;
        GLenum type;
        afxUnit stride;
    } fds[afxVertexFormat_TOTAL] =
    {
        [afxVertexFormat_R32] = { 1, GL_FLOAT, sizeof(afxReal32) },
        [afxVertexFormat_R32x2] = { 2, GL_FLOAT, sizeof(afxReal32) * 2 },
        [afxVertexFormat_R32x3] = { 3, GL_FLOAT, sizeof(afxReal32) * 3 },
        [afxVertexFormat_R32x4] = { 4, GL_FLOAT, sizeof(afxReal32) * 4 },

        [afxVertexFormat_R16] = { 1, GL_HALF_FLOAT, sizeof(afxReal16) },
        [afxVertexFormat_R16x2] = { 2, GL_HALF_FLOAT, sizeof(afxReal16) * 2 },
        [afxVertexFormat_R16x4] = { 4, GL_HALF_FLOAT, sizeof(afxReal16) * 4 },

        [afxVertexFormat_U32] = { 1, GL_UNSIGNED_INT, sizeof(afxUnit32) },
        [afxVertexFormat_U32x2] = { 2, GL_UNSIGNED_INT, sizeof(afxUnit32) * 2 },
        [afxVertexFormat_U32x4] = { 4, GL_UNSIGNED_INT, sizeof(afxUnit32) * 4 },

        [afxVertexFormat_U16] = { 1, GL_UNSIGNED_SHORT, sizeof(afxN16) },
        [afxVertexFormat_U16x2] = { 2, GL_UNSIGNED_SHORT, sizeof(afxN16) * 2 },
        [afxVertexFormat_U16x4] = { 4, GL_UNSIGNED_SHORT, sizeof(afxN16) * 4 },

        [afxVertexFormat_U8] = { 1, GL_UNSIGNED_BYTE, sizeof(afxUnit8) },
        [afxVertexFormat_U8x2] = { 2, GL_UNSIGNED_BYTE, sizeof(afxUnit8) * 2 },
        [afxVertexFormat_U8x4] = { 4, GL_UNSIGNED_BYTE, sizeof(afxUnit8) * 4 },

        [afxVertexFormat_I32] = { 1, GL_INT, sizeof(afxInt32) },
        [afxVertexFormat_I32x2] = { 2, GL_INT, sizeof(afxInt32) * 2 },
        [afxVertexFormat_I32x4] = { 4, GL_INT, sizeof(afxInt32) * 4 },

        [afxVertexFormat_I16] = { 1, GL_SHORT, sizeof(afxInt16) },
        [afxVertexFormat_I16x2] = { 2, GL_SHORT, sizeof(afxInt16) * 2 },
        [afxVertexFormat_I16x4] = { 4, GL_SHORT, sizeof(afxInt16) * 4 },

        [afxVertexFormat_I8] = { 1, GL_BYTE, sizeof(afxInt8) },
        [afxVertexFormat_I8x2] = { 2, GL_BYTE, sizeof(afxInt8) * 2 },
        [afxVertexFormat_I8x4] = { 4, GL_BYTE, sizeof(afxInt8) * 4 },
    };

    if (siz)
        *siz = fds[fmt].size;

    if (typ)
        *typ = fds[fmt].type;

    if (stride)
        *stride = fds[fmt].stride;

    //AFX_STATIC_ASSERT(afxVertexFormat_TOTAL == 5, "");

}
#endif
