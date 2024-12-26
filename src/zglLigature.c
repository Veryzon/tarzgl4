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


#if 0
_ZGL afxError _ZglDqueBindAndSyncLigaSub(afxDrawBridge dexu, afxUnit unit, avxLigature liga, avxLigature legt2)
{
    afxError err = AFX_ERR_NONE;
    avxLigature liga = AfxLegoGetTemplate(liga);
    AfxAssertObject(liga, afxFcc_LIGA);
    AFX_ASSERT(liga->m.entryCnt >= legt2->entryCnt);

    for (afxUnit j = 0; j < liga->m.entryCnt; j++)
    {
        avxLigatureEntry const *entry = &liga->m.entry[j];
        avxLigatureEntry const *entry2 = &legt2->entry[j];
        AFX_ASSERT(entry->binding == entry2->binding);
        AFX_ASSERT(entry->visibility == entry2->visibility);
        AFX_ASSERT(entry->type == entry2->type);

        avxPipelineRigData const *data = &liga->m.data[j];

        afxUnit setId = (unit * _ZGL_MAX_ENTRY_PER_LEGO);
        afxUnit binding = setId + entry->binding;

        switch (entry->type)
        {
        case avxShaderParam_SAMPLER_UNIT:
        {
            AfxAssertObject(data->smp, afxFcc_SAMP);
            _ZglDqueBindAndSyncSmp(dexu, binding, data->smp);
            break;
        }
        case avxShaderParam_IMAGE_UNIT:
        {
            AfxAssertObject(data->tex, afxFcc_RAS);
            _ZglDqueBindAndSyncTex(dexu, binding, data->tex);
            break;
        }
        case avxShaderParam_COMBINED_IMAGE_SAMPLER:
        {
            AfxAssertObject(data->tex, afxFcc_RAS);
            _ZglDqueBindAndSyncTex(dexu, binding, data->tex);

            AfxAssertObject(data->smp, afxFcc_SAMP);
            _ZglDqueBindAndSyncSmp(dexu, binding, data->smp);
#if 0
            afxUri128 uri;
            AfxMakeUri128(&uri, NIL);
            AfxPrintRaster(point->resource.img.tex, AfxFormatUri(&uri.uri, "system/tex-%u-%u.tga", i, entry->binding));
#endif
            break;
        }
        case avxShaderParam_UNIFORM_UNIT:
        {
            AfxAssertObject(data->buf, afxFcc_BUF);

            // https://stackoverflow.com/questions/44629165/bind-multiple-uniform-buffer-objects

            //loc = gl->GetUniformBlockIndex(dexu->state.pip->gpuHandle[dexu->queueIdx], entry->name.buf); _ZglThrowErrorOccuried();
            //gl->UniformBlockBinding(dexu->state.pip->gpuHandle[dexu->queueIdx], loc, ((i * _ZGL_MAX_ENTRY_PER_LEGO) + entry->binding));
            _ZglDqueBindAndSyncBuf(dexu, binding, data->buf, data->offset, data->range, GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW);
            //gl->BindBufferRange(GL_UNIFORM_BUFFER, binding, point->resource.data.buf->gpuHandle, point->resource.data.base, point->resource.data.range); _ZglThrowErrorOccuried();

            break;
        }
        default:
        {
            AfxLogError("");
        }
        }
    }
    return err;
}

_ZGL afxError _ZglDqueBindAndSyncLiga(afxDrawBridge dexu, afxUnit unit, avxLigature liga)
{
    //AfxEntry("pip=%p", pip);
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_LIGA, 1, &liga);
    avxLigature liga = AfxLegoGetTemplate(liga);
    AfxAssertObject(liga, afxFcc_LIGA);
    glVmt const* gl = &dexu->wglVmt;
    
    if (dexu->state.pip)
    {
        avxLigature legt2;
        AfxPipelineRigEnumerateTemplates(AfxPipelineGetRig(dexu->state.pip), unit, 1, &legt2);

        if (_ZglDqueBindAndSyncLegoSub(dexu, unit, liga, legt2))
            AfxThrowError();
    }
    else
    {
        afxUnit shdCnt;
        avxShader shd;
        shdCnt = dexu->state.shdCnt;

        for (afxUnit i = 0; i < shdCnt; i++)
        {
            shd = dexu->state.shd[i];
            avxLigature legt2 = shd->liga[unit];

            if (_ZglDqueBindAndSyncLegoSub(dexu, unit, liga, legt2))
                AfxThrowError();
        }
    }
    return err;
}
#endif 

