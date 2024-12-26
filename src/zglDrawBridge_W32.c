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
#include "qwadro/inc/sim/akxRenderer.h"

// RENDERING SCOPE

_ZGL void _DpuConcludeDrawScope(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    _DpuBindAndSyncCanv(dpu, GL_READ_FRAMEBUFFER, 0);
    _DpuBindAndSyncCanv(dpu, GL_DRAW_FRAMEBUFFER, 0);
    //gl->Flush(); _ZglThrowErrorOccuried(); // flush was presenting/swapping without wglSwapBuffers.

    avxCanvas canv = dpu->activeRs.canv;

    if (canv)
    {
        afxUnit surCnt;
        AfxCountDrawBufferSlots(canv, &surCnt, NIL);
#if 0
        if (surCnt)
        {
            AFX_ASSERT(_ZGL_MAX_SURF_PER_CANV >= surCnt);
            afxRaster surfaces[_ZGL_MAX_SURF_PER_CANV];
            AfxGetDrawBuffers(canv, 0, surCnt, surfaces);

            for (afxUnit i = 0; i < surCnt; i++)
            {
                afxRaster ras = surfaces[i];

                if (ras)
                {
                    AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &ras);
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
                gl->DrawBuffer(GL_BACK); _ZglThrowErrorOccuried();
                break;
            }
            case avxStoreOp_DISCARD:
            case avxStoreOp_DONT_CARE:
            default:
            {
                gl->DrawBuffer(GL_NONE); _ZglThrowErrorOccuried();
                gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 1, (GLenum[]) { GL_BACK }); _ZglThrowErrorOccuried();
                break;
            }
            }

            switch (dt->loadOp)
            {
            case avxLoadOp_LOAD: break;
            case avxLoadOp_CLEAR:
            {
                gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 1, (GLenum[]) { GL_BACK }); _ZglThrowErrorOccuried();
                afxReal const *rgba = dt->clearValue.rgba;
                gl->ClearColor(rgba[0], rgba[1], rgba[2], rgba[3]); _ZglThrowErrorOccuried();
                gl->Clear(GL_COLOR_BUFFER_BIT); _ZglThrowErrorOccuried();
                break;
            }
            case avxLoadOp_DONT_CARE:
            default:
            {
                gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 1, (GLenum[]) { GL_BACK }); _ZglThrowErrorOccuried();
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

        for (afxUnit i = 0; i < cCnt; i++)
        {
            glAttach = GL_COLOR_ATTACHMENT0 + i;

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

            // Prepare cleanups and invalidations.

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

        if (d) // depth (and stencil)
        {
            dt = d;
            glAttach = (s && combinedDs) ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;

            // Prepare cleanups and invalidations.

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
        
        if (s && !combinedDs) // stencil
        {
            dt = s;
            glAttach = GL_STENCIL_ATTACHMENT;

            // Prepare cleanups and invalidations.

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

        // TODO iterate for each canvas surface against arguments

        //_DpuBindAndSyncCanv(gl, GL_READ_FRAMEBUFFER, canv);
        _DpuBindAndSyncCanv(dpu, GL_DRAW_FRAMEBUFFER, canv);
        GLuint glHandle = canv->glHandle;
        AFX_ASSERT(gl->IsFramebuffer(glHandle));
        //dpu->activeRs.canv = canv;

#if !0
        if (canv->storeBitmask != storeBitmask)
#endif
        {
            canv->storeBitmask = storeBitmask; // cache it
            gl->DrawBuffers(storeCnt, storeBufs); _ZglThrowErrorOccuried();

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
        AFX_ASSERT(gl->ScissorArrayv);
        gl->ScissorArrayv(0, 1, scissor);
        afxBool clipped = (area->x || area->y || ((!area->w) || (area->w && canvWhd.w > area->w)) || (area->h && canvWhd.h > area->h));

        if (invalidCnt)
        {
            if (clipped)
            {
                gl->InvalidateSubFramebuffer(GL_DRAW_FRAMEBUFFER, invalidCnt, invalidBufs, area->x, area->y, area->w, area->h); _ZglThrowErrorOccuried();
            }
            else
            {
                gl->InvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, invalidCnt, invalidBufs); _ZglThrowErrorOccuried();
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
                gl->ClearBufferfv(GL_DEPTH, 0, &dt->clearValue.depth); _ZglThrowErrorOccuried();
                break;
            }
            case GL_DEPTH_STENCIL_ATTACHMENT:
            {
                dt = d;
                gl->ClearBufferfi(GL_DEPTH_STENCIL, 0, dt->clearValue.depth, dt->clearValue.stencil); _ZglThrowErrorOccuried();
                break;
            }
            case GL_STENCIL_ATTACHMENT:
            {
                dt = s;
                GLint sCv = dt->clearValue.stencil;
                gl->ClearBufferiv(GL_STENCIL, 0, &sCv); _ZglThrowErrorOccuried();
                break;
            }
            default:
            {
                dt = &c[i];
                afxReal const* rgba = dt->clearValue.rgba;
                GLint dbi = clearBufs[i] - GL_COLOR_ATTACHMENT0;
                gl->ClearBufferfv(GL_COLOR, /*GL_DRAW_BUFFER0 +*/ dbi, rgba); _ZglThrowErrorOccuried();
                break;
            }
            }
        }
    }
}

_ZGL void _DpuMemoryBarrier(zglDpu* dpu, avxPipelineStage src, avxPipelineStage dst)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    /*
        glMemoryBarrier defines a barrier ordering the memory transactions issued prior to the command relative to those issued after the barrier. For the purposes of this ordering, memory transactions performed by shaders are considered to be issued by the rendering command that triggered the execution of the shader. barriers is a bitfield indicating the set of operations that are synchronized with shader stores; the bits used in barriers are as follows:

        GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT
        If set, vertex data sourced from buffer objects after the barrier will reflect data written by shaders prior to the barrier. The set of buffer objects affected by this bit is derived from the buffer object bindings used for generic vertex attributes derived from the GL_VERTEX_ATTRIB_ARRAY_BUFFER bindings.

        GL_ELEMENT_ARRAY_BARRIER_BIT
        If set, vertex array indices sourced from buffer objects after the barrier will reflect data written by shaders prior to the barrier. The buffer objects affected by this bit are derived from the GL_ELEMENT_ARRAY_BUFFER binding.

        GL_UNIFORM_BARRIER_BIT
        Shader uniforms sourced from buffer objects after the barrier will reflect data written by shaders prior to the barrier.

        GL_TEXTURE_FETCH_BARRIER_BIT
        Texture fetches from shaders, including fetches from buffer object memory via buffer textures, after the barrier will reflect data written by shaders prior to the barrier.

        GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
        Memory accesses using shader image load, store, and atomic built-in functions issued after the barrier will reflect data written by shaders prior to the barrier. Additionally, image stores and atomics issued after the barrier will not execute until all memory accesses (e.g., loads, stores, texture fetches, vertex fetches) initiated prior to the barrier complete.

        GL_COMMAND_BARRIER_BIT
        Command data sourced from buffer objects by Draw*Indirect commands after the barrier will reflect data written by shaders prior to the barrier. The buffer objects affected by this bit are derived from the GL_DRAW_INDIRECT_BUFFER binding.

        GL_PIXEL_BUFFER_BARRIER_BIT
        Reads and writes of buffer objects via the GL_PIXEL_PACK_BUFFER and GL_PIXEL_UNPACK_BUFFER bindings (via glReadPixels, glTexSubImage1D, etc.) after the barrier will reflect data written by shaders prior to the barrier. Additionally, buffer object writes issued after the barrier will wait on the completion of all shader writes initiated prior to the barrier.

        GL_TEXTURE_UPDATE_BARRIER_BIT
        Writes to a texture via glTex(Sub)Image*, glCopyTex(Sub)Image*, glCompressedTex(Sub)Image*, and reads via glGetTexImage after the barrier will reflect data written by shaders prior to the barrier. Additionally, texture writes from these commands issued after the barrier will not execute until all shader writes initiated prior to the barrier complete.

        GL_BUFFER_UPDATE_BARRIER_BIT
        Reads or writes via glBufferSubData, glCopyBufferSubData, or glGetBufferSubData, or to buffer object memory mapped by glMapBuffer or glMapBufferRange after the barrier will reflect data written by shaders prior to the barrier. Additionally, writes via these commands issued after the barrier will wait on the completion of any shader writes to the same memory initiated prior to the barrier.

        GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT
        Access by the client to persistent mapped regions of buffer objects will reflect data written by shaders prior to the barrier. Note that this may cause additional synchronization operations.

        GL_FRAMEBUFFER_BARRIER_BIT
        Reads and writes via framebuffer object attachments after the barrier will reflect data written by shaders prior to the barrier. Additionally, framebuffer writes issued after the barrier will wait on the completion of all shader writes issued prior to the barrier.

        GL_TRANSFORM_FEEDBACK_BARRIER_BIT
        Writes via transform feedback bindings after the barrier will reflect data written by shaders prior to the barrier. Additionally, transform feedback writes issued after the barrier will wait on the completion of all shader writes issued prior to the barrier.

        GL_ATOMIC_COUNTER_BARRIER_BIT
        Accesses to atomic counters after the barrier will reflect writes prior to the barrier.

        GL_SHADER_STORAGE_BARRIER_BIT
        Accesses to shader storage blocks after the barrier will reflect writes prior to the barrier.

        GL_QUERY_BUFFER_BARRIER_BIT
        Writes of buffer objects via the GL_QUERY_BUFFER binding after the barrier will reflect data written by shaders prior to the barrier. Additionally, buffer object writes issued after the barrier will wait on the completion of all shader writes initiated prior to the barrier.

        If barriers is GL_ALL_BARRIER_BITS, shader memory accesses will be synchronized relative to all the operations described above.

        // -----
        glMemoryBarrierByRegion behaves as described above for glMemoryBarrier, with two differences:

        First, it narrows the region under consideration so that only reads and writes of prior fragment shaders that are invoked for a smaller region of the framebuffer will be completed/reflected prior to subsequent reads and writes of following fragment shaders. The size of the region is implementation-dependent and may be as small as one framebuffer pixel.

        Second, it only applies to memory transactions that may be read by or written by a fragment shader. Therefore, only the barrier bits

        GL_ATOMIC_COUNTER_BARRIER_BIT

        GL_FRAMEBUFFER_BARRIER_BIT

        GL_SHADER_IMAGE_ACCESS_BARRIER_BIT

        GL_SHADER_STORAGE_BARRIER_BIT

        GL_TEXTURE_FETCH_BARRIER_BIT

        GL_UNIFORM_BARRIER_BIT

        are supported.

        When barriers is GL_ALL_BARRIER_BITS, shader memory accesses will be synchronized relative to all these barrier bits, 
        but not to other barrier bits specific to glMemoryBarrier. This implies that reads and writes for scatter/gather-like algorithms may or may not be completed/reflected after a glMemoryBarrierByRegion command. However, for uses such as deferred shading, where a linked list of visible surfaces with the head at a framebuffer address may be constructed, and the entirety of the list is only dependent on previous executions at that framebuffer address, glMemoryBarrierByRegion may be significantly more efficient than glMemoryBarrier.
    */

    GLbitfield flags = NIL;
#if 0
    if (dst & avxPipelineStage_VERTEX)
        flags |= GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT;
    else if (dst & avxPipelineStage_COPY)
        flags |= ;
    else if (dst & avxPipelineStage_COPY)
        flags |= ;
    else if (dst & avxPipelineStage_COPY)
        flags |= ;
    else if (dst & avxPipelineStage_COPY)
        flags |= ;
    else if (dst & avxPipelineStage_COPY)
        flags |= ;
#endif
    gl->MemoryBarrier(GL_ALL_BARRIER_BITS);
}

