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
_ZGL void _AfxStdUbufImplUnmap(afxBuffer buf)
{
    if (!buf->m.mapped) AfxLogError("");
    else
    {
        afxBuffer base = AfxBuffer.GetBase(buf);

        buf->m.mapped = FALSE;

        if (base->gpuHandle)
        {
            glVmt const* gl = &(((afxDrawSystem)AfxGetProvider(&buf->m.obj))->vmt);
            afxFlags usage = base->usage;
            GLenum target = NIL;

            if (usage & afxBufferUsage_VERTEX)
                target = GL_ARRAY_BUFFER;
            else if (usage & afxBufferUsage_INDEX)
                target = GL_ELEMENT_ARRAY_BUFFER;
            else if (usage & afxBufferUsage_UNIFORM)
                target = GL_UNIFORM_BUFFER;
            else if (usage & afxBufferUsage_STORAGE)
                target = GL_SHADER_STORAGE_BUFFER;
            else if (usage & afxBufferUsage_INDIRECT)
                target = GL_DRAW_INDIRECT_BUFFER;
            else if (usage & afxBufferUsage_UNIFORM_TEXELS)
                target = GL_PIXEL_UNPACK_BUFFER;
            else if (usage & afxBufferUsage_STORAGE_TEXELS)
                target = GL_PIXEL_PACK_BUFFER;
            else if (usage == afxBufferUsage_SRC)
                target = GL_COPY_READ_BUFFER;
            else if (usage == afxBufferUsage_DST)
                target = GL_COPY_WRITE_BUFFER;
            else AfxLogError("");

            GLint glaccess;

            if (usage & (afxBufferUsage_SRC | afxBufferUsage_DST))
            {
                // copy
                glaccess = GL_READ_WRITE;
            }
            else if (usage & afxBufferUsage_SRC)
            {
                glaccess = GL_READ_ONLY;
            }
            else if (usage & afxBufferUsage_DST)
            {
                glaccess = GL_WRITE_ONLY;
            }
            else
            {
                glaccess = NIL;
                AfxLogError("");
            }

            AfxAssert(gl->IsBuffer(base->gpuHandle));

            gl->BindBuffer(target, base->gpuHandle); _SglThrowErrorOccuried();
            gl->BufferSubData(target, buf->m.mappedOff, buf->m.mappedRange, &(((afxByte*)buf->m.buf)[buf->m.mappedOff])); _SglThrowErrorOccuried();
            gl->BindBuffer(target, 0); _SglThrowErrorOccuried();

            base->lastUpdTime = AfxGetTimer();
            buf->lastUpdTime = AfxGetTimer();
        }
    }
}

_ZGL void* _AfxStdUbufImplMap(afxBuffer buf, afxSize off, afxSize siz)
{
    void *ptr = NIL;

    if (buf->m.mapped) AfxLogError("");
    else
    {
        buf->m.mappedOff = off;
        buf->m.mappedRange = siz ? siz : buf->m.cap - off;
        buf->m.mapped = TRUE;
    }
    return ptr;
}
#endif//0

