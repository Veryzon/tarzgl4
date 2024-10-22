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

#ifndef AVX_TO_GL_H
#define AVX_TO_GL_H

#include "zglDefs.h"

ZGL GLenum AfxToGlFrontFace(avxFrontFace ff);
ZGL GLenum AfxToGlCullMode(avxCullMode cm);
ZGL GLenum AfxToGlFillMode(avxFillMode pm);
ZGL GLenum SglToGlCompareOp(avxCompareOp co);
ZGL GLuint AfxToGlShaderStage(avxShaderStage s);
ZGL GLuint AfxToGlShaderStageBit(avxShaderStage s);
ZGL GLenum AfxToGlTopology(avxTopology pm);
ZGL GLenum AfxToGlBufferUsage(afxInt bt);
ZGL GLenum AfxToGlStencilOp(avxStencilOp so);
ZGL GLenum AfxToGlBlendOp(avxBlendOp f);
ZGL GLenum AfxToGlBlendFactor(avxBlendFactor f);
ZGL void AfxToGlVertexFormat(afxVertexFormat fmt, GLint* siz, GLenum* typ, afxUnit32* stride);
ZGL GLenum AfxToGlLogicOp(avxLogicOp f);
//ZGL void SglToGlFormat(avxFormat pf, GLint *fmt, GLenum *layout, GLenum *type);
ZGL GLenum SglToGlImageDimensions(afxUnit d);
ZGL GLenum SglToGlTexelFilterMode(avxTexelFilter tfm);
ZGL GLenum SglToGlTexelFilterModeMipmapped(avxTexelFilter min, avxTexelFilter mip);
ZGL GLenum SglToGlTexWrapMode(avxTexelAddress twm);
ZGL GLenum SglToGlColorSwizzle(avxColorSwizzle swizzle);
ZGL void SglToGlColorSwizzling(avxColorSwizzling const *swizzling, GLenum *arrayedSwizzles);
ZGL void SglDetermineGlTargetInternalFormatType(afxRaster tex, GLenum *target, GLint *intFmt, GLenum *fmt, GLenum *type);

ZGL void SglToGlFormat2(avxFormat pf, GLint *fmt, GLenum *layout, GLenum *type);

#endif//AVX_TO_GL_H
