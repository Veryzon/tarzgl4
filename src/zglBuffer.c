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

_ZGL afxError DpuBindAndSyncBuf(zglDpu* dpu, GLenum glTarget, afxBuffer buf)
{
    //AfxEntry("buf=%p", buf);
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    if (!buf)
    {
        gl->BindBuffer(glTarget, 0); _ZglThrowErrorOccuried();
    }
    else
    {
        AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);
        zglUpdateFlags devUpdReq = (buf->updFlags & ZGL_UPD_FLAG_DEVICE);
        GLuint glHandle = buf->glHandle;
            
        if ((!glHandle) || (devUpdReq & ZGL_UPD_FLAG_DEVICE_INST))
        {
            if (glHandle)
            {
                gl->DeleteBuffers(1, &(glHandle)); _ZglThrowErrorOccuried();
                buf->glHandle = NIL;
                glHandle = NIL;

                if (buf->glTexHandle)
                {
                    AFX_ASSERT(buf->glTarget == GL_TEXTURE_BUFFER);
                    AFX_ASSERT(buf->m.usage & afxBufferUsage_TENSOR);
                    gl->DeleteTextures(1, &buf->glTexHandle); _ZglThrowErrorOccuried();
                    buf->glTexHandle = NIL;
                }
            }

            _ZglThrowErrorOccuried();
            gl->GenBuffers(1, &(glHandle)); _ZglThrowErrorOccuried();
            gl->BindBuffer(buf->glTarget, glHandle); _ZglThrowErrorOccuried();
            AFX_ASSERT(gl->IsBuffer(glHandle));
            buf->glHandle = glHandle;

            AFX_ASSERT(gl->BufferStorage);
            gl->BufferStorage(buf->glTarget, buf->m.cap, NIL, buf->glAccess | GL_DYNAMIC_STORAGE_BIT); _ZglThrowErrorOccuried();
            AfxLogEcho("GPU buffer %p ready. %u, %u, %x", buf, buf->glTarget, glHandle, buf->m.usage);
            buf->updFlags &= ~(ZGL_UPD_FLAG_DEVICE);

            if (buf->glTarget == GL_TEXTURE_BUFFER)
            {
                AFX_ASSERT(buf->m.usage & afxBufferUsage_TENSOR);
                gl->ActiveTexture(ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT);
                gl->GenTextures(1, &buf->glTexHandle); _ZglThrowErrorOccuried();
                gl->BindTexture(GL_TEXTURE_BUFFER, buf->glTexHandle); _ZglThrowErrorOccuried();
                gl->TexBuffer(GL_TEXTURE_BUFFER, buf->glTexIntFmt, glHandle); _ZglThrowErrorOccuried();
                
                if (glTarget != buf->glTarget)
                {
                    gl->BindTexture(GL_TEXTURE_BUFFER, 0); _ZglThrowErrorOccuried();
                }
            }

            if (glTarget != buf->glTarget)
            {
                gl->BindBuffer(buf->glTarget, 0); _ZglThrowErrorOccuried();
                gl->BindBuffer(glTarget, glHandle); _ZglThrowErrorOccuried();
            }
        }
        else
        {
            gl->BindBuffer(glTarget, glHandle); _ZglThrowErrorOccuried();
            
            if (buf->glTarget == GL_TEXTURE_BUFFER)
            {
                AFX_ASSERT(buf->m.usage & afxBufferUsage_TENSOR);
                // Como selecionar a texture unit?
                //gl->BindTexture(GL_TEXTURE_BUFFER, buf->glTexHandle); _ZglThrowErrorOccuried();
            }
        }
    }
    return err;
}

_ZGL void DpuBufCpy(zglDpu* dpu, afxBuffer src, afxBuffer dst, afxUnit opCnt, afxBufferCopy const ops[])
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    DpuBindAndSyncBuf(dpu, GL_COPY_READ_BUFFER, src);
    DpuBindAndSyncBuf(dpu, GL_COPY_WRITE_BUFFER, dst);

    for (afxUnit i = 0; i < opCnt; i++)
    {
        gl->CopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, ops[i].srcOffset, ops[i].dstOffset, ops[i].range); _ZglThrowErrorOccuried();
    }
}

_ZGL void DpuBufSet(zglDpu* dpu, afxBuffer buf, afxUnit offset, afxUnit range, afxUnit data)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    GLenum glTarget = GL_COPY_WRITE_BUFFER;
    DpuBindAndSyncBuf(dpu, glTarget, buf);

    AFX_ASSERT(buf->glAccess & GL_MAP_WRITE_BIT);
    void* dst = gl->MapBufferRange(glTarget, offset, range, GL_MAP_WRITE_BIT); _ZglThrowErrorOccuried();
    AFX_ASSERT(dst);
    AfxStream(range, 0, sizeof(data), &data, dst);
    gl->UnmapBuffer(glTarget); _ZglThrowErrorOccuried();
    gl->FlushMappedBufferRange(glTarget, offset, range); _ZglThrowErrorOccuried();
}