_ZGL afxError _DpuBindAndResolveLiga(zglDpu* dpu, avxLigature liga, GLuint glHandle)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_LIGA, 1, &liga);
    glVmt const* gl = &dpu->gl;
    AFX_ASSERT(glHandle);
    
    afxString32 tmp;
    AfxMakeString32(&tmp, 0);
    afxChar const *rawName = (void const *)AfxGetStringStorage(&tmp.str, 0);

    if (liga->m.pushables)
    {
        // https://stackoverflow.com/questions/44629165/bind-multiple-uniform-buffer-objects

        GLuint unifBlckIdx = gl->GetUniformBlockIndex(glHandle, "pushable"); _ZglThrowErrorOccuried();

        if (unifBlckIdx != GL_INVALID_INDEX)
        {
            gl->UniformBlockBinding(glHandle, unifBlckIdx, dpu->pushConstUboIdx); _ZglThrowErrorOccuried();
        }
        else
        {
            //AfxThrowError();
            AfxLogError("Uniform buffer unit '%s' not found in ligature %p.", "pushable", liga);
        }
    }

    afxUnit setCnt = liga->m.setCnt;

    for (afxUnit i = 0; i < setCnt; i++)
    {
        afxUnit set = liga->m.sets[i].set;
        afxUnit entryCnt = liga->m.sets[i].entryCnt;
        afxUnit baseEntry = liga->m.sets[i].baseEntry;

        for (afxUnit j = 0; j < entryCnt; j++)
        {
            avxLigatureEntry const *entry = &liga->m.totalEntries[baseEntry + j];
            AFX_ASSERT(!AfxIsStringEmpty(&entry->name.str));
            AfxCopyString(&tmp.str, &entry->name.str);
            //AFX_ASSERT(entry->visibility);
            //AFX_ASSERT(entry->cnt);

            afxUnit glBinding = (set * _ZGL_MAX_ENTRY_PER_LEGO) + entry->binding;
            afxUnit loc;

            AFX_ASSERT(entry->type);
            switch (entry->type)
            {
            case avxShaderParam_SAMPLER_UNIT:
            {
                loc = gl->GetUniformLocation(glHandle, rawName); _ZglThrowErrorOccuried();

                if (loc != GL_INVALID_INDEX)
                {
                    gl->Uniform1i(loc, glBinding); _ZglThrowErrorOccuried();
                }
                else
                {
                    //AfxThrowError();
                    AfxLogError("Sampler unit '%s' not found in ligature %p.", rawName, liga);
                }
                break;
            }
            case avxShaderParam_IMAGE_UNIT:
            {
                loc = gl->GetUniformLocation(glHandle, rawName); _ZglThrowErrorOccuried();

                if (loc != GL_INVALID_INDEX)
                {
                    gl->Uniform1i(loc, glBinding); _ZglThrowErrorOccuried();
                }
                else
                {
                    //AfxThrowError();
                    AfxLogError("Raster unit '%s' not found in ligature %p.", rawName, liga);
                }
                break;
            }
            case avxShaderParam_COMBINED_IMAGE_SAMPLER:
            {
                loc = gl->GetUniformLocation(glHandle, rawName); _ZglThrowErrorOccuried();

                if (loc != GL_INVALID_INDEX)
                {
                    gl->Uniform1i(loc, glBinding); _ZglThrowErrorOccuried();
                }
                else
                {
                    //AfxThrowError();
                    AfxLogError("Combined raster/sampler unit '%s' not found in ligature %p.", rawName, liga);
                }
                break;
            }
            case avxShaderParam_UNIFORM_UNIT:
            {
                // https://stackoverflow.com/questions/44629165/bind-multiple-uniform-buffer-objects

                GLuint unifBlckIdx = gl->GetUniformBlockIndex(glHandle, rawName); _ZglThrowErrorOccuried();

                if (unifBlckIdx != GL_INVALID_INDEX)
                {
                    gl->UniformBlockBinding(glHandle, unifBlckIdx, glBinding); _ZglThrowErrorOccuried();
                }
                else
                {
                    //AfxThrowError();
                    AfxLogError("Uniform buffer unit '%s' not found in ligature %p.", rawName, liga);
                }
                break;
            }
            case avxShaderParam_STORAGE_UNIT:
            {
                GLuint storBlckIdx = gl->GetUniformBlockIndex(glHandle, rawName); _ZglThrowErrorOccuried();

                if (storBlckIdx != GL_INVALID_INDEX)
                {
                    gl->ShaderStorageBlockBinding(glHandle, storBlckIdx, glBinding); _ZglThrowErrorOccuried();
                }
                else
                {
                    //AfxThrowError();
                    AfxLogError("Storage buffer unit '%s' not found in ligature %p.", rawName, liga);
                }
                break;
            }
            case avxShaderParam_FETCH_UNIT:
            {
                loc = gl->GetUniformLocation(glHandle, rawName); _ZglThrowErrorOccuried();

                if (loc != GL_INVALID_INDEX)
                {
                    gl->Uniform1i(loc, glBinding); _ZglThrowErrorOccuried();
                }
                else
                {
                    //AfxThrowError();
                    AfxLogError("Matrix buffer unit '%s' not found in ligature %p.", rawName, liga);
                }
                break;
            }
            default:
            {
                AfxLogError("");
            }
            }
        }
    }
    err = 0;
    return err;
}

_ZGL afxError _ZglLigaDtor(avxLigature liga)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_LIGA, 1, &liga);

    if (_AVX_LIGA_CLASS_CONFIG.dtor(liga))
        AfxThrowError();

    return err;
}

_ZGL afxError _ZglLigaCtor(avxLigature liga, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_LIGA, 1, &liga);

    if (_AVX_LIGA_CLASS_CONFIG.ctor(liga, args, invokeNo)) AfxThrowError();
    else
    {
        if (err && _AVX_LIGA_CLASS_CONFIG.dtor(liga))
            AfxThrowError();
    }
    AFX_ASSERT_OBJECTS(afxFcc_LIGA, 1, &liga);
    return err;
}