// RESOURCE BINDING

_ZGL void _DpuBindBuffers(zglDpu* dpu, afxUnit set, afxUnit first, afxUnit cnt, afxBuffer buffers[], afxUnit const offsets[], afxUnit const ranges[])
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_RANGE(_ZGL_MAX_ENTRY_PER_LEGO, first, cnt);
    AFX_ASSERT_RANGE(_ZGL_MAX_LEGO_PER_BIND, set, 1);

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
    AFX_ASSERT_RANGE(_ZGL_MAX_ENTRY_PER_LEGO, first, cnt);
    AFX_ASSERT_RANGE(_ZGL_MAX_LEGO_PER_BIND, set, 1);

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
    AFX_ASSERT_RANGE(_ZGL_MAX_ENTRY_PER_LEGO, first, cnt);
    AFX_ASSERT_RANGE(_ZGL_MAX_LEGO_PER_BIND, set, 1);

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

    // copy right into mapped memory.
    AfxCopy(&dpu->pushConstMappedMem[offset], data, siz);
}

_ZGL void _ZglFlushLsChanges(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    avxLigature liga;

    if (!(liga = dpu->activeLiga))
        return;

    AFX_ASSERT_OBJECTS(afxFcc_LIGA, 1, &liga);

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
            afxUnit resIdx = liga->m.sets[i].baseEntry + j;
            avxLigatureEntry const *entry = &liga->m.totalEntries[resIdx];

            if (!(updMask & AFX_BIT(entry->binding))) // skip if not changed
                continue;

            AFX_ASSERT(entry->type);
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
                    gl->BindBufferBase(glTarget, glUnit, 0); _ZglThrowErrorOccuried();
                }
                else
                {
                    DpuBindAndSyncBuf(dpu, glTarget, buf);
                    bufSiz = AfxGetBufferCapacity(buf, 0);

                    if (offset || range)
                    {
                        AFX_ASSERT(range);
                        AFX_ASSERT_RANGE(bufSiz, offset, range);
                        gl->BindBufferRange(glTarget, glUnit, buf->glHandle, offset, AFX_ALIGNED_SIZE(range, AFX_SIMD_ALIGNMENT)); _ZglThrowErrorOccuried();
                    }
                    else
                    {
                        gl->BindBufferBase(glTarget, glUnit, buf->glHandle); _ZglThrowErrorOccuried();
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
                        gl->ActiveTexture(GL_TEXTURE0 + glUnit); _ZglThrowErrorOccuried();
                        gl->BindTexture(GL_TEXTURE_2D, 0); _ZglThrowErrorOccuried();
                    }
                    else
                    {
                        DpuBindAndSyncRas(dpu, glUnit, ras);
                        gl->ActiveTexture(GL_TEXTURE0 + glUnit); _ZglThrowErrorOccuried();
                        gl->BindTexture(ras->glTarget, ras->glHandle); _ZglThrowErrorOccuried();
                    }
                }

                if (entry->type == avxShaderParam_SAMPLER_UNIT || entry->type == avxShaderParam_COMBINED_IMAGE_SAMPLER)
                {
                    if (!samp)
                    {
                        gl->BindSampler(glUnit, 0); _ZglThrowErrorOccuried();
                    }
                    else
                    {
                        _DpuBindAndSyncSamp(dpu, glUnit, samp);
                        gl->BindSampler(glUnit, samp->glHandle); _ZglThrowErrorOccuried();
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
                    gl->ActiveTexture(glUnit); _ZglThrowErrorOccuried();
                    gl->BindBuffer(glTarget, 0); _ZglThrowErrorOccuried();
                }
                else
                {
                    gl->ActiveTexture(glUnit); _ZglThrowErrorOccuried();
                    DpuBindAndSyncBuf(dpu, glTarget, buf);
                    bufSiz = AfxGetBufferCapacity(buf, 0);
                    gl->BindTexture(glTarget, buf->glTexHandle); _ZglThrowErrorOccuried();

                    if (offset || range)
                    {
                        AFX_ASSERT(range);
                        AFX_ASSERT_RANGE(bufSiz, offset, range);

                        if (gl->TexBufferRange)
                        {
                            gl->TexBufferRange(glTarget, buf->glTexIntFmt, buf->glHandle, offset, AFX_ALIGNED_SIZE(range, AFX_SIMD_ALIGNMENT)); _ZglThrowErrorOccuried();
                        }
                        else
                        {
                            gl->TexBuffer(glTarget, buf->glTexIntFmt, buf->glHandle); _ZglThrowErrorOccuried();
                        }
                    }
                    else
                    {
                        gl->TexBuffer(glTarget, buf->glTexIntFmt, buf->glHandle); _ZglThrowErrorOccuried();
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
    avxVertexDecl vin;

    if (!(vin = dpu->activeVin))
        return;

    AFX_ASSERT_OBJECTS(afxFcc_VIN, 1, &vin);

    // BIND RESOURCES (VERTEX AND INDEX BUFFERS)

    zglVertexInputState* activeVinBindings = &vin->bindings;
    zglVertexInputState* nextVinBindings = &dpu->nextVinBindings;
    afxMask sourcesUpdMask = nextVinBindings->sourcesUpdMask;

    if (sourcesUpdMask)
    {
        nextVinBindings->sourcesUpdMask = NIL;

        for (afxUnit i = 0; i < vin->m.streamCnt; i++)
        {
            avxVertexFetch const* stream = &vin->m.streams[i];
            afxUnit streamIdx = stream->srcIdx;
            AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, streamIdx, 1);

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
                    AFX_ASSERT(gl->BindVertexBuffer);
                    gl->BindVertexBuffer(streamIdx, 0, 0, 16); _ZglThrowErrorOccuried();

                    activeVinBindings->sources[streamIdx].bufGpuHandle = NIL;
                }
                else
                {
                    AFX_ASSERT(nextRange);
                    AFX_ASSERT(nextStride);

                    if (!nextBuf->glHandle)
                        DpuBindAndSyncBuf(dpu, GL_ARRAY_BUFFER, nextBuf);

                    AFX_ASSERT(gl->BindVertexBuffer);
                    gl->BindVertexBuffer(streamIdx, nextBuf->glHandle, nextOff, nextStride); _ZglThrowErrorOccuried();

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
        //_ZglFlushVertexInputStateChanges(dpu, FALSE);

    //AFX_ASSERT(!dpu->nextVinBindingsMask);
    //AFX_ASSERT(!dpu->nextVtxInAttribUpdMask);
    //AFX_ASSERT(!dpu->flushIbb);

    AFX_ASSERT(dpu->activePip);
    afxUnit32 vtxCnt = data->vtxCnt;
    AFX_ASSERT(vtxCnt);
    //AFX_ASSERT(cmd->instCnt);
    GLenum top = AfxToGlTopology(dpu->activeTs.primTop);

    afxUnit firstInst = data->firstInst;
    afxUnit firstVtx = data->firstVtx;
    afxUnit instCnt = data->instCnt;

#if FORCE_GL_GENERIC_FUNCS
    AFX_ASSERT(gl->DrawArraysInstancedBaseInstance);
    gl->DrawArraysInstancedBaseInstance(top, firstVtx, vtxCnt, instCnt, firstInst); _ZglThrowErrorOccuried();
#else
    if (instCnt)
    {
        if (firstInst)
        {
            AFX_ASSERT(gl->DrawArraysInstancedBaseInstance);
            gl->DrawArraysInstancedBaseInstance(top, firstVtx, vtxCnt, instCnt, firstInst); _ZglThrowErrorOccuried();
        }
        else
        {
            gl->DrawArraysInstanced(top, firstVtx, vtxCnt, instCnt); _ZglThrowErrorOccuried();
        }
    }
    else
    {
        gl->DrawArrays(top, firstVtx, vtxCnt); _ZglThrowErrorOccuried();
    }
#endif
    dpu->m.numOfFedVertices += vtxCnt;
    dpu->m.numOfFedInstances += instCnt;

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
        //_ZglFlushVertexInputStateChanges(dpu, TRUE);

    //AFX_ASSERT(!dpu->nextVinBindingsMask);
    //AFX_ASSERT(!dpu->nextVtxInAttribUpdMask);
    //AFX_ASSERT(!dpu->flushIbb);

    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &dpu->activeVin->bindings.idxSrcBuf);
    //AFX_ASSERT(dpu->state.vertexBindingCnt);

    afxUnit idxCnt = data->idxCnt;
    AFX_ASSERT(idxCnt);
    //AFX_ASSERT(cmd->instCnt);

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
    AFX_ASSERT(gl->DrawElementsInstancedBaseVertexBaseInstance);
    gl->DrawElementsInstancedBaseVertexBaseInstance(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, instCnt, vtxOff2, firstInst); _ZglThrowErrorOccuried();
#else
    if (instCnt)
    {
        if (firstInst)
        {
            AFX_ASSERT(gl->DrawElementsInstancedBaseVertexBaseInstance);
            gl->DrawElementsInstancedBaseVertexBaseInstance(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, instCnt, vtxOff2, firstInst); _ZglThrowErrorOccuried();
        }
        else
        {
            if (vtxOff2)
            {
                gl->DrawElementsInstancedBaseVertex(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, instCnt, vtxOff2); _ZglThrowErrorOccuried();
            }
            else
            {
                gl->DrawElementsInstanced(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, instCnt); _ZglThrowErrorOccuried();
            }
        }
    }
    else
    {
        if (vtxOff2)
        {
            gl->DrawElementsBaseVertex(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff, vtxOff2); _ZglThrowErrorOccuried();
        }
        else
        {
            gl->DrawElements(top, idxCnt, idxSizGl[idxSiz], (void const*)dataOff); _ZglThrowErrorOccuried();
        }
    }
#endif

    //dpu->numOfFedVertices += cmd->idxCnt;
    dpu->m.numOfFedIndices += idxCnt;
    dpu->m.numOfFedInstances += instCnt;

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

    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);
    DpuBindAndSyncBuf(dpu, GL_DRAW_INDIRECT_BUFFER, buf);

#if FORCE_GL_GENERIC_FUNCS
    AFX_ASSERT(gl->MultiDrawArraysIndirect);
    gl->MultiDrawArraysIndirect(top, offPtr, drawCnt, stride); _ZglThrowErrorOccuried();
#else
    if (1 >= drawCnt)
    {
        gl->DrawArraysIndirect(top, offPtr); _ZglThrowErrorOccuried();
    }
    else
    {
        AFX_ASSERT(gl->MultiDrawArraysIndirect);
        gl->MultiDrawArraysIndirect(top, offPtr, drawCnt, stride); _ZglThrowErrorOccuried();
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

    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);
    DpuBindAndSyncBuf(dpu, GL_DRAW_INDIRECT_BUFFER, buf);
    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &cntBuf);
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

    AFX_ASSERT(gl->MultiDrawArraysIndirectCount);
    gl->MultiDrawArraysIndirectCount(top, offPtr, cntBufOff, maxDrawCnt, stride); _ZglThrowErrorOccuried();
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

    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);
    DpuBindAndSyncBuf(dpu, GL_DRAW_INDIRECT_BUFFER, buf);

