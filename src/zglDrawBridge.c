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
#include "zglUtils.h"
#include "qwadro/inc/sim/rendering/akxRenderer.h"

ZGL afxError _SglDexuPingCb(afxDrawBridge dexu, afxUnit queIdx);

// RENDERING SCOPE

_ZGL void _DpuConcludeDrawScope(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    _DpuBindAndSyncCanv(dpu, GL_READ_FRAMEBUFFER, 0);
    _DpuBindAndSyncCanv(dpu, GL_DRAW_FRAMEBUFFER, 0);
    //gl->Flush(); _SglThrowErrorOccuried(); // flush was presenting/swapping without wglSwapBuffers.

    avxCanvas canv = dpu->activeRs.canv;

    if (canv)
    {
        afxUnit surCnt;
        AfxCountDrawBufferSlots(canv, &surCnt, NIL);
#if 0
        if (surCnt)
        {
            AfxAssert(_ZGL_MAX_SURF_PER_CANV >= surCnt);
            afxRaster surfaces[_ZGL_MAX_SURF_PER_CANV];
            AfxGetDrawBuffers(canv, 0, surCnt, surfaces);

            for (afxUnit i = 0; i < surCnt; i++)
            {
                afxRaster ras = surfaces[i];

                if (ras)
                {
                    AfxAssertObjects(1, &ras, afxFcc_RAS);
                    ras->updFlags |= ZGL_UPD_FLAG_HOST_FLUSH;
                }
            }
        }
#endif
    }
}

_ZGL void _DpuCommenceDrawScope(zglDpu* dpu, avxCanvas canv, afxRect const* area, afxUnit baseLayer, afxUnit layerCnt, afxUnit cCnt, avxDrawTarget const* c, avxDrawTarget const* d, avxDrawTarget const* s, afxBool defFbo)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    avxDrawTarget const* dt;
    
    if (!canv)
    {
        if (defFbo)
        {
            //_DpuBindAndSyncCanv(gl, GL_READ_FRAMEBUFFER, canv);
            _DpuBindAndSyncCanv(dpu, GL_DRAW_FRAMEBUFFER, canv);
            //dpu->activeRs.canv = canv;

            dt = &c[0];

            switch (dt->storeOp)
            {
            case avxStoreOp_STORE:
            {
                gl->DrawBuffer(GL_BACK); _SglThrowErrorOccuried();
                break;
            }
            case avxStoreOp_DISCARD:
            case avxStoreOp_DONT_CARE:
            default:
            {
                gl->DrawBuffer(GL_NONE); _SglThrowErrorOccuried();
                gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 1, (GLenum[]) { GL_BACK }); _SglThrowErrorOccuried();
                break;
            }
            }

            switch (dt->loadOp)
            {
            case avxLoadOp_LOAD: break;
            case avxLoadOp_CLEAR:
            {
                gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 1, (GLenum[]) { GL_BACK }); _SglThrowErrorOccuried();
                afxReal const *rgba = dt->clearValue.rgba;
                gl->ClearColor(rgba[0], rgba[1], rgba[2], rgba[3]); _SglThrowErrorOccuried();
                gl->Clear(GL_COLOR_BUFFER_BIT); _SglThrowErrorOccuried();
                break;
            }
            case avxLoadOp_DONT_CARE:
            default:
            {
                gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 1, (GLenum[]) { GL_BACK }); _SglThrowErrorOccuried();
                break;
            }
            }
        }
        else
        {
            AfxThrowError();
        }
    }
    else
    {
        afxWhd canvWhd = AfxGetCanvasExtent(canv);

        afxUnit maxSurCnt, maxColSurCnt;
        AfxCountDrawBufferSlots(canv, &maxSurCnt, &maxColSurCnt);
        cCnt = AfxMin(cCnt, maxSurCnt);

        afxUnit dsSurIdx[2];
        afxBool hasDs = AfxGetDepthBufferSlots(canv, &dsSurIdx[0], &dsSurIdx[1]);
        afxBool combinedDs = (dsSurIdx[1] == dsSurIdx[0]);

        afxUnit storeCnt = 0;
        GLenum storeBufs[_ZGL_MAX_SURF_PER_CANV];
        afxMask storeBitmask = NIL;

        afxUnit clearCnt = 0;
        GLenum clearBufs[_ZGL_MAX_SURF_PER_CANV];
        afxUnit invalidCnt = 0;
        GLenum invalidBufs[_ZGL_MAX_SURF_PER_CANV];

        GLenum glAttach = GL_NONE;

        for (afxUnit i = 0; i < maxSurCnt; i++)
        {
            if (i < maxColSurCnt)
            {
                glAttach = GL_COLOR_ATTACHMENT0 + i;

                if (i < cCnt)
                {
                    dt = &c[i];
                    switch (dt->storeOp)
                    {
                    case avxStoreOp_STORE:
                    {
                        storeBufs[storeCnt] = glAttach;
                        storeCnt++;
                        storeBitmask |= AFX_BIT(i);
                        break;
                    }
                    case avxStoreOp_DISCARD:
                    {
                        storeBufs[storeCnt] = GL_NONE;
                        storeCnt++;
                        storeBitmask |= AFX_BIT(8 + i);
                        break;
                    }
                    case avxStoreOp_DONT_CARE:
                    default:
                    {
                        storeBufs[storeCnt] = GL_NONE;
                        storeCnt++;
                        storeBitmask |= AFX_BIT(16 + i);
                        break;
                    }
                    }
                }
                else
                {
                    dt = NIL;
                    // by default, store it to avoid more confusion with black screens.
                    storeBufs[storeCnt] = glAttach;
                    storeCnt++;
                    storeBitmask |= AFX_BIT(i);
                }
            }
            else if (hasDs)
            {
                if (i == dsSurIdx[0]) // depth (and stencil)
                {
                    dt = d;
                    glAttach = combinedDs ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
                }
                else if (i == dsSurIdx[1]) // stencil
                {
                    dt = s;
                    glAttach = GL_STENCIL_ATTACHMENT;
                }
                else AfxThrowError();
            }
            else
            {
                dt = NIL;
                break;
            }

            // Prepare cleanups and invalidations.

            if (dt)
            {
                if (dt->loadOp == avxLoadOp_CLEAR)
                {
                    clearBufs[clearCnt] = glAttach;
                    ++clearCnt;
                }

                if (dt->loadOp != avxLoadOp_LOAD)
                {
                    invalidBufs[invalidCnt] = glAttach;
                    ++invalidCnt;
                }
            }
        }

        // TODO iterate for each canvas surface against arguments

        //_DpuBindAndSyncCanv(gl, GL_READ_FRAMEBUFFER, canv);
        _DpuBindAndSyncCanv(dpu, GL_DRAW_FRAMEBUFFER, canv);
        GLuint glHandle = canv->glHandle;
        AfxAssert(gl->IsFramebuffer(glHandle));
        //dpu->activeRs.canv = canv;

#if !0
        if (canv->storeBitmask != storeBitmask)
#endif
        {
            canv->storeBitmask = storeBitmask; // cache it
            gl->DrawBuffers(storeCnt, storeBufs); _SglThrowErrorOccuried();

            // What to do with depth/stencil storage set to discard?
            // Should we force a glDepthMask to zero right there?
            // Should we force a glStencilMask to zero right there?
        }
#if 0
        // Prepare cleanups and invalidations.

        for (afxUnit i = 0; i < cCnt; i++)
        {
            dt = &c[i];

            if (dt->loadOp == avxLoadOp_CLEAR)
            {
                clearBufs[clearCnt] = GL_COLOR_ATTACHMENT0 + i;
                ++clearCnt;
            }

            if (dt->loadOp != avxLoadOp_LOAD)
            {
                invalidBufs[invalidCnt] = GL_COLOR_ATTACHMENT0 + i;
                ++invalidCnt;
            }
        }

        if (hasDs)
        {
            if (dsSurIdx[0] != AFX_INVALID_INDEX)
            {
                dt = d;

                if (dt)
                {
                    if (dt->loadOp == avxLoadOp_CLEAR)
                    {
                        clearBufs[clearCnt] = combinedDs ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
                        ++clearCnt;
                    }

                    if (dt->loadOp != avxLoadOp_LOAD)
                    {
                        invalidBufs[invalidCnt] = combinedDs ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
                        ++invalidCnt;
                    }
                }
            }

            if ((dsSurIdx[1] != AFX_INVALID_INDEX) && !combinedDs)
            {
                dt = s;

                if (dt)
                {
                    if (dt->loadOp == avxLoadOp_CLEAR)
                    {
                        clearBufs[clearCnt] = GL_STENCIL_ATTACHMENT;
                        ++clearCnt;
                    }

                    if (dt->loadOp != avxLoadOp_LOAD)
                    {
                        invalidBufs[invalidCnt] = GL_STENCIL_ATTACHMENT;
                        ++invalidCnt;
                    }
                }
            }
        }
#endif
        // Effectively invalidate the buffers.

        
        // TODO: Sanitize area to canvas' bounds.
        GLint scissor[4];
        scissor[0] = area->x;
        scissor[1] = area->y;
        scissor[2] = area->w;
        scissor[3] = area->h;
        AfxAssert(gl->ScissorArrayv);
        gl->ScissorArrayv(0, 1, scissor);
        afxBool clipped = (area->x || area->y || ((!area->w) || (area->w && canvWhd.w > area->w)) || (area->h && canvWhd.h > area->h));

        if (invalidCnt)
        {
            if (clipped)
            {
                gl->InvalidateSubFramebuffer(GL_DRAW_FRAMEBUFFER, invalidCnt, invalidBufs, area->x, area->y, area->w, area->h); _SglThrowErrorOccuried();
            }
            else
            {
                gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, invalidCnt, invalidBufs); _SglThrowErrorOccuried();
            }
        }

        // Effectively clear the buffers.

        for (afxUnit i = 0; i < clearCnt; i++)
        {
            switch (clearBufs[i])
            {
            case GL_DEPTH_ATTACHMENT:
            {
                dt = d;
                gl->ClearBufferfv(GL_DEPTH, 0, &dt->clearValue.depth); _SglThrowErrorOccuried();
                break;
            }
            case GL_DEPTH_STENCIL_ATTACHMENT:
            {
                dt = d;
                gl->ClearBufferfi(GL_DEPTH_STENCIL, 0, dt->clearValue.depth, dt->clearValue.stencil); _SglThrowErrorOccuried();
                break;
            }
            case GL_STENCIL_ATTACHMENT:
            {
                dt = s;
                GLint sCv = dt->clearValue.stencil;
                gl->ClearBufferiv(GL_STENCIL, 0, &sCv); _SglThrowErrorOccuried();
                break;
            }
            default:
            {
                dt = &c[i];
                afxReal const* rgba = dt->clearValue.rgba;
                GLint dbi = clearBufs[i] - GL_COLOR_ATTACHMENT0;
                gl->ClearBufferfv(GL_COLOR, /*GL_DRAW_BUFFER0 +*/ dbi, rgba); _SglThrowErrorOccuried();
                break;
            }
            }
        }
    }
}

