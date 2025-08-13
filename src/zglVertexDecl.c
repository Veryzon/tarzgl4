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

_ZGL void _ZglFlushVertexInputState(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

    avxVertexInput vin = dpu->nextVin;

    if (!vin)
    {
        dpu->activeVin = NIL;

        if (dpu->activeVinGpuHandle != dpu->emptyVao)
        {
            gl->BindVertexArray(dpu->emptyVao); _ZglThrowErrorOccuried();
            dpu->activeVinGpuHandle = dpu->emptyVao;
        }
        return;
    }

    AFX_ASSERT_OBJECTS(afxFcc_VIN, 1, &vin);
    GLuint glHandle = vin->glHandle[dpu->m.exuIdx];

    if (glHandle && !(vin->updFlags & ZGL_UPD_FLAG_DEVICE_INST))
    {
        if ((dpu->activeVin != vin) ||
            (dpu->activeVinGpuHandle != glHandle))
        {
            AFX_ASSERT(gl->IsVertexArray(glHandle));
            gl->BindVertexArray(glHandle); _ZglThrowErrorOccuried();
            dpu->activeVinGpuHandle = glHandle;
            dpu->activeVin = vin;
            // force recheck to reapply bound buffers to new bound VAO.
            dpu->nextVinBindings.sourcesUpdMask = 0xfffffff;
            dpu->nextVinBindings.iboUpdReq = 1;
        }
    }
    else
    {
        if (glHandle)
        {
            gl->DeleteVertexArrays(1, &glHandle), glHandle = NIL; _ZglThrowErrorOccuried();
        }
        gl->GenVertexArrays(1, &glHandle); _ZglThrowErrorOccuried();
        gl->BindVertexArray(glHandle); _ZglThrowErrorOccuried();
        AFX_ASSERT(gl->IsVertexArray(glHandle));
        dpu->activeVinGpuHandle = glHandle;
        dpu->activeVin = vin;
        // force recheck to reapply bound buffers to new bound VAO.
        dpu->nextVinBindings.sourcesUpdMask = 0xfffffff;
        dpu->nextVinBindings.iboUpdReq = 1;

        if (vin->m.tag.len)
        {
            gl->ObjectLabel(GL_VERTEX_ARRAY, glHandle, vin->m.tag.len, (GLchar const*)vin->m.tag.start); _ZglThrowErrorOccuried();
        }

        avxVertexFetch const* srcs = vin->m.srcs;
        afxUnit streamCnt = vin->m.srcCnt;
        for (afxUnit i = 0; i < streamCnt; i++)
        {
            avxVertexFetch const* stream = &srcs[i];
            AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, stream->pin, 1);
            //AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_STRIDE, 0, stream->stride);
            //gl->BindVertexBuffer(stream->slotIdx, 0, 0, stream->stride); _ZglThrowErrorOccuried();

            gl->VertexBindingDivisor(stream->pin, stream->instRate); _ZglThrowErrorOccuried();

            afxUnit totalAttrCnt = vin->m.totalAttrCnt;
            afxUnit attrCnt = stream->attrCnt;
            afxUnit baseAttrIdx = stream->baseAttrIdx;
            avxVertexAttr const* attrs = vin->m.attrs;
            for (afxUnit j = 0; j < attrCnt; j++)
            {
                afxUnit attrIdx = baseAttrIdx + j;
                avxVertexAttr const* attr = &attrs[attrIdx];
                AFX_ASSERT_RANGE(avxFormat_TOTAL, attr->fmt, 1);
                AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIBS, attr->location, 1);
                AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, stream->pin, 1);
                AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, attr->offset, 1);


                afxUnit location = attr->location;
                AFX_ASSERT(16 > location);  // max vertex attrib
                gl->EnableVertexAttribArray(location); _ZglThrowErrorOccuried();
                AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, stream->pin, 1);
                gl->VertexAttribBinding(location, stream->pin); _ZglThrowErrorOccuried();

#if 0
                if (stream->instRate)
                {
                    // avoid calling this function directly to avoid misleading driver hints.
                    // glVertexBindingDivisor is much like glVertexAttribDivisor, except applied to a binding index instead of an attribute index. All vertex attributes associated with this binding index will use the same divisor.
                    gl->VertexAttribDivisor(location, stream->instRate); _ZglThrowErrorOccuried();
                }
#endif

                avxFormatDescription pfd;
                AvxDescribeFormats(1, &attr->fmt, &pfd);
                GLint glsiz = pfd.compCnt;
                GLint glIntFmt;
                GLenum glLayout;
                GLenum gltype;
                ZglToGlFormat2(attr->fmt, &glIntFmt, &glLayout, &gltype);

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
            }
        }
        vin->glHandle[dpu->m.exuIdx] = glHandle;
        vin->updFlags &= ~(ZGL_UPD_FLAG_DEVICE);
    }

    // BIND RESOURCES (VERTEX AND INDEX BUFFERS)

    zglVertexInputState* activeBindings = &vin->bindings;
    zglVertexInputState* nextBindings = &dpu->nextVinBindings;
    afxMask sourcesUpdMask = nextBindings->sourcesUpdMask;

