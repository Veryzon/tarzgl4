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
#include "zglDdrv.h"
#include "qwadro/exec/afxSystem.h"

ZGLINL void ZglBindTextureUnit(zglDpu* dpu, GLuint unit, GLenum target, GLuint texture)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

    if (gl->BindTextureUnit)
    {
        gl->BindTextureUnit(unit, texture);
    }
    else
    {
        gl->ActiveTexture(GL_TEXTURE0 + unit); _ZglThrowErrorOccuried();
        gl->BindTexture(target, texture); _ZglThrowErrorOccuried();
    }
}

ZGLINL void ZglUsePipeline(zglDpu* dpu, GLuint program)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    gl->UseProgram(program); _ZglThrowErrorOccuried();
}

ZGLINL void ZglBindFramebuffer(zglDpu* dpu, GLenum target, GLuint fbo)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    gl->BindFramebuffer(target, fbo); _ZglThrowErrorOccuried();
}
