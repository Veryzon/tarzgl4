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


_ZGL afxError _SglTexSubImage(glVmt const* gl, GLenum glTarget, afxRasterRegion const* rgn, GLenum glFmt, GLenum glType, afxAddress const src)
{
    afxError err = NIL;

    switch (glTarget)
    {
    case GL_TEXTURE_1D_ARRAY:
    {
        gl->TexSubImage2D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.z, rgn->extent.w, rgn->extent.d, glFmt, glType, (void const*)src); _SglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_2D:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
    {
        gl->TexSubImage2D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->extent.w, rgn->extent.h, glFmt, glType, (void const*)src); _SglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_CUBE_MAP:
    {
        gl->TexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + rgn->origin.z, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->extent.w, rgn->extent.h, glFmt, glType, (void const*)src); _SglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_1D:
    {
        gl->TexSubImage1D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->extent.w, glFmt, glType, (void const*)src); _SglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_3D:
    {
        gl->TexSubImage3D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z, rgn->extent.w, rgn->extent.h, rgn->extent.d, glFmt, glType, (void const*)src); _SglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_2D_ARRAY:
    {
        gl->TexSubImage3D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z, rgn->extent.w, rgn->extent.h, rgn->extent.d, glFmt, glType, (void const*)src); _SglThrowErrorOccuried();
        break;
    }
    default:
    {
        AfxThrowError();
        break;
    }
    }
    return err;
}

_ZGL afxError _SglCompressedTexSubImage(glVmt const* gl, GLenum glTarget, afxRasterRegion const* rgn, GLenum glFmt, GLenum glType, afxUnit compressedSiz, afxAddress const src)
{
    afxError err = NIL;

    switch (glTarget)
    {
    case GL_TEXTURE_1D_ARRAY:
    {
        gl->CompressedTexSubImage2D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.z, rgn->extent.w, rgn->extent.d, glFmt, compressedSiz, (void const*)src); _SglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_2D:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
    {
        gl->CompressedTexSubImage2D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->extent.w, rgn->extent.h, glFmt, compressedSiz, (void const*)src); _SglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_CUBE_MAP:
    {
        gl->CompressedTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + rgn->origin.z, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->extent.w, rgn->extent.h, glFmt, compressedSiz, (void const*)src); _SglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_1D:
    {
        gl->CompressedTexSubImage1D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->extent.w, glFmt, compressedSiz, (void const*)src); _SglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_3D:
    {
        gl->CompressedTexSubImage3D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z, rgn->extent.w, rgn->extent.h, rgn->extent.d, glFmt, compressedSiz, (void const*)src); _SglThrowErrorOccuried();
        break;
    }
    case GL_TEXTURE_2D_ARRAY:
    {
        gl->CompressedTexSubImage3D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z, rgn->extent.w, rgn->extent.h, rgn->extent.d, glFmt, compressedSiz, (void const*)src); _SglThrowErrorOccuried();
        break;
    }
    default:
    {
        AfxThrowError();
        break;
    }
    }
    return err;
}