// RESOURCE BINDING

_ZGL void _DpuBindBuffers(zglDpu* dpu, afxUnit set, afxUnit first, afxUnit cnt, afxBuffer buffers[], afxUnit const offsets[], afxUnit const ranges[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssertRange(_ZGL_MAX_ENTRY_PER_LEGO, first, cnt);
    AfxAssertRange(_ZGL_MAX_LEGO_PER_BIND, set, 1);

    // deferred because it requires the pipeline ligature info.

    for (afxUnit i = 0; i < cnt; i++)
    {
        afxUnit entryIdx = first + i;
        afxBuffer buf = buffers[i];
        afxUnit32 offset = offsets[i];
        afxUnit32 range = ranges[i];

        dpu->nextLs[set][entryIdx].buf = buf;
        dpu->nextLs[set][entryIdx].offset = offset;
        dpu->nextLs[set][entryIdx].range = range;
        dpu->nextLsUpdMask[set] |= AFX_BIT(entryIdx);
    }
}

_ZGL void _DpuBindRasters(zglDpu* dpu, afxUnit set, afxUnit first, afxUnit cnt, afxRaster rasters[], afxUnit const subras[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssertRange(_ZGL_MAX_ENTRY_PER_LEGO, first, cnt);
    AfxAssertRange(_ZGL_MAX_LEGO_PER_BIND, set, 1);

    // deferred because it requires the pipeline ligature info.

    for (afxUnit i = 0; i < cnt; i++)
    {
        afxUnit entryIdx = first + i;
        afxRaster ras = rasters[i];
        dpu->nextLs[set][entryIdx].ras = ras;
        dpu->nextLs[set][entryIdx].subrasIdx = subras[i];
        dpu->nextLsUpdMask[set] |= AFX_BIT(entryIdx);
    }
}

_ZGL void _DpuBindSamplers(zglDpu* dpu, afxUnit set, afxUnit first, afxUnit cnt, avxSampler samplers[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssertRange(_ZGL_MAX_ENTRY_PER_LEGO, first, cnt);
    AfxAssertRange(_ZGL_MAX_LEGO_PER_BIND, set, 1);

    // deferred because it requires the pipeline ligature info.

    for (afxUnit i = 0; i < cnt; i++)
    {
        afxUnit entryIdx = first + i;
        avxSampler smp = samplers[i];
        dpu->nextLs[set][entryIdx].smp = smp;
        dpu->nextLsUpdMask[set] |= AFX_BIT(entryIdx);
    }
}

_ZGL void _DpuPushConstants(zglDpu* dpu, afxUnit32 offset, afxUnit32 siz, void const* data)
{
    afxError err = AFX_ERR_NONE;

    // deferred because it requires the pipeline ligature info.

    AfxCopy(&dpu->pushConstMappedMem[offset], data, siz);
}

_ZGL void _ZglFlushLsChanges(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    avxLigature liga;

    if (!(liga = dpu->activeLiga))
        return;

    AfxAssertObjects(1, &liga, afxFcc_LIGA);

    // BIND RESOURCES (TEXTURES, SAMPLERS AND BUFFERS)

    for (afxUnit i = 0; i < liga->m.setCnt; i++)
    {
        afxUnit set = liga->m.sets[i].set;
        afxMask updMask = dpu->nextLsUpdMask[set];

        if (!updMask) // skip if has not updates
            continue;

        dpu->nextLsUpdMask[set] = NIL;

        for (afxUnit j = 0; j < liga->m.sets[i].entryCnt; j++)
        {
            if (!(updMask & AFX_BIT(j))) // skip if not changed
                continue;

            afxUnit resIdx = liga->m.sets[i].baseEntry + j;
            avxLigatureEntry const *entry = &liga->m.totalEntries[resIdx];
            AfxAssert(entry->type);
            afxUnit binding = entry->binding;
            afxUnit glUnit = (set * _ZGL_MAX_ENTRY_PER_LEGO) + binding;
            afxBool reqUpd = FALSE, reqUpd2 = FALSE;
            GLuint glHandle = 0, glHandle2 = 0;
            afxSize bufSiz = 0;

            switch (entry->type)
            {
            case avxShaderParam_UNIFORM_UNIT:
            case avxShaderParam_STORAGE_UNIT:
            {
                afxUnit offset = dpu->nextLs[set][binding].offset;
                afxUnit range = dpu->nextLs[set][binding].range;
                afxBuffer buf = dpu->nextLs[set][binding].buf;
                afxBool rebind = FALSE;
                GLenum glTarget = GL_INVALID_ENUM;

                if (entry->type == avxShaderParam_UNIFORM_UNIT)
                    glTarget = GL_UNIFORM_BUFFER;
                else if (entry->type == avxShaderParam_STORAGE_UNIT)
                    glTarget = GL_SHADER_STORAGE_BUFFER;
                else AfxThrowError();

                if ((dpu->activeLs[set][binding].buf != buf) ||
                    (dpu->activeLs[set][binding].offset != offset) ||
                    (dpu->activeLs[set][binding].range != range)
                    )
                {
                    dpu->activeLs[set][binding].buf = buf;
                    dpu->activeLs[set][binding].offset = offset;
                    dpu->activeLs[set][binding].range = range;

                    dpu->nextLs[set][binding].buf = NIL; // force update in "next first time".
                    rebind = TRUE;
                }

                if (!buf)
                {
                    gl->BindBufferBase(glTarget, glUnit, 0); _SglThrowErrorOccuried();
                }
                else
                {
                    DpuBindAndSyncBuf(dpu, glTarget, buf);
                    bufSiz = AfxGetBufferCapacity(buf, 0);

                    if (offset || range)
                    {
                        AfxAssert(range);
                        AfxAssertRange(bufSiz, offset, range);
                        gl->BindBufferRange(glTarget, glUnit, buf->glHandle, offset, AFX_ALIGNED_SIZE(range, AFX_SIMD_ALIGNMENT)); _SglThrowErrorOccuried();
                    }
                    else
                    {
                        gl->BindBufferBase(glTarget, glUnit, buf->glHandle); _SglThrowErrorOccuried();
                    }
                }
                break;
            }
            case avxShaderParam_COMBINED_IMAGE_SAMPLER:
            case avxShaderParam_IMAGE_UNIT:
            case avxShaderParam_SAMPLER_UNIT:
            {
                avxSampler samp = dpu->nextLs[set][binding].smp;
                afxRaster ras = dpu->nextLs[set][binding].ras;
                afxBool rebindRas = FALSE;
                afxBool rebindSamp = FALSE;

                if (dpu->activeLs[set][binding].ras != ras)
                {
                    dpu->activeLs[set][binding].ras = ras;
                    dpu->nextLs[set][binding].ras = NIL; // force update in "next first time".
                    rebindRas = TRUE;
                }

                if (dpu->activeLs[set][binding].smp != samp)
                {
                    dpu->activeLs[set][binding].smp = samp;
                    dpu->nextLs[set][binding].smp = NIL; // force update in "next first time".
                    rebindSamp = TRUE;
                }

                if (entry->type == avxShaderParam_IMAGE_UNIT || entry->type == avxShaderParam_COMBINED_IMAGE_SAMPLER)
                {
                    if (!ras)
                    {
                        gl->ActiveTexture(GL_TEXTURE0 + glUnit); _SglThrowErrorOccuried();
                        gl->BindTexture(GL_TEXTURE_2D, 0); _SglThrowErrorOccuried();
                    }
                    else
                    {
                        DpuBindAndSyncRas(dpu, glUnit, ras);
                        gl->ActiveTexture(GL_TEXTURE0 + glUnit); _SglThrowErrorOccuried();
                        gl->BindTexture(ras->glTarget, ras->glHandle); _SglThrowErrorOccuried();
                    }
                }

                if (entry->type == avxShaderParam_SAMPLER_UNIT || entry->type == avxShaderParam_COMBINED_IMAGE_SAMPLER)
                {
                    if (!samp)
                    {
                        gl->BindSampler(glUnit, 0); _SglThrowErrorOccuried();
                    }
                    else
                    {
                        _DpuBindAndSyncSamp(dpu, glUnit, samp);
                        gl->BindSampler(glUnit, samp->glHandle); _SglThrowErrorOccuried();
                    }
                }
                break;
            }
            case avxShaderParam_FETCH_UNIT:
            {
                afxUnit offset = dpu->nextLs[set][binding].offset;
                afxUnit range = dpu->nextLs[set][binding].range;
                afxBuffer buf = dpu->nextLs[set][binding].buf;
                afxBool rebind = FALSE;
                GLenum glTarget = GL_TEXTURE_BUFFER;

                if ((dpu->activeLs[set][binding].buf != buf) ||
                    (dpu->activeLs[set][binding].offset != offset) ||
                    (dpu->activeLs[set][binding].range != range)
                    )
                {
                    dpu->activeLs[set][binding].buf = buf;
                    dpu->activeLs[set][binding].offset = offset;
                    dpu->activeLs[set][binding].range = range;

                    dpu->nextLs[set][binding].buf = NIL; // force update in "next first time".
                    rebind = TRUE;
                }

                if (!buf)
                {
                    gl->ActiveTexture(glUnit); _SglThrowErrorOccuried();
                    gl->BindBuffer(glTarget, 0); _SglThrowErrorOccuried();
                }
                else
                {
                    gl->ActiveTexture(glUnit); _SglThrowErrorOccuried();
                    DpuBindAndSyncBuf(dpu, glTarget, buf);
                    bufSiz = AfxGetBufferCapacity(buf, 0);
                    gl->BindTexture(glTarget, buf->glTexHandle); _SglThrowErrorOccuried();

                    if (offset || range)
                    {
                        AfxAssert(range);
                        AfxAssertRange(bufSiz, offset, range);

                        if (gl->TexBufferRange)
                        {
                            gl->TexBufferRange(glTarget, buf->glTexIntFmt, buf->glHandle, offset, AFX_ALIGNED_SIZE(range, AFX_SIMD_ALIGNMENT)); _SglThrowErrorOccuried();
                        }
                        else
                        {
                            gl->TexBuffer(glTarget, buf->glTexIntFmt, buf->glHandle); _SglThrowErrorOccuried();
                        }
                    }
                    else
                    {
                        gl->TexBuffer(glTarget, buf->glTexIntFmt, buf->glHandle); _SglThrowErrorOccuried();
                    }
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
}

_ZGL void _ZglFlushVsChanges(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    avxVertexInput vin;

    if (!(vin = dpu->activeVin))
        return;

    AfxAssertObjects(1, &vin, afxFcc_VIN);

    // BIND RESOURCES (VERTEX AND INDEX BUFFERS)

    zglVertexInputState* activeVinBindings = &vin->bindings;
    zglVertexInputState* nextVinBindings = &dpu->nextVinBindings;
    afxMask sourcesUpdMask = nextVinBindings->sourcesUpdMask;

    if (sourcesUpdMask)
    {
        nextVinBindings->sourcesUpdMask = NIL;

        for (afxUnit i = 0; i < vin->m.streamCnt; i++)
        {
            avxVertexInputStream const* stream = &vin->m.streams[i];
            afxUnit streamIdx = stream->slotIdx;
            AfxAssertRange(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, streamIdx, 1);

            if (!(sourcesUpdMask & AFX_BIT(streamIdx))) // skip if has not updates
                continue;

            afxBuffer nextBuf = nextVinBindings->sources[streamIdx].buf;
            afxUnit nextOff = nextVinBindings->sources[streamIdx].offset;
            afxUnit nextRange = nextVinBindings->sources[streamIdx].range;
            afxUnit nextStride = nextVinBindings->sources[streamIdx].stride;
            afxBool forceRebind = FALSE;

            if ((forceRebind) ||
                (nextBuf && ((!nextBuf->glHandle) || (nextBuf->glHandle != activeVinBindings->sources[streamIdx].bufGpuHandle))) || // if it has not a GL handle yet or it is different.
                (activeVinBindings->sources[streamIdx].buf != nextBuf) ||
                (activeVinBindings->sources[streamIdx].offset != nextOff) ||
                (activeVinBindings->sources[streamIdx].range != nextRange) ||
                (activeVinBindings->sources[streamIdx].stride != nextStride))
            {
                if (!nextBuf)
                {
                    AfxAssert(gl->BindVertexBuffer);
                    gl->BindVertexBuffer(streamIdx, 0, 0, 16); _SglThrowErrorOccuried();

                    activeVinBindings->sources[streamIdx].bufGpuHandle = NIL;
                }
                else
                {
                    AfxAssert(nextRange);
                    AfxAssert(nextStride);

                    if (!nextBuf->glHandle)
                        DpuBindAndSyncBuf(dpu, GL_ARRAY_BUFFER, nextBuf);

                    AfxAssert(gl->BindVertexBuffer);
                    gl->BindVertexBuffer(streamIdx, nextBuf->glHandle, nextOff, nextStride); _SglThrowErrorOccuried();

                    activeVinBindings->sources[streamIdx].bufGpuHandle = nextBuf->glHandle;
                }

                activeVinBindings->sources[streamIdx].buf = nextBuf;
                activeVinBindings->sources[streamIdx].offset = nextOff;
                activeVinBindings->sources[streamIdx].range = nextRange;
                activeVinBindings->sources[streamIdx].stride = nextStride;

                nextVinBindings->sources[streamIdx].buf = NIL; // force update in "next first time".
            }
        }
    }

    if (nextVinBindings->iboUpdReq)
    {
        nextVinBindings->iboUpdReq = FALSE;

        afxBuffer nextBuf = nextVinBindings->idxSrcBuf;
        afxUnit nextOff = nextVinBindings->idxSrcOff;
        afxUnit nextRange = nextVinBindings->idxSrcRange;
        afxUnit nextStride = nextVinBindings->idxSrcSiz;
        afxBool forceRebind = FALSE;
        afxBool bound = FALSE;

        if ((forceRebind) ||
            (nextBuf && ((!nextBuf->glHandle) || (nextBuf->glHandle != nextVinBindings->idxSrcGpuHandle))) || // if it has not a GL handle yet or it is different.
            (activeVinBindings->idxSrcBuf != nextBuf) ||
            (activeVinBindings->idxSrcOff != nextOff) ||
            (activeVinBindings->idxSrcRange != nextRange) ||
            (activeVinBindings->idxSrcSiz != nextStride))
        {
            DpuBindAndSyncBuf(dpu, GL_ELEMENT_ARRAY_BUFFER, nextBuf);

            if (!nextBuf)
            {
                //gl->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

                activeVinBindings->idxSrcGpuHandle = NIL;
                activeVinBindings->idxSrcBuf = NIL;
                activeVinBindings->idxSrcOff = 0;
                activeVinBindings->idxSrcRange = 0;
                activeVinBindings->idxSrcSiz = 0;
            }
            else
            {
                //DpuBindAndSyncBuf(dpu, GL_ELEMENT_ARRAY_BUFFER, nextBuf);
                //gl->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->glHandle);

                activeVinBindings->idxSrcGpuHandle = nextBuf->glHandle;
                activeVinBindings->idxSrcBuf = nextBuf;
                activeVinBindings->idxSrcOff = nextOff;
                activeVinBindings->idxSrcRange = nextRange;
                activeVinBindings->idxSrcSiz = nextStride;
            }

            nextVinBindings->idxSrcBuf = NIL; // force update in "next first time".
        }
    }
}

_ZGL void _DpuDraw(zglDpu* dpu, avxDrawIndirectCmd const* data)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    afxUnit cnt;

    //_DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    //dpu->activePip = dpu->nextPip;

    if (!dpu->activePip)
    {
        AfxThrowError();
        return;
    }

    _ZglFlushTsChanges(dpu);
    _ZglFlushRsChanges(dpu);
    _ZglFlushLsChanges(dpu);
    _ZglFlushVsChanges(dpu);

    //if (dpu->nextVtxInStreamUpdMask || dpu->nextVtxInAttribUpdMask || dpu->flushIbb)
        //_SglFlushVertexInputStateChanges(dpu, FALSE);

    //AfxAssert(!dpu->nextVinBindingsMask);
    //AfxAssert(!dpu->nextVtxInAttribUpdMask);
    //AfxAssert(!dpu->flushIbb);

    AfxAssert(dpu->activePip);
    afxUnit32 vtxCnt = data->vtxCnt;
    AfxAssert(vtxCnt);
    //AfxAssert(cmd->instCnt);
    GLenum top = AfxToGlTopology(dpu->activeTs.primTop);

    afxUnit firstInst = data->firstInst;
    afxUnit firstVtx = data->firstVtx;
    afxUnit instCnt = data->instCnt;

#if FORCE_GL_GENERIC_FUNCS
    AfxAssert(gl->DrawArraysInstancedBaseInstance);
    gl->DrawArraysInstancedBaseInstance(top, firstVtx, vtxCnt, instCnt, firstInst); _SglThrowErrorOccuried();
#else
    if (instCnt)
    {
        if (firstInst)
        {
            AfxAssert(gl->DrawArraysInstancedBaseInstance);
            gl->DrawArraysInstancedBaseInstance(top, firstVtx, vtxCnt, instCnt, firstInst); _SglThrowErrorOccuried();
        }
        else
        {
            gl->DrawArraysInstanced(top, firstVtx, vtxCnt, instCnt); _SglThrowErrorOccuried();
        }
    }
    else
    {
        gl->DrawArrays(top, firstVtx, vtxCnt); _SglThrowErrorOccuried();
    }
#endif
    dpu->numOfFedVertices += vtxCnt;
    dpu->numOfFedInstances += instCnt;

    //AfxLogEcho("Geometry drawn. vtxCnt %u, instCnt %u, firstVtx %u, firstInst %u", vtxCnt, instCnt, firstVtx, firstInst);
}

_ZGL void _DpuDrawIndexed(zglDpu* dpu, avxDrawIndexedIndirectCmd const* data)
{
    /*
        When the command is executed, primitives are assembled using the current primitive topology and indexCount vertices whose indices are retrieved from the index buffer.
        The index buffer is treated as an array of tightly packed unsigned integers of size defined by the vkCmdBindIndexStream::indexType parameter with which the buffer was bound.

        The first vertex index is at an offset of firstIndex × indexSize + offset within the bound index buffer, where offset is the offset specified by CmdBindIndexStream and indexSize is the byte size of the type specified by indexType.
        Subsequent index values are retrieved from consecutive locations in the index buffer.
        Indices are first compared to the primitive restart value, then zero extended to 32 bits (if the indexType is VK_INDEX_TYPE_UINT8_EXT or VK_INDEX_TYPE_UINT16) and have vertexOffset added to them, before being supplied as the vertexIndex value.

        The primitives are drawn instanceCount times with instanceIndex starting with firstInstance and increasing sequentially for each instance.
        The assembled primitives execute the bound graphics pipeline.
    */

    afxError err = AFX_ERR_NONE;

    //_DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    //dpu->activePip = dpu->nextPip;

    if (!dpu->activePip)
    {
        AfxThrowError();
        return;
    }

    _ZglFlushTsChanges(dpu);
    _ZglFlushRsChanges(dpu);
    _ZglFlushLsChanges(dpu);
    _ZglFlushVsChanges(dpu);

    //if (dpu->nextVtxInStreamUpdMask || dpu->nextVtxInAttribUpdMask || dpu->flushIbb)
        //_SglFlushVertexInputStateChanges(dpu, TRUE);

    //AfxAssert(!dpu->nextVinBindingsMask);
    //AfxAssert(!dpu->nextVtxInAttribUpdMask);
    //AfxAssert(!dpu->flushIbb);

    AfxAssertObjects(1, &dpu->activeVin->bindings.idxSrcBuf, afxFcc_BUF);
    //AfxAssert(dpu->state.vertexBindingCnt);

    afxUnit idxCnt = data->idxCnt;
    AfxAssert(idxCnt);
    //AfxAssert(cmd->instCnt);

    static const GLenum idxSizGl[] =
    {
        0,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_SHORT,
        0,
        GL_UNSIGNED_INT
    };

    afxSize idxSiz = dpu->activeVin->bindings.idxSrcSiz;
    afxSize idxBaseOff = dpu->activeVin->bindings.idxSrcOff;

    GLint vtxOff2 = data->vtxOff;
    afxUnit32 firstIdx = data->firstIdx;
    afxUnit32 instCnt = data->instCnt;
    afxUnit32 firstInst = data->firstInst;
    afxSize dataOff = (idxBaseOff + (idxSiz * firstIdx));

    GLenum top = AfxToGlTopology(dpu->activeTs.primTop);

    glVmt const* gl = &dpu->gl;

#if FORCE_GL_GENERIC_FUNCS
    AfxAssert(gl->DrawElementsInstancedBaseVertexBaseInstance);
    gl->DrawElementsInstancedBaseVertexBaseInstance(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, instCnt, vtxOff2, firstInst); _SglThrowErrorOccuried();
#else
    if (instCnt)
    {
        if (firstInst)
        {
            AfxAssert(gl->DrawElementsInstancedBaseVertexBaseInstance);
            gl->DrawElementsInstancedBaseVertexBaseInstance(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, instCnt, vtxOff2, firstInst); _SglThrowErrorOccuried();
        }
        else
        {
            if (vtxOff2)
            {
                gl->DrawElementsInstancedBaseVertex(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, instCnt, vtxOff2); _SglThrowErrorOccuried();
            }
            else
            {
                gl->DrawElementsInstanced(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, instCnt); _SglThrowErrorOccuried();
            }
        }
    }
    else
    {
        if (vtxOff2)
        {
            gl->DrawElementsBaseVertex(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, vtxOff2); _SglThrowErrorOccuried();
        }
        else
        {
            gl->DrawElements(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff); _SglThrowErrorOccuried();
        }
    }
#endif

    //dpu->numOfFedVertices += cmd->idxCnt;
    dpu->numOfFedIndices += idxCnt;
    dpu->numOfFedInstances += instCnt;

    //AfxLogEcho("Indexed geometry drawn. idxCnt %u, instCnt %u, baseIdx %u, vtxOff %u, baseInst %u", idxCnt, instCnt, firstIdx, vtxOff, firstInst);
}

_ZGL void _DpuDrawIndirect(zglDpu* dpu, afxBuffer buf, afxUnit32 offset, afxUnit32 drawCnt, afxUnit32 stride)
{
    afxError err = AFX_ERR_NONE;

    //_DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    //dpu->activePip = dpu->nextPip;

    if (!dpu->activePip)
    {
        AfxThrowError();
        return;
    }

    _ZglFlushTsChanges(dpu);
    _ZglFlushRsChanges(dpu);
    _ZglFlushLsChanges(dpu);
    _ZglFlushVsChanges(dpu);

    glVmt const* gl = &dpu->gl;

    GLenum top = AfxToGlTopology(dpu->activeTs.primTop);
    void* offPtr = (void*)offset;

    AfxAssertObjects(1, &buf, afxFcc_BUF);
    DpuBindAndSyncBuf(dpu, GL_DRAW_INDIRECT_BUFFER, buf);

#if FORCE_GL_GENERIC_FUNCS
    AfxAssert(gl->MultiDrawArraysIndirect);
    gl->MultiDrawArraysIndirect(top, offPtr, drawCnt, stride); _SglThrowErrorOccuried();
#else
    if (1 >= drawCnt)
    {
        gl->DrawArraysIndirect(top, offPtr); _SglThrowErrorOccuried();
    }
    else
    {
        AfxAssert(gl->MultiDrawArraysIndirect);
        gl->MultiDrawArraysIndirect(top, offPtr, drawCnt, stride); _SglThrowErrorOccuried();
    }
#endif
}

_ZGL void _DpuDrawIndirectCount(zglDpu* dpu, afxBuffer buf, afxUnit32 offset, afxBuffer cntBuf, afxUnit32 cntBufOff, afxUnit32 maxDrawCnt, afxUnit32 stride)
{
    afxError err = AFX_ERR_NONE;

    //_DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    //dpu->activePip = dpu->nextPip;


    if (!dpu->activePip)
    {
        AfxThrowError();
        return;
    }

    _ZglFlushTsChanges(dpu);
    _ZglFlushRsChanges(dpu);
    _ZglFlushLsChanges(dpu);
    _ZglFlushVsChanges(dpu);

    glVmt const* gl = &dpu->gl;

    GLenum top = AfxToGlTopology(dpu->activeTs.primTop);
    void* offPtr = (void*)offset;

    AfxAssertObjects(1, &buf, afxFcc_BUF);
    DpuBindAndSyncBuf(dpu, GL_DRAW_INDIRECT_BUFFER, buf);
    AfxAssertObjects(1, &cntBuf, afxFcc_BUF);
    DpuBindAndSyncBuf(dpu, GL_PARAMETER_BUFFER, cntBuf);

    /*
    The command void MultiDrawArraysIndirectCount( enum mode, const void *indirect, intptr drawcount, intptr maxdrawcount, sizei stride );
    behaves similarly to MultiDrawArraysIndirect, except that drawcount defines an offset (in bytes) into the buffer object bound to the PARAMETER_BUFFER
    binding point at which a single sizei typed value is stored, which contains the draw count.
    maxdrawcount specifies the maximum number of draws that are expected to be stored in the buffer. If the value stored at drawcount into the buffer is greater
    than maxdrawcount, the implementation stops processing draws after maxdrawcount parameter sets. drawcount must be a multiple of four.

    Errors
    In addition to errors that would be generated by MultiDrawArraysIndirect:
    An INVALID_OPERATION error is generated if no buffer is bound to the PARAMETER_BUFFER binding point.
    An INVALID_VALUE error is generated if drawcount is not a multiple of four.
    An INVALID_OPERATION error is generated if reading a sizei typed value from the buffer bound to the PARAMETER_BUFFER target at the offset specified by drawcount would result in an out-of-bounds access.
    */

    AfxAssert(gl->MultiDrawArraysIndirectCount);
    gl->MultiDrawArraysIndirectCount(top, offPtr, cntBufOff, maxDrawCnt, stride); _SglThrowErrorOccuried();
}

_ZGL void _DpuDrawIndexedIndirect(zglDpu* dpu, afxBuffer buf, afxUnit32 offset, afxUnit32 drawCnt, afxUnit32 stride)
{
    afxError err = AFX_ERR_NONE;

    //_DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    //dpu->activePip = dpu->nextPip;

    if (!dpu->activePip)
    {
        AfxThrowError();
        return;
    }

    _ZglFlushTsChanges(dpu);
    _ZglFlushRsChanges(dpu);
    _ZglFlushLsChanges(dpu);
    _ZglFlushVsChanges(dpu);

    glVmt const* gl = &dpu->gl;

    static const GLenum idxSizGl[] =
    {
        0,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_SHORT,
        0,
        GL_UNSIGNED_INT
    };
    afxSize idxSiz = dpu->activeVin->bindings.idxSrcSiz;
    GLenum top = AfxToGlTopology(dpu->activeTs.primTop);
    void* offPtr = (void*)offset;

    AfxAssertObjects(1, &buf, afxFcc_BUF);
    DpuBindAndSyncBuf(dpu, GL_DRAW_INDIRECT_BUFFER, buf);

#if FORCE_GL_GENERIC_FUNCS
    AfxAssert(gl->MultiDrawElementsIndirect);
    gl->MultiDrawElementsIndirect(top, idxSizGl[idxSiz], offPtr, drawCnt, stride); _SglThrowErrorOccuried();
#else
    if (1 >= drawCnt)
    {
        gl->DrawElementsIndirect(top, idxSizGl[idxSiz], offPtr); _SglThrowErrorOccuried();
    }
    else
    {
        AfxAssert(gl->MultiDrawElementsIndirect);
        gl->MultiDrawElementsIndirect(top, idxSizGl[idxSiz], offPtr, drawCnt, stride); _SglThrowErrorOccuried();
    }
#endif
}

_ZGL void _DpuDrawIndexedIndirectCount(zglDpu* dpu, afxBuffer buf, afxUnit32 offset, afxBuffer cntBuf, afxUnit32 cntBufOff, afxUnit32 maxDrawCnt, afxUnit32 stride)
{
    afxError err = AFX_ERR_NONE;

    //_DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    //dpu->activePip = dpu->nextPip;

    if (!dpu->activePip)
    {
        AfxThrowError();
        return;
    }

    _ZglFlushTsChanges(dpu);
    _ZglFlushRsChanges(dpu);
    _ZglFlushLsChanges(dpu);
    _ZglFlushVsChanges(dpu);

    glVmt const* gl = &dpu->gl;

    static const GLenum idxSizGl[] =
    {
        0,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_SHORT,
        0,
        GL_UNSIGNED_INT
    };
    afxSize idxSiz = dpu->activeVin->bindings.idxSrcSiz;
    GLenum top = AfxToGlTopology(dpu->activeTs.primTop);
    void* offPtr = (void*)offset;

    AfxAssertObjects(1, &buf, afxFcc_BUF);
    DpuBindAndSyncBuf(dpu, GL_DRAW_INDIRECT_BUFFER, buf);
    AfxAssertObjects(1, &cntBuf, afxFcc_BUF);
    DpuBindAndSyncBuf(dpu, GL_PARAMETER_BUFFER, cntBuf);

    /*
    The command void MultiDrawElementsIndirectCount( enum mode, enum type, const void *indirect, intptr drawcount, sizei maxdrawcount, sizei stride );
    behaves similarly to MultiDrawElementsIndirect, except that drawcount defines an offset (in bytes) into the buffer object bound to the PARAMETER_BUFFER
    binding point at which a single sizei typed value is stored, which contains the draw count. 
    maxdrawcount specifies the maximum number of draws that are expected to be stored in the buffer. 
    If the value stored at drawcount into the buffer is greater than maxdrawcount, the implementation stops processing draws after maxdrawcount parameter sets. 
    drawcount must be a multiple of four.
        
    Errors
    In addition to errors that would be generated by MultiDrawElementsIndirect:
    An INVALID_OPERATION error is generated if no buffer is bound to the PARAMETER_BUFFER binding point.
    An INVALID_VALUE error is generated if drawcount is not a multiple of four.
    An INVALID_VALUE error is generated if maxdrawcount is negative.
    An INVALID_OPERATION error is generated if reading a sizei typed value
    from the buffer bound to the PARAMETER_BUFFER target at the offset specified by drawcount would result in an out-of-bounds access.
    */

    AfxAssert(gl->MultiDrawElementsIndirectCount);
    gl->MultiDrawElementsIndirectCount(top, idxSizGl[idxSiz], offPtr, cntBufOff, maxDrawCnt, stride); _SglThrowErrorOccuried();
}

_ZGL void _DpuDispatch(zglDpu* dpu, afxUnit grpCntX, afxUnit grpCntY, afxUnit grpCntZ)
{
    afxError err = AFX_ERR_NONE;

    //_DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    //dpu->activePip = dpu->nextPip;

    if (!dpu->activePip)
    {
        AfxThrowError();
        return;
    }

    _ZglFlushLsChanges(dpu);

    glVmt const* gl = &dpu->gl;

    AfxAssert(gl->DispatchCompute);
    gl->DispatchCompute(grpCntX, grpCntY, grpCntZ); _SglThrowErrorOccuried();
}

_ZGL void _DpuDispatchIndirect(zglDpu* dpu, afxBuffer buf, afxUnit32 offset)
{
    afxError err = AFX_ERR_NONE;

    //_DpuBindAndSyncPip(dpu, (dpu->activePip != dpu->nextPip), TRUE, dpu->nextPip);
    //dpu->activePip = dpu->nextPip;

    if (!dpu->activePip)
    {
        AfxThrowError();
        return;
    }

    _ZglFlushLsChanges(dpu);

    glVmt const* gl = &dpu->gl;

    AfxAssertObjects(1, &buf, afxFcc_BUF);
    DpuBindAndSyncBuf(dpu, GL_DISPATCH_INDIRECT_BUFFER, buf);

    AfxAssert(gl->DispatchComputeIndirect);
    gl->DispatchComputeIndirect(offset); _SglThrowErrorOccuried();
}

_ZGL void _DpuCmdUpdateUniformVector(zglDpu* dpu, _zglCmdUniformVectorEXT const* cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    GLuint loc = gl->GetUniformLocation(dpu->activePip->glHandle, cmd->name);
    gl->Uniform4fv(loc, cmd->cnt, cmd->v);
}

_ZGL void _DpuCmdUpdateUniformMatrix(zglDpu* dpu, _zglCmdUniformMatrixEXT const* cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    GLuint loc = gl->GetUniformLocation(dpu->activePip->glHandle, cmd->name);
    //gl->UniformMatrix4fv(loc, cmd->cnt, GL_FALSE, cmd->m);
    AfxThrowError();
}

_ZGL void _DpuEndOfCommands(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    dpu->nextVin = NIL;
    dpu->nextPip = NIL;

    for (afxUnit i = 0; i < _ZGL_MAX_LEGO_PER_BIND; i++)
    {
        for (afxUnit j = 0; j < _ZGL_MAX_ENTRY_PER_LEGO; j++)
        {
            dpu->nextLs[j][i].buf = NIL;
            dpu->nextLs[j][i].ras = NIL;
            dpu->nextLs[j][i].smp = NIL;
        }
    }

    for (afxUnit i = 0; i < ZGL_MAX_VERTEX_ATTRIB_BINDINGS; i++)
    {
        dpu->nextVinBindings.sources[i].buf = NIL;
    }
    dpu->nextVinBindings.idxSrcBuf = NIL;
}

// QUEUE STUFF

_ZGL afxError _DpuExecSubmCallback(zglDpu* dpu, afxDrawBridge dexu, afxUnit queIdx, _zglQueueingWork* subm)
{
    afxError err = AFX_ERR_NONE;
    subm->subm.f(dpu, dexu, queIdx, subm->subm.udd);
    return err;
}

_ZGL afxError _DpuExecSubm(zglDpu* dpu, afxDrawBridge dexu, afxUnit queIdx, _zglQueueingExecute* subm)
{
    afxError err = AFX_ERR_NONE;
    //AfxAssertObject(dexu, afxFcc_DEXU);
    //glVmt const* gl = &thdIdd->wglVmt;

    for (afxUnit i = 0; i < subm->cmdbCnt; i++)
    {
        avxCmdb cmdb = subm->cmdbs[i];
        AfxAssertObjects(1, &cmdb, afxFcc_CMDB);

        if (cmdb->m.state == avxCmdbState_PENDING)
        {
            AfxAssert(cmdb->m.state == avxCmdbState_PENDING);

            _zglCmd * cmdHdr;
            AfxChainForEveryLinkageB2F(&cmdb->commands, _zglCmd, script, cmdHdr)
            {
                if (cmdHdr->id == NIL/*ZGL_CMD_END*/)
                {
                    _DpuEndOfCommands(dpu);
                    break;
                }

                if (cmdb->m.state != avxCmdbState_PENDING)
                {
                    AfxThrowError();
                    break;
                }

                AfxAssertRange(ZGL_CMD_TOTAL, cmdHdr->id, 1);

                if (err)
                {
                    int a = 0;
                }

                // This shit fucks the CPU prediction. Anyway, we will just ignore it for now.

                _SglDecodeCmdVmt.f[cmdHdr->id](dpu, cmdHdr);
            }

            if (!err)
            {
                cmdb->m.state = avxCmdbState_EXECUTABLE;
            }

            if (err || cmdb->m.disposable)
            {
                AfxAssert(cmdb->m.portId == dpu->portId);
                cmdb->m.state = avxCmdbState_INVALID;
                afxDrawQueue dque = AfxGetProvider(cmdb);
                AfxAssertObjects(1, &dque, afxFcc_DQUE);

                afxUnit poolIdx = cmdb->m.poolIdx;

                AfxEnterSlockExclusive(&dque->m.cmdbReqLock);

                if (AfxPushQueue(&dque->m.cmdbRecycQue, &cmdb))
                {
                    AfxReleaseObjects(1, (void**)&cmdb);
                }

                AfxExitSlockExclusive(&dque->m.cmdbReqLock);
            }
        }
        AfxDecAtom32(&cmdb->m.submCnt);
        AfxReleaseObjects(1, &cmdb);
    }
    return err;
}

_ZGL afxError _DpuExecuteSubmMmap(zglDpu* dpu, afxDrawBridge dexu, afxUnit queIdx, _zglQueueingMmap* subm)
{
    afxError err = AFX_ERR_NONE;
    
    if (_DpuRemapBuf(dpu, subm->buf, subm->off, subm->ran, subm->flags, subm->placeholder))
        AfxThrowError();

    AfxDecAtom32(&subm->buf->m.pendingRemap);
    AfxReleaseObjects(1, &subm->buf);

    return err;
}

_ZGL afxError _DpuExecSubmTransfer(zglDpu* dpu, afxDrawBridge dexu, afxUnit queIdx, _zglQueueingTransfer* subm)
{
    afxError err = AFX_ERR_NONE;
    //AfxAssertObject(dexu, afxFcc_DEXU);

    switch (subm->srcFcc)
    {
    case NIL:
    {
        switch (subm->dstFcc)
        {
        case afxFcc_BUF: // raw to buf
            if (_DpuLoadBuf2(dpu, subm->data.buf, subm->data.src, subm->itemCnt, subm->items.bufIo))
                AfxThrowError();

            AfxReleaseObjects(1, &subm->data.buf);
            break;
        case afxFcc_RAS: // raw to ras
            if (_DpuUnpackRasFromMem(dpu, subm->img.ras, subm->img.src, subm->itemCnt, subm->items.rasIo))
                AfxThrowError();

            AfxReleaseObjects(1, &subm->img.ras);
            break;
        default: AfxThrowError(); break;
        }
        break;
    }
    case afxFcc_BUF:
    {
        switch (subm->dstFcc)
        {
        case NIL: // buf to raw
            if (_DpuStoreBuf2(dpu, subm->data.buf, subm->data.dst, subm->itemCnt, subm->items.bufIo))
                AfxThrowError();

            AfxReleaseObjects(1, &subm->data.buf);
            break;
        case afxFcc_RAS:
            if (_DpuUnpackRasFromBuf(dpu, subm->img.ras, subm->img.buf, subm->itemCnt, subm->items.rasIo))
                AfxThrowError();

            AfxReleaseObjects(1, &subm->img.ras);
            AfxReleaseObjects(1, &subm->img.buf);
            break;
        case afxFcc_IOB: // buf to iob
            if (_DpuOutputBuf2(dpu, subm->data.buf, subm->data.iob, subm->itemCnt, subm->items.bufIo))
                AfxThrowError();

            AfxReleaseObjects(1, &subm->data.buf);
            AfxReleaseObjects(1, &subm->data.iob);
            break;
        default: AfxThrowError(); break;
        }
        break;
    }
    case afxFcc_RAS:
    {
        switch (subm->dstFcc)
        {
        case NIL: // ras to raw
            if (_DpuPackRasToMem(dpu, subm->img.ras, subm->img.dst, subm->itemCnt, subm->items.rasIo))
                AfxThrowError();

            AfxReleaseObjects(1, &subm->img.ras);
            break;
        case afxFcc_BUF:
            if (_DpuPackRasToBuf(dpu, subm->img.ras, subm->img.buf, subm->itemCnt, subm->items.rasIo))
                AfxThrowError();

            AfxReleaseObjects(1, &subm->img.ras);
            AfxReleaseObjects(1, &subm->img.buf);
            break;
        case afxFcc_IOB: // ras to iob
            if (_DpuPackRasToIob(dpu, subm->img.ras, subm->img.iob, subm->itemCnt, subm->items.rasIo))
                AfxThrowError();

            AfxReleaseObjects(1, &subm->img.ras);
            AfxReleaseObjects(1, &subm->img.iob);
            break;
        default: AfxThrowError(); break;
        }
        break;
    }
    case afxFcc_IOB:
    {
        switch (subm->dstFcc)
        {
        case afxFcc_BUF: // iob to buf
            if (_DpuInputBuf2(dpu, subm->data.buf, subm->data.iob, subm->itemCnt, subm->items.bufIo))
                AfxThrowError();

            AfxReleaseObjects(1, &subm->data.buf);
            AfxReleaseObjects(1, &subm->data.iob);
            break;
        case afxFcc_RAS: // iob to ras
            if (_DpuUnpackRasFromIob(dpu, subm->img.ras, subm->img.iob, subm->itemCnt, subm->items.rasIo))
                AfxThrowError();

            AfxReleaseObjects(1, &subm->img.ras);
            AfxReleaseObjects(1, &subm->img.iob);
            break;
        default: AfxThrowError(); break;
        }
        break;
    }
    default: AfxThrowError(); break;
    }
    return err;
}

#if 0
_ZGL afxError _DpuExecSubmStamp(zglDpu* dpu, afxDrawBridge dexu, afxUnit queIdx, _zglQueueingStamp* subm)
{
    afxError err = AFX_ERR_NONE;

    for (afxUnit itemIdx = 0; itemIdx < subm->itemCnt; itemIdx++)
    {
        afxDrawOutput dout = subm->items[itemIdx].dout;
        AfxAssertObjects(1, &dout, afxFcc_DOUT);
        afxUnit outBufIdx = subm->items[itemIdx].bufIdx;

        afxDrawContext dctx = dpu->activeDctx;
        AfxAssertObjects(1, &dctx, afxFcc_DCTX);

        afxWhd whd;
        avxCanvas canv;
        AfxGetDrawOutputCanvases(dout, outBufIdx, 1, &canv);
        AfxGetCanvasExtent(canv, whd);

        afxRect area = { 0 };
        area.extent[0] = whd[0];
        area.extent[1] = whd[1];
        avxDrawTarget const dt = { .loadOp = avxLoadOp_LOAD,.storeOp = avxStoreOp_STORE };
        _DpuCommenceDrawScope(dpu, canv, &area, whd[2], 1, &dt, NIL, NIL, FALSE);

        afxViewport vp;
        vp.extent[0] = whd[0];
        vp.extent[1] = whd[1];
        vp.depth[1] = 1;
        _DpuSetViewports(dpu, 0, 1, &vp);

        _DpuBindRasterizer(dpu, dctx->fntRazr, NIL, NIL);

        akxViewConstants vc;
        AfxM4dReset(vc.v);
        AfxComputeOffcenterOrthographicMatrix(vc.p, 0, vp.extent[0], 0, vp.extent[1], -1.f, 1.f, &AVX_CLIP_SPACE_OPENGL);
        //AfxComputeBasicOrthographicMatrix(vc.p, vp.extent[0] / vp.extent[1], 1.0, 3.0, &AVX_CLIP_SPACE_OPENGL);
        DpuBufRw(dpu, dctx->fntUnifBuf, 0, sizeof(vc), FALSE, &vc);

        _DpuBindBuffers(dpu, 0, 0, 1, &dctx->fntUnifBuf, (afxUnit const[]) {0}, (afxUnit const[]) { 0 });

        _DpuBindSamplers(dpu, 0, 1, 1, &dctx->fntSamp);
        _DpuBindRasters(dpu, 0, 1, 1, &dctx->fntRas, NIL);

        afxReal x = subm->origin[0];
        afxReal y = subm->origin[1];
        afxUnit numchar = subm->caption.str.len;
        afxReal r = 1, g = 1, b = 1;
        afxReal x2 = x;

        afxReal* verts;
        _DpuRemapBuf(dpu, dctx->fntDataBuf, 0, AfxGetBufferCapacity(dctx->fntDataBuf, 0), NIL, (void**)&verts);
        //afxReal* verts = (void*)dctx->fntDataBuf->m.bytemap;// AfxMapBuffer(dctx->fntDataBuf, 0, 2048, afxBufferFlag_W, TRUE); // TODO map directly
        void const* bufStart = verts;

        for (char const *ptr = subm->caption.str.start, i = 0; *ptr != '\0'; ptr++)
        {
            // Decrement 'y' for any CR's
            if (*ptr == '\n')
            {
                x = x2;
                y -= 16;
                continue;
            }

#if !0
            // Just advance spaces instead of rendering empty quads
            if (*ptr == ' ')
            {
                x += 16;
                numchar--;
                continue;
            }
#endif
            // Emit position, atlas offset, and color for this character
            *verts++ = x;
            *verts++ = y;
            *verts++ = (float)(*ptr % 16)*0.0625f; // *verts++ = (float)(*ptr % 16)*0.0625f;
            *verts++ = 1.0f - (float)(*ptr / 16)*0.0625f; // *verts++ = 1.0f - (float)(*ptr / 16)*0.0625f;
            //*verts++ = r;
            //*verts++ = g;
            //*verts++ = b;
            *verts++ = 1.f;

            // Advance one character
            x += 16;
        }

        _DpuRemapBuf(dpu, dctx->fntDataBuf, 0, 0, NIL, NIL);
        //AfxUnmapBuffer(dctx->fntDataBuf, FALSE);

        zglBufferInfo bufi = { 0 };
        bufi.buf = dctx->fntDataBuf;
        bufi.stride = 20;
        _DpuBindVertexSources(dpu, 0, 1, &bufi);
        
        avxDrawIndirectCmd dic;
        dic.vtxCnt = 4;
        dic.firstInst = 0;
        dic.firstVtx = 0;
        dic.instCnt = numchar;
        _DpuDraw(dpu, &dic);

        _DpuConcludeDrawScope(dpu);
        AfxDecAtom32(&dout->submCnt);
    }

    return err;
}
#endif

_ZGL void* _AfxDqueRequestArenaSpace(afxDrawQueue dque, afxUnit siz)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dque, afxFcc_DQUE);
    //AfxEnterSlockExclusive(&dque->m.workArenaSlock);
    void *block = AfxAllocateArena(&dque->m.workArena, siz);

    if (!block)
        AfxThrowError();

    //AfxExitSlockExclusive(&dque->m.workArenaSlock);
    return block;
}

_ZGL void _AfxDqueRecycleArenaSpace(afxDrawQueue dque, void *block, afxUnit siz)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dque, afxFcc_DQUE);
    //AfxEnterSlockExclusive(&dque->m.workArenaSlock);
    AfxAssert(block);
    AfxRecycleArena(&dque->m.workArena, block, siz);
    //AfxExitSlockExclusive(&dque->m.workArenaSlock);
}

_ZGL afxError _SglDqueRequestSubmCb(afxDrawQueue dque, afxFence fenc, afxUnit cnt, avxQueueOp const req[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dque, afxFcc_DQUE);
    AfxAssert(cnt);
    AfxAssert(req);

    while (1)
    {
        if (AfxTryLockMutex(&dque->m.workChnMtx))
        {
            for (afxUnit i = 0; i < cnt; i++)
            {
                _zglQueueingWork* subm;
                afxUnit queuingSiz = 0;

                switch (req[i].submType)
                {
                case 0:
                {
                    queuingSiz = sizeof(*subm);
                    subm = _AfxDqueRequestArenaSpace(dque, queuingSiz);
                    *subm = (_zglQueueingWork) { 0 };
                    subm->hdr.siz = queuingSiz;
                    AfxGetTime(&subm->hdr.pushTime);
                    subm->hdr.exec = (void*)_DpuExecSubmCallback;
                    subm->subm.f = req[i].f;
                    subm->subm.udd = req[i].udd;
                    break;
                }
                case avxQueueOp_MAPPING:
                {
                    queuingSiz = sizeof(*subm) + sizeof(((avxQueueOpData*)0)->map);
                    subm = _AfxDqueRequestArenaSpace(dque, queuingSiz);
                    *subm = (_zglQueueingWork) { 0 };
                    subm->hdr.siz = queuingSiz;
                    AfxGetTime(&subm->hdr.pushTime);
                    subm->hdr.exec = (void*)_DpuExecuteSubmMmap;
                    avxQueueOpData* data = &subm->data[0];
                    data->map = req[i].data->map;

                    for (afxUnit j = 0; j < data->present.bufCnt; j++)
                    {
                        AfxIncAtom32(&data->map.buf->m.pendingRemap);
                    }
                    break;
                }
                default:
                {
                    AfxThrowError();
                    break;
                }
                };

                if (!err)
                    AfxPushLinkage(&subm->hdr.chain, &dque->m.workChn);
            }

            AfxUnlockMutex(&dque->m.workChnMtx);
            _SglDexuPingCb(dque->m.dexu, 0);
            break;
        }
    }
    return err;
}

_ZGL afxError _SglDqueEnqueueExecuteCb(afxDrawQueue dque, avxSubmission const* ctrl, afxUnit cnt, avxCmdb cmdbs[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dque, afxFcc_DQUE);
    AfxAssert(cnt);
    AfxAssert(cmdbs);

    if (AfxTryLockMutex(&dque->m.workChnMtx))
    {
        _zglQueueingExecute* subm;
        afxUnit queuingSiz = sizeof(*subm) + (cnt * sizeof(subm->cmdbs[0]));
        subm = _AfxDqueRequestArenaSpace(dque, queuingSiz);
        *subm = (_zglQueueingExecute) { 0 };
        subm->hdr.siz = queuingSiz;
        AfxGetTime(&subm->hdr.pushTime);
        subm->hdr.exec = (void*)_DpuExecSubm;
        subm->cmdbCnt = cnt;

        for (afxUnit i = 0; i < cnt; i++)
        {
            avxCmdb cmdb = cmdbs[i];
            AfxAssertObjects(1, &cmdb, afxFcc_CMDB);

            AfxReacquireObjects(1, &cmdb);
            subm->cmdbs[i] = cmdb;
            AfxIncAtom32(&subm->cmdbs[i]->m.submCnt);
            subm->cmdbs[i]->m.state = avxCmdbState_PENDING;
        }

        AfxPushLinkage(&subm->hdr.chain, &dque->m.workChn);
        AfxUnlockMutex(&dque->m.workChnMtx);
        _SglDexuPingCb(dque->m.dexu, 0);
    }
    return err;
}

_ZGL afxError _SglDqueEnqueueTransferCb(afxDrawQueue dque, avxTransference const* ctrl, afxUnit opCnt, void const* ops)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dque, afxFcc_DQUE);
    AfxAssert(opCnt);
    AfxAssert(ops);

    if (AfxTryLockMutex(&dque->m.workChnMtx))
    {
        _zglQueueingTransfer* subm;
        afxUnit queuingSiz = sizeof(*subm) + (opCnt * AfxMax(sizeof(subm->items.bufIo[0]), sizeof(subm->items.rasIo[0])));
        subm = _AfxDqueRequestArenaSpace(dque, queuingSiz);
        *subm = (_zglQueueingTransfer) { 0 };
        subm->hdr.siz = queuingSiz;
        AfxGetTime(&subm->hdr.pushTime);
        subm->hdr.exec = (void*)_DpuExecSubmTransfer;

        subm->srcFcc = ctrl->srcFcc;
        subm->dstFcc = ctrl->dstFcc;
        subm->codec = ctrl->codec;
        subm->decSiz = ctrl->decSiz;

        subm->itemCnt = opCnt;

        if ((ctrl->srcFcc == afxFcc_RAS) || (ctrl->dstFcc == afxFcc_RAS))
        {
            AfxReacquireObjects(1, &ctrl->img.ras);
            
            if (ctrl->srcFcc == afxFcc_BUF || ctrl->dstFcc == afxFcc_BUF)
                AfxReacquireObjects(1, &ctrl->img.buf);
            else if (ctrl->srcFcc == afxFcc_IOB || ctrl->dstFcc == afxFcc_IOB)
                AfxReacquireObjects(1, &ctrl->img.iob);
        }
        else if ((ctrl->srcFcc == afxFcc_BUF) || (ctrl->dstFcc == afxFcc_BUF))
        {
            AfxReacquireObjects(1, &ctrl->data.buf);

            if (ctrl->srcFcc == afxFcc_IOB || ctrl->dstFcc == afxFcc_IOB)
                AfxReacquireObjects(1, &ctrl->data.iob);
        }

        for (afxUnit i = 0; i < opCnt; i++)
        {
            if (ctrl->srcFcc == afxFcc_RAS || ctrl->dstFcc == afxFcc_RAS)
            {
                AfxCopy(&subm->img, &ctrl->img, sizeof(ctrl->img));
                afxRasterIo const* riops = ops;
                subm->items.rasIo[i] = riops[i];
            }
            else
            {
                AfxAssert((ctrl->srcFcc == afxFcc_BUF || ctrl->dstFcc == afxFcc_BUF));
                AfxCopy(&subm->data, &ctrl->data, sizeof(ctrl->data));
                afxBufferIo const* biops = ops;
                subm->items.bufIo[i] = biops[i];
            }
        }

        AfxPushLinkage(&subm->hdr.chain, &dque->m.workChn);
        AfxUnlockMutex(&dque->m.workChnMtx);
        _SglDexuPingCb(dque->m.dexu, 0);
    }
    return err;
}

_ZGL afxError _SglDqueEnqueueMmapCb(afxDrawQueue dque, afxBuffer buf, afxSize off, afxUnit ran, afxFlags flags, void** placeholder)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dque, afxFcc_DQUE);

    if (AfxTryLockMutex(&dque->m.workChnMtx))
    {
        _zglQueueingMmap* subm;
        afxUnit queuingSiz = sizeof(*subm);
        subm = _AfxDqueRequestArenaSpace(dque, queuingSiz);
        *subm = (_zglQueueingMmap) { 0 };
        subm->hdr.siz = queuingSiz;
        subm->hdr.exec = (void*)_DpuExecuteSubmMmap;
        AfxGetTime(&subm->hdr.pushTime);

        subm->buf = buf;
        subm->off = off;
        subm->ran = ran;
        subm->flags = flags;
        subm->placeholder = placeholder;


        AfxReacquireObjects(1, &buf);
        AfxIncAtom32(&buf->m.pendingRemap);

        AfxPushLinkage(&subm->hdr.chain, &dque->m.workChn);
        AfxUnlockMutex(&dque->m.workChnMtx);

        _SglDexuPingCb(dque->m.dexu, 0);
    }
    else
        err = 1;

    return err;
}

