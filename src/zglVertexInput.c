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

_ZGL afxError _DpuBindVertexInput(zglDpu* dpu, avxVertexInput vin)
{
    //AfxEntry("pip=%p", pip);
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    if (!vin)
    {
        if (dpu->activeVin ||
            (dpu->activeVinGpuHandle != dpu->emptyVao))
        {
            gl->BindVertexArray(dpu->emptyVao); _SglThrowErrorOccuried();

            dpu->nextVin = NIL;
            dpu->activeVin = NIL;
            dpu->activeVinGpuHandle = dpu->emptyVao;
        }
    }
    else
    {
        AfxAssertObjects(1, &vin, afxFcc_VIN);
        GLuint glHandle = vin->glHandle;

        if (!glHandle || (vin->updFlags & ZGL_UPD_FLAG_DEVICE_INST))
        {
            if (glHandle)
            {
                gl->DeleteVertexArrays(1, &glHandle), glHandle = NIL; _SglThrowErrorOccuried();
            }

            gl->GenVertexArrays(1, &glHandle); _SglThrowErrorOccuried();
            gl->BindVertexArray(glHandle); _SglThrowErrorOccuried();
            AfxAssert(gl->IsVertexArray(glHandle));
            
            afxUnit attrCnt = vin->m.attrCnt;

            for (afxUnit i = 0; i < attrCnt; i++)
            {
                avxVertexInputAttr const* attr = &vin->m.attrs[i];
                AfxAssertRange(afxVertexFormat_TOTAL, attr->fmt, 1);
                AfxAssertRange(ZGL_MAX_VERTEX_ATTRIBS, attr->location, 1);
                AfxAssertRange(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, attr->streamIdx, 1);
                AfxAssertRange(ZGL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, attr->offset, 1);

                afxUnit location = attr->location;
                afxUnit srcIdx = attr->streamIdx;

                GLint glsiz;
                GLenum gltype;
                GLuint glStride;
                AfxToGlVertexFormat(attr->fmt, &glsiz, &gltype, &glStride);

                //gl->BindAttribLocation(glHandle, location, AfxGetStringData(&(vsh->base.ioDecls[i].semantic), 0)); _SglThrowErrorOccuried();

                AfxAssert(16 > location);  // max vertex attrib
                gl->EnableVertexAttribArray(location); _SglThrowErrorOccuried();
                AfxAssert(gl->BindVertexBuffer);
                gl->VertexAttribFormat(location, glsiz, gltype, !!attr->normalized, attr->offset); _SglThrowErrorOccuried();
                //afxUnit srcIdx = streamIdx;// dpu->state.vertexInput.streams[streamIdx].srcIdx;
                //AfxAssertRange(_ZGL_MAX_VBO_PER_BIND, srcIdx, 1);
                AfxAssertRange(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, srcIdx, 1);
                gl->VertexAttribBinding(location, srcIdx); _SglThrowErrorOccuried();

                afxUnit instDivisor = vin->m.streams[srcIdx].instanceRate;

                if (instDivisor)
                {
                    gl->VertexAttribDivisor(location, instDivisor); _SglThrowErrorOccuried();
                }
            }

            afxUnit streamCnt = vin->m.streamCnt;

            for (afxUnit i = 0; i < streamCnt; i++)
            {
                avxVertexInputStream const* stream = &vin->m.streams[i];
                AfxAssertRange(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, stream->slotIdx, 1);
                //AfxAssertRange(ZGL_MAX_VERTEX_ATTRIB_STRIDE, 0, stream->stride);
                //gl->BindVertexBuffer(stream->slotIdx, 0, 0, stream->stride); _SglThrowErrorOccuried();
            }

            vin->glHandle = glHandle;
            vin->updFlags &= ~(ZGL_UPD_FLAG_DEVICE);

            dpu->nextVin = NIL; // force update in "next first time".
            dpu->activeVin = vin;
            dpu->activeVinGpuHandle = glHandle;

            // we must force recheck to reapply bound buffers to new bound VAO.
            dpu->nextVinBindings.sourcesUpdMask = 0xfffffff;
            dpu->nextVinBindings.iboUpdReq = 1;
        }
        else
        {
            if ((dpu->activeVin != vin) ||
                (dpu->activeVinGpuHandle != glHandle))
            {
                AfxAssert(gl->IsVertexArray(glHandle));
                gl->BindVertexArray(glHandle); _SglThrowErrorOccuried();

                dpu->nextVin = NIL; // force update in "next first time".
                dpu->activeVin = vin;
                dpu->activeVinGpuHandle = glHandle;

                // we must force recheck to reapply bound buffers to new bound VAO.
                dpu->nextVinBindings.sourcesUpdMask = 0xfffffff;
                dpu->nextVinBindings.iboUpdReq = 1;
            }
        }
    }
    return err;
}

_ZGL afxError _SglVinDtor(avxVertexInput vin)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &vin, afxFcc_VIN);
    afxDrawContext dctx = AfxGetProvider(vin);

    if (vin->glHandle)
    {
        _SglDctxEnqueueDeletion(dctx, 0, GL_VERTEX_ARRAY, (afxSize)vin->glHandle);
        vin->glHandle = 0;
    }

    if (_AvxVinStdImplementation.dtor(vin))
        AfxThrowError();

    return err;
}

_ZGL afxError _SglVinCtor(avxVertexInput vin, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &vin, afxFcc_VIN);

    if (_AvxVinStdImplementation.ctor(vin, args, invokeNo)) AfxThrowError();
    else
    {
        vin->glHandle = 0;
        vin->updFlags = ZGL_UPD_FLAG_DEVICE_INST;

        if (err && _AvxVinStdImplementation.dtor(vin))
            AfxThrowError();
    }
    return err;
}