# if 0
_ZGL afxError _SglTexFlushDevice(glVmt const* gl, GLenum glTarget, afxRaster ras) // ras must be bound
{
    afxError err = AFX_ERR_NONE;

    //AfxAssert((ras->updFlags & ZGL_UPD_FLAG_DEVICE_FLUSH) == ZGL_UPD_FLAG_DEVICE_FLUSH);

    //afxWhd const xyz = { 0, 0, 0 };
    //afxWhd extent;
    //AfxImage.GetExtent(&ras->img, whd);

    //afxBool const isSurface = AfxTestRasterFlags(ras, afxRasterUsage_DRAW);
    afxBool const isCubemap = AfxTestRasterFlags(ras, afxRasterFlag_CUBEMAP);
    
    afxUnit const lvlCnt = AfxCountRasterMipmaps(ras);
    AfxAssert(lvlCnt);

    afxPixelLayout pfd;
    AfxDescribePixelFormat(ras->m.fmt, &pfd);
    GLint gpuUnpackAlign = 1;// (8 >= pfd.bpp ? 1 : ((16 >= pfd.bpp ? 2 : (pfd.bpp <= 32 ? 4 : (pfd.bpp <= 32))));

    afxRasterRegion rgn;
    afxUnit rgnSize, rowSiz;
    void const* data;

    switch (glTarget)
    {
    case GL_TEXTURE_CUBE_MAP:
    {
        AfxAssert(isCubemap);
        AfxAssert(ras->m.layerCnt == 6);
        
        for (afxUnit j = 0; j < ras->m.layerCnt; j++)
        {
            for (afxUnit i = 0; i < lvlCnt; i++)
            {
                rgn.lodIdx = i;
                rgn.baseLayer = j;
                rgn.layerCnt = 1;
                rgn.offset[0] = ras->lastUpdOffset[0];
                rgn.offset[1] = ras->lastUpdOffset[1];
                rgn.offset[2] = ras->lastUpdOffset[2];
                rgn.extent.w = ras->lastUpdRange[0];
                rgn.extent.h = ras->lastUpdRange[1];
                rgn.extent.d = ras->lastUpdRange[2];

                data = NIL;

                //if (!isSurface)
                    data = AfxMapRaster(ras, &rgn, afxRasterAccess_R, &rgnSize, &rowSiz);

                AfxAssert(data);
                gl->PixelStorei(GL_UNPACK_ALIGNMENT, gpuUnpackAlign);
                _SglTexSubImage(gl, GL_TEXTURE_CUBE_MAP, rgn.lodIdx, rgn.baseLayer, rgn.layerCnt, rgn.offset, rgn.whd, ras->glFmt, ras->glType, (afxAddress const)data);

                //if (!isSurface)
                    AfxUnmapRaster(ras, &rgn);
            }
        }
        break;
    }
    default:
    {
        for (afxUnit i = ras->lastUpdLodBase; i < ras->lastUpdLodRange; i++)
        {
            for (afxUnit j = ras->lastUpdImgBase; j < ras->lastUpdImgRange; j++)
            {
                rgn.lodIdx = i;
                rgn.baseLayer = j;
                rgn.layerCnt = 1;
                rgn.offset[0] = ras->lastUpdOffset[0];
                rgn.offset[1] = ras->lastUpdOffset[1];
                rgn.offset[2] = ras->lastUpdOffset[2];
                rgn.extent.w = ras->lastUpdRange[0];
                rgn.extent.h = ras->lastUpdRange[1];
                rgn.extent.d = ras->lastUpdRange[2];

                data = NIL;

                //if (!isSurface)
                data = AfxMapRaster(ras, &rgn, afxRasterAccess_R, &rgnSize, &rowSiz);

                AfxAssert(data);
                gl->PixelStorei(GL_UNPACK_ALIGNMENT, gpuUnpackAlign);
                _SglTexSubImage(gl, glTarget, rgn.lodIdx, rgn.baseLayer, rgn.layerCnt, rgn.offset, rgn.whd, ras->glFmt, ras->glType, (afxAddress const)data);

                //if (!isSurface)
                AfxUnmapRaster(ras, &rgn);
            }
        }
        break;
    }
    }
    return err;
}
#endif

