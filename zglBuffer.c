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
#include "qwadro/draw/dev/afxDrawSystem.h"

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
            glVmt const* gl = &(((afxDrawSystem)AfxGetParent(&buf->m.obj))->vmt);
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

_ZGL afxError DpuBindAndSyncBuf(sglDpu* dpu, GLenum glTarget, afxBuffer buf)
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
        sglUpdateFlags devUpdReq = (buf->updFlags & ZGL_UPD_FLAG_DEVICE);
        GLuint glHandle = buf->glHandle;
            
        if ((!glHandle) || (devUpdReq & ZGL_UPD_FLAG_DEVICE_INST))
        {
            if (glHandle)
            {
                gl->DeleteBuffers(1, &(glHandle)); _SglThrowErrorOccuried();
                buf->glHandle = NIL;
                glHandle = NIL;
            }

            _SglThrowErrorOccuried();
            gl->GenBuffers(1, &(glHandle)); _SglThrowErrorOccuried();
            gl->BindBuffer(buf->glTarget, glHandle); _SglThrowErrorOccuried();
            AfxAssert(gl->IsBuffer(glHandle));
            buf->glHandle = glHandle;

            AfxAssert(gl->BufferStorage);
            gl->BufferStorage(buf->glTarget, buf->m.cap, NIL, buf->glAccess | GL_DYNAMIC_STORAGE_BIT); _SglThrowErrorOccuried();
            AfxLogEcho("Hardware-side buffer %p ready. %u, %u, %x", buf, buf->glTarget, glHandle, buf->m.usage);
            buf->updFlags &= ~(ZGL_UPD_FLAG_DEVICE);

            if (glTarget != buf->glTarget)
            {
                gl->BindBuffer(buf->glTarget, 0); _SglThrowErrorOccuried();
                gl->BindBuffer(glTarget, glHandle); _SglThrowErrorOccuried();
            }
        }
        else
        {
            gl->BindBuffer(glTarget, glHandle); _SglThrowErrorOccuried();
        }
    }
    return err;
}

_ZGL afxError _DpuLoadBuf(sglDpu* dpu, afxBuffer buf, afxSize offset, afxNat range, afxByte const* src)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    GLenum glTarget = GL_COPY_WRITE_BUFFER;
    DpuBindAndSyncBuf(dpu, glTarget, buf);
    gl->BufferSubData(glTarget, offset, range, src); _SglThrowErrorOccuried();
    return err;
}

