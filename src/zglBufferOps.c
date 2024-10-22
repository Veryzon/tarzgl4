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

_ZGL void DpuBufCpy(zglDpu* dpu, afxBuffer src, afxBuffer dst, afxUnit opCnt, afxBufferCopy const ops[])
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    DpuBindAndSyncBuf(dpu, GL_COPY_READ_BUFFER, src);
    DpuBindAndSyncBuf(dpu, GL_COPY_WRITE_BUFFER, dst);

    for (afxUnit i = 0; i < opCnt; i++)
    {
        gl->CopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, ops[i].srcOffset, ops[i].dstOffset, ops[i].range); _SglThrowErrorOccuried();
    }
}

_ZGL void DpuBufSet(zglDpu* dpu, afxBuffer buf, afxUnit offset, afxUnit range, afxUnit data)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    GLenum glTarget = GL_COPY_WRITE_BUFFER;
    DpuBindAndSyncBuf(dpu, glTarget, buf);

    AfxAssert(buf->glAccess & GL_MAP_WRITE_BIT);
    void* dst = gl->MapBufferRange(glTarget, offset, range, GL_MAP_WRITE_BIT); _SglThrowErrorOccuried();
    AfxAssert(dst);
    AfxStream(range, 0, sizeof(data), &data, dst);
    gl->UnmapBuffer(glTarget); _SglThrowErrorOccuried();
    gl->FlushMappedBufferRange(glTarget, offset, range); _SglThrowErrorOccuried();
}

_ZGL void DpuBufRw(zglDpu* dpu, afxBuffer buf, afxUnit offset, afxUnit range, afxBool toHost, void* srcOrDst)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    GLenum glTarget = toHost ? GL_COPY_READ_BUFFER : GL_COPY_WRITE_BUFFER;
    GLenum glAccess = toHost ? GL_MAP_READ_BIT : GL_MAP_WRITE_BIT;
    AfxAssert(buf->glAccess & glAccess);

    glTarget = buf->glTarget;
    DpuBindAndSyncBuf(dpu, glTarget, buf);

    if (toHost)
    {
        gl->GetBufferSubData(glTarget, offset, range, srcOrDst); _SglThrowErrorOccuried();
    }
    else
    {
        gl->BufferSubData(glTarget, offset, range, srcOrDst); _SglThrowErrorOccuried();
    }
}