_ZGL afxError DpuBindAndSyncRas(zglDpu* dpu, afxUnit glUnit, afxRaster ras)
{
    //AfxEntry("img=%p", img);
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    
    if (!ras)
    {
        gl->ActiveTexture(GL_TEXTURE0 + glUnit); _SglThrowErrorOccuried();
        gl->BindTexture(GL_TEXTURE_2D, 0); _SglThrowErrorOccuried();
    }
    else
    {
        AfxAssertObjects(1, &ras, afxFcc_RAS);
        zglUpdateFlags devUpdReq = (ras->updFlags & ZGL_UPD_FLAG_DEVICE);

        GLuint glHandle = ras->glHandle;
        GLenum glTarget = ras->glTarget;            
        afxBool bound = FALSE;

        _SglThrowErrorOccuried();
        gl->ActiveTexture(GL_TEXTURE0 + glUnit); _SglThrowErrorOccuried();

        if ((ras->m.usage & afxRasterUsage_ALL) == afxRasterUsage_DRAW)
        {
            if ((!ras->glRboHandle) || (devUpdReq & ZGL_UPD_FLAG_DEVICE_INST))
            {
                if (ras->glRboHandle)
                {
                    gl->DeleteRenderbuffers(1, &ras->glRboHandle); _SglThrowErrorOccuried();
                    ras->glRboHandle = NIL;
                }
                gl->GenRenderbuffers(1, &ras->glRboHandle); _SglThrowErrorOccuried();
                gl->BindRenderbuffer(GL_RENDERBUFFER, ras->glRboHandle); _SglThrowErrorOccuried();
                AfxLogEcho("GPU RBO %p ready. %x, %x, [%u,%u,%u]", ras, ras->glTarget, ras->glIntFmt, ras->m.extent.w, ras->m.extent.h, ras->m.extent.d);

                if (!err)
                    ras->updFlags &= ~(ZGL_UPD_FLAG_DEVICE);
            }
        }
        else
        {
            if ((!glHandle && !ras->glRboHandle) || (devUpdReq & ZGL_UPD_FLAG_DEVICE_INST))
            {
                if (glHandle)
                {
                    AfxAssert(gl->IsTexture(glHandle));
                    gl->DeleteTextures(1, &glHandle); _SglThrowErrorOccuried();
                    ras->glHandle = NIL;
                    glHandle = NIL;
                }

                gl->GenTextures(1, &(glHandle)); _SglThrowErrorOccuried();
                gl->BindTexture(glTarget, glHandle); _SglThrowErrorOccuried();
                AfxAssert(gl->IsTexture(glHandle));
                bound = TRUE;
                ras->glHandle = glHandle;

                //gl->TextureParameteri(glHandle, GL_TEXTURE_TILING_EXT, GL_OPTIMAL_TILING_EXT); _SglThrowErrorOccuried();

                GLenum glIntFmt = ras->glIntFmt;

                switch (glTarget)
                {
                case GL_TEXTURE_1D:
                case GL_PROXY_TEXTURE_1D:
                {
                    AfxAssert(gl->TexStorage1D);
                    gl->TexStorage1D(glTarget, ras->m.lodCnt, glIntFmt, ras->m.extent.w); _SglThrowErrorOccuried();
                    break;
                }
                case GL_TEXTURE_2D:
                case GL_TEXTURE_RECTANGLE:
                case GL_TEXTURE_CUBE_MAP:
                case GL_PROXY_TEXTURE_2D:
                case GL_PROXY_TEXTURE_RECTANGLE:
                case GL_PROXY_TEXTURE_CUBE_MAP:
                {
                    AfxAssert(gl->TexStorage2D);
                    gl->TexStorage2D(glTarget, ras->m.lodCnt, glIntFmt, ras->m.extent.w, ras->m.extent.h); _SglThrowErrorOccuried();
                    break;
                }
                case GL_TEXTURE_1D_ARRAY:
                case GL_PROXY_TEXTURE_1D_ARRAY:
                {
                    AfxAssert(gl->TexStorage2D);
                    gl->TexStorage2D(glTarget, ras->m.lodCnt, glIntFmt, ras->m.extent.w, ras->m.extent.d); _SglThrowErrorOccuried();
                    break;
                }
                case GL_TEXTURE_3D:
                {
                    AfxAssert(gl->TexStorage3D);
                    gl->TexStorage3D(glTarget, ras->m.lodCnt, glIntFmt, ras->m.extent.w, ras->m.extent.h, ras->m.extent.d); _SglThrowErrorOccuried();
                    break;
                }
                case GL_TEXTURE_2D_ARRAY:
                {
                    AfxAssert(gl->TexStorage3D);
                    gl->TexStorage3D(glTarget, ras->m.lodCnt, glIntFmt, ras->m.extent.w, ras->m.extent.h, ras->m.extent.d); _SglThrowErrorOccuried();
                    break;
                }
                case GL_TEXTURE_2D_MULTISAMPLE:
                case GL_PROXY_TEXTURE_2D_MULTISAMPLE:
                {
                    AfxAssert(gl->TexStorage2DMultisample);
                    gl->TexStorage2DMultisample(glTarget, ras->m.lodCnt, glIntFmt, ras->m.extent.w, ras->m.extent.h, GL_FALSE); _SglThrowErrorOccuried();
                    break;
                }
                case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
                case GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY:
                {
                    AfxAssert(gl->TexStorage3DMultisample);
                    gl->TexStorage3DMultisample(glTarget, ras->m.lodCnt, glIntFmt, ras->m.extent.w, ras->m.extent.h, ras->m.extent.d, GL_FALSE); _SglThrowErrorOccuried();
                    break;
                }
                default: AfxThrowError(); break;
                }
                AfxLogEcho("GPU TO %p ready. %x, %x, [%u,%u,%u]", ras, glTarget, glIntFmt, ras->m.extent.w, ras->m.extent.h, ras->m.extent.d);

                if (!err)
                    ras->updFlags &= ~(ZGL_UPD_FLAG_DEVICE);
            }
            else
            {
                gl->BindTexture(glTarget, glHandle); _SglThrowErrorOccuried();
            }
        }
    }
    return err;
}

_ZGL afxError _DpuUnpackRasFromMem(zglDpu* dpu, afxRaster ras, afxByte const* src, afxUnit opCnt, afxRasterIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    DpuBindAndSyncBuf(dpu, GL_PIXEL_UNPACK_BUFFER, NIL);
    DpuBindAndSyncRas(dpu, ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT, ras);

    for (afxUnit i = 0; i < opCnt; i++)
    {
        afxRasterIo const* op = &ops[i];

        // GL_UNPACK_ROW_LENGTH is NOT row stride. It is the number of texel blocks per row of a image.
        afxUnit rowsPerImg = op->rowsPerImg && (op->rowsPerImg != op->rgn.extent.h) ? op->rowsPerImg : 0;
        afxUnit rowLen = op->rowStride ? op->rowStride / (op->rowStride / op->rgn.extent.w) : 0;

        if (rowLen == op->rgn.extent.w)
            rowLen = 0; // zero it to let the implementation handling with default behavior.

        gl->PixelStorei(GL_UNPACK_ALIGNMENT, 1); _SglThrowErrorOccuried();
        gl->PixelStorei(GL_UNPACK_ROW_LENGTH, rowLen); _SglThrowErrorOccuried();
        gl->PixelStorei(GL_UNPACK_IMAGE_HEIGHT, rowsPerImg); _SglThrowErrorOccuried();
        
        if (!AfxIsPixelFormatCompressed(ras->m.fmt))
        {
            if (_SglTexSubImage(gl, ras->glTarget, &op->rgn, ras->glFmt, ras->glType, (afxAddress const)&(src[op->offset])))
                AfxThrowError();
        }
        else
        {
            afxUnit compSiz = AfxElse(rowLen, op->rgn.extent.w) * AfxElse(rowsPerImg, op->rgn.extent.h) * AfxElse(op->rgn.extent.d, 1);
            
            if (_SglCompressedTexSubImage(gl, ras->glTarget, &op->rgn, ras->glFmt, ras->glType, compSiz, (afxAddress const)&(src[op->offset])))
                AfxThrowError();
        }
    }
    return err;
}