_ZGL afxError DpuBindAndSyncBuf(zglDpu* dpu, GLenum glTarget, afxBuffer buf)
{
    //AfxEntry("buf=%p", buf);
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    if (!buf)
    {
        gl->BindBuffer(glTarget, 0); _SglThrowErrorOccuried();
    }
    else
    {
        AfxAssertObjects(1, &buf, afxFcc_BUF);
        zglUpdateFlags devUpdReq = (buf->updFlags & ZGL_UPD_FLAG_DEVICE);
        GLuint glHandle = buf->glHandle;
            
        if ((!glHandle) || (devUpdReq & ZGL_UPD_FLAG_DEVICE_INST))
        {
            if (glHandle)
            {
                gl->DeleteBuffers(1, &(glHandle)); _SglThrowErrorOccuried();
                buf->glHandle = NIL;
                glHandle = NIL;

                if (buf->glTexHandle)
                {
                    AfxAssert(buf->glTarget == GL_TEXTURE_BUFFER);
                    AfxAssert(buf->m.usage & afxBufferUsage_MATRIX);
                    gl->DeleteTextures(1, &buf->glTexHandle); _SglThrowErrorOccuried();
                    buf->glTexHandle = NIL;
                }
            }

            _SglThrowErrorOccuried();
            gl->GenBuffers(1, &(glHandle)); _SglThrowErrorOccuried();
            gl->BindBuffer(buf->glTarget, glHandle); _SglThrowErrorOccuried();
            AfxAssert(gl->IsBuffer(glHandle));
            buf->glHandle = glHandle;

            AfxAssert(gl->BufferStorage);
            gl->BufferStorage(buf->glTarget, buf->m.bufCap, NIL, buf->glAccess | GL_DYNAMIC_STORAGE_BIT); _SglThrowErrorOccuried();
            AfxLogEcho("GPU buffer %p ready. %u, %u, %x", buf, buf->glTarget, glHandle, buf->m.usage);
            buf->updFlags &= ~(ZGL_UPD_FLAG_DEVICE);

            if (buf->glTarget == GL_TEXTURE_BUFFER)
            {
                AfxAssert(buf->m.usage & afxBufferUsage_MATRIX);
                gl->ActiveTexture(ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT);
                gl->GenTextures(1, &buf->glTexHandle); _SglThrowErrorOccuried();
                gl->BindTexture(GL_TEXTURE_BUFFER, buf->glTexHandle); _SglThrowErrorOccuried();
                gl->TexBuffer(GL_TEXTURE_BUFFER, buf->glTexIntFmt, glHandle); _SglThrowErrorOccuried();
                
                if (glTarget != buf->glTarget)
                {
                    gl->BindTexture(GL_TEXTURE_BUFFER, 0); _SglThrowErrorOccuried();
                }
            }

            if (glTarget != buf->glTarget)
            {
                gl->BindBuffer(buf->glTarget, 0); _SglThrowErrorOccuried();
                gl->BindBuffer(glTarget, glHandle); _SglThrowErrorOccuried();
            }
        }
        else
        {
            gl->BindBuffer(glTarget, glHandle); _SglThrowErrorOccuried();
            
            if (buf->glTarget == GL_TEXTURE_BUFFER)
            {
                AfxAssert(buf->m.usage & afxBufferUsage_MATRIX);
                // Como selecionar a texture unit?
                //gl->BindTexture(GL_TEXTURE_BUFFER, buf->glTexHandle); _SglThrowErrorOccuried();
            }
        }
    }
    return err;
}

_ZGL afxError _DpuLoadBuf(zglDpu* dpu, afxBuffer buf, afxSize offset, afxUnit range, afxByte const* src)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    GLenum glTarget = GL_COPY_WRITE_BUFFER;
    DpuBindAndSyncBuf(dpu, glTarget, buf);
    gl->BufferSubData(glTarget, offset, range, src); _SglThrowErrorOccuried();
    return err;
}

_ZGL afxError _DpuLoadBuf2(zglDpu* dpu, afxBuffer buf, afxByte const* src, afxUnit opCnt, afxBufferIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    GLenum glTarget = GL_COPY_WRITE_BUFFER;
    GLenum glAccess = GL_MAP_WRITE_BIT;
    DpuBindAndSyncBuf(dpu, glTarget, buf);

    for (afxUnit i = 0; i < opCnt; i++)
    {
        afxBufferIo const* op = &ops[i];

        afxUnit rowCnt = op->rowCnt;
        afxUnit dstOffset = op->dstOffset;
        afxUnit dstStride = op->dstStride;
        afxUnit srcStride = op->srcStride;
        afxUnit range = AfxMin(AFX_ALIGNED_SIZE(rowCnt * dstStride, AFX_BUF_ALIGNMENT), AfxGetBufferCapacity(buf, dstOffset));

        if ((dstStride != srcStride) && (dstStride > 1 || srcStride > 1))
        {
            afxByte* dst = gl->MapBufferRange(glTarget, dstOffset, range, glAccess); _SglThrowErrorOccuried();

            if (!dst) AfxThrowError();
            else
            {
                AfxStream2(rowCnt, &src[op->srcOffset], srcStride, dst, dstStride);
                gl->UnmapBuffer(glTarget); _SglThrowErrorOccuried();
            }
        }
        else
        {
            gl->BufferSubData(glTarget, dstOffset, range, &src[op->srcOffset]); _SglThrowErrorOccuried();
        }
    }
    return err;
}