#ifndef FORCE_VBO_REBIND
    if (sourcesUpdMask)
#endif
    {
        nextBindings->sourcesUpdMask = NIL;

        for (afxUnit i = 0; i < vin->m.srcCnt; i++)
        {
            avxVertexFetch const* stream = &vin->m.srcs[i];
            afxUnit streamIdx = stream->pin;
            AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, streamIdx, 1);

#ifndef FORCE_VBO_REBIND
            if (!(sourcesUpdMask & AFX_BITMASK(streamIdx))) // skip if has not updates
                continue;
#endif

            avxBuffer nextBuf = nextBindings->sources[streamIdx].buf;
            afxUnit nextOff = nextBindings->sources[streamIdx].offset;
            afxUnit nextRange = nextBindings->sources[streamIdx].range;
            afxUnit nextStride = nextBindings->sources[streamIdx].stride;
            afxBool forceRebind = TRUE;

            afxUnit bufUniqueId = nextBuf ? nextBuf->bufUniqueId : activeBindings->sources[streamIdx].bufUniqueId;

#ifndef FORCE_VBO_REBIND
            if ((forceRebind) ||
                (activeVinBindings->sources[streamIdx].bufUniqueId != bufUniqueId) || // if it has not a GL handle yet or it is different.
                (activeVinBindings->sources[streamIdx].buf != nextBuf) ||
                (activeVinBindings->sources[streamIdx].offset != nextOff) ||
                (activeVinBindings->sources[streamIdx].range != nextRange) ||
                (activeVinBindings->sources[streamIdx].stride != nextStride))
#endif
            {
                if (!nextBuf)
                {
                    AFX_ASSERT(gl->BindVertexBuffer);
                    gl->BindVertexBuffer(streamIdx, 0, 0, 16); _ZglThrowErrorOccuried();

                    activeBindings->sources[streamIdx].buf = 0;
                    activeBindings->sources[streamIdx].offset = 0;
                    activeBindings->sources[streamIdx].range = 0;
                    activeBindings->sources[streamIdx].stride = 0;
                }
                else
                {
                    activeBindings->sources[streamIdx].buf = nextBuf;
                    activeBindings->sources[streamIdx].offset = nextOff;
                    activeBindings->sources[streamIdx].range = nextRange;
                    activeBindings->sources[streamIdx].stride = nextStride;

                    AFX_ASSERT(nextRange);
                    AFX_ASSERT(nextStride);

                    if (!nextBuf->glHandle)
                        DpuBindAndSyncBuf(dpu, GL_ARRAY_BUFFER, nextBuf, FALSE);

                    /*
                        glBindVertexBuffer is kind of like glBindBufferRange, but it is specifically intended for vertex buffer objects. 
                        The bindingindex is, as the name suggests, not a vertex attribute. It is a binding index, which can range from 0 to GL_MAX_VERTEX_ATTRIB_BINDINGS - 1. 
                        This will almost certainly be 16.
                    */

                    AFX_ASSERT(gl->BindVertexBuffer);
                    gl->BindVertexBuffer(streamIdx, nextBuf->glHandle, nextOff, nextStride); _ZglThrowErrorOccuried();

                    // What to do about it? Dynamic divisors vs instance rate being set on attribute specification.
                    //gl->VertexBindingDivisor(streamIdx, stream->instRate); _ZglThrowErrorOccuried();
                }

                activeBindings->sources[streamIdx].bufUniqueId = bufUniqueId;

                //nextVinBindings->sources[streamIdx].buf = NIL; // force update in "next first time".
            }
        }
    }

#ifndef FORCE_IBO_REBIND
    if (nextVinBindings->iboUpdReq)
