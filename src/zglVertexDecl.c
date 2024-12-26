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

_ZGL afxError _DpuBindVertexInput(zglDpu* dpu, avxVertexDecl vin)
{
    //AfxEntry("pip=%p", pip);
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    if (!vin)
    {
        if (dpu->activeVin ||
            (dpu->activeVinGpuHandle != dpu->emptyVao))
        {
            gl->BindVertexArray(dpu->emptyVao); _ZglThrowErrorOccuried();

            dpu->nextVin = NIL;
            dpu->activeVin = NIL;
            dpu->activeVinGpuHandle = dpu->emptyVao;
        }
    }
    else
    {
        AFX_ASSERT_OBJECTS(afxFcc_VIN, 1, &vin);
        GLuint glHandle = vin->glHandle;

        if (!glHandle || (vin->updFlags & ZGL_UPD_FLAG_DEVICE_INST))
        {
            if (glHandle)
            {
                gl->DeleteVertexArrays(1, &glHandle), glHandle = NIL; _ZglThrowErrorOccuried();
            }

            gl->GenVertexArrays(1, &glHandle); _ZglThrowErrorOccuried();
            gl->BindVertexArray(glHandle); _ZglThrowErrorOccuried();
            AFX_ASSERT(gl->IsVertexArray(glHandle));
            
            afxUnit attrCnt = vin->m.attrCnt;

            for (afxUnit i = 0; i < attrCnt; i++)
            {
                avxVertexInput const* attr = &vin->m.attrs[i];
                AFX_ASSERT_RANGE(avxFormat_TOTAL, attr->fmt, 1);
                AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIBS, attr->location, 1);
                AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, attr->srcIdx, 1);
                AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, attr->offset, 1);

                afxUnit location = attr->location;
                afxUnit srcIdx = attr->srcIdx;

                avxFormatDescription pfd;
                AfxDescribePixelFormat(attr->fmt, &pfd);
                GLint glsiz = pfd.compCnt;
                GLenum gltype;
                GLuint glStride;
                //AfxToGlVertexFormat(attr->fmt, &glsiz, &gltype, &glStride);

                GLint glIntFmt;
                GLenum glLayout;
                GLenum glType;
                ZglToGlFormat2(attr->fmt, &glIntFmt, &glLayout, &glType);
                gltype = glType;
                
                //gl->BindAttribLocation(glHandle, location, AfxGetStringData(&(vsh->base.ioDecls[i].semantic), 0)); _ZglThrowErrorOccuried();

                AFX_ASSERT(16 > location);  // max vertex attrib
                gl->EnableVertexAttribArray(location); _ZglThrowErrorOccuried();
                AFX_ASSERT(gl->BindVertexBuffer);
                gl->VertexAttribFormat(location, glsiz, gltype, /*!!attr->normalized*/pfd.isNormalized[0], attr->offset); _ZglThrowErrorOccuried();
                //afxUnit srcIdx = streamIdx;// dpu->state.vertexInput.streams[streamIdx].srcIdx;
                //AFX_ASSERT_RANGE(_ZGL_MAX_VBO_PER_BIND, srcIdx, 1);
                AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, srcIdx, 1);
                gl->VertexAttribBinding(location, srcIdx); _ZglThrowErrorOccuried();

                afxUnit instDivisor = vin->m.streams[srcIdx].instanceRate;

                if (instDivisor)
                {
                    gl->VertexAttribDivisor(location, instDivisor); _ZglThrowErrorOccuried();
                }
            }

            afxUnit streamCnt = vin->m.streamCnt;

            for (afxUnit i = 0; i < streamCnt; i++)
            {
                avxVertexFetch const* stream = &vin->m.streams[i];
                AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, stream->srcIdx, 1);
                //AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_STRIDE, 0, stream->stride);
                //gl->BindVertexBuffer(stream->slotIdx, 0, 0, stream->stride); _ZglThrowErrorOccuried();
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
                AFX_ASSERT(gl->IsVertexArray(glHandle));
                gl->BindVertexArray(glHandle); _ZglThrowErrorOccuried();

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

_ZGL afxError _ZglVinDtor(avxVertexDecl vin)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_VIN, 1, &vin);
    afxDrawSystem dsys = AfxGetProvider(vin);

    if (vin->glHandle)
    {
        _ZglDsysEnqueueDeletion(dsys, 0, GL_VERTEX_ARRAY, (afxSize)vin->glHandle);
        vin->glHandle = 0;
    }

    if (_AVX_VIN_CLASS_CONFIG.dtor(vin))
        AfxThrowError();

    return err;
}

_ZGL afxError _ZglVinCtor(avxVertexDecl vin, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_VIN, 1, &vin);

    if (_AVX_VIN_CLASS_CONFIG.ctor(vin, args, invokeNo)) AfxThrowError();
    else
    {
        vin->glHandle = 0;
        vin->updFlags = ZGL_UPD_FLAG_DEVICE_INST;

        if (err && _AVX_VIN_CLASS_CONFIG.dtor(vin))
            AfxThrowError();
    }
    return err;
}