_ZGL afxError _DpuStoreBuf(zglDpu* dpu, afxBuffer buf, afxSize offset, afxUnit range, afxByte* dst)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    GLenum glTarget = GL_COPY_READ_BUFFER;
    DpuBindAndSyncBuf(dpu, glTarget, buf);
    gl->GetBufferSubData(glTarget, offset, range, dst); _SglThrowErrorOccuried();
    return err;
}

_ZGL afxError _DpuStoreBuf2(zglDpu* dpu, afxBuffer buf, afxByte* dst, afxUnit opCnt, afxBufferIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    GLenum glTarget = GL_COPY_READ_BUFFER;
    GLenum glAccess = GL_MAP_READ_BIT;
    DpuBindAndSyncBuf(dpu, glTarget, buf);
    
    for (afxUnit i = 0; i < opCnt; i++)
    {
        afxBufferIo const* op = &ops[i];
        afxSize srcOffset = op->srcOffset;
        afxUnit rowCnt = op->rowCnt;
        afxUnit srcStride = op->srcStride;
        afxUnit dstStride = op->dstStride;
        afxUnit range = AfxMin(AFX_ALIGNED_SIZE(rowCnt * srcStride, AFX_BUF_ALIGNMENT), AfxGetBufferCapacity(buf, srcOffset));

        if ((dstStride != srcStride) && (dstStride > 1 || srcStride > 1))
        {
            void* src = gl->MapBufferRange(glTarget, srcOffset, range, glAccess); _SglThrowErrorOccuried();

            if (!src) AfxThrowError();
            else
            {
                AfxStream2(rowCnt, src, srcStride, dst, dstStride);
                gl->UnmapBuffer(glTarget); _SglThrowErrorOccuried();
            }
        }
        else
        {
            gl->GetBufferSubData(glTarget, srcOffset, range, dst); _SglThrowErrorOccuried();
        }
    }
    return err;
}

_ZGL afxError _DpuOutputBuf(zglDpu* dpu, afxBuffer buf, afxSize offset, afxUnit range, afxStream out, afxSize at)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    GLenum glTarget = GL_COPY_READ_BUFFER;
    GLenum glAccess = GL_MAP_READ_BIT;
    DpuBindAndSyncBuf(dpu, glTarget, buf);

    void* src = gl->MapBufferRange(glTarget, offset, range, glAccess); _SglThrowErrorOccuried();

    if (!src) AfxThrowError();
    else
    {
        AfxWriteStreamAt(out, at, range, 0, src);

        gl->UnmapBuffer(glTarget); _SglThrowErrorOccuried();
    }
    return err;
}

_ZGL afxError _DpuOutputBuf2(zglDpu* dpu, afxBuffer buf, afxStream out, afxUnit opCnt, afxBufferIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    GLenum glTarget = GL_COPY_READ_BUFFER;
    GLenum glAccess = GL_MAP_READ_BIT;
    DpuBindAndSyncBuf(dpu, glTarget, buf);

    for (afxUnit i = 0; i < opCnt; i++)
    {
        afxBufferIo const* op = &ops[i];

        afxSize srcOffset = op->srcOffset;
        afxUnit rowCnt = op->rowCnt;
        afxUnit srcStride = op->srcStride;
        afxUnit dstStride = op->dstStride;
        afxSize at = op->dstOffset;
        afxUnit range = AfxMin(AFX_ALIGNED_SIZE(rowCnt * srcStride, AFX_BUF_ALIGNMENT), AfxGetBufferCapacity(buf, srcOffset));

        void* src = gl->MapBufferRange(glTarget, srcOffset, range, glAccess); _SglThrowErrorOccuried();

        if (!src) AfxThrowError();
        else
        {
            if ((dstStride != srcStride) && (dstStride > 1 || srcStride > 1))
                AfxWriteStreamAt2(out, at, range, dstStride, src, srcStride);
            else
                AfxWriteStreamAt(out, at, range, 0, src);

            gl->UnmapBuffer(glTarget); _SglThrowErrorOccuried();
        }
    }
    return err;
}