_ZGL afxError _DpuPackRasToMem(zglDpu* dpu, afxRaster ras, afxByte* dst, afxUnit opCnt, afxRasterIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    //DpuBindAndSyncBuf(dpu, GL_PIXEL_PACK_BUFFER, NIL);
    DpuBindAndSyncRas(dpu, ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT, ras);

    for (afxUnit i = 0; i < opCnt; i++)
    {
        afxRasterIo const* op = &ops[i];

        // GL_PACK_ROW_LENGTH is NOT row stride. It is the number of texel blocks per row of a image.
        afxUnit rowsPerImg = op->rowsPerImg && (op->rowsPerImg != op->rgn.extent.h) ? op->rowsPerImg : 0;
        afxUnit rowLen = op->rowStride ? op->rowStride / (op->rowStride / op->rgn.extent.w) : 0;

        if (rowLen == op->rgn.extent.w)
            rowLen = 0; // zero it to let the implementation handling with default behavior.

        afxUnit bufSiz = AfxElse(rowLen, op->rgn.extent.w) * AfxElse(rowsPerImg, op->rgn.extent.h) * AfxElse(op->rgn.extent.d, 1);

        gl->PixelStorei(GL_PACK_ALIGNMENT, 1); _SglThrowErrorOccuried();
        gl->PixelStorei(GL_PACK_ROW_LENGTH, rowLen); _SglThrowErrorOccuried();
        gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, rowsPerImg); _SglThrowErrorOccuried();

#if 0
        afxBool is3d = AfxTestRasterFlags(ras, afxRasterFlag_3D);

        gl->BindFramebuffer(GL_READ_FRAMEBUFFER, dpu->fboOpSrc); _SglThrowErrorOccuried();
        
        if (_SglBindFboAttachment(&dpu->gl, GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ras->glTarget, ras->glHandle, op->rgn.lodIdx, is3d ? 0 : op->rgn.origin.z, is3d ? op->rgn.origin.z : 0))
            AfxThrowError();

        gl->ReadBuffer(GL_COLOR_ATTACHMENT0); _SglThrowErrorOccuried();
        gl->ReadPixels(op->rgn.origin.x, op->rgn.origin.y, op->rgn.extent.w, op->rgn.extent.h, ras->glFmt, ras->glType, NIL); _SglThrowErrorOccuried();
#endif
        gl->PixelStorei(GL_PACK_SKIP_PIXELS, op->rgn.origin.x); _SglThrowErrorOccuried();
        gl->PixelStorei(GL_PACK_SKIP_ROWS, op->rgn.origin.y); _SglThrowErrorOccuried();
        gl->PixelStorei(GL_PACK_SKIP_IMAGES, op->rgn.origin.z); _SglThrowErrorOccuried();

        if (AfxIsPixelFormatCompressed(ras->m.fmt))
        {
            // pack into PBO
            gl->GetCompressedTexImage(ras->glTarget, op->rgn.lodIdx, dst); _SglThrowErrorOccuried();
        }
        else
        {
            // pack into PBO
            gl->GetTexImage(ras->glTarget, op->rgn.lodIdx, ras->glFmt, ras->glType, dst); _SglThrowErrorOccuried();
        }
    }

    // Restore pixel pack settings to avoid troubles.
    gl->PixelStorei(GL_PACK_SKIP_PIXELS, 0); _SglThrowErrorOccuried();
    gl->PixelStorei(GL_PACK_SKIP_ROWS, 0); _SglThrowErrorOccuried();
    gl->PixelStorei(GL_PACK_SKIP_IMAGES, 0); _SglThrowErrorOccuried();
    return err;
}

