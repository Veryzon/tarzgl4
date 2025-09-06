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

 // RESOURCE BINDING

_ZGL void DpuBindBuffers(zglDpu* dpu, afxUnit set, afxUnit first, afxUnit cnt, avxBufferedMap const maps[])
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_RANGE(_ZGL_MAX_ENTRY_PER_LEGO, first, cnt);
    AFX_ASSERT_RANGE(_ZGL_MAX_LEGO_PER_BIND, set, 1);

    // deferred because it requires the pipeline ligature info.

    for (afxUnit i = 0; i < cnt; i++)
    {
        avxBufferedMap const* map = &maps[i];

        afxUnit entryIdx = first + i;
        avxBuffer buf = map->buf;
        afxUnit32 offset = map->offset;
        afxUnit32 range = map->range;

        AFX_TRY_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);

        dpu->nextLs[set][entryIdx].buf = buf;
        dpu->nextLs[set][entryIdx].offset = offset;
        dpu->nextLs[set][entryIdx].range = range;
        dpu->nextLsUpdMask[set] |= AFX_BITMASK(entryIdx);
    }
}

_ZGL void DpuBindRasters(zglDpu* dpu, afxUnit set, afxUnit first, afxUnit cnt, avxRaster const rasters[])
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_RANGE(_ZGL_MAX_ENTRY_PER_LEGO, first, cnt);
    AFX_ASSERT_RANGE(_ZGL_MAX_LEGO_PER_BIND, set, 1);

    // deferred because it requires the pipeline ligature info.

    for (afxUnit i = 0; i < cnt; i++)
    {
        afxUnit entryIdx = first + i;
        avxRaster ras = rasters[i];
        AFX_TRY_ASSERT_OBJECTS(afxFcc_RAS, 1, &ras);
        dpu->nextLs[set][entryIdx].ras = ras;
        dpu->nextLsUpdMask[set] |= AFX_BITMASK(entryIdx);
    }
}

_ZGL void DpuBindSamplers(zglDpu* dpu, afxUnit set, afxUnit first, afxUnit cnt, avxSampler const samplers[])
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_RANGE(_ZGL_MAX_ENTRY_PER_LEGO, first, cnt);
    AFX_ASSERT_RANGE(_ZGL_MAX_LEGO_PER_BIND, set, 1);

    // deferred because it requires the pipeline ligature info.

    for (afxUnit i = 0; i < cnt; i++)
    {
        afxUnit entryIdx = first + i;
        avxSampler smp = samplers[i];
        AFX_TRY_ASSERT_OBJECTS(afxFcc_SAMP, 1, &smp);
        dpu->nextLs[set][entryIdx].smp = smp;
        dpu->nextLsUpdMask[set] |= AFX_BITMASK(entryIdx);
    }
}

_ZGL void DpuUnbindSamplers(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    
    // Samplers are bound to GL program. 
    // So we must unbind them to provoke rebinding on next program binding.

    avxLigature liga;
    if (!(liga = dpu->activeLiga))
        return;
    AFX_ASSERT_OBJECTS(afxFcc_LIGA, 1, &liga);

    _avxLigamentSet* sets = liga->m.sets;
    _avxLigament* totalEntries = liga->m.totalEntries;
    for (afxUnit i = 0; i < liga->m.setCnt; i++)
    {
        afxUnit set = sets[i].set;

        for (afxUnit j = 0; j < sets[i].entryCnt; j++)
        {
            afxUnit resIdx = sets[i].baseEntry + j;
            _avxLigament const *entry = &totalEntries[resIdx];

            if (entry->type == avxShaderParam_SAMPLER)
            {
                dpu->nextLs[set][resIdx].smp = NIL;
                dpu->nextLsUpdMask[set] |= AFX_BITMASK(resIdx);
            }
        }
    }
}