_ZGL void DpuBufRw(zglDpu* dpu, afxBuffer buf, afxUnit offset, afxUnit range, afxBool toHost, void* srcOrDst)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    GLenum glTarget = toHost ? GL_COPY_READ_BUFFER : GL_COPY_WRITE_BUFFER;
    GLenum glAccess = toHost ? GL_MAP_READ_BIT : GL_MAP_WRITE_BIT;
    AFX_ASSERT(buf->glAccess & glAccess);

    glTarget = buf->glTarget;
    DpuBindAndSyncBuf(dpu, glTarget, buf);

    if (toHost)
    {
        gl->GetBufferSubData(glTarget, offset, range, srcOrDst); _ZglThrowErrorOccuried();
    }
    else
    {
        gl->BufferSubData(glTarget, offset, range, srcOrDst); _ZglThrowErrorOccuried();
    }
}

_ZGL afxError _DpuLoadBuf(zglDpu* dpu, afxBuffer buf, afxSize offset, afxUnit range, afxByte const* src)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    GLenum glTarget = GL_COPY_WRITE_BUFFER;
    DpuBindAndSyncBuf(dpu, glTarget, buf);
    gl->BufferSubData(glTarget, offset, range, src); _ZglThrowErrorOccuried();
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
            afxByte* dst = gl->MapBufferRange(glTarget, dstOffset, range, glAccess); _ZglThrowErrorOccuried();

            if (!dst) AfxThrowError();
            else
            {
                AfxStream2(rowCnt, &src[op->srcOffset], srcStride, dst, dstStride);
                gl->UnmapBuffer(glTarget); _ZglThrowErrorOccuried();
            }
        }
        else
        {
            gl->BufferSubData(glTarget, dstOffset, range, &src[op->srcOffset]); _ZglThrowErrorOccuried();
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
    gl->GetBufferSubData(glTarget, offset, range, dst); _ZglThrowErrorOccuried();
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
            void* src = gl->MapBufferRange(glTarget, srcOffset, range, glAccess); _ZglThrowErrorOccuried();

            if (!src) AfxThrowError();
            else
            {
                AfxStream2(rowCnt, src, srcStride, dst, dstStride);
                gl->UnmapBuffer(glTarget); _ZglThrowErrorOccuried();
            }
        }
        else
        {
            gl->GetBufferSubData(glTarget, srcOffset, range, dst); _ZglThrowErrorOccuried();
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

    void* src = gl->MapBufferRange(glTarget, offset, range, glAccess); _ZglThrowErrorOccuried();

    if (!src) AfxThrowError();
    else
    {
        AfxWriteStreamAt(out, at, range, 0, src);

        gl->UnmapBuffer(glTarget); _ZglThrowErrorOccuried();
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

        void* src = gl->MapBufferRange(glTarget, srcOffset, range, glAccess); _ZglThrowErrorOccuried();

        if (!src) AfxThrowError();
        else
        {
            if ((dstStride != srcStride) && (dstStride > 1 || srcStride > 1))
                AfxWriteStreamAt2(out, at, range, dstStride, src, srcStride);
            else
                AfxWriteStreamAt(out, at, range, 0, src);

            gl->UnmapBuffer(glTarget); _ZglThrowErrorOccuried();
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

    afxByte* dst = gl->MapBufferRange(glTarget, offset, range, glAccess); _ZglThrowErrorOccuried();

    if (!dst) AfxThrowError();
    else
    {
        AfxReadStreamAt(in, at, range, 0, dst);
        gl->UnmapBuffer(glTarget); _ZglThrowErrorOccuried();
        //gl->FlushMappedBufferRange(glTarget, op->dstOffset, op->range); _ZglThrowErrorOccuried();
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

        afxByte* dst = gl->MapBufferRange(glTarget, dstOffset, range, glAccess); _ZglThrowErrorOccuried();

        if (!dst) AfxThrowError();
        else
        {
            if ((dstStride != srcStride) && (dstStride > 1 || srcStride > 1))
                AfxReadStreamAt2(in, at, range, srcStride, dst, dstStride);
            else
                AfxReadStreamAt(in, at, range, 0, dst);

            gl->UnmapBuffer(glTarget); _ZglThrowErrorOccuried();
            //gl->FlushMappedBufferRange(glTarget, op->dstOffset, op->range); _ZglThrowErrorOccuried();
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
        AFX_ASSERT(!buf->m.bytemap);

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

            buf->m.bytemap = gl->MapBuffer(glTarget, glAccess); _ZglThrowErrorOccuried();
        }
        else
#endif
        {
            buf->m.bytemap = gl->MapBufferRange(glTarget, offset, range, buf->glAccess); _ZglThrowErrorOccuried();
        }

        buf->m.mappedOffset = offset;
        buf->m.mappedRange = range;
        buf->m.mappedFlags = flags;
        AFX_ASSERT(buf->m.bytemap);

        //if (placeholder)
        *placeholder = buf->m.bytemap;
        AFX_ASSERT(*placeholder);
    }
    else
    {
        AFX_ASSERT(buf->m.bytemap);
        gl->UnmapBuffer(glTarget); _ZglThrowErrorOccuried();
        buf->m.bytemap = NIL;

        //gl->FlushMappedBufferRange(glTarget, subm->buf->glMappedOff, subm->buf->glMappedSiz); _ZglThrowErrorOccuried();
        buf->m.mappedOffset = 0;
        buf->m.mappedRange = 0;
        buf->m.mappedFlags = NIL;
    }
    //AFX_ASSERT(!AfxLoadAtom32(&buf->m.pendingRemap));
    return err;
}

_ZGL afxError _BufRemapCb(afxBuffer buf, afxUnit offset, afxUnit range, afxFlags flags, void** placeholder)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);
    AFX_ASSERT_RANGE(buf->m.cap, offset, range);

    afxDrawSystem dsys = AfxGetBufferContext(buf);
    AFX_ASSERT_OBJECTS(afxFcc_DSYS, 1, &dsys);

    afxDrawQueue dque;
    afxDrawBridge dexu;
    afxUnit portId = 0;
    AfxGetDrawBridges(dsys, portId, 1, &dexu);
    AfxGetDrawQueues(dexu, 0, 1, &dque);
    AFX_ASSERT_OBJECTS(afxFcc_DQUE, 1, &dque);

    afxBool queued = FALSE;

    if (!range) // to unmap
    {
        void* ptr = NIL;
        if (_AvxSubmitRemapping(dque, buf, 0, 0, 0, &ptr))
        {
            AfxThrowError();
        }
        else
        {
            queued = TRUE;
            afxBool wait = !!flags;

            if (wait && AfxWaitForEmptyDrawQueue(dque, AFX_TIME_INFINITE))
                AfxThrowError();
        }

        if (!queued)
            AfxThrowError();
    }
    else // to map
    {
        if (buf->m.mappedRange) AfxThrowError();
        else
        {
            void* ptr = NIL;
            if (_AvxSubmitRemapping(dque, buf, offset, range, flags, &ptr))
            {
                AfxThrowError();
            }
            else
            {
                queued = TRUE;

                if (AfxWaitForEmptyDrawQueue(dque, AFX_TIME_INFINITE))
                    AfxThrowError();

                AFX_ASSERT(ptr);
                AFX_ASSERT(buf->m.bytemap);
            }
            
            *placeholder = ptr;

            if (!queued)
                AfxThrowError();
        }
    }
    return err;
}