_ZGL afxError _DpuUnpackRasFromBuf(zglDpu* dpu, afxRaster ras, afxBuffer buf, afxUnit opCnt, afxRasterIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    DpuBindAndSyncBuf(dpu, GL_PIXEL_UNPACK_BUFFER, buf);
    DpuBindAndSyncRas(dpu, ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT, ras);

    for (afxUnit i = 0; i < opCnt; i++)
    {
        afxRasterIo const* op = &ops[i];

        // GL_UNPACK_ROW_LENGTH is NOT row stride. It is the number of texel blocks per row of a image.
        afxUnit rowsPerImg = op->rowsPerImg && (op->rowsPerImg != op->rgn.extent.h) ? op->rowsPerImg : 0;
        afxUnit rowLen = op->rowStride ? op->rowStride / (op->rowStride / op->rgn.extent.w) : 0;

        if (rowLen == op->rgn.extent.w)
            rowLen = 0; // zero it to let the implementation handling with default behavior.

        gl->PixelStorei(GL_UNPACK_ALIGNMENT, 1); _SglThrowErrorOccuried();
        gl->PixelStorei(GL_UNPACK_ROW_LENGTH, rowLen); _SglThrowErrorOccuried();
        gl->PixelStorei(GL_UNPACK_IMAGE_HEIGHT, rowsPerImg); _SglThrowErrorOccuried();

        if (!AfxIsPixelFormatCompressed(ras->m.fmt))
        {
            if (_SglTexSubImage(gl, ras->glTarget, &op->rgn, ras->glFmt, ras->glType, (afxAddress const)op->offset))
                AfxThrowError();
        }
        else
        {
            afxUnit compSiz = AfxElse(rowLen, op->rgn.extent.w) * AfxElse(rowsPerImg, op->rgn.extent.h) * AfxElse(op->rgn.extent.d, 1);
            
            if (_SglCompressedTexSubImage(gl, ras->glTarget, &op->rgn, ras->glFmt, ras->glType, compSiz, (afxAddress const)op->offset))
                AfxThrowError();
        }
    }
    return err;
}

_ZGL afxError _DpuPackRasToBuf(zglDpu* dpu, afxRaster ras, afxBuffer buf, afxUnit opCnt, afxRasterIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    DpuBindAndSyncBuf(dpu, GL_PIXEL_PACK_BUFFER, buf);
    DpuBindAndSyncRas(dpu, ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT, ras);

    for (afxUnit i = 0; i < opCnt; i++)
    {
        afxRasterIo const* op = &ops[i];

        // GL_PACK_ROW_LENGTH is NOT row stride. It is the number of texel blocks per row of a image.
        afxUnit rowsPerImg = op->rowsPerImg && (op->rowsPerImg != op->rgn.extent.h) ? op->rowsPerImg : 0;
        afxUnit rowLen = op->rowStride ? op->rowStride / (op->rowStride / op->rgn.extent.w) : 0;

        if (rowLen == op->rgn.extent.w)
            rowLen = 0; // zero it to let the implementation handling with default behavior.

        gl->PixelStorei(GL_PACK_ALIGNMENT, 1); _SglThrowErrorOccuried();
        gl->PixelStorei(GL_PACK_ROW_LENGTH, rowLen); _SglThrowErrorOccuried();
        gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, rowsPerImg); _SglThrowErrorOccuried();

#if 0
        afxBool is3d = AfxTestRasterFlags(ras, afxRasterFlag_3D);

        gl->BindFramebuffer(GL_READ_FRAMEBUFFER, dpu->fboOpSrc); _SglThrowErrorOccuried();
        
        if (_SglBindFboAttachment(&dpu->gl, GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ras->glTarget, ras->glHandle, op->rgn.lodIdx, is3d ? 0 : op->rgn.origin.z, is3d ? op->rgn.origin.z : 0))
            AfxThrowError();

        gl->ReadBuffer(GL_COLOR_ATTACHMENT0); _SglThrowErrorOccuried();
        gl->ReadPixels(op->rgn.origin.x, op->rgn.origin.y, op->rgn.extent.w, op->rgn.extent.h, ras->glFmt, ras->glType, (void*)op->offset); _SglThrowErrorOccuried();
#else
        gl->PixelStorei(GL_PACK_SKIP_PIXELS, op->rgn.origin.x); _SglThrowErrorOccuried();
        gl->PixelStorei(GL_PACK_SKIP_ROWS, op->rgn.origin.y); _SglThrowErrorOccuried();
        gl->PixelStorei(GL_PACK_SKIP_IMAGES, op->rgn.origin.z); _SglThrowErrorOccuried();

        if (AfxIsPixelFormatCompressed(ras->m.fmt))
        {
            // pack into PBO
            gl->GetCompressedTexImage(ras->glTarget, op->rgn.lodIdx, NIL); _SglThrowErrorOccuried();
        }
        else
        {
            // pack into PBO
            gl->GetTexImage(ras->glTarget, op->rgn.lodIdx, ras->glFmt, ras->glType, NIL); _SglThrowErrorOccuried();
        }
#endif
        //GLenum pboTarget = GL_PIXEL_PACK_BUFFER;
        //gl->GetBufferSubData(pboTarget, 0, bufSiz, (void*)op->offset); _SglThrowErrorOccuried();
    }

    // Restore pixel pack settings to avoid troubles.
    gl->PixelStorei(GL_PACK_SKIP_PIXELS, 0); _SglThrowErrorOccuried();
    gl->PixelStorei(GL_PACK_SKIP_ROWS, 0); _SglThrowErrorOccuried();
    gl->PixelStorei(GL_PACK_SKIP_IMAGES, 0); _SglThrowErrorOccuried();
    
    return err;
}

