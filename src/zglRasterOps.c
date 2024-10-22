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

_ZGL void _SglCopyTexSubImage(zglDpu* dpu, GLenum glDstTarget, GLenum glSrcTarget, GLuint glSrcHandle, afxUnit opCnt, afxRasterCopy const ops[])
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    afxBool srcIs3d = FALSE;

    if (glSrcTarget == GL_TEXTURE_3D)
        srcIs3d = TRUE;

    gl->BindFramebuffer(GL_READ_FRAMEBUFFER, dpu->fboOpSrc);
    
    for (afxUnit i = 0; i < opCnt; i++)
    {
        afxRasterCopy const* op = &ops[i];
        afxWhd const srcOrigin = op->srcOrigin;
        afxRasterRegion const* dstRgn = &op->dst;

        _SglBindFboAttachment(gl, GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, glSrcTarget, glSrcHandle, op->srcLodIdx, !srcIs3d ? op->srcOrigin.z : 0, srcIs3d ? op->srcOrigin.z : 0);
        gl->ReadBuffer(GL_COLOR_ATTACHMENT0);

        switch (glDstTarget)
        {
        case GL_TEXTURE_2D:
        {
            gl->CopyTexSubImage2D(glDstTarget, dstRgn->lodIdx, dstRgn->origin.x, dstRgn->origin.y, srcOrigin.x, srcOrigin.y, dstRgn->extent.w, dstRgn->extent.h); _SglThrowErrorOccuried();
            break;
        }
        case GL_TEXTURE_1D_ARRAY:
        {
            for (afxUnit i = dstRgn->extent.d; i < dstRgn->extent.d; i++)
            {
                gl->CopyTexSubImage2D(glDstTarget, dstRgn->lodIdx, dstRgn->origin.x, i, srcOrigin.x, srcOrigin.y, dstRgn->extent.w, dstRgn->extent.h); _SglThrowErrorOccuried();
            }
            break;
        }
        case GL_TEXTURE_1D:
        {
            gl->CopyTexSubImage1D(glDstTarget, dstRgn->lodIdx, dstRgn->origin.x, srcOrigin.x, srcOrigin.y, dstRgn->extent.w);
            break;
        }
        case GL_TEXTURE_3D:
        {
            for (afxUnit i = dstRgn->origin.z; i < dstRgn->extent.d; i++)
            {
                gl->CopyTexSubImage3D(glDstTarget, dstRgn->lodIdx, dstRgn->origin.x, dstRgn->origin.y, i, srcOrigin.x, srcOrigin.y, dstRgn->extent.w, dstRgn->extent.h); _SglThrowErrorOccuried();
            }
            break;
        }
        case GL_TEXTURE_2D_ARRAY:
        {
            for (afxUnit i = dstRgn->origin.z; i < dstRgn->extent.d; i++)
            {
                gl->CopyTexSubImage3D(glDstTarget, dstRgn->lodIdx, dstRgn->origin.x, dstRgn->origin.y, i, srcOrigin.x, srcOrigin.y, dstRgn->extent.w, dstRgn->extent.h); _SglThrowErrorOccuried();
            }
            break;
        }
        case GL_TEXTURE_CUBE_MAP_ARRAY:
        {
            for (afxUnit i = dstRgn->origin.z; i < dstRgn->extent.d; i++)
            {
                gl->CopyTexSubImage3D(glDstTarget, dstRgn->lodIdx, dstRgn->origin.x, dstRgn->origin.y, i, srcOrigin.x, srcOrigin.y, dstRgn->extent.w, dstRgn->extent.h); _SglThrowErrorOccuried();
            }
            break;
        }
        case GL_TEXTURE_CUBE_MAP:
        {
            for (afxUnit i = dstRgn->origin.z; i < dstRgn->extent.d; i++)
            {
                gl->CopyTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, dstRgn->lodIdx, dstRgn->origin.x, dstRgn->origin.y, srcOrigin.x, srcOrigin.y, dstRgn->extent.w, dstRgn->extent.h); _SglThrowErrorOccuried();
            }
            break;
        }
        case GL_TEXTURE_RECTANGLE:
        {
            AfxAssert(dstRgn->lodIdx == 0);
            gl->CopyTexSubImage2D(glDstTarget, dstRgn->lodIdx, dstRgn->origin.x, dstRgn->origin.y, srcOrigin.x, srcOrigin.y, dstRgn->extent.w, dstRgn->extent.h); _SglThrowErrorOccuried();
        };
        default:
            AfxThrowError();
            break;
        }
    }
}

_ZGL void _DecodeCmdRasCopy(zglDpu* dpu, _zglCmdCopyRasterRegions const* cmd)
{
    afxError err = AFX_ERR_NONE;
    _DpuCopyRas(dpu, cmd->src, cmd->dst, cmd->opCnt, cmd->ops);
}

_ZGL void _DecodeCmdRasPack(zglDpu* dpu, _zglCmdPackRasterRegions const* cmd)
{
    afxError err = AFX_ERR_NONE;

    if (cmd->unpack)
    {
        _DpuUnpackRasFromBuf(dpu, cmd->ras, cmd->buf, cmd->opCnt, cmd->ops);
    }
    else
    {
        _DpuPackRasToBuf(dpu, cmd->ras, cmd->buf, cmd->opCnt, cmd->ops);
    }
}

_ZGL void _DecodeCmdRasSubsample(zglDpu* dpu, _zglCmdRegenerateMipmaps const* cmd)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    afxRaster ras = cmd->ras;
    AfxAssertObjects(1, &ras, afxFcc_RAS);
    afxUnit lodCnt = cmd->lodCnt;
    afxUnit baseLod = cmd->baseLod;
    AfxAssert(lodCnt == 1);
    AfxAssert(baseLod == 0);

    afxUnit rasLodCnt = ras->m.lodCnt;

    DpuBindAndSyncRas(dpu, ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT, ras);
    gl->GenerateMipmap(ras->glTarget); _SglThrowErrorOccuried();
    DpuBindAndSyncRas(dpu, ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT, NIL);

    AfxAssertRange(rasLodCnt, baseLod, lodCnt);
    AfxThrowError();
}