_ZGL afxError _DpuInputBuf(zglDpu* dpu, afxBuffer buf, afxSize offset, afxUnit range, afxStream in, afxSize at)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    GLenum glTarget = GL_COPY_WRITE_BUFFER;
    GLenum glAccess = GL_MAP_WRITE_BIT;
    DpuBindAndSyncBuf(dpu, glTarget, buf);

    afxByte* dst = gl->MapBufferRange(glTarget, offset, range, glAccess); _SglThrowErrorOccuried();

    if (!dst) AfxThrowError();
    else
    {
        AfxReadStreamAt(in, at, range, 0, dst);
        gl->UnmapBuffer(glTarget); _SglThrowErrorOccuried();
        //gl->FlushMappedBufferRange(glTarget, op->dstOffset, op->range); _SglThrowErrorOccuried();
    }
    return err;
}

_ZGL afxError _DpuInputBuf2(zglDpu* dpu, afxBuffer buf, afxStream in, afxUnit opCnt, afxBufferIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    GLenum glTarget = GL_COPY_WRITE_BUFFER;
    GLenum glAccess = GL_MAP_WRITE_BIT;
    DpuBindAndSyncBuf(dpu, glTarget, buf);

    for (afxUnit i = 0; i < opCnt; i++)
    {
        afxBufferIo const* op = &ops[i];
        afxSize dstOffset = op->dstOffset;
        afxUnit rowCnt = op->rowCnt;
        afxUnit dstStride = op->dstStride;
        afxSize at = op->srcOffset;
        afxUnit srcStride = op->srcStride;
        afxUnit range = AfxMin(AFX_ALIGNED_SIZE(rowCnt * dstStride, AFX_BUF_ALIGNMENT), AfxGetBufferCapacity(buf, dstOffset));

        afxByte* dst = gl->MapBufferRange(glTarget, dstOffset, range, glAccess); _SglThrowErrorOccuried();

        if (!dst) AfxThrowError();
        else
        {
            if ((dstStride != srcStride) && (dstStride > 1 || srcStride > 1))
                AfxReadStreamAt2(in, at, range, srcStride, dst, dstStride);
            else
                AfxReadStreamAt(in, at, range, 0, dst);

            gl->UnmapBuffer(glTarget); _SglThrowErrorOccuried();
            //gl->FlushMappedBufferRange(glTarget, op->dstOffset, op->range); _SglThrowErrorOccuried();
        }
    }
    return err;
}

_ZGL afxError _DpuDequeMmapCb(zglDpu* dpu, afxDrawBridge dexu, afxUnit queIdx, afxBuffer buf)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    GLenum glTarget = (buf->m.flags & afxBufferFlag_W) ? GL_COPY_WRITE_BUFFER : GL_COPY_READ_BUFFER;
    DpuBindAndSyncBuf(dpu, glTarget, buf);

    if (buf->m.bytemap)
    {
        gl->UnmapBuffer(glTarget); _SglThrowErrorOccuried();
        //gl->FlushMappedBufferRange(glTarget, subm->buf->glMappedOff, subm->buf->glMappedSiz); _SglThrowErrorOccuried();
        buf->m.bytemap = NIL;
        buf->m.mappedOffset = 0;
        buf->m.mappedRange = 0;
        buf->m.mappedFlags = NIL;
    }
    else if (buf->m.mappedRange)
    {
        if ((0 == buf->m.mappedOffset) && (buf->m.mappedRange == AfxGetBufferCapacity(buf, 0)))
        {
            GLenum glAccess = NIL;

            if (!(buf->m.flags & afxBufferFlag_R))
            {
                if (buf->m.flags & afxBufferFlag_W)
                    glAccess = GL_WRITE_ONLY;
            }
            else
            {
                if (buf->m.flags & afxBufferFlag_W)
                    glAccess = GL_READ_WRITE;
                else
                    glAccess = GL_READ_ONLY;
            }

            buf->m.bytemap = gl->MapBuffer(glTarget, glAccess); _SglThrowErrorOccuried();
            AfxAssert(buf->m.bytemap);
        }
        else
        {
            buf->m.bytemap = gl->MapBufferRange(glTarget, buf->m.mappedOffset, buf->m.mappedRange, buf->glAccess); _SglThrowErrorOccuried();
            AfxAssert(buf->m.bytemap);
        }
    }
    return err;
}

