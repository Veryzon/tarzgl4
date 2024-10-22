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

_ZGL void _DpuBeginQuery(zglDpu* dpu, avxQueryPool qryp, afxUnit qryIdx, afxBool precise)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &qryp, afxFcc_QRYP);
    AfxAssertRange(qryp->m.cap, qryIdx, 1);
    glVmt const* gl = &dpu->gl;

    gl->BeginQuery(qryp->glTarget, qryp->glHandle[qryIdx]);
    dpu->timeElapsedQueryIdActive = qryp->glHandle[qryIdx];
}

_ZGL void _DpuEndQuery(zglDpu* dpu, avxQueryPool qryp, afxUnit queryIdx)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &qryp, afxFcc_QRYP);
    AfxAssertRange(qryp->m.cap, queryIdx, 1);
    glVmt const* gl = &dpu->gl;

    gl->EndQuery(qryp->glTarget);
    dpu->timeElapsedQueryIdActive = 0;
}

_ZGL void _DpuCopyQueryResults(zglDpu* dpu, avxQueryPool qryp, afxUnit baseQuery, afxUnit queryCnt, afxBuffer buf, afxSize offset, afxSize stride, afxQueryResultFlags flags)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &qryp, afxFcc_QRYP);
    AfxAssertRange(qryp->m.cap, baseQuery, queryCnt);
    AfxAssertObjects(1, &buf, afxFcc_BUF);
    AfxAssertRange(AfxGetBufferCapacity(buf, 0), offset, stride);
    glVmt const* gl = &dpu->gl;
    GLenum pname = (flags & afxQueryResultFlag_WAIT) ? GL_QUERY_RESULT : GL_QUERY_RESULT_NO_WAIT;

    //DpuBindAndSyncBuf(dpu, TRUE, TRUE, GL_QUERY_RESULT_BUFFER, );

    if (flags & afxQueryResultFlag_64)
    {
        for (afxUnit i = 0; i < qryp->m.cap; i++)
        {
            gl->GetQueryObjectui64v(qryp->glHandle[i], pname, (GLuint64[]) { offset });

            if (flags & afxQueryResultFlag_WITH_AVAIL)
            {
                GLuint avail;
                gl->GetQueryObjectuiv(qryp->glHandle[i], GL_QUERY_RESULT_AVAILABLE, &avail);
                // ??? escrever direta num offset?
            }
        }
    }
    else
    {
        for (afxUnit i = 0; i < qryp->m.cap; i++)
        {
            gl->GetQueryObjectuiv(qryp->glHandle[i], pname, (GLuint[]) { offset });

            if (flags & afxQueryResultFlag_WITH_AVAIL)
            {
                GLuint avail;
                gl->GetQueryObjectuiv(qryp->glHandle[i], GL_QUERY_RESULT_AVAILABLE, &avail);
                // ??? escrever direta num offset?
            }
        }
    }
}

_ZGL void _DpuResetQueries(zglDpu* dpu, avxQueryPool qryp, afxUnit baseQuery, afxUnit queryCnt)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &qryp, afxFcc_QRYP);
    AfxAssertRange(qryp->m.cap, baseQuery, queryCnt);
    glVmt const* gl = &dpu->gl;


}

_ZGL void _DpuWriteTimestamp(zglDpu* dpu, avxQueryPool qryp, afxUnit queryIdx, avxPipelineStage stage)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &qryp, afxFcc_QRYP);
    AfxAssertRange(qryp->m.cap, queryIdx, 1);
    glVmt const* gl = &dpu->gl;
    AfxAssert(qryp->glTarget == GL_TIMESTAMP);
    gl->QueryCounter(qryp->glHandle[queryIdx], qryp->glTarget);
}

_ZGL afxError _DpuBindAndSyncQryp(zglDpu* dpu, afxBool syncOnly, avxQueryPool qryp)
{
    //AfxEntry("pip=%p", pip);
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;


    if (!qryp)
    {
        
    }
    else
    {
        AfxAssertObjects(1, &qryp, afxFcc_QRYP);
        GLuint* glHandle = qryp->glHandle;

        if (/*(!glHandle) || */(qryp->updFlags & ZGL_UPD_FLAG_DEVICE))
        {
            if (/*(!glHandle) || */(qryp->updFlags & ZGL_UPD_FLAG_DEVICE_INST))
            {
                if (glHandle)
                {
                    gl->DeleteQueries(qryp->m.cap, glHandle); _SglThrowErrorOccuried();
                    glHandle = NIL;
                }
                gl->GenQueries(qryp->m.cap, glHandle); _SglThrowErrorOccuried();
                qryp->glHandle = glHandle;
                AfxLogEcho("Query qryp inited. %u", qryp->m.cap);
            }

            qryp->updFlags |= ~ZGL_UPD_FLAG_DEVICE;
        }
    }

    return err;
}

_ZGL afxError _SglQrypDtor(avxQueryPool qryp)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &qryp, afxFcc_QRYP);
    afxDrawContext dctx = AfxGetQueryPoolContext(qryp);

    if (qryp->glHandle)
    {
        for (afxUnit i = 0; i < qryp->m.cap; i++)
        {
            _SglDctxEnqueueDeletion(dctx, 0, GL_QUERY_BUFFER, (afxSize)qryp->glHandle[i]);
        }
        AfxDeallocate(qryp->glHandle);
        qryp->glHandle = 0;
    }

    return err;
}

_ZGL afxError _SglQrypCtor(avxQueryPool qryp, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &qryp, afxFcc_QRYP);

    afxDrawContext dctx = args[0];
    afxQueryType type = *(afxQueryType const*)args[1];
    afxUnit cap = *(afxUnit const*)args[2];

    qryp->glHandle = AfxAllocate(qryp->m.cap, sizeof(qryp->glHandle[0]), 0, AfxHere());
    qryp->updFlags = ZGL_UPD_FLAG_DEVICE_INST;
    qryp->m.type = type;
    qryp->m.cap = cap;

    if (qryp->m.type == afxQueryType_OCCLUSION)
    {
        qryp->glTarget = GL_SAMPLES_PASSED;
    }
    else if (qryp->m.type == afxQueryType_PIPELINE)
    {
        qryp->glTarget = 0;
    }
    else if (qryp->m.type == afxQueryType_TIMESTAMP)
    {
        qryp->glTarget = GL_TIME_ELAPSED;
    }
    else AfxThrowError();

    return err;
}