#if FORCE_GL_GENERIC_FUNCS
    AFX_ASSERT(gl->MultiDrawElementsIndirect);
    gl->MultiDrawElementsIndirect(top, idxSizGl[idxSiz], offPtr, drawCnt, stride); _ZglThrowErrorOccuried();
#else
    if (1 >= drawCnt)
    {
        gl->DrawElementsIndirect(top, idxSizGl[idxSiz], offPtr); _ZglThrowErrorOccuried();
    }
    else
    {
        AFX_ASSERT(gl->MultiDrawElementsIndirect);
        gl->MultiDrawElementsIndirect(top, idxSizGl[idxSiz], offPtr, drawCnt, stride); _ZglThrowErrorOccuried();
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

    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);
    DpuBindAndSyncBuf(dpu, GL_DRAW_INDIRECT_BUFFER, buf);
    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &cntBuf);
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

    AFX_ASSERT(gl->MultiDrawElementsIndirectCount);
    gl->MultiDrawElementsIndirectCount(top, idxSizGl[idxSiz], offPtr, cntBufOff, maxDrawCnt, stride); _ZglThrowErrorOccuried();
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

    AFX_ASSERT(gl->DispatchCompute);
    gl->DispatchCompute(grpCntX, grpCntY, grpCntZ); _ZglThrowErrorOccuried();
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

    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);
    DpuBindAndSyncBuf(dpu, GL_DISPATCH_INDIRECT_BUFFER, buf);

    AFX_ASSERT(gl->DispatchComputeIndirect);
    gl->DispatchComputeIndirect(offset); _ZglThrowErrorOccuried();
}
#if 0
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
#endif
_ZGL void _DpuEndOfCommands(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

}