_ZGL afxError _SglDqueCtorCb(afxDrawQueue dque, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dque, afxFcc_DQUE);

    afxDrawContext dctx = args[0];
    AfxAssertObjects(1, &dctx, afxFcc_DCTX);
    afxDrawBridge dexu = args[1];
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);
    afxDrawBridgeConfig const *cfg = ((afxDrawBridgeConfig const *)args[2]);
    AfxAssert(cfg);

    if (_AvxDqueStdImplementation.ctor(dque, args, invokeNo)) AfxThrowError();
    else
    {
        AfxDeregisterClass(&dque->m.cmdbCls);
        afxClassConfig tmpClsCfg = _SglCmdbMgrCfg;
        tmpClsCfg.fixedSiz = sizeof(AFX_OBJECT(avxCmdb)),
        AfxRegisterClass(&dque->m.cmdbCls, NIL, &dque->m.classes, &tmpClsCfg);

        dque->m.submCb = _SglDqueRequestSubmCb;
        dque->m.submitCb = _SglDqueEnqueueExecuteCb;
        dque->m.transferCb = _SglDqueEnqueueTransferCb;
    }
    return err;
}

_ZGL afxBool _DexuProcCb(afxDrawBridge dexu, afxThread thr)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);
    AfxAssertObjects(1, &thr, afxFcc_THR);

    afxDrawContext dctx = AfxGetDrawBridgeContext(dexu);
    afxUnit portId = dexu->m.portId;
    zglDpu *dpu = &dexu->dpu;

    AfxAssert(portId == dexu->m.portId);

    for (afxUnit queIdx = 0; queIdx < dexu->m.queCnt; queIdx++)
    {
        afxDrawQueue dque;

        if (!AfxGetDrawQueues(dctx, dexu->m.baseQueIdx + queIdx, 1, &dque))
            continue;

        AfxAssertObjects(1, &dque, afxFcc_DQUE);

        if (AfxTryLockMutex(&dque->m.workChnMtx))
        {
            afxBool ctxEntered = FALSE;

            if (!ctxEntered)
            {
                HGLRC rc = wglGetCurrentContextSIG();

                if (rc == dexu->hRC) ctxEntered = TRUE;
                else
                {
                    if (!(ctxEntered = !!wglMakeContextCurrentSIG(dexu->hDC, dexu->hDC, dexu->hRC)))
                    {
                        AfxThrowError();
                        AfxLogError("DPU %u failed to take active context.", portId);
                    }
                }

                if (ctxEntered)
                    dpu->activeDctx = dctx;
            }

            if (ctxEntered)
            {
                dpu->numOfFedVertices = 0;
                dpu->numOfFedIndices = 0;
                dpu->numOfFedInstances = 0;

                _zglQueueing* subm;
                AfxChainForEveryLinkageB2F(&dque->m.workChn, _zglQueueing, chain, subm)
                {
                    AfxAssert(dque->m.workChn.cnt);
                    AfxGetTime(&subm->pullTime);
                    subm->exec(dpu, dexu, queIdx, subm);
                    AfxGetTime(&subm->complTime);

                    AfxPopLinkage(&subm->chain);
                    _AfxDqueRecycleArenaSpace(dque, subm, subm->siz);
                }
            }

            AfxUnlockMutex(&dque->m.workChnMtx);
            AfxSignalCondition(&dque->m.idleCnd);
        }
    }
    return TRUE;
}