_ZGL afxError _DpuPackRasToIob(zglDpu* dpu, afxRaster ras, afxStream out, afxUnit opCnt, afxRasterIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    DpuBindAndSyncRas(dpu, ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT, ras);
    afxBool is3d = AfxTestRasterFlags(ras, afxRasterFlag_3D);
    avxFormatDescription pfd;
    AfxDescribePixelFormat(ras->m.fmt, &pfd);

    for (afxUnit i = 0; i < opCnt; i++)
    {
        afxRasterIo const* op = &ops[i];

        // GL_PACK_ROW_LENGTH is NOT row stride. It is the number of texel blocks per row of a image.
        afxUnit rowsPerImg = op->rowsPerImg && (op->rowsPerImg != op->rgn.extent.h) ? op->rowsPerImg : 0;
        afxUnit rowLen = op->rowStride ? op->rowStride / (op->rowStride / op->rgn.extent.w) : 0;

        if (rowLen == op->rgn.extent.w)
            rowLen = 0; // zero it to let the implementation handling with default behavior.
#if 0
        GLint compressed;
        gl->GetTexLevelParameteriv(ras->glTarget, op->rgn.lodIdx, GL_TEXTURE_COMPRESSED, &compressed);

        if (compressed)
        {
            GLint lodSiz, blockSiz, intFmt;
            gl->GetTexLevelParameteriv(ras->glTarget, op->rgn.lodIdx, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &lodSiz); _SglThrowErrorOccuried();
            gl->GetTexLevelParameteriv(ras->glTarget, op->rgn.lodIdx, GL_TEXTURE_INTERNAL_FORMAT, &intFmt); _SglThrowErrorOccuried();
            
            blockSiz = pfd.stride;

            GLuint pbo;
            GLenum pboTarget = GL_PIXEL_PACK_BUFFER;
            gl->GenBuffers(1, &pbo); _SglThrowErrorOccuried();
            gl->BindBuffer(pboTarget, pbo); _SglThrowErrorOccuried();
            gl->BufferStorage(pboTarget, lodSiz, NIL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT); _SglThrowErrorOccuried();

            gl->PixelStorei(GL_PACK_ALIGNMENT, 1); _SglThrowErrorOccuried();
            gl->PixelStorei(GL_PACK_ROW_LENGTH, rowLen); _SglThrowErrorOccuried();
            gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, rowsPerImg); _SglThrowErrorOccuried();

            gl->GetCompressedTexImage(ras->glTarget, op->rgn.lodIdx, NIL); _SglThrowErrorOccuried();
            
            afxUnit depthPitch = is3d ? op->rgn.extent.d : 1;
            afxUnit rowPitch = ras->m.extent.w + lodSiz / blockSiz % ras->m.extent.h;

            afxUnit num_blocks_x = (op->rgn.extent.w + 3) / 4;
            afxUnit num_blocks_y = (op->rgn.extent.h + 3) / 4;
            afxUnit block_address = (op->rgn.origin.y * ((ras->m.extent.w + 3) / 4) + op->rgn.origin.x) * pfd.stride;
            
            afxSize bufSiz = num_blocks_x * num_blocks_y * pfd.stride;

            afxByte* bytemap = gl->MapBuffer(pboTarget, GL_READ_ONLY); _SglThrowErrorOccuried();
            AfxWriteStreamAt(out, op->offset, bufSiz, 0, &bytemap[block_address]);
            gl->UnmapBuffer(pboTarget); _SglThrowErrorOccuried();
            gl->BindBuffer(pboTarget, 0); _SglThrowErrorOccuried();
            gl->DeleteBuffers(1, &pbo); _SglThrowErrorOccuried();
        }
        else
        {
#if 0
            afxUnit bufSiz = AfxElse(rowLen, op->rgn.extent.w) * AfxElse(rowsPerImg, op->rgn.extent.h) * AfxElse(op->rgn.extent.d, 1);

            GLuint pbo;
            GLenum pboTarget = GL_PIXEL_PACK_BUFFER;
            gl->GenBuffers(1, &pbo); _SglThrowErrorOccuried();
            gl->BindBuffer(pboTarget, pbo); _SglThrowErrorOccuried();
            gl->BufferStorage(pboTarget, bufSiz, NIL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT); _SglThrowErrorOccuried();

            gl->PixelStorei(GL_PACK_ALIGNMENT, 1); _SglThrowErrorOccuried();
            gl->PixelStorei(GL_PACK_ROW_LENGTH, rowLen); _SglThrowErrorOccuried();
            gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, rowsPerImg); _SglThrowErrorOccuried();



            gl->BindFramebuffer(GL_READ_FRAMEBUFFER, dpu->fboOpSrc); _SglThrowErrorOccuried();

            if (_SglBindFboAttachment(&dpu->gl, GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ras->glTarget, ras->glHandle, op->rgn.lodIdx, is3d ? 0 : op->rgn.origin.z, is3d ? op->rgn.origin.z : 0))
                AfxThrowError();

            gl->ReadBuffer(GL_COLOR_ATTACHMENT0); _SglThrowErrorOccuried();
            gl->ReadPixels(op->rgn.origin.x, op->rgn.origin.y, op->rgn.extent.w, op->rgn.extent.h, ras->glFmt, ras->glType, NIL); _SglThrowErrorOccuried();

            afxByte* bytemap = gl->MapBuffer(pboTarget, GL_READ_ONLY); _SglThrowErrorOccuried();
            AfxWriteStreamAt(out, op->offset, bufSiz, 0, bytemap);
            gl->UnmapBuffer(pboTarget); _SglThrowErrorOccuried();
            gl->BindBuffer(pboTarget, 0); _SglThrowErrorOccuried();
            gl->DeleteBuffers(1, &pbo); _SglThrowErrorOccuried();
#endif

        }