_ZGL afxError _DpuLoadBuf2(sglDpu* dpu, afxBuffer buf, afxByte const* src, afxNat opCnt, afxBufferIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    GLenum glTarget = GL_COPY_WRITE_BUFFER;
    GLenum glAccess = GL_MAP_WRITE_BIT;
    DpuBindAndSyncBuf(dpu, glTarget, buf);

    for (afxNat i = 0; i < opCnt; i++)
    {
        afxBufferIo const* op = &ops[i];

        afxNat rowCnt = op->rowCnt;
        afxNat dstOffset = op->dstOffset;
        afxNat dstStride = op->dstStride;
        afxNat srcStride = op->srcStride;
        afxNat range = AfxMin(AFX_ALIGNED_SIZEOF(rowCnt * dstStride, 64), AfxGetBufferCapacity(buf, dstOffset));

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

_ZGL afxError _DpuStoreBuf(sglDpu* dpu, afxBuffer buf, afxSize offset, afxNat range, afxByte* dst)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    GLenum glTarget = GL_COPY_READ_BUFFER;
    DpuBindAndSyncBuf(dpu, glTarget, buf);
    gl->GetBufferSubData(glTarget, offset, range, dst); _SglThrowErrorOccuried();
    return err;
}

_ZGL afxError _DpuStoreBuf2(sglDpu* dpu, afxBuffer buf, afxByte* dst, afxNat opCnt, afxBufferIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    GLenum glTarget = GL_COPY_READ_BUFFER;
    GLenum glAccess = GL_MAP_READ_BIT;
    DpuBindAndSyncBuf(dpu, glTarget, buf);
    
    for (afxNat i = 0; i < opCnt; i++)
    {
        afxBufferIo const* op = &ops[i];
        afxSize srcOffset = op->srcOffset;
        afxNat rowCnt = op->rowCnt;
        afxNat srcStride = op->srcStride;
        afxNat dstStride = op->dstStride;
        afxNat range = AfxMin(AFX_ALIGNED_SIZEOF(rowCnt * srcStride, 64), AfxGetBufferCapacity(buf, srcOffset));

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

_ZGL afxError _DpuOutputBuf(sglDpu* dpu, afxBuffer buf, afxSize offset, afxNat range, afxStream out, afxSize at)
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

_ZGL afxError _DpuOutputBuf2(sglDpu* dpu, afxBuffer buf, afxStream out, afxNat opCnt, afxBufferIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    GLenum glTarget = GL_COPY_READ_BUFFER;
    GLenum glAccess = GL_MAP_READ_BIT;
    DpuBindAndSyncBuf(dpu, glTarget, buf);

    for (afxNat i = 0; i < opCnt; i++)
    {
        afxBufferIo const* op = &ops[i];

        afxSize srcOffset = op->srcOffset;
        afxNat rowCnt = op->rowCnt;
        afxNat srcStride = op->srcStride;
        afxNat dstStride = op->dstStride;
        afxSize at = op->dstOffset;
        afxNat range = AfxMin(AFX_ALIGNED_SIZEOF(rowCnt * srcStride, 64), AfxGetBufferCapacity(buf, srcOffset));

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

_ZGL afxError _DpuInputBuf(sglDpu* dpu, afxBuffer buf, afxSize offset, afxNat range, afxStream in, afxSize at)
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

_ZGL afxError _DpuInputBuf2(sglDpu* dpu, afxBuffer buf, afxStream in, afxNat opCnt, afxBufferIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    GLenum glTarget = GL_COPY_WRITE_BUFFER;
    GLenum glAccess = GL_MAP_WRITE_BIT;
    DpuBindAndSyncBuf(dpu, glTarget, buf);

    for (afxNat i = 0; i < opCnt; i++)
    {
        afxBufferIo const* op = &ops[i];
        afxSize dstOffset = op->dstOffset;
        afxNat rowCnt = op->rowCnt;
        afxNat dstStride = op->dstStride;
        afxSize at = op->srcOffset;
        afxNat srcStride = op->srcStride;
        afxNat range = AfxMin(AFX_ALIGNED_SIZEOF(rowCnt * dstStride, 64), AfxGetBufferCapacity(buf, dstOffset));

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

_ZGL afxError _DpuRemapBuf(sglDpu* dpu, afxBuffer buf, afxSize offset, afxNat range, afxFlags flags, void** placeholder)
{
    afxError err = AFX_ERR_NONE;
    //AfxAssertObject(dexu, afxFcc_DEXU);
    glVmt const* gl = &dpu->gl;

    GLenum glTarget = NIL;

    if (buf->m.access & afxBufferAccess_W)
        glTarget = GL_COPY_WRITE_BUFFER;
    else
        glTarget = GL_COPY_READ_BUFFER;

    DpuBindAndSyncBuf(dpu, glTarget, buf);

    if (range)
    {
        GLenum glAccess = NIL;
        AfxAssert(!buf->m.bytemap);

#if !0
        if (range == AfxGetBufferCapacity(buf, 0))
        {
            if (!(buf->m.access & afxBufferAccess_R))
            {
                if (buf->m.access & afxBufferAccess_W)
                    glAccess = GL_WRITE_ONLY;
            }
            else
            {
                if (buf->m.access & afxBufferAccess_W)
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

        if (placeholder)
            *placeholder = buf->m.bytemap;
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

_ZGL afxError _BufRemapCb(afxBuffer buf, afxNat offset, afxNat range, afxFlags flags, void** placeholder)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &buf, afxFcc_BUF);
    AfxAssertRange(buf->m.cap, offset, range);
    
    if (buf->m.mappedRange) AfxThrowError();
    else
    {
        afxDrawContext dctx = AfxGetBufferContext(buf);
        AfxAssertObjects(1, &dctx, afxFcc_DCTX);
        afxNat portId = 0;
        afxDrawBridge dexu;
        AfxQueryDrawBridges(dctx, portId, 0, 1, &dexu);
        AfxAssertObjects(1, &dexu, afxFcc_DEXU);
        afxNat queIdx = AFX_INVALID_INDEX;

        if (AFX_INVALID_INDEX == (queIdx = _SglDexuEnqueueMmapCb(dexu, buf, offset, range, flags, placeholder)))
        {
            AfxThrowError();
        }
        else
        {
            if (_AvxWaitForIdleDrawQueue(dexu, queIdx))
                AfxThrowError();
        }
    }
    return err;
}

_ZGL afxError _BufUnmapCb(afxBuffer buf, afxBool wait)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &buf, afxFcc_BUF);

    if (!buf->m.mappedRange) AfxThrowError();
    else
    {
        afxDrawContext dctx = AfxGetBufferContext(buf);
        AfxAssertObjects(1, &dctx, afxFcc_DCTX);
        afxNat portId = 0;
        afxDrawBridge dexu;
        AfxQueryDrawBridges(dctx, portId, 0, 1, &dexu);
        AfxAssertObjects(1, &dexu, afxFcc_DEXU);
        afxNat queIdx = AFX_INVALID_INDEX;

        if (AFX_INVALID_INDEX == (queIdx = _SglDexuEnqueueMmapCb(dexu, buf, 0, 0, 0, NIL)))
        {
            AfxThrowError();
        }
        else
        {
            if (wait && _AvxWaitForIdleDrawQueue(dexu, queIdx))
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

    if (_AvxBufStdImplementation.dtor(buf))
        AfxThrowError();

    return err;
}

_ZGL afxError _BufCtorCb(afxBuffer buf, void** args, afxNat invokeNo)
{
    afxResult err = NIL;
    AfxAssertObjects(1, &buf, afxFcc_BUF);

    if (_AvxBufStdImplementation.ctor(buf, args, invokeNo)) AfxThrowError();
    else
    {
        afxBufferSpecification const *spec = ((afxBufferSpecification const *)args[1]) + invokeNo;

        buf->glHandle = NIL;
        buf->glTarget = NIL;
        buf->updFlags = ZGL_UPD_FLAG_DEVICE_INST;

        afxBufferUsage usage = buf->m.usage;
        afxBufferAccess access = buf->m.access;
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
        else
            AfxThrowError();

        if (access & afxBufferAccess_R)
            glAccess |= GL_MAP_READ_BIT;

        if (access & afxBufferAccess_W)
            glAccess |= GL_MAP_WRITE_BIT;

        if (access & afxBufferAccess_X)
            glAccess |= GL_MAP_PERSISTENT_BIT;

        if (access & afxBufferAccess_COHERENT)
            glAccess |= GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT;

        buf->glAccess = glAccess;

        buf->m.remap = _BufRemapCb;
        buf->m.unmap = _BufUnmapCb;

        if (spec->src)
        {
            afxBufferIo iop = { 0 };
            iop.rowCnt = buf->m.cap;
            iop.srcStride = 1;
            iop.dstStride = 1;

            if (AfxUpdateBuffer(buf, 0, 1, &iop, spec->src))
                AfxThrowError();
        }

        if (err && _AvxBufStdImplementation.dtor(buf))
            AfxThrowError();
    }
    return err;
}