_ZGL afxInt DexuThrProc(afxDrawBridge dexu)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);

    afxDrawDevice ddev = AfxGetDrawBridgeDevice(dexu);

    zglDpu* dpu = &dexu->dpu;
    glVmt const* gl = &dpu->gl;

    wglMakeContextCurrentSIG(dexu->hDC, dexu->hDC, dexu->hRC);

    AfxMakeString(&dexu->subsysName, 0, (afxChar const*)gl->GetString(GL_RENDERER), 0); _SglThrowErrorOccuried();
    AfxMakeString(&dexu->subsysVer, 0, (afxChar const*)gl->GetString(GL_VERSION), 0); _SglThrowErrorOccuried();

    AfxDbgLogf(6, NIL, "SIGMA GL/2 Draw Execution Unit %.03u\n\t%.*s %.*s", dexu->m.portId, AfxPushString(&dexu->subsysName), AfxPushString(&dexu->subsysVer));

#if 0
    AfxLogAssistence("%s", dpu->wgl.GetExtensionsStringARB(dpu->dc));
#endif

    dpu->exuIdx = dexu->m.exuIdx;
    dpu->portId = dexu->m.portId;

#if _AFX_DEBUG
    gl->Enable(GL_DEBUG_OUTPUT);
    gl->Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    gl->DebugMessageCallback(_glDbgMsgCb, NIL);
    //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

    // RESET DEFAULT STATE TO OPENGL DEFAULTS

    dpu->activeRs.depthCompareOp = avxCompareOp_LESS;
    dpu->activeRs.depthWriteDisabled = FALSE;
    dpu->activeRs.depthBounds[0] = 0.f;
    dpu->activeRs.depthBounds[1] = 1.f;
    dpu->activeRs.stencilFront.compareOp = avxCompareOp_ALWAYS;
    dpu->activeRs.stencilFront.reference = 0;
    dpu->activeRs.stencilFront.compareMask = (dpu->activeRs.stencilFront.writeMask = 0xFFFFFFFF);
    dpu->activeRs.stencilFront.failOp = (dpu->activeRs.stencilFront.passOp = (dpu->activeRs.stencilFront.depthFailOp = avxStencilOp_KEEP));
    dpu->activeRs.stencilBack.compareOp = avxCompareOp_ALWAYS;
    dpu->activeRs.stencilBack.reference = 0;
    dpu->activeRs.stencilBack.compareMask = (dpu->activeRs.stencilBack.writeMask = 0xFFFFFFFF);
    dpu->activeRs.stencilBack.failOp = (dpu->activeRs.stencilBack.passOp = (dpu->activeRs.stencilBack.depthFailOp = avxStencilOp_KEEP));
    dpu->activeRs.depthBiasConstFactor = 0.f;
    dpu->activeRs.depthBiasSlopeScale = 0.f;

    for (afxUnit i = 0; i < 8; i++)
    {
        dpu->activeRs.outs[i].blendConfig.aBlendOp = avxBlendOp_ADD;
        dpu->activeRs.outs[i].blendConfig.rgbBlendOp = avxBlendOp_ADD;
        dpu->activeRs.outs[i].blendConfig.aSrcFactor = avxBlendFactor_ONE;
        dpu->activeRs.outs[i].blendConfig.rgbSrcFactor = avxBlendFactor_ONE;
        dpu->activeRs.outs[i].blendConfig.aDstFactor = avxBlendFactor_ZERO;
        dpu->activeRs.outs[i].blendConfig.rgbDstFactor = avxBlendFactor_ZERO;
        dpu->activeRs.outs[i].discardMask = NIL;
    }

    // In Qwadro specification, seamless cubemap sampling is ALWAYS enabled by default.
    // As SIGMA GL/2 is only bound to Qwadro, actually there is not way of disable it.
    gl->Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS); _SglThrowErrorOccuried();
    //gl->ClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE); _SglThrowErrorOccuried();// set screen origin to top to bottm, and set depth to [ 0, 1 ]

    // Scissor test is ALWAYS enabled.
    // Our need it to crop the specified canvas rect, for example.
    gl->Enable(GL_SCISSOR_TEST); _SglThrowErrorOccuried();

    gl->GenVertexArrays(1, &dpu->emptyVao); _SglThrowErrorOccuried();
    gl->BindVertexArray(dpu->emptyVao); _SglThrowErrorOccuried();
    gl->BindVertexArray(0); _SglThrowErrorOccuried();

    gl->GenFramebuffers(1, &dpu->fboOpSrc); _SglThrowErrorOccuried();
    gl->BindFramebuffer(GL_FRAMEBUFFER, dpu->fboOpSrc); _SglThrowErrorOccuried();
    gl->GenFramebuffers(1, &dpu->fboOpDst); _SglThrowErrorOccuried();
    gl->BindFramebuffer(GL_FRAMEBUFFER, dpu->fboOpDst); _SglThrowErrorOccuried();
    gl->BindFramebuffer(GL_FRAMEBUFFER, 0); _SglThrowErrorOccuried();
    dpu->fboOpSrcAnnex = NIL;
    dpu->fboOpDstAnnex = NIL;

    GLint datai;
    gl->GetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &datai); _SglThrowErrorOccuried();

    dpu->texPackUnitIdx = datai - 1;
    dpu->texUnpackUnitIdx = datai - 2;
    dpu->srcPboGpuHandle = NIL;
    dpu->dstPboGpuHandle = NIL;

    // GL_MAX_COMBINED_UNIFORM_BLOCKS --- the maximum number of uniform blocks per program.The value must be at least 70.
    gl->GetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &datai); _SglThrowErrorOccuried();

    gl->GenBuffers(1, &dpu->pushConstUbo); _SglThrowErrorOccuried();
    gl->BindBuffer(GL_UNIFORM_BUFFER, dpu->pushConstUbo); _SglThrowErrorOccuried();
    gl->BufferStorage(GL_UNIFORM_BUFFER, ddev->limits.maxPushConstsSiz, NIL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT); _SglThrowErrorOccuried();
    dpu->pushConstMappedMem = gl->MapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    gl->BindBuffer(GL_UNIFORM_BUFFER, 0);
    dpu->pushConstUboIdx = datai - 1;
    gl->BindBufferBase(GL_UNIFORM_BUFFER, dpu->pushConstUboIdx, dpu->pushConstUbo); _SglThrowErrorOccuried();

    dpu->instanced = TRUE;
    dpu->running = TRUE;
    
    do
    {
        AfxLockMutex(&dexu->schedCndMtx);

        while (!dexu->schedCnt)
            AfxWaitCondition(&dexu->schedCnd, &dexu->schedCndMtx);

        dexu->schedCnt = 0;
        AfxUnlockMutex(&dexu->schedCndMtx);

        afxThread thr;
        AfxGetThread(&thr);
        AfxAssert(thr == dexu->worker);
        _DexuProcCb(dexu, dexu->worker);

        _SglProcessDeletionQueue(gl, &dexu->deletionQueue);
    } while (!AfxShouldThreadBeInterrupted());

    gl->DeleteVertexArrays(1, &dpu->emptyVao); _SglThrowErrorOccuried();
    gl->DeleteFramebuffers(1, &dpu->fboOpSrc); _SglThrowErrorOccuried();
    gl->DeleteFramebuffers(1, &dpu->fboOpDst); _SglThrowErrorOccuried();

    gl->DeleteBuffers(1, &dpu->pushConstUbo); _SglThrowErrorOccuried();

    wglMakeContextCurrentSIG(NIL, NIL, NIL);

    return 0;
}

