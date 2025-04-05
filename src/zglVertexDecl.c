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

_ZGL afxError DpuBindVertexInput(zglDpu* dpu, avxVertexInput vin)
{
    //AfxEntry("pip=%p", pip);
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

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
        GLuint glHandle = vin->glHandle[dpu->m.exuIdx];

        if (!glHandle || (vin->updFlags & ZGL_UPD_FLAG_DEVICE_INST))
        {
            if (glHandle)
            {
                gl->DeleteVertexArrays(1, &glHandle), glHandle = NIL; _ZglThrowErrorOccuried();
            }

            gl->GenVertexArrays(1, &glHandle); _ZglThrowErrorOccuried();
            gl->BindVertexArray(glHandle); _ZglThrowErrorOccuried();
            AFX_ASSERT(gl->IsVertexArray(glHandle));

            if (vin->m.tag.len)
            {
                gl->ObjectLabel(GL_VERTEX_ARRAY, glHandle, vin->m.tag.len, (GLchar const*)vin->m.tag.start); _ZglThrowErrorOccuried();
            }
            afxUnit attrCnt = vin->m.attrCnt;

            for (afxUnit i = 0; i < attrCnt; i++)
            {
                avxVertexAttr const* attr = &vin->m.attrs[i];
                AFX_ASSERT_RANGE(avxFormat_TOTAL, attr->fmt, 1);
                AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIBS, attr->location, 1);
                AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, attr->srcIdx, 1);
                AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, attr->offset, 1);

                afxUnit location = attr->location;
                afxUnit srcIdx = attr->srcIdx;

                avxFormatDescription pfd;
                AvxDescribeFormats(1, &attr->fmt, &pfd);
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

                /*
                    Refer to Identification of Formats and Conversion from Normalized Fixed-Point to Floating-Point in the specification.

                    UNORM is a float in the range of [0, 1].
                    SNORM is the same but in the range of [-1, 1]
                    USCALED is the unsigned integer value converted to float
                    SSCALED is the integer value converted to float
                    UINT is an unsigned integer
                    SINT is a signed integer
                    I.e. for the VK_FORMAT_R8_*:

                    for UNORM raw 0 would give 0.0f, raw 255 would give 1.0f
                    for SNORM raw -127 (resp. 129) would give -1.0f, raw 127 would give 1.0f
                    USCALED raw 0 would give 0.0f, raw 255 would give 255.0f
                    SSCALEDraw -128 (resp. 128) would give -128.0f, raw 127 would give 127.0f
                    -128 (-2n-1) is not meaningful in SNORM, and simply clamps to -1.0f.
                */

                /*
                    For glVertexAttribFormat and glVertexArrayAttribFormat, if normalized is GL_TRUE, then integer data is normalized 
                    to the range [-1, 1] or [0, 1] if it is signed or unsigned, respectively. If normalized is GL_FALSE then integer 
                    data is directly converted to floating point.

                    glVertexAttribLFormat and glVertexArrayAttribLFormat is used to specify layout for data associated with
                    a generic attribute variable declared as 64-bit double precision components. For glVertexAttribLFormat
                    and glVertexArrayAttribLFormat, type must be GL_DOUBLE. In contrast to glVertexAttribFormat or
                    glVertexArrayAttribFormat, which will cause data declared as GL_DOUBLE to be "converted" to 32-bit
                    representation, glVertexAttribLFormat and glVertexArrayAttribLFormat cause such data to be left in its
                    natural, 64-bit representation.

                    Data for an array specified by glVertexAttribIFormat and glVertexArrayAttribIFormat will always be
                    left as integer values; such data are referred to as pure integers.

                    VertexAttribFormat()'s 'normalized' specifies whether fixed-point data values should be normalized (GL_TRUE) 
                    or converted directly as fixed-point values (GL_FALSE) when they are accessed. 
                    This parameter is ignored if type is GL_FIXED.
                */