#endif
    {
        nextBindings->iboUpdReq = FALSE;

        avxBuffer nextBuf = nextBindings->idxSrcBuf;
        afxUnit nextOff = nextBindings->idxSrcOff;
        afxUnit nextRange = nextBindings->idxSrcRange;
        afxUnit nextStride = nextBindings->idxSrcSiz;
        afxBool forceRebind = FALSE;
        afxBool bound = FALSE;

        afxUnit bufUniqueId = nextBuf ? nextBuf->bufUniqueId : activeBindings->iboUniqueId;

#ifndef FORCE_IBO_REBIND
        if ((forceRebind) ||
            (activeVinBindings->iboUniqueId != bufUniqueId) || // if it has not a GL handle yet or it is different.
            (activeVinBindings->idxSrcBuf != nextBuf) ||
            (activeVinBindings->idxSrcOff != nextOff) ||
            (activeVinBindings->idxSrcRange != nextRange) ||
            (activeVinBindings->idxSrcSiz != nextStride))
#endif
        {
            DpuBindAndSyncBuf(dpu, GL_ELEMENT_ARRAY_BUFFER, nextBuf, TRUE);

            if (!nextBuf)
            {
                //gl->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

                activeBindings->idxSrcBuf = NIL;
                activeBindings->idxSrcOff = 0;
                activeBindings->idxSrcRange = 0;
                activeBindings->idxSrcSiz = 0;
            }
            else
            {
                //DpuBindAndSyncBuf(dpu, GL_ELEMENT_ARRAY_BUFFER, nextBuf);
                //gl->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->glHandle);

                activeBindings->idxSrcBuf = nextBuf;
                activeBindings->idxSrcOff = nextOff;
                activeBindings->idxSrcRange = nextRange;
                activeBindings->idxSrcSiz = nextStride;
            }

            activeBindings->iboUniqueId = bufUniqueId;
            //nextBindings->idxSrcBuf = NIL; // force update in "next first time".
        }
    }
}

_ZGL void DpuBindVertexBuffers(zglDpu* dpu, afxUnit first, afxUnit cnt, avxBufferedStream const src[])
{
    /*
        The values taken from elements i of pBuffers and pOffsets replace the current state for the vertex input binding firstBinding + i, for i in[0, bindingCount).
        The vertex input binding is updated to start at the offset indicated by pOffsets[i] from the start of the buffer pBuffers[i].
        If pSizes is not NULL then pSizes[i] specifies the bound size of the vertex buffer starting from the corresponding elements of pBuffers[i] plus pOffsets[i].
        All vertex input attributes that use each of these bindings will use these updated addresses in their address calculations for subsequent drawing commands.
        If the nullDescriptor feature is enabled, elements of pBuffers can be VK_NULL_HANDLE, and can be used by the vertex shader.
        If a vertex input attribute is bound to a vertex input binding that is VK_NULL_HANDLE, the values taken from memory are considered to be zero, and missing G, B, or A components are filled with(0, 0, 1).

        This command also dynamically sets the byte strides between consecutive elements within buffer pBuffers[i] to the corresponding pStrides[i] value when drawing using shader objects, or when the graphics pipeline is created with VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE set in VkPipelineDynamicStateCreateInfo::pDynamicStates.Otherwise, strides are specified by the VkVertexInputBindingDescription::stride values used to create the currently active pipeline.

        If drawing using shader objects or if the bound pipeline state object was also created with the VK_DYNAMIC_STATE_VERTEX_INPUT_EXT dynamic state enabled then vkCmdSetVertexInputEXT can be used instead of vkCmdBindVertexBuffers2 to set the stride.
    */

    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

    AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, first, cnt);

    // deferred because it requires the vertex input info.

    for (afxUnit i = 0; i < cnt; i++)
    {
        avxBufferedStream const* info = &src[i];

        avxBuffer buf = info->buf;
        afxUnit32 offset = info->offset;
        afxUnit32 range = info->range;
        afxUnit32 stride = info->stride;

        afxUnit bindingIdx = first + i;
        AFX_ASSERT_RANGE(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, bindingIdx, 1);

        dpu->nextVinBindings.sources[bindingIdx].buf = buf;
        dpu->nextVinBindings.sources[bindingIdx].offset = /*buf ? AFX_MIN(offset, AvxGetBufferCapacity(buf, 0) - 1) : */offset;
        dpu->nextVinBindings.sources[bindingIdx].range = !range && buf ? AvxGetBufferCapacity(buf, 0) - offset : range;
        dpu->nextVinBindings.sources[bindingIdx].stride = stride;
        dpu->nextVinBindings.sourcesUpdMask |= AFX_BITMASK(bindingIdx);
    }
}

_ZGL void DpuBindIndexBuffer(zglDpu* dpu, avxBuffer buf, afxUnit32 offset, afxUnit32 range, afxUnit32 stride)
{
    afxError err = AFX_ERR_NONE;

    // deferred because it requires the vertex input info.

    dpu->nextVinBindings.idxSrcBuf = buf;
    dpu->nextVinBindings.idxSrcOff = offset;
    dpu->nextVinBindings.idxSrcRange = range;
    dpu->nextVinBindings.idxSrcSiz = stride;
    dpu->nextVinBindings.iboUpdReq = TRUE;
}


_ZGL void DpuBindVertexInput(zglDpu* dpu, avxVertexInput vin)
{
    afxError err = AFX_ERR_NONE;
    
    dpu->nextVin = vin;
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
