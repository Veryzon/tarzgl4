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
// SAMPLER                                                                    //
////////////////////////////////////////////////////////////////////////////////

_ZGL afxError _DpuBindAndSyncSamp(zglDpu* dpu, afxUnit glUnit, avxSampler samp)
{
    //AfxEntry("smp=%p", smp);
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    if (!samp)
    {
        gl->BindSampler(glUnit, 0); _ZglThrowErrorOccuried();
    }
    else
    {
        AFX_ASSERT_OBJECTS(afxFcc_SAMP, 1, &samp);
        zglUpdateFlags devUpdReq = (samp->updFlags & ZGL_UPD_FLAG_DEVICE);
        GLuint glHandle = samp->glHandle;
        afxBool bound = FALSE;

        if ((!glHandle) || (devUpdReq & ZGL_UPD_FLAG_DEVICE_INST))
        {
            if (glHandle)
            {
                gl->DeleteBuffers(1, &(glHandle)); _ZglThrowErrorOccuried();
                samp->glHandle = NIL;
                glHandle = NIL;
            }

            gl->GenSamplers(1, &(glHandle)); _ZglThrowErrorOccuried();
            gl->BindSampler(glUnit, glHandle); _ZglThrowErrorOccuried();
            AFX_ASSERT(gl->IsSampler(glHandle));
            samp->glHandle = glHandle;
            bound = TRUE;

            GLenum magF = ZglToGlTexelFilterMode(samp->m.cfg.base.magnify);
            GLenum minF = ZglToGlTexelFilterModeMipmapped(samp->m.cfg.base.minify, samp->m.cfg.base.mipify);
            GLint wrapS = ZglToGlTexWrapMode(samp->m.cfg.base.uvw[0]);
            GLint wrapT = ZglToGlTexWrapMode(samp->m.cfg.base.uvw[1]);
            GLint wrapR = ZglToGlTexWrapMode(samp->m.cfg.base.uvw[2]);
            GLint cop = ZglToGlCompareOp(samp->m.cfg.base.compareOp);

            gl->SamplerParameteri(glHandle, GL_TEXTURE_MAG_FILTER, magF); _ZglThrowErrorOccuried();
            gl->SamplerParameteri(glHandle, GL_TEXTURE_MIN_FILTER, minF); _ZglThrowErrorOccuried();

            gl->SamplerParameteri(glHandle, GL_TEXTURE_WRAP_S, wrapS); _ZglThrowErrorOccuried();
            gl->SamplerParameteri(glHandle, GL_TEXTURE_WRAP_T, wrapT); _ZglThrowErrorOccuried();
            gl->SamplerParameteri(glHandle, GL_TEXTURE_WRAP_R, wrapR); _ZglThrowErrorOccuried();

            if (samp->m.cfg.base.anisotropyEnabled)
            {
                gl->SamplerParameterf(glHandle, GL_TEXTURE_MAX_ANISOTROPY, samp->m.cfg.base.anisotropyMaxDegree); _ZglThrowErrorOccuried();
            }
            else
            {
                //gl->SamplerParameterf(smp->glHandle, GL_TEXTURE_MAX_ANISOTROPY, 0); _ZglThrowErrorOccuried();
            }

            gl->SamplerParameterf(glHandle, GL_TEXTURE_LOD_BIAS, samp->m.cfg.base.lodBias); _ZglThrowErrorOccuried();
            gl->SamplerParameterf(glHandle, GL_TEXTURE_MIN_LOD, samp->m.cfg.base.minLod); _ZglThrowErrorOccuried();
            gl->SamplerParameterf(glHandle, GL_TEXTURE_MAX_LOD, samp->m.cfg.base.maxLod); _ZglThrowErrorOccuried();

            if (samp->m.cfg.base.compareEnabled)
            {
                // what about GL_TEXTURE_COMPARE_MODE?
            }

            gl->SamplerParameteri(glHandle, GL_TEXTURE_COMPARE_MODE, GL_NONE); _ZglThrowErrorOccuried();
            gl->SamplerParameteri(glHandle, GL_TEXTURE_COMPARE_FUNC, cop); _ZglThrowErrorOccuried();
            gl->SamplerParameterfv(glHandle, GL_TEXTURE_BORDER_COLOR, (void*)samp->m.cfg.base.borderColor); _ZglThrowErrorOccuried();

            AfxLogEcho("Hardware-side sampler %p ready.", samp);
            samp->updFlags &= ~(ZGL_UPD_FLAG_DEVICE);
        }
        else
        {
            gl->BindSampler(glUnit, glHandle); _ZglThrowErrorOccuried();
        }
    }
    return err;
}

_ZGL afxError _ZglSampDtor(avxSampler samp)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_SAMP, 1, &samp);

    if (samp->glHandle)
    {
        afxDrawSystem dsys = AfxGetProvider(samp);
        _ZglDsysEnqueueDeletion(dsys, 0, GL_SAMPLER, (afxSize)samp->glHandle);
        samp->glHandle = 0;
    }

    if (_AVX_SAMP_CLASS_CONFIG.dtor(samp))
        AfxThrowError();

    return err;
}

_ZGL afxError _ZglSampCtor(avxSampler samp, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_SAMP, 1, &samp);

    if (_AVX_SAMP_CLASS_CONFIG.ctor(samp, args, invokeNo)) AfxThrowError();
    else
    {
        samp->glHandle = 0;
        samp->updFlags = ZGL_UPD_FLAG_DEVICE_INST;

        if (err && _AVX_SAMP_CLASS_CONFIG.dtor(samp))
            AfxThrowError();
    }
    return err;
}