#if !0
                switch (pfd.type[0])
                {
                case avxFormatType_F:
                case avxFormatType_SFIXED:
                {
                    if (pfd.bpc[0] == 64)
                    {
                        AFX_ASSERT(gltype == GL_DOUBLE);
                        gl->VertexAttribLFormat(location, glsiz, gltype, attr->offset); _ZglThrowErrorOccuried();
                    }
                    else
                    {
                        //AFX_ASSERT(gltype == GL_FLOAT);
                        gl->VertexAttribFormat(location, glsiz, gltype, FALSE, attr->offset); _ZglThrowErrorOccuried();
                    }
                    break;
                }
                case avxFormatType_SN:
                case avxFormatType_UN:
                {
                    //AFX_ASSERT(pfd.isNormalized[0]);
                    gl->VertexAttribFormat(location, glsiz, gltype, TRUE, attr->offset); _ZglThrowErrorOccuried();
                    break;
                }
                case avxFormatType_SS:
                case avxFormatType_US:
                {
                    //AFX_ASSERT(gltype == GL_FLOAT);
                    //AFX_ASSERT(!pfd.isNormalized[0]);
                    gl->VertexAttribFormat(location, glsiz, gltype, FALSE, attr->offset); _ZglThrowErrorOccuried();
                    break;
                }
                case avxFormatType_I:
                case avxFormatType_U:
                {
                    gl->VertexAttribIFormat(location, glsiz, gltype, attr->offset); _ZglThrowErrorOccuried();
                    break;
                }
                default:
                {
                    gl->VertexAttribFormat(location, glsiz, gltype, /*!!attr->normalized*/pfd.isNormalized[0], attr->offset); _ZglThrowErrorOccuried();
                    break;
                }
                }
#else
                gl->VertexAttribFormat(location, glsiz, gltype, /*!!attr->normalized*/pfd.isNormalized[0], attr->offset); _ZglThrowErrorOccuried();
#endif
                //afxUnit srcIdx = streamIdx;// dpu->state.vertexInput.streams[streamIdx].srcIdx;
                //AFX_ASSERT_RANGE(_ZGL_MAX_VBO_PER_BIND, srcIdx, 1);
                AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, srcIdx, 1);
                gl->VertexAttribBinding(location, srcIdx); _ZglThrowErrorOccuried();

                afxUnit instDivisor = vin->m.srcs[srcIdx].instanceRate;

                if (instDivisor)
                {
                    gl->VertexAttribDivisor(location, instDivisor); _ZglThrowErrorOccuried();
                }
            }

            afxUnit streamCnt = vin->m.srcCnt;

            for (afxUnit i = 0; i < streamCnt; i++)
            {
                avxVertexFetch const* src = &vin->m.srcs[i];
                AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, src->srcIdx, 1);
                //AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_STRIDE, 0, stream->stride);
                //gl->BindVertexBuffer(stream->slotIdx, 0, 0, stream->stride); _ZglThrowErrorOccuried();
            }

            vin->glHandle[dpu->m.exuIdx] = glHandle;
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

_ZGL afxError _ZglVinDtor(avxVertexInput vin)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_VIN, 1, &vin);
    afxDrawSystem dsys = AfxGetProvider(vin);

    for (afxUnit i = 0; i < ZGL_MAX_VAO_HANDLES; i++)
    {
        if (vin->glHandle)
        {
            _ZglDsysEnqueueDeletion(dsys, i, GL_VERTEX_ARRAY, (afxSize)vin->glHandle[i]);
            vin->glHandle[i] = 0;
        }
    }

    if (_AVX_VIN_CLASS_CONFIG.dtor(vin))
        AfxThrowError();

    return err;
}

_ZGL afxError _ZglVinCtor(avxVertexInput vin, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_VIN, 1, &vin);

    if (_AVX_VIN_CLASS_CONFIG.ctor(vin, args, invokeNo)) AfxThrowError();
    else
    {
        AfxZero(vin->glHandle, sizeof(vin->glHandle));
        vin->updFlags = ZGL_UPD_FLAG_DEVICE_INST;

        afxDrawSystem dsys = AfxGetProvider(vin);
        vin->vdeclUniqueId = ++dsys->vdecUniqueId;

        if (err && _AVX_VIN_CLASS_CONFIG.dtor(vin))
            AfxThrowError();
    }
    return err;
}