#endif

        GLuint pbo;
        GLenum pboTarget = GL_PIXEL_PACK_BUFFER;
        gl->GenBuffers(1, &pbo); _SglThrowErrorOccuried();
        gl->BindBuffer(pboTarget, pbo); _SglThrowErrorOccuried();

        GLint lodSiz = 0;

        if (AfxIsPixelFormatCompressed(ras->m.fmt))
        {
            GLint compressed;
            gl->GetTexLevelParameteriv(ras->glTarget, op->rgn.lodIdx, GL_TEXTURE_COMPRESSED, &compressed);
            AfxAssert(compressed);
            gl->GetTexLevelParameteriv(ras->glTarget, op->rgn.lodIdx, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &lodSiz); _SglThrowErrorOccuried();

            gl->BufferStorage(pboTarget, lodSiz * 2, NIL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT); _SglThrowErrorOccuried();

            gl->PixelStorei(GL_PACK_ALIGNMENT, 1); _SglThrowErrorOccuried();
            gl->PixelStorei(GL_PACK_ROW_LENGTH, rowLen); _SglThrowErrorOccuried();
            gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, rowsPerImg); _SglThrowErrorOccuried();

            gl->PixelStorei(GL_PACK_SKIP_PIXELS, op->rgn.origin.x); _SglThrowErrorOccuried();
            gl->PixelStorei(GL_PACK_SKIP_ROWS, op->rgn.origin.y); _SglThrowErrorOccuried();
            gl->PixelStorei(GL_PACK_SKIP_IMAGES, op->rgn.origin.z); _SglThrowErrorOccuried();

            // pack into PBO
            gl->GetCompressedTexImage(ras->glTarget, op->rgn.lodIdx, NIL); _SglThrowErrorOccuried();

            afxByte* bytemap = gl->MapBuffer(pboTarget, GL_READ_ONLY); _SglThrowErrorOccuried();
            
            AfxWriteStreamAt(out, op->offset, op->rgn.extent.d * op->rgn.extent.h * (op->rowStride? op->rowStride : op->rgn.extent.w * pfd.stride), 0, bytemap);
        }
        else
        {
            AfxAssert(pfd.stride);
            lodSiz = op->rgn.extent.d * AFX_ALIGNED_SIZE(op->rgn.extent.h, pfd.bcWh[1]) * op->rgn.extent.w * pfd.stride;
            gl->BufferStorage(pboTarget, lodSiz * 2, NIL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT); _SglThrowErrorOccuried();

            gl->PixelStorei(GL_PACK_ALIGNMENT, 1); _SglThrowErrorOccuried();
            gl->PixelStorei(GL_PACK_ROW_LENGTH, rowLen); _SglThrowErrorOccuried();
            gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, rowsPerImg); _SglThrowErrorOccuried();

            gl->PixelStorei(GL_PACK_SKIP_PIXELS, op->rgn.origin.x); _SglThrowErrorOccuried();
            gl->PixelStorei(GL_PACK_SKIP_ROWS, op->rgn.origin.y); _SglThrowErrorOccuried();
            gl->PixelStorei(GL_PACK_SKIP_IMAGES, op->rgn.origin.z); _SglThrowErrorOccuried();

            // pack into PBO
            gl->GetTexImage(ras->glTarget, op->rgn.lodIdx, ras->glFmt, ras->glType, NIL); _SglThrowErrorOccuried();
            afxByte* bytemap = gl->MapBuffer(pboTarget, GL_READ_ONLY); _SglThrowErrorOccuried();
            AfxWriteStreamAt(out, op->offset, op->rgn.extent.d * op->rgn.extent.h * op->rgn.extent.w * pfd.stride, 0, bytemap);
        }

        gl->UnmapBuffer(pboTarget); _SglThrowErrorOccuried();
        gl->BindBuffer(pboTarget, 0); _SglThrowErrorOccuried();
        gl->DeleteBuffers(1, &pbo); _SglThrowErrorOccuried();
    }

    // Restore pixel pack settings to avoid troubles.
    gl->PixelStorei(GL_PACK_SKIP_PIXELS, 0); _SglThrowErrorOccuried();
    gl->PixelStorei(GL_PACK_SKIP_ROWS, 0); _SglThrowErrorOccuried();
    gl->PixelStorei(GL_PACK_SKIP_IMAGES, 0); _SglThrowErrorOccuried();
    return err;
}

