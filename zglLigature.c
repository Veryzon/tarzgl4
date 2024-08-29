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


#include "zglCommands.h"
#include "zglObjects.h"


#if 0
_ZGL afxError _SglDqueBindAndSyncLigaSub(afxDrawBridge dexu, afxNat unit, avxLigature liga, avxLigature legt2)
{
    afxError err = AFX_ERR_NONE;
    avxLigature liga = AfxLegoGetTemplate(liga);
    AfxAssertObject(liga, afxFcc_BSCH);
    AfxAssert(liga->m.entryCnt >= legt2->entryCnt);

    for (afxNat j = 0; j < liga->m.entryCnt; j++)
    {
        avxLigatureEntry const *entry = &liga->m.entry[j];
        avxLigatureEntry const *entry2 = &legt2->entry[j];
        AfxAssert(entry->binding == entry2->binding);
        AfxAssert(entry->visibility == entry2->visibility);
        AfxAssert(entry->type == entry2->type);

        avxPipelineRigData const *data = &liga->m.data[j];

        afxNat setId = (unit * _ZGL_MAX_ENTRY_PER_LEGO);
        afxNat binding = setId + entry->binding;

        switch (entry->type)
        {
        case avxShaderResType_SAMPLER:
        {
            AfxAssertObject(data->smp, afxFcc_SAMP);
            _SglDqueBindAndSyncSmp(dexu, binding, data->smp);
            break;
        }
        case avxShaderResType_SAMPLED_IMAGE:
        {
            AfxAssertObject(data->tex, afxFcc_RAS);
            _SglDqueBindAndSyncTex(dexu, binding, data->tex);
            break;
        }
        case avxShaderResType_COMBINED_IMAGE_SAMPLER:
        {
            AfxAssertObject(data->tex, afxFcc_RAS);
            _SglDqueBindAndSyncTex(dexu, binding, data->tex);

            AfxAssertObject(data->smp, afxFcc_SAMP);
            _SglDqueBindAndSyncSmp(dexu, binding, data->smp);
#if 0
            afxUri128 uri;
            AfxMakeUri128(&uri, NIL);
            AfxPrintRaster(point->resource.img.tex, AfxFormatUri(&uri.uri, "system/tex-%u-%u.tga", i, entry->binding));
#endif
            break;
        }
        case avxShaderResType_CONSTANT_BUFFER:
        {
            AfxAssertObject(data->buf, afxFcc_BUF);

            // https://stackoverflow.com/questions/44629165/bind-multiple-uniform-buffer-objects

            //loc = gl->GetUniformBlockIndex(dexu->state.pip->gpuHandle[dexu->queueIdx], entry->name.buf); _SglThrowErrorOccuried();
            //gl->UniformBlockBinding(dexu->state.pip->gpuHandle[dexu->queueIdx], loc, ((i * _ZGL_MAX_ENTRY_PER_LEGO) + entry->binding));
            _SglDqueBindAndSyncBuf(dexu, binding, data->buf, data->offset, data->range, GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW);
            //gl->BindBufferRange(GL_UNIFORM_BUFFER, binding, point->resource.data.buf->gpuHandle, point->resource.data.base, point->resource.data.range); _SglThrowErrorOccuried();

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

_ZGL afxError _SglDqueBindAndSyncLiga(afxDrawBridge dexu, afxNat unit, avxLigature liga)
{
    //AfxEntry("pip=%p", pip);
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &liga, afxFcc_BSCH);
    avxLigature liga = AfxLegoGetTemplate(liga);
    AfxAssertObject(liga, afxFcc_BSCH);
    glVmt const* gl = &dexu->wglVmt;
    
    if (dexu->state.pip)
    {
        avxLigature legt2;
        AfxPipelineRigEnumerateTemplates(AfxPipelineGetRig(dexu->state.pip), unit, 1, &legt2);

        if (_SglDqueBindAndSyncLegoSub(dexu, unit, liga, legt2))
            AfxThrowError();
    }
    else
    {
        afxNat shdCnt;
        avxShader shd;
        shdCnt = dexu->state.shdCnt;

        for (afxNat i = 0; i < shdCnt; i++)
        {
            shd = dexu->state.shd[i];
            avxLigature legt2 = shd->liga[unit];

            if (_SglDqueBindAndSyncLegoSub(dexu, unit, liga, legt2))
                AfxThrowError();
        }
    }
    return err;
}
#endif 

_ZGL afxError _DpuBindAndResolveLiga(sglDpu* dpu, avxLigature liga, GLuint glHandle)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &liga, afxFcc_BSCH);
    glVmt const* gl = &dpu->gl;
    AfxAssert(glHandle);
    
    afxString32 tmp;
    AfxMakeString32(&tmp, 0);
    afxChar const *rawName = (void const *)AfxGetStringStorage(&tmp.str, 0);

    afxNat setCnt = liga->m.setCnt;

    for (afxNat i = 0; i < setCnt; i++)
    {
        afxNat set = liga->m.sets[i].set;
        afxNat entryCnt = liga->m.sets[i].entryCnt;
        afxNat baseEntry = liga->m.sets[i].baseEntry;

        for (afxNat j = 0; j < entryCnt; j++)
        {
            avxLigatureEntry const *entry = &liga->m.totalEntries[baseEntry + j];
            AfxAssert(!AfxStringIsEmpty(&entry->name.str));
            AfxCopyString(&tmp.str, &entry->name.str);
            //AfxAssert(entry->visibility);
            //AfxAssert(entry->cnt);

            afxNat glBinding = (set * _ZGL_MAX_ENTRY_PER_LEGO) + entry->binding;
            afxNat loc;

            AfxAssert(entry->type);
            switch (entry->type)
            {
            case avxShaderResType_SAMPLER:
            {
                loc = gl->GetUniformLocation(glHandle, rawName); _SglThrowErrorOccuried();

                if (loc != GL_INVALID_INDEX)
                {
                    gl->Uniform1i(loc, glBinding); _SglThrowErrorOccuried();
                }
                else
                {
                    //AfxThrowError();
                    AfxLogError("Sampler unit '%s' not found in ligature %p.", rawName, liga);
                }
                break;
            }
            case avxShaderResType_SAMPLED_IMAGE:
            {
                loc = gl->GetUniformLocation(glHandle, rawName); _SglThrowErrorOccuried();

                if (loc != GL_INVALID_INDEX)
                {
                    gl->Uniform1i(loc, glBinding); _SglThrowErrorOccuried();
                }
                else
                {
                    //AfxThrowError();
                    AfxLogError("Raster unit '%s' not found in ligature %p.", rawName, liga);
                }
                break;
            }
            case avxShaderResType_COMBINED_IMAGE_SAMPLER:
            {
                loc = gl->GetUniformLocation(glHandle, rawName); _SglThrowErrorOccuried();

                if (loc != GL_INVALID_INDEX)
                {
                    gl->Uniform1i(loc, glBinding); _SglThrowErrorOccuried();
                }
                else
                {
                    //AfxThrowError();
                    AfxLogError("Combined raster/sampler unit '%s' not found in ligature %p.", rawName, liga);
                }
                break;
            }
            case avxShaderResType_CONSTANT_BUFFER:
            {
                // https://stackoverflow.com/questions/44629165/bind-multiple-uniform-buffer-objects

                GLuint unifBlckIdx = gl->GetUniformBlockIndex(glHandle, rawName); _SglThrowErrorOccuried();

                if (unifBlckIdx != GL_INVALID_INDEX)
                {
                    gl->UniformBlockBinding(glHandle, unifBlckIdx, glBinding); _SglThrowErrorOccuried();
                }
                else
                {
                    //AfxThrowError();
                    AfxLogError("Uniform buffer unit '%s' not found in ligature %p.", rawName, liga);
                }
                break;
            }
            case avxShaderResType_STORAGE_BUFFER:
            {
                GLuint storBlckIdx = gl->GetUniformBlockIndex(glHandle, rawName); _SglThrowErrorOccuried();

                if (storBlckIdx != GL_INVALID_INDEX)
                {
                    gl->ShaderStorageBlockBinding(glHandle, storBlckIdx, glBinding); _SglThrowErrorOccuried();
                }
                else
                {
                    //AfxThrowError();
                    AfxLogError("Storage buffer unit '%s' not found in ligature %p.", rawName, liga);
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

_ZGL afxError _SglLigaDtor(avxLigature liga)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &liga, afxFcc_BSCH);

    if (_AvxLigaStdImplementation.dtor(liga))
        AfxThrowError();

    return err;
}

_ZGL afxError _SglLigaCtor(avxLigature liga, void** args, afxNat invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &liga, afxFcc_BSCH);

    if (_AvxLigaStdImplementation.ctor(liga, args, invokeNo)) AfxThrowError();
    else
    {
        if (err && _AvxLigaStdImplementation.dtor(liga))
            AfxThrowError();
    }
    AfxAssertObjects(1, &liga, afxFcc_BSCH);
    return err;
}
