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

#include "zglUtils.h"
#include "zglCommands.h"
#include "zglObjects.h"


////////////////////////////////////////////////////////////////////////////////
// SHADER                                                                     //
////////////////////////////////////////////////////////////////////////////////

_ZGL afxError _DpuSyncShd(zglDpu* dpu, avxShader shd, avxShaderType stage, glVmt const* gl)
{
    //AfxEntry("shd=%p", shd);
    afxError err = AFX_ERR_NONE;
    (void)dpu;
    AFX_ASSERT_OBJECTS(afxFcc_SHD, 1, &shd);

    if ((shd->updFlags & ZGL_UPD_FLAG_DEVICE))
    {
        if (!shd->glHandle || (shd->updFlags & ZGL_UPD_FLAG_DEVICE_INST))
        {
            if (shd->glHandle)
            {
                gl->DeleteShader(shd->glHandle); _ZglThrowErrorOccuried();
                shd->glHandle = NIL;
            }

            AFX_ASSERT(NIL == shd->glHandle);

            if (!(shd->glHandle = gl->CreateShader(AfxToGlShaderStage(stage))))
            {
                AfxThrowError();
                _ZglThrowErrorOccuried();
            }
            else
            {
                AFX_ASSERT(shd);
                AFX_ASSERT(gl->IsShader(shd->glHandle));
                const GLint codeLens[] = { shd->m.codeLen };
                GLchar const* const codes[] = { (GLchar const*)shd->m.code };
                gl->ShaderSource(shd->glHandle, 1, codes, codeLens); _ZglThrowErrorOccuried();
                gl->CompileShader(shd->glHandle); _ZglThrowErrorOccuried();
                GLint status = 0;
                gl->GetShaderiv(shd->glHandle, GL_COMPILE_STATUS, &status); _ZglThrowErrorOccuried();

                if (status == GL_FALSE)
                {
                    AfxThrowError();
                    afxChar str[1024];
                    gl->GetShaderInfoLog(shd->glHandle, sizeof(str), NIL, (GLchar*)str); _ZglThrowErrorOccuried();
                    AfxReportError(str);
                    gl->DeleteShader(shd->glHandle); _ZglThrowErrorOccuried();
                    shd->glHandle = NIL;
                }
                else
                {
                    shd->glProgHandle = 0;
                    shd->compiled = TRUE;
                    shd->updFlags &= ~(ZGL_UPD_FLAG_DEVICE_INST | ZGL_UPD_FLAG_DEVICE_FLUSH);
                    AfxReportMessage("avxShader %p hardware-side data instanced.", shd);
                }
            }
        }
        else if ((shd->updFlags & ZGL_UPD_FLAG_DEVICE_FLUSH))
        {
            AFX_ASSERT(shd->glHandle);
            
            AfxThrowError(); // can't be modified
        }
    }
    return err;
}

_ZGL afxError _ZglShdDtor(avxShader shd)
{
    afxError err = AFX_ERR_NONE;

    afxDrawSystem dsys = AfxGetProvider(shd);
    
    if (shd->glHandle)
    {
        _ZglDsysEnqueueDeletion(dsys, 0, GL_SHADER, (afxSize)shd->glHandle);
        shd->glHandle = 0;
    }

    if (shd->glProgHandle)
    {
        _ZglDsysEnqueueDeletion(dsys, 0, GL_PROGRAM, (afxSize)shd->glProgHandle);
        shd->glProgHandle = 0;
    }

    if (_AVX_SHD_CLASS_CONFIG.dtor(shd))
        AfxThrowError();

    return err;
}

_ZGL afxError _ZglShdCtor(avxShader shd, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_SHD, 1, &shd);

    if (_AVX_SHD_CLASS_CONFIG.ctor(shd, args, invokeNo)) AfxThrowError();
    else
    {
        shd->glHandle = NIL;
        shd->glProgHandle = NIL;
        shd->updFlags = ZGL_UPD_FLAG_DEVICE_INST;
        shd->compiled = FALSE;

        afxDrawSystem dsys = AfxGetProvider(shd);
        shd->shdUniqueId = ++dsys->shdUniqueId;

        if (err && _AVX_SHD_CLASS_CONFIG.dtor(shd))
            AfxThrowError();
    }
    AFX_ASSERT_OBJECTS(afxFcc_SHD, 1, &shd);
    return err;
}
