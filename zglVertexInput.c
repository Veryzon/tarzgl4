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
#include "qwadro/io/afxXml.h"

#include "qwadro/draw/pipe/avxPipeline.h"

#include "qwadro/draw/dev/afxDrawSystem.h"
#include "qwadro/io/afxUri.h"
#include "qwadro/exec/afxSystem.h"

_ZGL afxError _DpuBindAndSyncVin(sglDpu* dpu, avxVertexInput vin, sglVertexInputState* nextVinBindings)
{
    //AfxEntry("pip=%p", pip);
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    if (!vin)
    {
        gl->BindVertexArray(dpu->emptyVao); _SglThrowErrorOccuried();
    }
    else
    {
        AfxAssertObjects(1, &vin, afxFcc_VIN);
        GLuint glHandle = vin->glHandle;

        if ((!glHandle) || (vin->updFlags & ZGL_UPD_FLAG_DEVICE_INST))
        {
            if (glHandle)
            {
                gl->DeleteVertexArrays(1, &glHandle); _SglThrowErrorOccuried();
                glHandle = NIL;
            }
            gl->GenVertexArrays(1, &glHandle); _SglThrowErrorOccuried();
            gl->BindVertexArray(glHandle); _SglThrowErrorOccuried();
            vin->glHandle = glHandle;

            afxNat attrCnt = vin->m.attrCnt;

            for (afxNat i = 0; i < attrCnt; i++)
            {
                avxVertexInputAttr const* attr = &vin->m.attrs[i];
                AfxAssertRange(afxVertexFormat_TOTAL, attr->fmt, 1);
                AfxAssertRange(ZGL_MAX_VERTEX_ATTRIBS, attr->location, 1);
                AfxAssertRange(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, attr->streamIdx, 1);
                AfxAssertRange(ZGL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, attr->offset, 1);

                afxNat location = attr->location;
                afxNat srcIdx = attr->streamIdx;

                GLint glsiz;
                GLenum gltype;
                GLuint glStride;
                AfxToGlVertexFormat(attr->fmt, &glsiz, &gltype, &glStride);

                //gl->BindAttribLocation(glHandle, location, AfxGetStringData(&(vsh->base.ioDecls[i].semantic), 0)); _SglThrowErrorOccuried();

                AfxAssert(16 > location);  // max vertex attrib
                gl->EnableVertexAttribArray(location); _SglThrowErrorOccuried();
                AfxAssert(gl->BindVertexBuffer);
                gl->VertexAttribFormat(location, glsiz, gltype, !!attr->normalized, attr->offset); _SglThrowErrorOccuried();
                //afxNat srcIdx = streamIdx;// dpu->state.vertexInput.streams[streamIdx].srcIdx;
                //AfxAssertRange(_ZGL_MAX_VBO_PER_BIND, srcIdx, 1);
                AfxAssertRange(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, srcIdx, 1);
                gl->VertexAttribBinding(location, srcIdx); _SglThrowErrorOccuried();

                afxNat instDivisor = vin->m.streams[srcIdx].instanceRate;

                if (instDivisor)
                {
                    gl->VertexAttribDivisor(location, instDivisor); _SglThrowErrorOccuried();
                }
            }

            afxNat streamCnt = vin->m.streamCnt;

            for (afxNat i = 0; i < streamCnt; i++)
            {
                avxVertexInputStream const* stream = &vin->m.streams[i];
                AfxAssertRange(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, stream->slotIdx, 1);
                //AfxAssertRange(ZGL_MAX_VERTEX_ATTRIB_STRIDE, 0, stream->stride);
                //gl->BindVertexBuffer(stream->slotIdx, 0, 0, stream->stride); _SglThrowErrorOccuried();
            }
            vin->updFlags &= ~(ZGL_UPD_FLAG_DEVICE);
        }
        else
        {
            gl->BindVertexArray(glHandle); _SglThrowErrorOccuried();
        }

        //sglVertexInputState const* nextVinBindings = &dpu->nextVinBindings;
        afxMask updMask = nextVinBindings->streamUpdMask;

        //if (updMask)
        {
            for (afxNat i = 0; i < vin->m.streamCnt; i++)
            {
                avxVertexInputStream const* stream = &vin->m.streams[i];
                afxNat streamIdx = stream->slotIdx;
                AfxAssertRange(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, streamIdx, 1);
                
                afxNat32 stride = nextVinBindings->sources[streamIdx].stride;
                afxNat32 range = nextVinBindings->sources[streamIdx].range;
                afxNat32 offset = nextVinBindings->sources[streamIdx].offset;
                afxBuffer buf = nextVinBindings->sources[streamIdx].buf;
                afxBool rebind = FALSE;

                if (updMask & AFX_BIT(streamIdx))
                {
                    

                    if ((vin->bindings.sources[streamIdx].buf != buf) ||
                        (vin->bindings.sources[streamIdx].offset != offset) ||
                        (vin->bindings.sources[streamIdx].range != range) ||
                        (vin->bindings.sources[streamIdx].stride != stride)
                        )
                    {
                        vin->bindings.sources[streamIdx].buf = buf;
                        vin->bindings.sources[streamIdx].offset = offset;
                        vin->bindings.sources[streamIdx].range = range;
                        vin->bindings.sources[streamIdx].stride = stride;
                        rebind = TRUE;
                    }
                }

                AfxAssert(gl->BindVertexBuffer);

                if (!buf)
                {
                    gl->BindVertexBuffer(streamIdx, 0, 0, 16); _SglThrowErrorOccuried();
                }
                else
                {
                    AfxAssert(range);
                    AfxAssert(stride);
                    DpuBindAndSyncBuf(dpu, GL_ARRAY_BUFFER, buf);
                    gl->BindVertexBuffer(streamIdx, buf->glHandle, offset, stride); _SglThrowErrorOccuried();
                }
            }
        }

        afxBool rebind = FALSE;
        afxBuffer buf = nextVinBindings->idxSrcBuf;

        if (!buf)
        {
            gl->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            vin->bindings.idxSrcBuf = NIL;
            vin->bindings.idxSrcOff = 0;
            vin->bindings.idxSrcRange = 0;
            vin->bindings.idxSrcSiz = 0;
        }
        else
        {
            afxNat32 off = nextVinBindings->idxSrcOff;
            afxNat32 range = nextVinBindings->idxSrcRange;
            afxNat32 idxSrcSiz = nextVinBindings->idxSrcSiz;

            DpuBindAndSyncBuf(dpu, GL_ELEMENT_ARRAY_BUFFER, buf);
            gl->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->glHandle);
            
            vin->bindings.idxSrcBuf = buf;
            vin->bindings.idxSrcOff = off;
            vin->bindings.idxSrcRange = range;
            vin->bindings.idxSrcSiz = idxSrcSiz;
        }
    }
    return err;
}

_ZGL afxError _SglVinDtor(avxVertexInput vin)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &vin, afxFcc_VIN);
    afxDrawContext dctx = AfxGetParent(vin);

    if (vin->glHandle)
    {
        _SglDctxEnqueueDeletion(dctx, 0, GL_VERTEX_ARRAY, (afxSize)vin->glHandle);
        vin->glHandle = 0;
    }

    if (_AvxVinStdImplementation.dtor(vin))
        AfxThrowError();

    return err;
}

_ZGL afxError _SglVinCtor(avxVertexInput vin, void** args, afxNat invokeNo)
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
