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

//#define _ZGL_DBG_IGNORE_DEPTH_RANGE
#define _ZGL_DBG_IGNORE_PRIM_RESTART
#define _ZGL_DBG_IGNORE_DEPTH_CLAMP

#include "zglUtils.h"
#include "zglCommands.h"
#include "zglObjects.h"

_ZGL void _ZglFlushTsChanges(zglDpu* dpu)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    dpu->activeTs.primTop = dpu->nextTs.primTop;

    avxCullMode cullMode = dpu->nextTs.cullMode;

    if (dpu->activeTs.cullMode != cullMode)
    {
        /*
            GL_CULL_FACE
            If enabled, cull polygons based on their winding in window coordinates. See glCullFace.
        */

        if (cullMode)
        {
            AfxAssert(!dpu->activeTs.cullMode);
            gl->Enable(GL_CULL_FACE); _SglThrowErrorOccuried();
        }
        else
        {
            AfxAssert(dpu->activeTs.cullMode);
            gl->Disable(GL_CULL_FACE); _SglThrowErrorOccuried();
        }

        if (cullMode)
        {
            /*
                glCullFace � specify whether front- or back-facing facets can be culled
                void glCullFace(GLenum mode);

                mode Specifies whether front- or back-facing facets are candidates for culling. Symbolic constants GL_FRONT, GL_BACK, and GL_FRONT_AND_BACK are accepted. The initial value is GL_BACK.

                glCullFace specifies whether front- or back-facing facets are culled (as specified by mode) when facet culling is enabled.
                Facet culling is initially disabled. To enable and disable facet culling, call the glEnable and glDisable commands with the argument GL_CULL_FACE.
                Facets include triangles, quadrilaterals, polygons, and rectangles.

                glFrontFace specifies which of the clockwise and counterclockwise facets are front-facing and back-facing. See glFrontFace.

                If mode is GL_FRONT_AND_BACK, no facets are drawn, but other primitives such as points and lines are drawn.
            */
            gl->CullFace(AfxToGlCullMode(cullMode)); _SglThrowErrorOccuried();
        }
        dpu->activeTs.cullMode = cullMode;
    }

    if (dpu->activeTs.cullMode)
    {
        afxBool cwFrontFacing = dpu->nextTs.cwFrontFacing;

        if (dpu->activeTs.cwFrontFacing != cwFrontFacing)
        {
            /*
                glFrontFace � define front- and back-facing polygons
                void glFrontFace(GLenum mode);

                mode Specifies the orientation of front-facing polygons. GL_CW and GL_CCW are accepted. The initial value is GL_CCW.

                In a scene composed entirely of opaque closed surfaces, back-facing polygons are never visible.
                Eliminating these invisible polygons has the obvious benefit of speeding up the rendering of the image.
                To enable and disable elimination of back-facing polygons, call glEnable and glDisable with argument GL_CULL_FACE.

                The projection of a polygon to window coordinates is said to have clockwise winding if an imaginary object following the path from its first vertex, its second vertex, and so on, to its last vertex, and finally back to its first vertex, moves in a clockwise direction about the interior of the polygon.
                The polygon's winding is said to be counterclockwise if the imaginary object following the same path moves in a counterclockwise direction about the interior of the polygon.
                glFrontFace specifies whether polygons with clockwise winding in window coordinates, or counterclockwise winding in window coordinates, are taken to be front-facing.
                Passing GL_CCW to mode selects counterclockwise polygons as front-facing; GL_CW selects clockwise polygons as front-facing.
                By default, counterclockwise polygons are taken to be front-facing.
            */
            gl->FrontFace(cwFrontFacing ? GL_CW : GL_CCW); _SglThrowErrorOccuried();
            dpu->activeTs.cwFrontFacing = cwFrontFacing;
        }
    }

    if (dpu->nextTs.vpsUpdMask)
    {
        afxUnit vpCnt = dpu->activePip->m.vpCnt;
        AfxAssert(vpCnt);
        afxMask vpsUpdMask = dpu->nextTs.vpsUpdMask;
        dpu->nextTs.vpsUpdMask = NIL;

#if FORCE_GL_GENERIC_FUNCS
        GLfloat v[ZGL_MAX_VIEWPORTS][4];
        GLdouble v2[ZGL_MAX_VIEWPORTS][2];

        for (afxUnit i = 0; i < vpCnt; i++)
        {
            if (vpsUpdMask & AFX_BIT(i))
            {
                v[i][0] = dpu->nextTs.vps[i].offset[0];
                v[i][1] = dpu->nextTs.vps[i].offset[1];
                v[i][2] = dpu->nextTs.vps[i].extent[0];
                v[i][3] = dpu->nextTs.vps[i].extent[1];

                v2[i][0] = dpu->nextTs.vps[i].depth[0];
                v2[i][1] = dpu->nextTs.vps[i].depth[1];
                gl->ViewportArrayv(i, 1, &v[i][0]); _SglThrowErrorOccuried();
                gl->DepthRangeArrayv(i, 1, &v2[i][0]); _SglThrowErrorOccuried();
            }
        }
        //gl->ViewportArrayv(0, vpCnt, &v[0][0]); _SglThrowErrorOccuried();
        //gl->DepthRangeArrayv(0, vpCnt, &v2[0][0]); _SglThrowErrorOccuried();
#else
        if (1 < vpCnt)
        {
            AfxAssert(vpCnt);
            GLint x = (GLint)(dpu->nextTs.vps[0].offset[0]);
            GLint y = (GLint)(dpu->nextTs.vps[0].offset[1]);
            GLsizei w = (GLsizei)(dpu->nextTs.vps[0].extent[0]);
            GLsizei h = (GLsizei)(dpu->nextTs.vps[0].extent[1]);
            gl->Viewport(x, y, w, h); _SglThrowErrorOccuried();

#ifndef _ZGL_DBG_IGNORE_DEPTH_RANGE
            GLdouble n = dpu->nextTs.vps[0].depth[0];
            GLdouble f = dpu->nextTs.vps[0].depth[1];
            gl->DepthRange(n, f); _SglThrowErrorOccuried();
#endif
        }
        else
        {
            afxUnit cnt = dpu->nextViewportUpdCnt;
            AfxAssert(cnt);
            cnt = AfxClamp(cnt, 0, vpCnt);

            afxMask updMask = dpu->nextViewportUpdMask;

            for (afxUnit i = 0; i < ZGL_MAX_VIEWPORTS; i++)
            {
                if (AfxTestBitEnabled(updMask, i))
                {
                    if (gl->ViewportArrayv)
                    {
                        GLfloat v[ZGL_MAX_VIEWPORTS][4];
                        AfxAssert(ZGL_MAX_VIEWPORTS >= cnt);

                        v[i][0] = dpu->nextTs.vps[i].offset[0],
                        v[i][1] = dpu->nextTs.vps[i].offset[1],
                        v[i][2] = dpu->nextTs.vps[i].extent[0],
                        v[i][3] = dpu->nextTs.vps[i].extent[1];

                        gl->ViewportArrayv(i, 1, &v[0][0]); _SglThrowErrorOccuried();
                    }
#ifndef _ZGL_DBG_IGNORE_DEPTH_RANGE
                    if (gl->DepthRangeArrayv)
                    {
                        GLdouble v[ZGL_MAX_VIEWPORTS][2];
                        AfxAssert(ZGL_MAX_VIEWPORTS >= cnt);

                        v[i][0] = dpu->nextTs.vps[i].depth[0],
                            v[i][1] = dpu->nextTs.vps[i].depth[1];

                        gl->DepthRangeArrayv(0, 1, &v[0][0]); _SglThrowErrorOccuried();
                    }
#endif
                    dpu->activeTs.vps[i] = dpu->nextTs.vps[i];
                }

                if (--cnt == 0)
                    break;
            }
        }
#endif
    }