_ZGL afxError _DpuRemapBuf(zglDpu* dpu, afxBuffer buf, afxSize offset, afxUnit range, afxFlags flags, void** placeholder)
{
    afxError err = AFX_ERR_NONE;
    //AfxAssertObject(dexu, afxFcc_DEXU);
    glVmt const* gl = &dpu->gl;

    GLenum glTarget = NIL;

    if (buf->m.flags & afxBufferFlag_W)
        glTarget = GL_COPY_WRITE_BUFFER;
    else
        glTarget = GL_COPY_READ_BUFFER;

    DpuBindAndSyncBuf(dpu, glTarget, buf);

    if (range)
    {
        GLenum glAccess = NIL;
        AfxAssert(!buf->m.bytemap);

#if !0
        if (!offset && range == AfxGetBufferCapacity(buf, 0))
        {
            if (!(buf->m.flags & afxBufferFlag_R))
            {
                if (buf->m.flags & afxBufferFlag_W)
                    glAccess = GL_WRITE_ONLY;
            }
            else
            {
                if (buf->m.flags & afxBufferFlag_W)
                    glAccess = GL_READ_WRITE;
                else
                    glAccess = GL_READ_ONLY;
            }

            buf->m.bytemap = gl->MapBuffer(glTarget, glAccess); _SglThrowErrorOccuried();
        }
        else
#endif
        {
            buf->m.bytemap = gl->MapBufferRange(glTarget, offset, range, buf->glAccess); _SglThrowErrorOccuried();
        }

        buf->m.mappedOffset = offset;
        buf->m.mappedRange = range;
        buf->m.mappedFlags = flags;
        AfxAssert(buf->m.bytemap);

        //if (placeholder)
        *placeholder = buf->m.bytemap;
        AfxAssert(*placeholder);
    }
    else
    {
        AfxAssert(buf->m.bytemap);
        gl->UnmapBuffer(glTarget); _SglThrowErrorOccuried();
        buf->m.bytemap = NIL;

        //gl->FlushMappedBufferRange(glTarget, subm->buf->glMappedOff, subm->buf->glMappedSiz); _SglThrowErrorOccuried();
        buf->m.mappedOffset = 0;
        buf->m.mappedRange = 0;
        buf->m.mappedFlags = NIL;
    }
    //AfxAssert(!AfxLoadAtom32(&buf->m.pendingRemap));
    return err;
}

_ZGL afxError _BufRemapCb(afxBuffer buf, afxUnit offset, afxUnit range, afxFlags flags, void** placeholder)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &buf, afxFcc_BUF);
    AfxAssertRange(buf->m.bufCap, offset, range);

    afxDrawContext dctx = AfxGetBufferContext(buf);
    AfxAssertObjects(1, &dctx, afxFcc_DCTX);

    afxUnit portId = 0;
    afxDrawBridge dexu;
    AfxGetDrawBridges(dctx, portId, 1, &dexu);
    AfxAssertObjects(1, &dexu, afxFcc_DEXU);

    afxUnit baseQueIdx = 0, queCnt = 0;
    AfxQueryDrawQueues(dexu, &baseQueIdx, &queCnt);
    afxBool queued = FALSE;

    if (buf->m.bytemap) // to unmap
    {
        for (afxUnit j = 0; j < queCnt; j++)
        {
            afxDrawQueue dque;
            AfxGetDrawQueues(dctx, baseQueIdx + j, 1, &dque);
            AfxAssertObjects(1, &dque, afxFcc_DQUE);

            avxQueueOp req = { 0 };
            req.f = (void*)_DpuDequeMmapCb;
            req.udd = buf;

            if (!_SglDqueRequestSubmCb(dque, NIL, 1, &req))
            {
                queued = TRUE;
                afxBool wait = !!flags;

                if (wait && _AvxWaitForEmptyDrawQueue(dque, AFX_TIME_INFINITE))
                    AfxThrowError();

                break;
            }
        }

        if (!queued)
            AfxThrowError();
    }
    else // to map
    {
        if (!buf->m.mappedRange) AfxThrowError();
        else
        {
            void* ptr = NIL;
            
            for (afxUnit j = 0; j < queCnt; j++)
            {
                afxDrawQueue dque;
                AfxGetDrawQueues(dctx, baseQueIdx + j, 1, &dque);
                AfxAssertObjects(1, &dque, afxFcc_DQUE);

                avxQueueOp req = { 0 };
                req.f = (void*)_DpuDequeMmapCb;
                req.udd = buf;

                if (!_SglDqueRequestSubmCb(dque, NIL, 1, &req))
                {
                    queued = TRUE;

                    if (_AvxWaitForEmptyDrawQueue(dque, AFX_TIME_INFINITE))
                        AfxThrowError();

                    if (_AvxWaitForEmptyDrawQueue(dque, AFX_TIME_INFINITE))
                        AfxThrowError();

                    AfxAssert(buf->m.bytemap);
                    //AfxAssert(ptr);
                    break;
                }
            }
            
            //*placeholder = ptr;

            if (!queued)
                AfxThrowError();
        }
    }
    return err;
}