// QUEUE STUFF

_ZGL afxError _DpuWork_Remap(zglDpu* dpu, avxWork* subm)
{
    afxError err = AFX_ERR_NONE;
    
    if (_DpuRemapBuf(dpu, subm->Remap.buf, subm->Remap.off, subm->Remap.ran, subm->Remap.flags, subm->Remap.placeholder))
        AfxThrowError();

    AfxDecAtom32(&subm->Remap.buf->m.pendingRemap);
    AfxDisposeObjects(1, &subm->Remap.buf);

    return err;
}

_ZGL afxError _DpuWork_Transfer(zglDpu* dpu, avxWork* subm)
{
    afxError err = AFX_ERR_NONE;
    //AfxAssertObject(dexu, afxFcc_DEXU);

    switch (subm->Transfer.srcFcc)
    {
    case NIL:
    {
        switch (subm->Transfer.dstFcc)
        {
        case afxFcc_BUF: // raw to buf
        {
            afxBuffer buf = subm->Transfer.dst.buf;
            AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);

            if (_DpuLoadBuf2(dpu, subm->Transfer.dst.buf, subm->Transfer.src.src, subm->Transfer.opCnt, subm->Transfer.bufOps))
                AfxThrowError();

            AfxDisposeObjects(1, &buf);
            break;
        }
        case afxFcc_RAS: // raw to ras
            if (_DpuUnpackRasFromMem(dpu, subm->Transfer.dst.ras, subm->Transfer.src.src, subm->Transfer.opCnt, subm->Transfer.rasOps))
                AfxThrowError();

            afxRaster ras = subm->Transfer.dst.ras;
            AfxDisposeObjects(1, &ras);
            break;
        default: AfxThrowError(); break;
        }
        break;
    }
    case afxFcc_BUF:
    {
        switch (subm->Transfer.dstFcc)
        {
        case NIL: // buf to raw
        {
            if (_DpuStoreBuf2(dpu, subm->Transfer.src.buf, subm->Transfer.dst.dst, subm->Transfer.opCnt, subm->Transfer.bufOps))
                AfxThrowError();

            afxBuffer buf = subm->Transfer.src.buf;
            AfxDisposeObjects(1, &buf);
            break;
        }
        case afxFcc_RAS:
        {
            if (_DpuUnpackRasFromBuf(dpu, subm->Transfer.dst.ras, subm->Transfer.src.buf, subm->Transfer.opCnt, subm->Transfer.rasOps))
                AfxThrowError();

            afxBuffer buf = subm->Transfer.src.buf;
            afxRaster ras = subm->Transfer.dst.ras;
            AfxDisposeObjects(1, &ras);
            AfxDisposeObjects(1, &buf);
            break;
        }
        case afxFcc_IOB: // buf to iob
        {
            if (_DpuOutputBuf2(dpu, subm->Transfer.src.buf, subm->Transfer.dst.iob, subm->Transfer.opCnt, subm->Transfer.bufOps))
                AfxThrowError();

            afxStream iob = subm->Transfer.dst.iob;
            afxBuffer buf = subm->Transfer.src.buf;
            AfxDisposeObjects(1, &buf);
            AfxDisposeObjects(1, &iob);
            break;
        }
        default: AfxThrowError(); break;
        }
        break;
    }
    case afxFcc_RAS:
    {
        switch (subm->Transfer.dstFcc)
        {
        case NIL: // ras to raw
        {
            if (_DpuPackRasToMem(dpu, subm->Transfer.src.ras, subm->Transfer.dst.dst, subm->Transfer.opCnt, subm->Transfer.rasOps))
                AfxThrowError();

            afxRaster ras = subm->Transfer.src.ras;
            AfxDisposeObjects(1, &ras);
            break;
        }
        case afxFcc_BUF:
        {
            if (_DpuPackRasToBuf(dpu, subm->Transfer.src.ras, subm->Transfer.dst.buf, subm->Transfer.opCnt, subm->Transfer.rasOps))
                AfxThrowError();

            afxBuffer buf = subm->Transfer.dst.buf;
            afxRaster ras = subm->Transfer.src.ras;
            AfxDisposeObjects(1, &ras);
            AfxDisposeObjects(1, &buf);
            break;
        }
        case afxFcc_IOB: // ras to iob
        {
            if (_DpuPackRasToIob(dpu, subm->Transfer.src.ras, subm->Transfer.dst.iob, subm->Transfer.opCnt, subm->Transfer.rasOps))
                AfxThrowError();

            afxStream iob = subm->Transfer.dst.iob;
            afxRaster ras = subm->Transfer.src.ras;
            AfxDisposeObjects(1, &ras);
            AfxDisposeObjects(1, &iob);
            break;
        }
        default: AfxThrowError(); break;
        }
        break;
    }
    case afxFcc_IOB:
    {
        switch (subm->Transfer.dstFcc)
        {
        case afxFcc_BUF: // iob to buf
        {
            if (_DpuInputBuf2(dpu, subm->Transfer.dst.buf, subm->Transfer.src.iob, subm->Transfer.opCnt, subm->Transfer.bufOps))
                AfxThrowError();

            afxStream iob = subm->Transfer.src.iob;
            afxBuffer buf = subm->Transfer.dst.buf;
            AfxDisposeObjects(1, &buf);
            AfxDisposeObjects(1, &iob);
            break;
        }
        case afxFcc_RAS: // iob to ras
        {
            if (_DpuUnpackRasFromIob(dpu, subm->Transfer.dst.ras, subm->Transfer.src.iob, subm->Transfer.opCnt, subm->Transfer.rasOps))
                AfxThrowError();

            afxRaster ras = subm->Transfer.dst.ras;
            afxStream iob = subm->Transfer.src.iob;
            AfxDisposeObjects(1, &ras);
            AfxDisposeObjects(1, &iob);
            break;
        }
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
        AFX_ASSERT_OBJECTS(afxFcc_DOUT, 1, &dout);
        afxUnit outBufIdx = subm->items[itemIdx].bufIdx;

        afxDrawSystem dsys = dpu->activeDsys;
        AFX_ASSERT_OBJECTS(afxFcc_DSYS, 1, &dsys);

        afxWhd whd;
        avxCanvas canv;
        AfxGetDrawOutputCanvas(dout, outBufIdx, &canv);
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

        _DpuBindRasterizer(dpu, dsys->fntRazr, NIL, NIL);

        akxViewConstants vc;
        AfxM4dReset(vc.v);
        AfxComputeOffcenterOrthographicMatrix(vc.p, 0, vp.extent[0], 0, vp.extent[1], -1.f, 1.f, &AVX_CLIP_SPACE_OPENGL);
        //AfxComputeBasicOrthographicMatrix(vc.p, vp.extent[0] / vp.extent[1], 1.0, 3.0, &AVX_CLIP_SPACE_OPENGL);
        DpuBufRw(dpu, dsys->fntUnifBuf, 0, sizeof(vc), FALSE, &vc);

        _DpuBindBuffers(dpu, 0, 0, 1, &dsys->fntUnifBuf, (afxUnit const[]) {0}, (afxUnit const[]) { 0 });

        _DpuBindSamplers(dpu, 0, 1, 1, &dsys->fntSamp);
        _DpuBindRasters(dpu, 0, 1, 1, &dsys->fntRas, NIL);

        afxReal x = subm->origin[0];
        afxReal y = subm->origin[1];
        afxUnit numchar = subm->caption.str.len;
        afxReal r = 1, g = 1, b = 1;
        afxReal x2 = x;

        afxReal* verts;
        _DpuRemapBuf(dpu, dsys->fntDataBuf, 0, AfxGetBufferCapacity(dsys->fntDataBuf, 0), NIL, (void**)&verts);
        //afxReal* verts = (void*)dsys->fntDataBuf->m.bytemap;// AfxMapBuffer(dsys->fntDataBuf, 0, 2048, afxBufferFlag_W, TRUE); // TODO map directly
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

        _DpuRemapBuf(dpu, dsys->fntDataBuf, 0, 0, NIL, NIL);
        //AfxUnmapBuffer(dsys->fntDataBuf, FALSE);

        zglBufferInfo bufi = { 0 };
        bufi.buf = dsys->fntDataBuf;
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

_ZGL afxInt ZGL_DPU_THR_PROC(afxDrawBridge dexu)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DEXU, 1, &dexu);

    afxThread thr;
    AfxGetThread(&thr);
    AFX_ASSERT(thr == dexu->m.worker);

    afxDrawDevice ddev = AfxGetDrawBridgeDevice(dexu);

    zglDpu* dpu = &dexu->dpu;
    glVmt const* gl = &dpu->gl;

    if (!wglMakeContextCurrentSIG(dexu->hDC, dexu->hDC, dexu->hRC))
        AfxThrowError();

    AfxMakeString(&dexu->subsysName, 0, (afxChar const*)gl->GetString(GL_RENDERER), 0); _ZglThrowErrorOccuried();
    AfxMakeString(&dexu->subsysVer, 0, (afxChar const*)gl->GetString(GL_VERSION), 0); _ZglThrowErrorOccuried();

    AfxDbgLogf(6, NIL, "SIGMA GL/2 Draw Execution Unit %.03u\n\t%.*s %.*s", dexu->m.portId, AfxPushString(&dexu->subsysName), AfxPushString(&dexu->subsysVer));

#if 0
    AfxLogAssistence("%s", dpu->wgl.GetExtensionsStringARB(dpu->dc));
#endif

    dpu->m.exuIdx = dexu->m.exuIdx;
    dpu->m.portId = dexu->m.portId;
    dpu->m.dexu = dexu;
    dpu->m.dsys = AfxGetDrawBridgeContext(dexu);
    dpu->m.thr = thr;
    dexu->m.dpu = (void*)dpu;

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
    gl->Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS); _ZglThrowErrorOccuried();
    //gl->ClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE); _ZglThrowErrorOccuried();// set screen origin to top to bottm, and set depth to [ 0, 1 ]

    // Scissor test is ALWAYS enabled.
    // Our need it to crop the specified canvas rect, for example.
    gl->Enable(GL_SCISSOR_TEST); _ZglThrowErrorOccuried();

    gl->GenVertexArrays(1, &dpu->emptyVao); _ZglThrowErrorOccuried();
    gl->BindVertexArray(dpu->emptyVao); _ZglThrowErrorOccuried();
    gl->BindVertexArray(0); _ZglThrowErrorOccuried();

    gl->GenFramebuffers(1, &dpu->fboOpSrc); _ZglThrowErrorOccuried();
    gl->BindFramebuffer(GL_FRAMEBUFFER, dpu->fboOpSrc); _ZglThrowErrorOccuried();
    gl->GenFramebuffers(1, &dpu->fboOpDst); _ZglThrowErrorOccuried();
    gl->BindFramebuffer(GL_FRAMEBUFFER, dpu->fboOpDst); _ZglThrowErrorOccuried();
    gl->BindFramebuffer(GL_FRAMEBUFFER, 0); _ZglThrowErrorOccuried();
    dpu->fboOpSrcAnnex = NIL;
    dpu->fboOpDstAnnex = NIL;

    GLint datai;
    gl->GetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &datai); _ZglThrowErrorOccuried();

    dpu->texPackUnitIdx = datai - 1;
    dpu->texUnpackUnitIdx = datai - 2;
    dpu->srcPboGpuHandle = NIL;
    dpu->dstPboGpuHandle = NIL;

    // GL_MAX_COMBINED_UNIFORM_BLOCKS --- the maximum number of uniform blocks per program.The value must be at least 70.
    gl->GetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &datai); _ZglThrowErrorOccuried();

    gl->GenBuffers(1, &dpu->pushConstUbo); _ZglThrowErrorOccuried();
    gl->BindBuffer(GL_UNIFORM_BUFFER, dpu->pushConstUbo); _ZglThrowErrorOccuried();
    gl->BufferStorage(GL_UNIFORM_BUFFER, ddev->m.limits.maxPushConstsSiz, NIL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT); _ZglThrowErrorOccuried();
    dpu->pushConstMappedMem = gl->MapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    gl->BindBuffer(GL_UNIFORM_BUFFER, 0);
    dpu->pushConstUboIdx = datai - 1; // the last uniform block
    gl->BindBufferBase(GL_UNIFORM_BUFFER, dpu->pushConstUboIdx, dpu->pushConstUbo); _ZglThrowErrorOccuried();

    dpu->m.instanced = TRUE;
    dpu->m.running = TRUE;
    
    do
    {
#if 0
        AfxLockMutex(&dexu->m.schedCndMtx);
        afxTimeSpec ts = { 0 };
        ts.nsec = 1 * 100000; // 100000 = 0.0001 sec

        while (!dexu->m.schedCnt)
            AfxWaitTimedCondition(&dexu->m.schedCnd, &dexu->m.schedCndMtx, &ts);

        dexu->m.schedCnt = 0;
        AfxUnlockMutex(&dexu->m.schedCndMtx);
#endif
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
                    AfxLogError("DPU %u failed to take active context.", dexu->m.portId);
                }
            }

            if (ctxEntered)
                dpu->m.dsys = AfxGetDrawBridgeContext(dexu);
        }

        if (ctxEntered)
        {
            dpu->m.numOfFedVertices = 0;
            dpu->m.numOfFedIndices = 0;
            dpu->m.numOfFedInstances = 0;
        }

        dexu->m.procCb(dpu);

        _ZglProcessDeletionQueue(gl, &dexu->deletionQueue);
        AfxSleep(1);

        if (AfxShouldThreadBeInterrupted())
            break;

    } while (1);

    gl->DeleteVertexArrays(1, &dpu->emptyVao); _ZglThrowErrorOccuried();
    gl->DeleteFramebuffers(1, &dpu->fboOpSrc); _ZglThrowErrorOccuried();
    gl->DeleteFramebuffers(1, &dpu->fboOpDst); _ZglThrowErrorOccuried();

    gl->DeleteBuffers(1, &dpu->pushConstUbo); _ZglThrowErrorOccuried();

    wglMakeContextCurrentSIG(NIL, NIL, NIL);

    return 0;
}