_ZGL afxError _BufDtorCb(afxBuffer buf)
{
    afxError err = AFX_ERR_NONE;

    afxDrawSystem dsys = AfxGetBufferContext(buf);

    if (buf->m.mappedRange)
    {
        AfxUnmapBuffer(buf, TRUE);
        AFX_ASSERT(!buf->m.mappedRange);
    }

    if (buf->glHandle)
    {
        _ZglDsysEnqueueDeletion(dsys, 0, GL_BUFFER, (afxSize)buf->glHandle);
        buf->glHandle = 0;
    }

    if (buf->glTexHandle)
    {
        _ZglDsysEnqueueDeletion(dsys, 0, GL_TEXTURE, (afxSize)buf->glTexHandle);
        buf->glTexHandle = 0;
    }

    if (_AVX_BUF_CLASS_CONFIG.dtor(buf))
        AfxThrowError();

    return err;
}

_ZGL afxError _BufCtorCb(afxBuffer buf, void** args, afxUnit invokeNo)
{
    afxResult err = NIL;
    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);

    if (_AVX_BUF_CLASS_CONFIG.ctor(buf, args, invokeNo)) AfxThrowError();
    else
    {
        afxBufferInfo const *spec = ((afxBufferInfo const *)args[1]) + invokeNo;

        buf->glHandle = NIL;
        buf->glTexHandle = NIL;
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
        else  if (usage & afxBufferUsage_TENSOR)
            buf->glTarget = GL_TEXTURE_BUFFER;
        else if (usage & afxBufferUsage_DOWNLOAD)
            buf->glTarget = GL_PIXEL_PACK_BUFFER;
        else if (usage & afxBufferUsage_UPLOAD)
            buf->glTarget = GL_PIXEL_UNPACK_BUFFER;
        else
            AfxThrowError();

        GLint glIntFmt;
        GLenum glLayout;
        GLenum glType;
        ZglToGlFormat2(buf->m.fmt, &glIntFmt, &glLayout, &glType);
        buf->glTexIntFmt = glIntFmt; // tex buf needs

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

        if (err && _AVX_BUF_CLASS_CONFIG.dtor(buf))
            AfxThrowError();
    }
    return err;
}