_ZGL afxError _BufDtorCb(afxBuffer buf)
{
    afxError err = AFX_ERR_NONE;

    afxDrawContext dctx = AfxGetBufferContext(buf);

    if (buf->m.mappedRange)
    {
        AfxUnmapBuffer(buf, TRUE);
        AfxAssert(!buf->m.mappedRange);
    }

    if (buf->glHandle)
    {
        _SglDctxEnqueueDeletion(dctx, 0, GL_BUFFER, (afxSize)buf->glHandle);
        buf->glHandle = 0;
    }

    if (buf->glTexHandle)
    {
        _SglDctxEnqueueDeletion(dctx, 0, GL_TEXTURE, (afxSize)buf->glTexHandle);
        buf->glTexHandle = 0;
    }

    if (_AvxBufStdImplementation.dtor(buf))
        AfxThrowError();

    return err;
}

_ZGL afxError _BufCtorCb(afxBuffer buf, void** args, afxUnit invokeNo)
{
    afxResult err = NIL;
    AfxAssertObjects(1, &buf, afxFcc_BUF);

    if (_AvxBufStdImplementation.ctor(buf, args, invokeNo)) AfxThrowError();
    else
    {
        afxBufferInfo const *spec = ((afxBufferInfo const *)args[1]) + invokeNo;

        buf->glHandle = NIL;
        buf->glTarget = NIL;
        buf->updFlags = ZGL_UPD_FLAG_DEVICE_INST;

        afxBufferUsage usage = buf->m.usage;
        afxBufferFlags access = buf->m.flags;
        GLbitfield glAccess = NIL;

        if (usage & afxBufferUsage_VERTEX)
            buf->glTarget = GL_ARRAY_BUFFER;
        else if (usage & afxBufferUsage_INDEX)
            buf->glTarget = GL_ELEMENT_ARRAY_BUFFER;
        else if (usage & afxBufferUsage_UNIFORM)
            buf->glTarget = GL_UNIFORM_BUFFER;
        else if (usage & afxBufferUsage_STORAGE)
            buf->glTarget = GL_SHADER_STORAGE_BUFFER;
        else if (usage & afxBufferUsage_INDIRECT)
            buf->glTarget = GL_DRAW_INDIRECT_BUFFER;
        else if (usage & afxBufferUsage_QUERY)
            buf->glTarget = GL_QUERY_BUFFER;
        else if (usage & afxBufferUsage_DST)
            buf->glTarget = GL_PIXEL_PACK_BUFFER;
        else if (usage & afxBufferUsage_SRC)
            buf->glTarget = GL_PIXEL_UNPACK_BUFFER;
        else if (usage & afxBufferUsage_MATRIX)
        {
            buf->glTarget = GL_TEXTURE_BUFFER;

            GLint glIntFmt;
            SglToGlFormat2(buf->m.fmt, &glIntFmt, NIL, NIL);
            buf->glTexIntFmt = glIntFmt;
        }
        else
            AfxThrowError();

        if (access & afxBufferFlag_R)
            glAccess |= GL_MAP_READ_BIT;

        if (access & afxBufferFlag_W)
            glAccess |= GL_MAP_WRITE_BIT;

        if (access & afxBufferFlag_X)
            glAccess |= GL_MAP_PERSISTENT_BIT;

        if (access & afxBufferFlag_COHERENT)
            glAccess |= GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT;

        buf->glAccess = glAccess;

        buf->m.remap = _BufRemapCb;

        if (err && _AvxBufStdImplementation.dtor(buf))
            AfxThrowError();
    }
    return err;
}