_ZGL afxError _DpuUnpackRasFromIob(zglDpu* dpu, afxRaster ras, afxStream in, afxUnit opCnt, afxRasterIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    DpuBindAndSyncRas(dpu, ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT, ras);

    for (afxUnit i = 0; i < opCnt; i++)
    {
        afxRasterIo const* op = &ops[i];

        // GL_UNPACK_ROW_LENGTH is NOT row stride. It is the number of texel blocks per row of a image.
        afxUnit rowsPerImg = op->rowsPerImg && (op->rowsPerImg != op->rgn.extent.h) ? op->rowsPerImg : 0;
        afxUnit rowLen = op->rowStride ? op->rowStride / (op->rowStride / op->rgn.extent.w) : 0;

        if (rowLen == op->rgn.extent.w)
            rowLen = 0; // zero it to let the implementation handling with default behavior.

        if (rowsPerImg == op->rgn.extent.h)
            rowsPerImg = 0;

        afxUnit bufSiz = op->decSiz ? op->decSiz : AfxElse(rowLen, op->rgn.extent.w) * AfxElse(rowsPerImg, op->rgn.extent.h) * AfxElse(op->rgn.extent.d, 1);
        
        GLuint pbo;
        GLenum pboTarget = GL_PIXEL_UNPACK_BUFFER;
        gl->GenBuffers(1, &pbo); _SglThrowErrorOccuried();
        gl->BindBuffer(pboTarget, pbo); _SglThrowErrorOccuried();
        gl->BufferStorage(pboTarget, bufSiz, NIL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT); _SglThrowErrorOccuried();
        afxByte* bytemap = gl->MapBuffer(pboTarget, GL_WRITE_ONLY); _SglThrowErrorOccuried();
        AfxReadStreamAt(in, op->offset, op->encSiz ? op->encSiz : bufSiz, 0, bytemap);
        gl->UnmapBuffer(pboTarget); _SglThrowErrorOccuried();
        gl->PixelStorei(GL_UNPACK_ALIGNMENT, 1); _SglThrowErrorOccuried();
        gl->PixelStorei(GL_UNPACK_ROW_LENGTH, rowLen); _SglThrowErrorOccuried();
        gl->PixelStorei(GL_UNPACK_IMAGE_HEIGHT, rowsPerImg); _SglThrowErrorOccuried();

        if (!AfxIsPixelFormatCompressed(ras->m.fmt))
        {
            if (_SglTexSubImage(gl, ras->glTarget, &op->rgn, ras->glFmt, ras->glType, NIL))
                AfxThrowError();
        }
        else
        {
            afxUnit compSiz = op->decSiz ? op->decSiz : AfxElse(rowLen, op->rgn.extent.w) * AfxElse(rowsPerImg, op->rgn.extent.h) * AfxElse(op->rgn.extent.d, 1);

            if (_SglCompressedTexSubImage(gl, ras->glTarget, &op->rgn, ras->glFmt, ras->glType, compSiz, NIL))
                AfxThrowError();
        }
        
        gl->BindBuffer(pboTarget, 0); _SglThrowErrorOccuried();
        gl->DeleteBuffers(1, &pbo); _SglThrowErrorOccuried();
    }
    return err;
}

_ZGL afxError _DpuCopyRas(zglDpu* dpu, afxRaster src, afxRaster dst, afxUnit opCnt, afxRasterCopy const ops[])
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;
    AfxAssertObjects(1, &dst, afxFcc_RAS);
    AfxAssertObjects(1, &src, afxFcc_RAS);
    DpuBindAndSyncRas(dpu, ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT, src);
    DpuBindAndSyncRas(dpu, ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT - 1, dst);
    _SglCopyTexSubImage(dpu, dst->glTarget, src->glTarget, src->glHandle, opCnt, ops);
    return err;
}

_ZGL afxError _SglRasDtor(afxRaster ras)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &ras, afxFcc_RAS);

    afxDrawContext dctx = AfxGetProvider(ras);

    if (ras->glHandle)
    {
        _SglDctxEnqueueDeletion(dctx, 0, GL_TEXTURE, (afxSize)ras->glHandle);
        ras->glHandle = 0;
    }

    if (ras->glRboHandle)
    {
        _SglDctxEnqueueDeletion(dctx, 0, GL_RENDERBUFFER, (afxSize)ras->glRboHandle);
        ras->glRboHandle = 0;
    }

    if (_AvxRasStdImplementation.dtor(ras))
        AfxThrowError();

    return err;
}

_ZGL afxError _SglRasCtor(afxRaster ras, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;

    if (_AvxRasStdImplementation.ctor(ras, args, invokeNo)) AfxThrowError();
    else
    {
        ras->updFlags = ZGL_UPD_FLAG_DEVICE_INST;

        GLint glIntFmt;
        SglDetermineGlTargetInternalFormatType(ras, &ras->glTarget, &ras->glIntFmt, &ras->glFmt, &ras->glType);
        
        if (err && _AvxRasStdImplementation.dtor(ras))
            AfxThrowError();
    }
    return err;
}