_ZGL void DpuPushConstants(zglDpu* dpu, afxUnit32 offset, afxUnit32 siz, void const* data)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

    // copy right into mapped memory.
    // Copy or just hold the pointer to data in command buffer?
    afxUnit pushSetIdx = dpu->dpuIterIdx % _ZGL_PUSH_SET_POP;
    AfxCopy(&dpu->pushSets[pushSetIdx].pushConstMappedMem[offset], data, siz);
    dpu->pushSets[pushSetIdx].shouldPushConsts = TRUE;
    dpu->pushSets[pushSetIdx].shouldPushConstRange = (dpu->pushSets[pushSetIdx].shouldPushConstBase + dpu->pushSets[pushSetIdx].shouldPushConstRange) > (offset + siz) ? dpu->pushSets[pushSetIdx].shouldPushConstRange : siz;
    dpu->pushSets[pushSetIdx].shouldPushConstBase = AFX_MIN(dpu->pushSets[pushSetIdx].shouldPushConstBase, offset);
}

_ZGL void _ZglFlushLigatureState(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

    /*
        Persistent across program changes.
        These bindings are global and not affected by changing the currently active program:
            Textures (unit bindings)
                glBindTexture, glActiveTexture, glBindTextureUnit
                    Texture units are global; shader uses sampler uniform to link to units.
            Buffer bindings (e.g., UBO, SSBO)
                glBindBufferBase, glBindBufferRange
                    Binding is to indexed targets, global per context.
            Uniform buffer binding points
                glBindBufferBase(GL_UNIFORM_BUFFER, ...)
                    Programs reference binding points via layout(binding = N).
            Shader storage buffers
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ...)
                    Same idea as UBOs.
            Transform feedback
                glBindTransformFeedback, etc.
                    Global binding.
            Atomic counters
                Bound via buffer bindings.
                    Referenced through layout(binding = N).

        Even though the texture bound to unit 0 (via glBindTextureUnit(0, texID)) remains,
        uniforms like sampler bindings need to be reset per program. Example:
        {
            glUseProgram(programA);
            glUniform1i(glGetUniformLocation(programA, "texSampler"), 0);  // Set sampler to unit 0

            glUseProgram(programB);
            // Need to set sampler again if "texSampler" is also used in B
            glUniform1i(glGetUniformLocation(programB, "texSampler"), 0);
        }

        Per-program or re-evaluated on program change.
        These are program-specific or interpreted in the context of the currently active program:
            Uniform variables
                Values are per-program.
                    Changing the program discards uniform values set on the previous one.
            Subroutine uniforms
                Per-program; need to be set again after switching.
            Attribute locations
                Must match or be queried/set per program.
            Sampler bindings
                The unit number is stored in the program, but the texture bound to that unit is global.
    */

    afxUnit psoHandleIdx = dpu->dpuIterIdx % _ZGL_PSO_SET_POP;

    avxLigature liga = dpu->nextLiga;
    if (liga != dpu->activeLiga)
    {
        dpu->activeLiga = liga;
    }

    if (!liga)
    {

    }
    else
    {
        AFX_ASSERT_OBJECTS(afxFcc_LIGA, 1, &liga);

#ifdef COHERENT_PUSHABLES // TODO: Test it to avoid COHERENT mapping.

#else
        if (liga->m.pushCnt
#ifndef FORCE_ARG_REBIND
            && dpu->shouldPushConsts
#endif
            )
        {
            afxUnit pushSetIdx = dpu->dpuIterIdx % _ZGL_PUSH_SET_POP;

            if (gl->FlushMappedNamedBufferRange)
            {
                gl->FlushMappedNamedBufferRange(dpu->pushSets[pushSetIdx].pushConstUbo, dpu->pushSets[pushSetIdx].shouldPushConstBase, dpu->pushSets[pushSetIdx].shouldPushConstRange); _ZglThrowErrorOccuried();
            }
            else
            {
                gl->BindBuffer(GL_COPY_WRITE_BUFFER, dpu->pushSets[pushSetIdx].pushConstUbo); _ZglThrowErrorOccuried();
                gl->FlushMappedBufferRange(GL_COPY_WRITE_BUFFER, dpu->pushSets[pushSetIdx].shouldPushConstBase, dpu->pushSets[pushSetIdx].shouldPushConstRange); _ZglThrowErrorOccuried();
            }
            dpu->pushSets[pushSetIdx].shouldPushConstBase = 0;
            dpu->pushSets[pushSetIdx].shouldPushConstRange = 0;
            dpu->pushSets[pushSetIdx].shouldPushConsts = FALSE;
        }
#endif//COHERENT_PUSHABLES

        // BIND RESOURCES (TEXTURES, SAMPLERS AND BUFFERS)

        _avxLigamentSet* sets = liga->m.sets;
        _avxLigament* totalEntries = liga->m.totalEntries;
        for (afxUnit i = 0; i < liga->m.setCnt; i++)
        {
            afxUnit set = sets[i].set;
            afxMask updMask = dpu->nextLsUpdMask[set];

#ifndef FORCE_ARG_REBIND
            if (!updMask) // skip if has not updates
                continue;
#endif

            dpu->nextLsUpdMask[set] = NIL;

            for (afxUnit j = 0; j < sets[i].entryCnt; j++)
            {
                afxUnit resIdx = sets[i].baseEntry + j;
                _avxLigament const *entry = &totalEntries[resIdx];

#ifndef FORCE_ARG_REBIND
                if (!(updMask & AFX_BITMASK(entry->binding))) // skip if not changed
                    continue;
#endif

                AFX_ASSERT(entry->type);
                afxUnit binding = entry->binding;
                afxUnit glUnit = /*(set * _ZGL_MAX_ENTRY_PER_LEGO) +*/ binding;
                afxBool reqUpd = FALSE, reqUpd2 = FALSE;
                GLuint glHandle = 0, glHandle2 = 0;
                afxSize bufSiz = 0;

                switch (entry->type)
                {
                case avxShaderParam_UNIFORM:
                case avxShaderParam_BUFFER:
                {
                    afxUnit offset = dpu->nextLs[set][binding].offset;
                    afxUnit range = dpu->nextLs[set][binding].range;
                    avxBuffer buf = dpu->nextLs[set][binding].buf;
                    afxBool rebind = FALSE;
                    GLenum glTarget = GL_INVALID_ENUM;

                    if (entry->type == avxShaderParam_UNIFORM)
                        glTarget = GL_UNIFORM_BUFFER;
                    else if (entry->type == avxShaderParam_BUFFER)
                        glTarget = GL_SHADER_STORAGE_BUFFER;
                    else AfxThrowError();

                    afxUnit bufUniqId = buf ? buf->bufUniqueId : dpu->activeLs[set][binding].bufUniqId;

#ifndef FORCE_ARG_REBIND
                    if ((dpu->activeLs[set][binding].buf != buf) ||
                        (dpu->activeLs[set][binding].offset != offset) ||
                        (dpu->activeLs[set][binding].range != range)
#if !0
                        || (dpu->activeLs[set][binding].bufUniqId != bufUniqId)
#endif
                        )
#endif
                    {
                        dpu->activeLs[set][binding].buf = buf;
                        dpu->activeLs[set][binding].offset = offset;
                        dpu->activeLs[set][binding].range = range;

                        dpu->activeLs[set][binding].bufUniqId = bufUniqId;
                        //dpu->nextLs[set][binding].buf = NIL; // force update in "next first time".
                        rebind = TRUE;
                    }

#if !0
                    if (rebind)
#endif
                    {
                        if (!buf)
                        {
                            gl->BindBufferBase(glTarget, glUnit, 0); _ZglThrowErrorOccuried();
                        }
                        else
                        {
                            AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);
                            if (entry->type == avxShaderParam_UNIFORM)
                            {
                                AFX_ASSERT(AvxGetBufferUsage(buf, avxBufferUsage_UNIFORM) == avxBufferUsage_UNIFORM);
                            }
                            else if (entry->type == avxShaderParam_STORAGE)
                            {
                                AFX_ASSERT(AvxGetBufferUsage(buf, avxBufferUsage_STORAGE) == avxBufferUsage_STORAGE);
                            }
                            else AfxThrowError();

                            DpuBindAndSyncBuf(dpu, glTarget, buf, FALSE);
                            bufSiz = AvxGetBufferCapacity(buf, 0);

                            if (offset || range)
                            {
                                AFX_ASSERT(range);
                                AFX_ASSERT_RANGE(bufSiz, offset, range);

                                if (range)
                                    range = AFX_MIN(range, bufSiz - offset);
                                else
                                    range = AFX_CLAMP(bufSiz, range, bufSiz - offset);

                                gl->BindBufferRange(glTarget, glUnit, buf->glHandle, offset, AFX_ALIGN_SIZE(range, AFX_SIMD_ALIGNMENT)); _ZglThrowErrorOccuried();
                            }
                            else
                            {
                                gl->BindBufferBase(glTarget, glUnit, buf->glHandle); _ZglThrowErrorOccuried();
                            }
                        }
                    }
                    break;
                }
                case avxShaderParam_TEXTURE:
                case avxShaderParam_RASTER:
                case avxShaderParam_SAMPLER:
                {
                    avxSampler samp = dpu->nextLs[set][binding].smp;
                    avxRaster ras = dpu->nextLs[set][binding].ras;
                    afxBool rebindRas = FALSE;
                    afxBool rebindSamp = FALSE;

                    afxUnit rasUniqId = ras ? ras->rasUniqueId : dpu->activeLs[set][binding].rasUniqId;
                    afxUnit sampUniqId = samp ? samp->smpUniqueId : dpu->activeLs[set][binding].smpUniqId;

#ifndef FORCE_ARG_REBIND
                    if ((dpu->activeLs[set][binding].ras != ras)

#if !0
                        || (dpu->activeLs[set][binding].rasUniqId != rasUniqId)
#endif
                        )
#endif
                    {
                        dpu->activeLs[set][binding].rasUniqId = rasUniqId;
                        dpu->activeLs[set][binding].ras = ras;
                        //dpu->nextLs[set][binding].ras = NIL; // force update in "next first time".
                        rebindRas = TRUE;
                    }

#ifndef FORCE_ARG_REBIND
                    if ((dpu->activeLs[set][binding].smp != samp)
#if !0
                        || (dpu->activeLs[set][binding].smpUniqId != sampUniqId)
#endif
                        )
#endif
                    {
                        dpu->activeLs[set][binding].smpUniqId = sampUniqId;
                        dpu->activeLs[set][binding].smp = samp;
                        //dpu->nextLs[set][binding].smp = NIL; // force update in "next first time".
                        rebindSamp = TRUE;
                    }

#if !0
                    if (rebindRas)
#endif
                    {
                        if (entry->type == avxShaderParam_RASTER || entry->type == avxShaderParam_TEXTURE)
                        {
                            if (!ras)
                            {
                                if (gl->BindTextureUnit)
                                {
                                    gl->BindTextureUnit(glUnit, 0); _ZglThrowErrorOccuried();
                                }
                                else
                                {
                                    gl->ActiveTexture(GL_TEXTURE0 + glUnit); _ZglThrowErrorOccuried();
                                    gl->BindTexture(GL_TEXTURE_2D, 0); _ZglThrowErrorOccuried();
                                }
                            }
                            else
                            {
                                AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &ras);
                                AFX_ASSERT(AvxGetRasterUsage(ras, avxRasterUsage_TEXTURE) == avxRasterUsage_TEXTURE);

                                if (gl->BindTextureUnit)
                                {
                                    DpuBindAndSyncRas(dpu, glUnit, ras, FALSE);
                                    gl->BindTextureUnit(glUnit, ras->glHandle); _ZglThrowErrorOccuried();
                                }
                                else
                                {
                                    DpuBindAndSyncRas(dpu, glUnit, ras, TRUE);
                                    gl->ActiveTexture(GL_TEXTURE0 + glUnit); _ZglThrowErrorOccuried();
                                    gl->BindTexture(ras->glTarget, ras->glHandle); _ZglThrowErrorOccuried();
                                }
                            }
                        }
                    }

#if !0
                    if (rebindSamp)
#endif
                    {
                        if (entry->type == avxShaderParam_SAMPLER || entry->type == avxShaderParam_TEXTURE)
                        {
                            if (!samp)
                            {
                                gl->BindSampler(glUnit, 0); _ZglThrowErrorOccuried();
                            }
                            else
                            {
                                AFX_ASSERT_OBJECTS(afxFcc_SAMP, 1, &samp);
                                _DpuBindAndSyncSamp(dpu, glUnit, samp);
                                gl->BindSampler(glUnit, samp->glHandle); _ZglThrowErrorOccuried();
                            }
                        }
                    }
                    break;
                }
                case avxShaderParam_IMAGE:
                {
                    avxRaster ras = dpu->nextLs[set][binding].ras;
                    afxBool rebindRas = FALSE;
                    afxBool rebindSamp = FALSE;

                    afxUnit rasUniqId = ras ? ras->rasUniqueId : dpu->activeLs[set][binding].rasUniqId;

#ifndef FORCE_ARG_REBIND
                    if ((dpu->activeLs[set][binding].ras != ras)
#if !0
                        || (dpu->activeLs[set][binding].rasUniqId != rasUniqId)
#endif
                        )
#endif
                    {
                        dpu->activeLs[set][binding].rasUniqId = rasUniqId;
                        dpu->activeLs[set][binding].ras = ras;
                        //dpu->nextLs[set][binding].ras = NIL; // force update in "next first time".
                        rebindRas = TRUE;
                    }

#if !0
                    if (rebindRas)
#endif
                    {
                        AFX_ASSERT(gl->BindImageTexture);

                        if (!ras)
                        {
                            gl->BindImageTexture(glUnit, NIL, 0, FALSE, 0, GL_READ_ONLY, GL_R32F); _ZglThrowErrorOccuried();
                        }
                        else
                        {
                            AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &ras);
                            AFX_ASSERT(AvxGetRasterUsage(ras, avxRasterUsage_STORAGE) == avxRasterUsage_STORAGE);

                            gl->BindImageTexture(glUnit, ras->glHandle, ras->m.baseMip, (ras->m.whd.d > 1), ras->m.baseLayer, GL_READ_WRITE, ras->glIntFmt); _ZglThrowErrorOccuried();
                        }
                    }
                    break;
                }
                case avxShaderParam_FETCH:
                case avxShaderParam_TSBO:
                {
                    afxUnit offset = dpu->nextLs[set][binding].offset;
                    afxUnit range = dpu->nextLs[set][binding].range;
                    avxBuffer buf = dpu->nextLs[set][binding].buf;
                    afxBool rebind = FALSE;
                    GLenum glTarget = GL_TEXTURE_BUFFER;

                    afxUnit bufUniqId = buf ? buf->bufUniqueId : dpu->activeLs[set][binding].bufUniqId;

#ifndef FORCE_ARG_REBIND
                    if ((dpu->activeLs[set][binding].buf != buf) ||
                        (dpu->activeLs[set][binding].offset != offset) ||
                        (dpu->activeLs[set][binding].range != range)
#if !0
                        || (dpu->activeLs[set][binding].bufUniqId != bufUniqId)
#endif
                        )
#endif
                    {
                        dpu->activeLs[set][binding].buf = buf;
                        dpu->activeLs[set][binding].offset = offset;
                        dpu->activeLs[set][binding].range = range;

                        dpu->activeLs[set][binding].bufUniqId = bufUniqId;
                        //dpu->nextLs[set][binding].buf = NIL; // force update in "next first time".
                        rebind = TRUE;
                    }

#if !0
                    if (rebind)
#endif
                    {
                        GLuint glFixedTboHandle = liga->perDpu[dpu->m.exuIdx][psoHandleIdx].texBufGlHandle[resIdx];

                        if (!buf)
                        {
                            if (entry->type == avxShaderParam_TSBO)
                            {
                                AFX_ASSERT(gl->BindImageTexture);
                                gl->BindImageTexture(glUnit, NIL, 0, FALSE, 0, GL_READ_WRITE, GL_NONE); _ZglThrowErrorOccuried();
                            }
                            else
                            {
                                if (gl->BindTextureUnit)
                                {
                                    gl->BindTextureUnit(glUnit, 0); _ZglThrowErrorOccuried();
                                    gl->TextureBuffer(glFixedTboHandle, NIL, NIL); _ZglThrowErrorOccuried();
                                }
                                else
                                {
                                    gl->ActiveTexture(GL_TEXTURE0 + glUnit); _ZglThrowErrorOccuried();
                                    gl->BindTexture(glTarget, glFixedTboHandle); _ZglThrowErrorOccuried();
                                    gl->TexBuffer(glTarget, NIL, NIL); _ZglThrowErrorOccuried();
                                }
                            }
                        }
                        else
                        {
                            AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);

                            if (entry->type == avxShaderParam_TSBO)
                            {
                                AFX_ASSERT(AvxGetBufferUsage(buf, avxBufferUsage_TENSOR) == avxBufferUsage_TENSOR);
                                DpuBindAndSyncBuf(dpu, glTarget, buf, FALSE);
                                AFX_ASSERT(gl->BindImageTexture);
                                gl->BindImageTexture(glUnit, buf->glHandle, 0, FALSE, 0, GL_READ_WRITE, buf->glTexIntFmt);
                            }
                            else
                            {
                                AFX_ASSERT(AvxGetBufferUsage(buf, avxBufferUsage_FETCH) == avxBufferUsage_FETCH);

                                if (gl->BindTextureUnit)
                                {
                                    DpuBindAndSyncBuf(dpu, glTarget, buf, FALSE);

                                    if (offset || (range == buf->m.reqSiz))
                                    {
                                        if (gl->TextureBufferRange)
                                        {
                                            gl->TextureBufferRange(glFixedTboHandle, buf->glHandle, buf->glHandle, offset, range); _ZglThrowErrorOccuried();
                                        }
                                        else
                                        {
                                            gl->ActiveTexture(GL_TEXTURE0 + glUnit); _ZglThrowErrorOccuried();
                                            gl->BindTexture(glTarget, glFixedTboHandle); _ZglThrowErrorOccuried();
                                            gl->TexBufferRange(glTarget, buf->glTexIntFmt, buf->glHandle, offset, range); _ZglThrowErrorOccuried();
                                        }
                                    }
                                    else
                                    {
                                        if (gl->TextureBuffer)
                                        {
                                            gl->TextureBuffer(glFixedTboHandle, buf->glHandle, buf->glHandle); _ZglThrowErrorOccuried();
                                        }
                                        else
                                        {
                                            gl->ActiveTexture(GL_TEXTURE0 + glUnit); _ZglThrowErrorOccuried();
                                            gl->BindTexture(glTarget, glFixedTboHandle); _ZglThrowErrorOccuried();
                                            gl->TexBuffer(glTarget, buf->glTexIntFmt, buf->glHandle); _ZglThrowErrorOccuried();
                                        }
                                    }
                                    gl->BindTextureUnit(glUnit, glFixedTboHandle); _ZglThrowErrorOccuried();
                                }
                                else
                                {
                                    DpuBindAndSyncBuf(dpu, glTarget, buf, FALSE);
                                    gl->ActiveTexture(GL_TEXTURE0 + glUnit); _ZglThrowErrorOccuried();
                                    gl->BindTexture(glTarget, glFixedTboHandle); _ZglThrowErrorOccuried();

                                    if (offset || (range == buf->m.reqSiz))
                                    {
                                        gl->TexBufferRange(glTarget, buf->glTexIntFmt, buf->glHandle, offset, range); _ZglThrowErrorOccuried();
                                    }
                                    else
                                    {
                                        gl->TexBuffer(glTarget, buf->glTexIntFmt, buf->glHandle); _ZglThrowErrorOccuried();
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
                default:
                {
                    AfxReportError("");
                }
                }
            }
        }
    }
}

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
        case avxShaderParam_SAMPLER:
        {
            AfxAssertObject(data->smp, afxFcc_SAMP);
            _ZglDqueBindAndSyncSmp(dexu, binding, data->smp);
            break;
        }
        case avxShaderParam_RASTER:
        {
            AfxAssertObject(data->tex, afxFcc_RAS);
            _ZglDqueBindAndSyncTex(dexu, binding, data->tex);
            break;
        }
        case avxShaderParam_TEXTURE:
        {
            AfxAssertObject(data->tex, afxFcc_RAS);
            _ZglDqueBindAndSyncTex(dexu, binding, data->tex);

            AfxAssertObject(data->smp, afxFcc_SAMP);
            _ZglDqueBindAndSyncSmp(dexu, binding, data->smp);
#if 0
            afxUri128 uri;
            AfxMakeUri128(&uri, NIL);
            AvxPrintRaster(point->resource.img.tex, AfxFormatUri(&uri.uri, "system/tex-%u-%u.tga", i, entry->binding));
#endif
            break;
        }
        case avxShaderParam_UNIFORM:
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
            AfxReportError("");
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
    glVmt const* gl = dpu->gl;
    AFX_ASSERT(glHandle);

    afxUnit psoHandleIdx = dpu->dpuIterIdx % _ZGL_PSO_SET_POP;

    afxString32 tmp;
    AfxMakeString32(&tmp, 0);
    afxChar const *rawName = (void const *)AfxGetStringStorage(&tmp.s, 0);

    if (liga->m.pushCnt)
    {
        // https://stackoverflow.com/questions/44629165/bind-multiple-uniform-buffer-objects

        GLuint unifBlckIdx = gl->GetUniformBlockIndex(glHandle, "pushable"); _ZglThrowErrorOccuried();

        if (unifBlckIdx != GL_INVALID_INDEX)
        {
            gl->UniformBlockBinding(glHandle, unifBlckIdx, dpu->activePushUboReservedBindPoint); _ZglThrowErrorOccuried();
        }
        else
        {
            //AfxThrowError();
            AfxReportError("Uniform buffer unit '%s' not found in ligature %p.", "pushable", liga);
        }
    }

    afxUnit setCnt = liga->m.setCnt;
    _avxLigamentSet* sets = liga->m.sets;
    for (afxUnit i = 0; i < setCnt; i++)
    {
        afxUnit set = liga->m.sets[i].set;
        afxUnit entryCnt = sets[i].entryCnt;
        afxUnit baseEntry = sets[i].baseEntry;

        for (afxUnit j = 0; j < entryCnt; j++)
        {
            _avxLigament const *entry = &liga->m.totalEntries[baseEntry + j];
            AFX_ASSERT(!AfxIsStringEmpty(&entry->name.s));
            AfxClearStrings(1, &tmp.s);
            AfxCopyString(&tmp.s, 0, &entry->name.s, 0);
            //AFX_ASSERT(entry->visibility);
            //AFX_ASSERT(entry->cnt);

            afxUnit glBinding = /*(set * _ZGL_MAX_ENTRY_PER_LEGO) +*/ entry->binding;
            afxUnit loc;

            AFX_ASSERT(entry->type);
            switch (entry->type)
            {
            case avxShaderParam_SAMPLER:
            {
                loc = gl->GetUniformLocation(glHandle, rawName); _ZglThrowErrorOccuried();

                if (loc != GL_INVALID_INDEX)
                {
                    gl->Uniform1i(loc, glBinding); _ZglThrowErrorOccuried();
                }
                else
                {
                    //AfxThrowError();
                    AfxReportError("avxLigature(%p): Sampler ligament unit '%s' not found.", liga, rawName);
                }
                break;
            }
            case avxShaderParam_RASTER:
            {
                loc = gl->GetUniformLocation(glHandle, rawName); _ZglThrowErrorOccuried();

                if (loc != GL_INVALID_INDEX)
                {
                    gl->Uniform1i(loc, glBinding); _ZglThrowErrorOccuried();
                }
                else
                {
                    //AfxThrowError();
                    AfxReportError("avxLigature(%p): Raster ligament unit '%s' not found.", liga, rawName);
                }
                break;
            }
            case avxShaderParam_TEXTURE:
            {
                loc = gl->GetUniformLocation(glHandle, rawName); _ZglThrowErrorOccuried();

                if (loc != GL_INVALID_INDEX)
                {
                    gl->Uniform1i(loc, glBinding); _ZglThrowErrorOccuried();
                }
                else
                {
                    //AfxThrowError();
                    AfxReportError("avxLigature(%p): Texture ligament unit '%s' not found.", liga, rawName);
                }
                break;
            }
            case avxShaderParam_UNIFORM:
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
                    AfxReportError("avxLigature(%p): Uniform buffer ligament unit '%s' not found.", liga, rawName);
                }
                break;
            }
            case avxShaderParam_BUFFER:
            {
                GLuint storBlckIdx = gl->GetUniformBlockIndex(glHandle, rawName); _ZglThrowErrorOccuried();

                if (storBlckIdx != GL_INVALID_INDEX)
                {
                    gl->ShaderStorageBlockBinding(glHandle, storBlckIdx, glBinding); _ZglThrowErrorOccuried();
                }
                else
                {
                    //AfxThrowError();
                    AfxReportError("avxLigature(%p): Storage buffer ligament unit '%s' not found.", liga, rawName);
                }
                break;
            }
            case avxShaderParam_FETCH:
            case avxShaderParam_TSBO:
            {
                loc = gl->GetUniformLocation(glHandle, rawName); _ZglThrowErrorOccuried();

                if (loc != GL_INVALID_INDEX)
                {
                    gl->Uniform1i(loc, glBinding); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (avxShaderParam_FETCH)
                    {
                        AfxReportError("avxLigature(%p): Fetch buffer ligament unit '%s' not found.", liga, rawName);
                    }
                    else
                    {
                        AfxReportError("avxLigature(%p): Tensor buffer ligament unit '%s' not found.", liga, rawName);
                    }
                }

                if (gl->CreateTextures && gl->TextureBuffer)
                {
                    gl->CreateTextures(GL_TEXTURE_BUFFER, 1, &liga->perDpu[dpu->m.exuIdx][psoHandleIdx].texBufGlHandle[baseEntry + j]); _ZglThrowErrorOccuried();
                }
                else
                {
                    gl->ActiveTexture(GL_TEXTURE0 + ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT); _ZglThrowErrorOccuried();
                    gl->GenTextures(1, &liga->perDpu[dpu->m.exuIdx][psoHandleIdx].texBufGlHandle[baseEntry + j]); _ZglThrowErrorOccuried();
                    gl->BindTexture(GL_TEXTURE_BUFFER, liga->perDpu[dpu->m.exuIdx][psoHandleIdx].texBufGlHandle[baseEntry + j]); _ZglThrowErrorOccuried();
                    gl->BindTexture(GL_TEXTURE_BUFFER, 0); _ZglThrowErrorOccuried();
                }
                break;
            }
            default:
            {
                AfxReportError("");
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

    afxDrawSystem dsys = AvxGetLigatureHost(liga);

    for (afxUnit i = 0; i < liga->m.totalEntryCnt; i++)
    {
        _avxLigament* lig = &liga->m.totalEntries[i];

        if ((lig->type == avxShaderParam_FETCH) ||
            (lig->type == avxShaderParam_TSBO))
        {
            for (afxUnit j = 0; j < dsys->m.bridgeCnt; j++)
            {
                for (afxUnit k = 0; k < _ZGL_PSO_SET_POP; k++)
                {
                    GLuint glTexHandle = liga->perDpu[j][k].texBufGlHandle[i];

                    if (glTexHandle)
                    {
                        _ZglDsysEnqueueDeletion(dsys, j, GL_TEXTURE, (afxSize)glTexHandle);
                        liga->perDpu[j][k].texBufGlHandle[i] = 0;
                    }
                }
            }
        }
    }

    if (_AVX_LIGA_CLASS_CONFIG.dtor(liga))
        AfxThrowError();

    return err;
}

_ZGL afxError _ZglLigaCtor(avxLigature liga, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_LIGA, 1, &liga);

    if (_AVX_LIGA_CLASS_CONFIG.ctor(liga, args, invokeNo))
    {
        AfxThrowError();
        return err;
    }

    afxUnit tboCnt = 0;
    afxUnit tboIndices[80];
    AfxZero(liga->perDpu, sizeof(liga->perDpu));

    /*
        Texture and buffers can be shared across contexts if created in a shared context.
        The association (i.e., glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufferID)) is per-context.
        That means that if Context A binds a buffer to a texture, Context B won't see that binding unless it does the binding itself.
    */

    for (afxUnit i = 0; i < liga->m.totalEntryCnt; i++)
    {
        _avxLigament* lig = &liga->m.totalEntries[i];

        if ((lig->type == avxShaderParam_FETCH) ||
            (lig->type == avxShaderParam_TSBO))
        {
            tboIndices[tboCnt] = i;
            ++tboCnt;
        }
    }

    liga->tboCnt = tboCnt;

    if (err && _AVX_LIGA_CLASS_CONFIG.dtor(liga))
        AfxThrowError();

    AFX_ASSERT_OBJECTS(afxFcc_LIGA, 1, &liga);
    return err;
}