_ZGL afxError _SglDexuStartCb(afxDrawBridge dexu)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);

    if (!AfxIsThreadRunning(dexu->worker))
    {
        AfxRunThread(dexu->worker, (void*)DexuThrProc, dexu);
    }
    return err;
}

_ZGL afxError _SglDexuStopCb(afxDrawBridge dexu)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);

    afxInt exitCode = 0;
    do
    {
        AfxRequestThreadInterruption(dexu->worker);
        AfxWaitForDrawContext(dexu->m.dctx, AFX_TIME_INFINITE);
    } while (!AfxWaitForThreadExit(dexu->worker, &exitCode));

    return err;
}

_ZGL afxError _SglDexuPingCb(afxDrawBridge dexu, afxUnit queIdx)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);
    dexu->schedCnt += 1;
    AfxSignalCondition(&dexu->schedCnd);
    return err;
}

_ZGL afxError _SglDexuCtorCb(afxDrawBridge dexu, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);

    afxDrawDevice ddev = args[0];
    AfxAssertObjects(1, &ddev, afxFcc_DDEV);
    afxDrawContext dctx = args[1];
    AfxAssertObjects(1, &dctx, afxFcc_DCTX);

    if (_AvxDexuStdImplementation.ctor(dexu, args, invokeNo)) AfxThrowError();
    else
    {
        AfxZero(&dexu->dpu, sizeof(dexu->dpu));

        if (_SglCreateHwSurface(&ddev->idd->wndClss, &dexu->hWnd, &dexu->hDC, &dexu->dcPxlFmt, &dexu->dcPfd)) AfxThrowError();
        else
        {
            HDC bkpHdc = wglGetCurrentDrawDCSIG();
            HGLRC bkpGlrc = wglGetCurrentContextSIG();

            HGLRC hShareRC = dctx->hPrimeRC;

            if (_SglCreateHwContext(ddev->idd->oglDll, dexu->hDC, hShareRC, &dexu->hRC, &dexu->dpu.gl)) AfxThrowError();
            else
            {
                dexu->m.pingCb = _SglDexuPingCb;

                afxThreadConfig thrCfg = { 0 };
                //thrCfg.procCb = DrawThreadProc;
                thrCfg.purpose = afxThreadPurpose_DRAW;
                thrCfg.udd[0] = dexu;
                AfxAcquireThreads(AfxHere(), &thrCfg, 1, &dexu->worker);

                AfxDeployInterlockedQueue(&dexu->deletionQueue, sizeof(_zglDeleteGlRes), 32);

                AfxSetUpMutex(&dexu->schedCndMtx, AFX_MTX_PLAIN);
                AfxSetUpCondition(&dexu->schedCnd);
                dexu->schedCnt = 0;

                _SglDexuStartCb(dexu);

                if (err)
                    wglDeleteContextSIG(dexu->hRC);
            }

            if (err)
                ReleaseDC(dexu->hWnd, dexu->hDC), DestroyWindow(dexu->hWnd);

            wglMakeContextCurrentSIG(bkpHdc, bkpHdc, bkpGlrc);
        }

        if (err)
            _AvxDexuStdImplementation.dtor(dexu);
    }
    return err;
}

_ZGL afxError _SglDexuDtorCb(afxDrawBridge dexu)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);

    afxDrawDevice ddev = AfxGetDrawBridgeDevice(dexu);

    afxInt exitCode;
    AfxWaitForThreadExit(dexu->worker, &exitCode);
    AfxReleaseObjects(1, &dexu->worker);

    AfxWaitForDrawContext(dexu->m.dctx, AFX_TIME_INFINITE);
    AfxWaitForDrawContext(dexu->m.dctx, AFX_TIME_INFINITE); // yes, two times.
    
    _AvxDexuStdImplementation.dtor(dexu);

    wglDeleteContextSIG(dexu->hRC);
    ReleaseDC(dexu->hWnd, dexu->hDC);
    DestroyWindow(dexu->hWnd);

    AfxCleanUpCondition(&dexu->schedCnd);
    AfxCleanUpMutex(&dexu->schedCndMtx);

    AfxDismantleInterlockedQueue(&dexu->deletionQueue);

    return err;
}