_ZGL afxError _ZglDexuDtorCb(afxDrawBridge dexu)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DEXU, 1, &dexu);

    afxDrawSystem dsys = AfxGetDrawBridgeContext(dexu);

    AfxWaitForDrawSystem(dsys, AFX_TIME_INFINITE);
    AfxWaitForDrawSystem(dsys, AFX_TIME_INFINITE); // yes, two times.

    if (_AVX_DEXU_CLASS_CONFIG.dtor(dexu))
        AfxThrowError();

    wglDeleteContextSIG(dexu->hRC);
    ReleaseDC(dexu->hWnd, dexu->hDC);
    DestroyWindow(dexu->hWnd);

    AfxDismantleInterlockedQueue(&dexu->deletionQueue);

    return err;
}

_ZGL afxError _ZglDexuCtorCb(afxDrawBridge dexu, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_DEXU, 1, &dexu);

    afxDrawSystem dsys = args[0];
    AFX_ASSERT_OBJECTS(afxFcc_DSYS, 1, &dsys);
    _avxDrawBridgeAcquisition const* cfg = AFX_CAST(_avxDrawBridgeAcquisition const*, args[1]) + invokeNo;
    
    afxDrawDevice ddev = cfg->ddev;
    AFX_ASSERT_OBJECTS(afxFcc_DDEV, 1, &ddev);

    if (_ZglCreateHwSurface(&ddev->wndClss, &dexu->hWnd, &dexu->hDC, &dexu->dcPxlFmt, &dexu->dcPfd))
    {
        AfxThrowError();
        return err;
    }

    HDC bkpHdc = wglGetCurrentDrawDCSIG();
    HGLRC bkpGlrc = wglGetCurrentContextSIG();
    HGLRC hShareRC = dsys->hPrimeRC;

    AfxZero(&dexu->dpu, sizeof(dexu->dpu));
    if (_ZglCreateHwContext(ddev->oglDll, dexu->hDC, hShareRC, &dexu->hRC, &dexu->dpu.gl))
    {
        AfxThrowError();
        ReleaseDC(dexu->hWnd, dexu->hDC), DestroyWindow(dexu->hWnd);
        return err;
    }

    if (err)
        wglDeleteContextSIG(dexu->hRC);

    if (err)
        ReleaseDC(dexu->hWnd, dexu->hDC), DestroyWindow(dexu->hWnd);