#ifndef _ZGL_DBG_IGNORE_PRIM_RESTART

    afxBool primRestartEnabled = dpu->nextTs.primTop;

    if (dpu->activeTs.primRestartEnabled != primRestartEnabled)
    {
        /*
            GL_PRIMITIVE_RESTART
            Enables primitive restarting.
            If enabled, any one of the draw commands which transfers a set of generic attribute array elements to the GL will restart the primitive when the index of the vertex is equal to the primitive restart index.
            See glPrimitiveRestartIndex.
        */

        if (primRestartEnabled)
        {
            AfxAssert(!dpu->activeTs.primRestartEnabled);
            gl->Enable(GL_PRIMITIVE_RESTART); _SglThrowErrorOccuried();

            /*
                glPrimitiveRestartIndex � specify the primitive restart index
                void glPrimitiveRestartIndex(GLuint index);
                index Specifies the value to be interpreted as the primitive restart index.

                glPrimitiveRestartIndex specifies a vertex array element that is treated specially when primitive restarting is enabled. This is known as the primitive restart index.

                When one of the Draw* commands transfers a set of generic attribute array elements to the GL, if the index within the vertex arrays corresponding to that set is equal to the primitive restart index, then the GL does not process those elements as a vertex.
                Instead, it is as if the drawing command ended with the immediately preceding transfer, and another drawing command is immediately started with the same parameters, but only transferring the immediately following element through the end of the originally specified elements.

                When either glDrawElementsBaseVertex, glDrawElementsInstancedBaseVertex or glMultiDrawElementsBaseVertex is used, the primitive restart comparison occurs before the basevertex offset is added to the array index.
            */

            //gl->PrimitiveRestartIndex(); _SglThrowErrorOccuried();

            /*
                GL_PRIMITIVE_RESTART_FIXED_INDEX
                Enables primitive restarting with a fixed index.
                If enabled, any one of the draw commands which transfers a set of generic attribute array elements to the GL will restart the primitive when the index of the vertex is equal to the fixed primitive index for the specified index type.
                The fixed index is equal to 2^n - 1
                where n is equal to 8 for GL_UNSIGNED_BYTE, 16 for GL_UNSIGNED_SHORT and 32 for GL_UNSIGNED_INT.
            */

            //gl->Enable(GL_PRIMITIVE_RESTART_FIXED_INDEX); _SglThrowErrorOccuried();
        }
        else
        {
            AfxAssert(dpu->activeTs.primRestartEnabled);
            gl->Disable(GL_PRIMITIVE_RESTART); _SglThrowErrorOccuried();
        }
        dpu->activeTs.primRestartEnabled = primRestartEnabled;
    }