#if 0
    afxBool shared = FALSE;
    afxDrawBridge primeExu = NIL;
    afxUnit primeExuIdx = 0;
    for (afxUnit i = 0; i < dsys->m.exuCnt; i++)
    {
        if (dsys->m.exus[i] && (ddev == AfxGetProvider(dsys->m.exus[i])));
        {
            primeExuIdx = i;
            primeExu = dsys->m.exus[primeExuIdx];
            shared = TRUE;
        }
    }
#endif

#if 0
    dexu->vinExt.extId = afxFcc_DEXU;
    AfxInstallClassExtension(AvxGetVertexInputClass(dsys), &dexu->vinExt);

    dexu->pipExt.extId = afxFcc_DEXU;
    AfxInstallClassExtension(AvxGetPipelineClass(dsys), &dexu->pipExt);

    dexu->canvExt.extId = afxFcc_DEXU;
    AfxInstallClassExtension(AvxGetCanvasClass(dsys), &dexu->canvExt);
#endif

    if (_AVX_DEXU_CLASS_CONFIG.ctor(dexu, (void*[]) { dsys, (void*)cfg }, 0))
    {
        AfxThrowError();
        wglDeleteContextSIG(dexu->hRC);
        ReleaseDC(dexu->hWnd, dexu->hDC), DestroyWindow(dexu->hWnd);
        return err;
    }

    AfxDeployInterlockedQueue(&dexu->deletionQueue, sizeof(_zglDeleteGlRes), 32);

    static avxWorkList const dpuWorkExecDevmt =
    {
        .Callback = _AvxDpuWork_CallbackCb,
        .Execute = _AvxDpuWork_ExecuteCb,
        .Transfer = _DpuWork_Transfer,
        .Remap = _DpuWork_Remap,
        NIL,// _DpuPresentDout,
    };

    dexu->m.cmdVmt = &cmdDevmt;
    dexu->m.workVmt = &dpuWorkExecDevmt;
    dexu->m.workerProc = ZGL_DPU_THR_PROC;

    if (err)
    {
        _AVX_DEXU_CLASS_CONFIG.dtor(dexu);
        wglDeleteContextSIG(dexu->hRC);
        ReleaseDC(dexu->hWnd, dexu->hDC), DestroyWindow(dexu->hWnd);
    }
    return err;
}