#endif

#ifndef _ZGL_DBG_IGNORE_DEPTH_CLAMP

    afxBool depthClampEnabled = dpu->nextTs.depthClampEnabled;

    if (dpu->activeTs.depthClampEnabled != depthClampEnabled)
    {
        /*
            GL_DEPTH_CLAMP
            If enabled, the -wc =< zc =< wc plane equation is ignored by view volume clipping (effectively, there is no near or far plane clipping). See glDepthRange.
        */

        if (depthClampEnabled)
        {
            AfxAssert(!dpu->activeTs.depthClampEnabled);
            gl->Enable(GL_DEPTH_CLAMP); _SglThrowErrorOccuried();
        }
        else
        {
            AfxAssert(dpu->activeTs.depthClampEnabled);
            gl->Disable(GL_DEPTH_CLAMP); _SglThrowErrorOccuried();
        }
        dpu->activeTs.depthClampEnabled = depthClampEnabled;
    }
#endif
}

_ZGL void _DpuSetViewports(zglDpu* dpu, afxUnit first, afxUnit cnt, afxViewport const vp[])
{
    afxError err = AFX_ERR_NONE;
    AfxAssertRange(ZGL_MAX_VIEWPORTS, first, cnt);

    for (afxUnit i = 0; i < cnt; i++)
    {
        afxUnit vpIdx = first + i;
        dpu->nextTs.vps[vpIdx] = vp[i];
        dpu->nextTs.vpsUpdMask |= AFX_BIT(vpIdx);
    }
}

_ZGL void _DpuBindVertexSources(zglDpu* dpu, afxUnit first, afxUnit cnt, zglBufferInfo const src[])
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
    glVmt const* gl = &dpu->gl;

    AfxAssertRange(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, first, cnt);

    // deferred because it requires the vertex input info.

    for (afxUnit i = 0; i < cnt; i++)
    {
        zglBufferInfo const* info = &src[i];
        
        afxBuffer buf = info->buf;
        afxUnit32 offset = info->offset;
        afxUnit32 range = info->range;
        afxUnit32 stride = info->stride;

        afxUnit bindingIdx = first + i;
        AfxAssertRange(ZGL_MAX_VERTEX_ATTRIB_BINDINGS, bindingIdx, 1);

        dpu->nextVinBindings.sources[bindingIdx].buf = buf;
        dpu->nextVinBindings.sources[bindingIdx].offset = buf ? AfxMin(offset, AfxGetBufferCapacity(buf, 0) - 1) : offset;
        dpu->nextVinBindings.sources[bindingIdx].range = !range && buf ? AfxGetBufferCapacity(buf, 0) - offset : range;
        dpu->nextVinBindings.sources[bindingIdx].stride = stride;
        dpu->nextVinBindings.sourcesUpdMask |= AFX_BIT(bindingIdx);
    }
}

_ZGL void _DpuBindIndexSource(zglDpu* dpu, afxBuffer buf, afxUnit32 offset, afxUnit32 range, afxUnit32 stride)
{
    afxError err = AFX_ERR_NONE;

    // deferred because it requires the vertex input info.

    dpu->nextVinBindings.idxSrcBuf = buf;
    dpu->nextVinBindings.idxSrcOff = offset;
    dpu->nextVinBindings.idxSrcRange = range;
    dpu->nextVinBindings.idxSrcSiz = stride;
    dpu->nextVinBindings.iboUpdReq = TRUE;
}