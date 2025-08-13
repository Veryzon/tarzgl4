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

#define UNBIND_GL_TEX TRUE
#define RENDERBUFFER_ENABLED TRUE
#define _BLIT_RASTER_WITH_GEN_FBOS TRUE
#define _RESOLVE_RASTER_WITH_GEN_FBOS TRUE

# if 0
_ZGL afxError _ZglTexFlushDevice(glVmt const* gl, GLenum glTarget, avxRaster ras) // ras must be bound
{
    afxError err = AFX_ERR_NONE;

    //AFX_ASSERT((ras->updFlags & ZGL_UPD_FLAG_DEVICE_FLUSH) == ZGL_UPD_FLAG_DEVICE_FLUSH);

    //afxWarp const xyz = { 0, 0, 0 };
    //afxWarp whd;
    //AfxImage.GetExtent(&ras->img, whd);

    //afxBool const isSurface = AvxTestRasterFlags(ras, avxRasterUsage_DRAW);
    afxBool const isCubemap = AvxTestRasterFlags(ras, avxRasterFlag_CUBEMAP);
    
    afxUnit const lvlCnt = AfxCountRasterMipmaps(ras);
    AFX_ASSERT(lvlCnt);

    afxPixelLayout pfd;
    AvxDescribeFormat(ras->m.fmt, &pfd);
    GLint gpuUnpackAlign = 1;// (8 >= pfd.bpp ? 1 : ((16 >= pfd.bpp ? 2 : (pfd.bpp <= 32 ? 4 : (pfd.bpp <= 32))));

    avxRasterRegion rgn;
    afxUnit rgnSize, rowSiz;
    void const* data;

    switch (glTarget)
    {
    case GL_TEXTURE_CUBE_MAP:
    {
        AFX_ASSERT(isCubemap);
        AFX_ASSERT(ras->m.layerCnt == 6);
        
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
                rgn.whd.w = ras->lastUpdRange[0];
                rgn.whd.h = ras->lastUpdRange[1];
                rgn.whd.d = ras->lastUpdRange[2];

                data = NIL;

                //if (!isSurface)
                    data = AfxMapRaster(ras, &rgn, avxRasterAccess_R, &rgnSize, &rowSiz);

                AFX_ASSERT(data);
                gl->PixelStorei(GL_UNPACK_ALIGNMENT, gpuUnpackAlign);
                _ZglTexSubImage(gl, GL_TEXTURE_CUBE_MAP, rgn.lodIdx, rgn.baseLayer, rgn.layerCnt, rgn.offset, rgn.whd, ras->glFmt, ras->glType, (afxAddress const)data);

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
                rgn.whd.w = ras->lastUpdRange[0];
                rgn.whd.h = ras->lastUpdRange[1];
                rgn.whd.d = ras->lastUpdRange[2];

                data = NIL;

                //if (!isSurface)
                data = AfxMapRaster(ras, &rgn, avxRasterAccess_R, &rgnSize, &rowSiz);

                AFX_ASSERT(data);
                gl->PixelStorei(GL_UNPACK_ALIGNMENT, gpuUnpackAlign);
                _ZglTexSubImage(gl, glTarget, rgn.lodIdx, rgn.baseLayer, rgn.layerCnt, rgn.offset, rgn.whd, ras->glFmt, ras->glType, (afxAddress const)data);

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

_ZGL afxError DpuBindAndSyncRas(zglDpu* dpu, afxUnit glUnit, avxRaster ras, afxBool keepBound)
{
    //AfxEntry("img=%p", img);
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    
    if (!ras)
    {
        if (gl->BindTextureUnit)
        {
            gl->BindTextureUnit(glUnit, NIL); _ZglThrowErrorOccuried();
        }
        else
        {
            gl->ActiveTexture(GL_TEXTURE0 + glUnit); _ZglThrowErrorOccuried();
            gl->BindTexture(GL_TEXTURE_2D, NIL); _ZglThrowErrorOccuried();
        }
    }
    else
    {
        AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &ras);
        zglUpdateFlags devUpdReq = (ras->updFlags & ZGL_UPD_FLAG_DEVICE);
        GLuint glHandle = ras->glHandle;
        afxBool bound = FALSE;

#ifdef RENDERBUFFER_ENABLED
        if (ras->glTarget == GL_RENDERBUFFER)
        {
            if (glHandle && !(devUpdReq & ZGL_UPD_FLAG_DEVICE_INST))
            {
                if (keepBound)
                {
                    gl->BindRenderbuffer(GL_RENDERBUFFER, glHandle); _ZglThrowErrorOccuried();
                }
            }
            else
            {
                if (glHandle)
                {
                    gl->DeleteRenderbuffers(1, &glHandle); _ZglThrowErrorOccuried();
                    ras->glHandle = NIL;
                }

                afxUnit samples = (ras->m.lodCnt > 1) ? ras->m.lodCnt : 0;

                if ((!keepBound) && 
                    gl->CreateRenderbuffers && 
                    (((samples == 0) && gl->NamedRenderbufferStorage) ||
                    (samples && gl->NamedRenderbufferStorageMultisample)))
                {
                    gl->CreateRenderbuffers(1, &glHandle); _ZglThrowErrorOccuried();

                    if (ras->m.tag.len)
                    {
                        gl->ObjectLabel(GL_RENDERBUFFER, glHandle, ras->m.tag.len, (GLchar const*)ras->m.tag.start); _ZglThrowErrorOccuried();
                    }

                    if (1 >= samples)
                    {
                        gl->NamedRenderbufferStorage(glHandle, ras->glIntFmt, ras->m.whd.w, ras->m.whd.h); _ZglThrowErrorOccuried();
                    }
                    else
                    {
                        gl->NamedRenderbufferStorageMultisample(glHandle, samples, ras->glIntFmt, ras->m.whd.w, ras->m.whd.h); _ZglThrowErrorOccuried();
                    }
                }
                else
                {
                    gl->GenRenderbuffers(1, &glHandle); _ZglThrowErrorOccuried();
                    gl->BindRenderbuffer(GL_RENDERBUFFER, glHandle); _ZglThrowErrorOccuried();
                    bound = TRUE;

                    if (ras->m.tag.len)
                    {
                        gl->ObjectLabel(GL_RENDERBUFFER, glHandle, ras->m.tag.len, (GLchar const*)ras->m.tag.start); _ZglThrowErrorOccuried();
                    }
                    if (1 >= samples)
                    {
                        gl->RenderbufferStorage(GL_RENDERBUFFER, ras->glIntFmt, ras->m.whd.w, ras->m.whd.h); _ZglThrowErrorOccuried();
                    }
                    else
                    {
                        gl->RenderbufferStorageMultisample(GL_RENDERBUFFER, samples, ras->glIntFmt, ras->m.whd.w, ras->m.whd.h); _ZglThrowErrorOccuried();
                    }
                }

                if (!err)
                {
                    ras->glHandle = glHandle;
                    ras->updFlags &= ~(ZGL_UPD_FLAG_DEVICE);
                    //AfxReportMessage("GPU RBO %p ready. %x, %x, [%u,%u,%u]", ras, ras->glTarget, ras->glIntFmt, ras->m.whd.w, ras->m.whd.h, ras->m.whd.d);
                }

                if (!keepBound)
                {
                    if (bound)
                    {
                        // if bound and is to be kept bound, unbind it.
                        gl->BindRenderbuffer(GL_RENDERBUFFER, 0); _ZglThrowErrorOccuried();
                        bound = FALSE;
                    }
                }
                else
                {
                    if (!bound)
                    {
                        // if not bound and is to be kept bound, bind it.
                        gl->BindRenderbuffer(GL_RENDERBUFFER, glHandle); _ZglThrowErrorOccuried();
                        bound = TRUE;
                    }
                }
            }
        }
        else
#endif
        {
            GLuint glHandle = ras->glHandle;
            GLenum glTarget = ras->glTarget;
            afxBool bound = FALSE;

            if (glHandle && !(devUpdReq & ZGL_UPD_FLAG_DEVICE_INST))
            {
                if (keepBound)
                {
                    if (gl->BindTextureUnit)
                    {
                        gl->BindTextureUnit(glUnit, glHandle); _ZglThrowErrorOccuried();
                    }
                    else
                    {
                        gl->ActiveTexture(GL_TEXTURE0 + glUnit); _ZglThrowErrorOccuried();
                        gl->BindTexture(glTarget, glHandle); _ZglThrowErrorOccuried();
                    }
                }
            }
            else
            {
                if (glHandle)
                {
                    AFX_ASSERT(gl->IsTexture(glHandle));
                    gl->DeleteTextures(1, &glHandle); _ZglThrowErrorOccuried();
                    ras->glHandle = NIL;
                    glHandle = NIL;
                }

                afxUnit width = ras->m.whd.w;
                afxUnit height = ras->m.whd.h;
                afxUnit depth = ras->m.whd.d;
                afxUnit levelsOrSamples = ras->m.lodCnt;
                GLboolean fixedsamplelocations = GL_FALSE;

                switch (glTarget)
                {
                case GL_TEXTURE_2D:
                case GL_TEXTURE_CUBE_MAP:
                {
                    AFX_ASSERT((1 >= depth) || (glTarget == GL_TEXTURE_CUBE_MAP));

                    if (gl->CreateTextures && gl->TextureStorage2D)
                    {
                        gl->CreateTextures(glTarget, 1, &glHandle); _ZglThrowErrorOccuried();

                        if (ras->m.tag.len)
                        {
                            gl->ObjectLabel(GL_TEXTURE, glHandle, ras->m.tag.len, (GLchar const*)ras->m.tag.start); _ZglThrowErrorOccuried();
                        }
                        gl->TextureStorage2D(glHandle, levelsOrSamples, ras->glIntFmt, width, height); _ZglThrowErrorOccuried();
                    }
                    else
                    {
                        gl->GenTextures(1, &glHandle); _ZglThrowErrorOccuried();
                        gl->ActiveTexture(GL_TEXTURE0 + ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT); _ZglThrowErrorOccuried();
                        gl->BindTexture(glTarget, glHandle); _ZglThrowErrorOccuried();
                        AFX_ASSERT(gl->IsTexture(glHandle));

                        if (ras->m.tag.len)
                        {
                            gl->ObjectLabel(GL_TEXTURE, glHandle, ras->m.tag.len, (GLchar const*)ras->m.tag.start); _ZglThrowErrorOccuried();
                        }
                        gl->TexStorage2D(glTarget, levelsOrSamples, ras->glIntFmt, width, height); _ZglThrowErrorOccuried();
                        bound = TRUE;
                    }
                    break;
                }
                case GL_TEXTURE_2D_ARRAY:
                case GL_TEXTURE_CUBE_MAP_ARRAY:
                case GL_TEXTURE_3D:
                {
                    if (gl->CreateTextures && gl->TextureStorage3D)
                    {
                        gl->CreateTextures(glTarget, 1, &glHandle); _ZglThrowErrorOccuried();

                        if (ras->m.tag.len)
                        {
                            gl->ObjectLabel(GL_TEXTURE, glHandle, ras->m.tag.len, (GLchar const*)ras->m.tag.start); _ZglThrowErrorOccuried();
                        }
                        gl->TextureStorage3D(glHandle, levelsOrSamples, ras->glIntFmt, width, height, depth); _ZglThrowErrorOccuried();
                    }
                    else
                    {
                        gl->GenTextures(1, &glHandle); _ZglThrowErrorOccuried();
                        gl->ActiveTexture(GL_TEXTURE0 + ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT); _ZglThrowErrorOccuried();
                        gl->BindTexture(glTarget, glHandle); _ZglThrowErrorOccuried();
                        AFX_ASSERT(gl->IsTexture(glHandle));

                        if (ras->m.tag.len)
                        {
                            gl->ObjectLabel(GL_TEXTURE, glHandle, ras->m.tag.len, (GLchar const*)ras->m.tag.start); _ZglThrowErrorOccuried();
                        }
                        gl->TexStorage3D(glTarget, levelsOrSamples, ras->glIntFmt, width, height, depth); _ZglThrowErrorOccuried();
                        bound = TRUE;
                    }
                    break;
                }
                case GL_TEXTURE_1D_ARRAY:
                {
                    AFX_ASSERT(1 >= height);

                    if (gl->CreateTextures && gl->TextureStorage2D)
                    {
                        gl->CreateTextures(glTarget, 1, &glHandle); _ZglThrowErrorOccuried();

                        if (ras->m.tag.len)
                        {
                            gl->ObjectLabel(GL_TEXTURE, glHandle, ras->m.tag.len, (GLchar const*)ras->m.tag.start); _ZglThrowErrorOccuried();
                        }
                        gl->TextureStorage2D(glHandle, levelsOrSamples, ras->glIntFmt, width, depth); _ZglThrowErrorOccuried();
                    }
                    else
                    {
                        gl->GenTextures(1, &glHandle); _ZglThrowErrorOccuried();
                        gl->ActiveTexture(GL_TEXTURE0 + ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT); _ZglThrowErrorOccuried();
                        gl->BindTexture(glTarget, glHandle); _ZglThrowErrorOccuried();
                        AFX_ASSERT(gl->IsTexture(glHandle));

                        if (ras->m.tag.len)
                        {
                            gl->ObjectLabel(GL_TEXTURE, glHandle, ras->m.tag.len, (GLchar const*)ras->m.tag.start); _ZglThrowErrorOccuried();
                        }
                        gl->TexStorage2D(glTarget, levelsOrSamples, ras->glIntFmt, width, depth); _ZglThrowErrorOccuried();
                        bound = TRUE;
                    }
                    break;
                }
                case GL_TEXTURE_1D:
                {
                    AFX_ASSERT(1 >= height);
                    AFX_ASSERT(1 >= depth);

                    if (gl->CreateTextures && gl->TextureStorage1D)
                    {
                        gl->CreateTextures(glTarget, 1, &glHandle); _ZglThrowErrorOccuried();

                        if (ras->m.tag.len)
                        {
                            gl->ObjectLabel(GL_TEXTURE, glHandle, ras->m.tag.len, (GLchar const*)ras->m.tag.start); _ZglThrowErrorOccuried();
                        }
                        gl->TextureStorage1D(glHandle, levelsOrSamples, ras->glIntFmt, width); _ZglThrowErrorOccuried();
                    }
                    else
                    {
                        gl->GenTextures(1, &glHandle); _ZglThrowErrorOccuried();
                        gl->ActiveTexture(GL_TEXTURE0 + ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT); _ZglThrowErrorOccuried();
                        gl->BindTexture(glTarget, glHandle); _ZglThrowErrorOccuried();
                        AFX_ASSERT(gl->IsTexture(glHandle));

                        if (ras->m.tag.len)
                        {
                            gl->ObjectLabel(GL_TEXTURE, glHandle, ras->m.tag.len, (GLchar const*)ras->m.tag.start); _ZglThrowErrorOccuried();
                        }
                        gl->TexStorage1D(glTarget, levelsOrSamples, ras->glIntFmt, width); _ZglThrowErrorOccuried();
                        bound = TRUE;
                    }
                    break;
                }
                case GL_TEXTURE_2D_MULTISAMPLE:
                {
                    AFX_ASSERT(1 >= depth);

                    if (gl->CreateTextures && gl->TextureStorage2DMultisample)
                    {
                        gl->CreateTextures(glTarget, 1, &glHandle); _ZglThrowErrorOccuried();

                        if (ras->m.tag.len)
                        {
                            gl->ObjectLabel(GL_TEXTURE, glHandle, ras->m.tag.len, (GLchar const*)ras->m.tag.start); _ZglThrowErrorOccuried();
                        }
                        gl->TextureStorage2DMultisample(glHandle, levelsOrSamples, ras->glIntFmt, width, height, fixedsamplelocations); _ZglThrowErrorOccuried();
                    }
                    else
                    {
                        gl->GenTextures(1, &glHandle); _ZglThrowErrorOccuried();
                        gl->ActiveTexture(GL_TEXTURE0 + ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT); _ZglThrowErrorOccuried();
                        gl->BindTexture(glTarget, glHandle); _ZglThrowErrorOccuried();
                        AFX_ASSERT(gl->IsTexture(glHandle));

                        if (ras->m.tag.len)
                        {
                            gl->ObjectLabel(GL_TEXTURE, glHandle, ras->m.tag.len, (GLchar const*)ras->m.tag.start); _ZglThrowErrorOccuried();
                        }
                        gl->TexStorage2DMultisample(glTarget, levelsOrSamples, ras->glIntFmt, width, height, fixedsamplelocations); _ZglThrowErrorOccuried();
                        bound = TRUE;
                    }
                    break;
                }
                case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
                {
                    if (gl->CreateTextures && gl->TextureStorage3DMultisample)
                    {
                        gl->CreateTextures(glTarget, 1, &glHandle); _ZglThrowErrorOccuried();

                        if (ras->m.tag.len)
                        {
                            gl->ObjectLabel(GL_TEXTURE, glHandle, ras->m.tag.len, (GLchar const*)ras->m.tag.start); _ZglThrowErrorOccuried();
                        }
                        gl->TextureStorage3DMultisample(glHandle, levelsOrSamples, ras->glIntFmt, width, height, depth, fixedsamplelocations); _ZglThrowErrorOccuried();
                    }
                    else
                    {
                        gl->GenTextures(1, &glHandle); _ZglThrowErrorOccuried();
                        gl->ActiveTexture(GL_TEXTURE0 + ZGL_LAST_COMBINED_TEXTURE_IMAGE_UNIT); _ZglThrowErrorOccuried();
                        gl->BindTexture(glTarget, glHandle); _ZglThrowErrorOccuried();
                        AFX_ASSERT(gl->IsTexture(glHandle));

                        if (ras->m.tag.len)
                        {
                            gl->ObjectLabel(GL_TEXTURE, glHandle, ras->m.tag.len, (GLchar const*)ras->m.tag.start); _ZglThrowErrorOccuried();
                        }
                        gl->TexStorage3DMultisample(glTarget, levelsOrSamples, ras->glIntFmt, width, height, depth, fixedsamplelocations); _ZglThrowErrorOccuried();
                        bound = TRUE;
                    }
                    break;
                }
                default: AfxThrowError(); break;
                }

                if (!err)
                {
                    ras->updFlags &= ~(ZGL_UPD_FLAG_DEVICE);
                    ras->glHandle = glHandle;
                    //AfxReportMessage("GPU RAS %p ready. %x, %x, [%u,%u,%u]", ras, glTarget, ras->glIntFmt, ras->m.whd.w, ras->m.whd.h, ras->m.whd.d);
                }

                if (keepBound)
                {
                    if (!bound)
                    {
                        if (gl->BindTextureUnit)
                        {
                            gl->BindTextureUnit(glUnit, glHandle); _ZglThrowErrorOccuried();
                        }
                        else
                        {
                            gl->ActiveTexture(GL_TEXTURE0 + glUnit); _ZglThrowErrorOccuried();
                            gl->BindTexture(ras->glTarget, glHandle); _ZglThrowErrorOccuried();
                        }
                    }
                }
                else
                {
                    if (bound)
                    {
                        if (gl->BindTextureUnit)
                        {
                            gl->BindTextureUnit(glUnit, 0); _ZglThrowErrorOccuried();
                        }
                        else
                        {
                            gl->ActiveTexture(GL_TEXTURE0 + glUnit); _ZglThrowErrorOccuried();
                            gl->BindTexture(ras->glTarget, 0); _ZglThrowErrorOccuried();
                        }
                        bound = FALSE;
                    }
                }
            }
        }
    }
    return err;
}

_ZGL afxError _DpuUpdateRaster(zglDpu* dpu, avxRaster ras, afxByte const* src, afxUnit opCnt, avxRasterIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    AFX_ASSERT(dpu->inDrawScope == FALSE); // This is a transfer operation.

    GLenum glTarget = ras->glTarget;
    GLenum glFmt = ras->glFmt;
    GLenum glType = ras->glType;

    afxBool rasSynced = FALSE;
    afxBool rasBound = FALSE;

    DpuBindAndSyncBuf(dpu, GL_PIXEL_UNPACK_BUFFER, NIL, TRUE); // KEEP IT HERE?
    //DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
    rasSynced = TRUE;

    avxFormatDescription pfd;
    AvxDescribeFormats(1, &ras->m.fmt, &pfd);

    for (afxUnit i = 0; i < opCnt; i++)
    {
        avxRasterIo const* op = &ops[i];
        avxRasterRegion const* rgn = &op->rgn;

        // GL_UNPACK_ROW_LENGTH is NOT row stride. It is the number of texel blocks per row of a image.
        afxUnit rowsPerImg = op->rowsPerImg && (op->rowsPerImg != op->rgn.whd.h) ? op->rowsPerImg : 0;
        afxUnit rowLen = 0;// op->rowStride ? op->rowStride / (op->rowStride / op->rgn.whd.w) : 0;

        rowLen = op->rowStride ? op->rowStride / pfd.stride : 0;

        if (rowLen == op->rgn.whd.w)
            rowLen = 0; // zero it to let the implementation handling with default behavior.

        gl->PixelStorei(GL_UNPACK_ALIGNMENT, 1); _ZglThrowErrorOccuried();
        gl->PixelStorei(GL_UNPACK_ROW_LENGTH, rowLen); _ZglThrowErrorOccuried();
        gl->PixelStorei(GL_UNPACK_IMAGE_HEIGHT, rowsPerImg); _ZglThrowErrorOccuried();

        if (!AfxIsPixelFormatCompressed(ras->m.fmt))
        {
            switch (glTarget)
            {
            case GL_TEXTURE_2D:
            {
                AFX_ASSERT(1 >= rgn->whd.d);

                if (gl->TextureSubImage2D)
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->TextureSubImage2D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, ras->glFmt, ras->glType, &src[op->offset]); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    gl->TexSubImage2D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, ras->glFmt, ras->glType, &src[op->offset]); _ZglThrowErrorOccuried();
                }
                break;
            }
            case GL_TEXTURE_2D_ARRAY:
            case GL_TEXTURE_CUBE_MAP_ARRAY:
            case GL_TEXTURE_3D:
            {
                if (gl->TextureSubImage3D)
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->TextureSubImage3D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z, rgn->whd.w, rgn->whd.h, rgn->whd.d, ras->glFmt, ras->glType, &src[op->offset]); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    gl->TexSubImage3D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z, rgn->whd.w, rgn->whd.h, rgn->whd.d, ras->glFmt, ras->glType, &src[op->offset]); _ZglThrowErrorOccuried();
                }
                break;
            }
            case GL_TEXTURE_1D_ARRAY:
            {
                AFX_ASSERT(1 >= rgn->whd.h);

                if (gl->TextureSubImage2D)
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->TextureSubImage2D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.z, rgn->whd.w, rgn->whd.d, ras->glFmt, ras->glType, &src[op->offset]); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    gl->TexSubImage2D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.z, rgn->whd.w, rgn->whd.d, ras->glFmt, ras->glType, &src[op->offset]); _ZglThrowErrorOccuried();
                }
                break;
            }
            case GL_TEXTURE_1D:
            {
                AFX_ASSERT(1 >= rgn->whd.h);
                AFX_ASSERT(1 >= rgn->whd.d);

                if (gl->TextureSubImage1D)
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->TextureSubImage1D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->whd.w, ras->glFmt, ras->glType, &src[op->offset]); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    gl->TexSubImage1D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->whd.w, ras->glFmt, ras->glType, &src[op->offset]); _ZglThrowErrorOccuried();
                }
                break;
            }
            case GL_TEXTURE_CUBE_MAP:
            {
                if (gl->TextureSubImage3D)
                {
                    if (!rasSynced)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    for (afxUnit i = 0; i < rgn->whd.d; i++)
                    {
                        gl->TextureSubImage3D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z + i + (glTarget - GL_TEXTURE_CUBE_MAP_POSITIVE_X), rgn->whd.w, rgn->whd.h, 1, glFmt, glType, &src[op->offset]); _ZglThrowErrorOccuried();
                    }
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    for (afxUnit i = 0; i < rgn->whd.d; i++)
                    {
                        gl->TexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + rgn->origin.z + i, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, glFmt, glType, &src[op->offset]); _ZglThrowErrorOccuried();
                    }
                }
                break;
            }
            default: AfxThrowError(); break;
            }
        }
        else
        {
            afxUnit compressedSiz = AFX_OR(rowLen, op->rgn.whd.w) * AFX_OR(rowsPerImg, op->rgn.whd.h) * AFX_OR(op->rgn.whd.d, 1);

            switch (glTarget)
            {
            case GL_TEXTURE_2D:
            {
                AFX_ASSERT(1 >= rgn->whd.d);

                if (gl->CompressedTextureSubImage2D)
                {
                    if (!rasSynced)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->CompressedTextureSubImage2D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, glFmt, compressedSiz, &src[op->offset]); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    zglBindTextureUnit(gl, ZGL_COPY_WRITE_RASTER, glTarget, ras->glHandle);
                    gl->CompressedTexSubImage2D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, glFmt, compressedSiz, &src[op->offset]); _ZglThrowErrorOccuried();
                }
                break;
            }
            case GL_TEXTURE_2D_ARRAY:
            case GL_TEXTURE_CUBE_MAP_ARRAY:
            case GL_TEXTURE_3D:
            {
                if (gl->CompressedTextureSubImage3D)
                {
                    if (!rasSynced)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->CompressedTextureSubImage3D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z, rgn->whd.w, rgn->whd.h, rgn->whd.d, glFmt, compressedSiz, &src[op->offset]); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    zglBindTextureUnit(gl, ZGL_COPY_WRITE_RASTER, glTarget, ras->glHandle);
                    gl->CompressedTexSubImage3D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z, rgn->whd.w, rgn->whd.h, rgn->whd.d, glFmt, compressedSiz, &src[op->offset]); _ZglThrowErrorOccuried();
                }
                break;
            }
            case GL_TEXTURE_1D_ARRAY:
            {
                AFX_ASSERT(1 >= rgn->whd.h);

                if (gl->CompressedTextureSubImage2D)
                {
                    if (!rasSynced)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->CompressedTextureSubImage2D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, glFmt, compressedSiz, &src[op->offset]); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    zglBindTextureUnit(gl, ZGL_COPY_WRITE_RASTER, glTarget, ras->glHandle);
                    gl->CompressedTexSubImage2D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, glFmt, compressedSiz, &src[op->offset]); _ZglThrowErrorOccuried();
                }
                break;
            }
            case GL_TEXTURE_1D:
            {
                AFX_ASSERT(1 >= rgn->whd.h);
                AFX_ASSERT(1 >= rgn->whd.d);

                if (gl->CompressedTextureSubImage1D)
                {
                    if (!rasSynced)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->CompressedTextureSubImage1D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->whd.w, glFmt, compressedSiz, &src[op->offset]); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    zglBindTextureUnit(gl, ZGL_COPY_WRITE_RASTER, glTarget, ras->glHandle);
                    gl->CompressedTexSubImage1D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->whd.w, glFmt, compressedSiz, &src[op->offset]); _ZglThrowErrorOccuried();
                }
                break;
            }
            case GL_TEXTURE_CUBE_MAP:
            {
                if (gl->CompressedTextureSubImage2D)
                {
                    if (!rasSynced)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    for (afxUnit i = 0; i < rgn->whd.d; i++)
                    {
                        gl->CompressedTextureSubImage3D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z + i, rgn->whd.w, rgn->whd.h, 1, glFmt, compressedSiz, &src[op->offset]); _ZglThrowErrorOccuried();
                    }
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    for (afxUnit i = 0; i < rgn->whd.d; i++)
                    {
                        gl->CompressedTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + rgn->origin.z + i, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, glFmt, compressedSiz, &src[op->offset]); _ZglThrowErrorOccuried();
                    }
                }
                break;
            }
            default: AfxThrowError(); break;
            }
        }
    }

#if UNBIND_GL_TEX
    if (rasBound)
    {
        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, NIL, TRUE);
    }
#endif
    return err;
}

_ZGL afxError _DpuDumpRaster(zglDpu* dpu, avxRaster ras, afxByte* dst, afxUnit opCnt, avxRasterIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    AFX_ASSERT(dpu->inDrawScope == FALSE); // This is a transfer operation.

    DpuBindAndSyncBuf(dpu, GL_PIXEL_PACK_BUFFER, NIL, TRUE);
    //DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, TRUE);

    GLenum glTarget = ras->glTarget;

    afxBool rasSynced = FALSE;
    afxBool rasBound = FALSE;
    afxBool pixStoreChanged = FALSE;

    for (afxUnit i = 0; i < opCnt; i++)
    {
        avxRasterIo const* op = &ops[i];
        avxRasterRegion const* rgn = &op->rgn;

        // GL_PACK_ROW_LENGTH is NOT row stride. It is the number of texel blocks per row of a image.
        afxUnit rowsPerImg = op->rowsPerImg && (op->rowsPerImg != op->rgn.whd.h) ? op->rowsPerImg : 0;
        afxUnit rowLen = op->rowStride ? op->rowStride / (op->rowStride / op->rgn.whd.w) : 0;

        if (rowLen == op->rgn.whd.w)
            rowLen = 0; // zero it to let the implementation handling with default behavior.

        afxUnit bufSiz = AFX_OR(rowLen, op->rgn.whd.w) * AFX_OR(rowsPerImg, op->rgn.whd.h) * AFX_OR(op->rgn.whd.d, 1);

        gl->PixelStorei(GL_PACK_ALIGNMENT, 1); _ZglThrowErrorOccuried();
        gl->PixelStorei(GL_PACK_ROW_LENGTH, rowLen); _ZglThrowErrorOccuried();
        gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, rowsPerImg); _ZglThrowErrorOccuried();

        if (AfxIsPixelFormatCompressed(ras->m.fmt))
        {
            if (gl->GetCompressedTextureSubImage)
            {
                if (!rasSynced)
                {
                    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, FALSE); // sync
                    rasSynced = TRUE;
                }
                gl->GetCompressedTextureSubImage(ras->glHandle, op->rgn.lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z, rgn->whd.w, rgn->whd.h, rgn->whd.d, bufSiz, &dst[op->offset]); _ZglThrowErrorOccuried();
            }
            else if (gl->GetnCompressedTexImage)
            {
                if (!rasBound)
                {
                    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, TRUE); // bind
                    rasSynced = TRUE;
                    rasBound = TRUE;
                }
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, rgn->origin.x); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, rgn->origin.y); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, rgn->origin.z); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, rgn->whd.w); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, rgn->whd.h); _ZglThrowErrorOccuried();
                gl->GetnCompressedTexImage(glTarget, op->rgn.lodIdx, bufSiz, &dst[op->offset]); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, 0); _ZglThrowErrorOccuried();
            }
            else
            {
                if (!rasBound)
                {
                    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, TRUE); // bind
                    rasSynced = TRUE;
                    rasBound = TRUE;
                }
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, rgn->origin.x); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, rgn->origin.y); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, rgn->origin.z); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, rgn->whd.w); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, rgn->whd.h); _ZglThrowErrorOccuried();
                gl->GetCompressedTexImage(glTarget, op->rgn.lodIdx, &dst[op->offset]); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, 0); _ZglThrowErrorOccuried();
            }
        }
        else
        {
            if (gl->GetTextureSubImage)
            {
                if (!rasSynced)
                {
                    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, FALSE); // sync
                    rasSynced = TRUE;
                }
                gl->GetTextureSubImage(ras->glHandle, op->rgn.lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z, rgn->whd.w, rgn->whd.h, rgn->whd.d, ras->glFmt, ras->glType, bufSiz, &dst[op->offset]); _ZglThrowErrorOccuried();
            }
            else if (gl->GetTextureImage)
            {
                if (!rasSynced)
                {
                    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, FALSE); // sync
                    rasSynced = TRUE;
                }
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, rgn->origin.x); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, rgn->origin.y); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, rgn->origin.z); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, rgn->whd.w); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, rgn->whd.h); _ZglThrowErrorOccuried();
                gl->GetTextureImage(ras->glHandle, rgn->lodIdx, ras->glFmt, ras->glType, bufSiz, &dst[op->offset]); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, 0); _ZglThrowErrorOccuried();
            }
            else if (gl->GetnTexImage)
            {
                if (!rasBound)
                {
                    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, TRUE); // bind
                    rasSynced = TRUE;
                    rasBound = TRUE;
                }
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, rgn->origin.x); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, rgn->origin.y); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, rgn->origin.z); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, rgn->whd.w); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, rgn->whd.h); _ZglThrowErrorOccuried();
                gl->GetnTexImage(glTarget, rgn->lodIdx, ras->glFmt, ras->glType, bufSiz, &dst[op->offset]); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, 0); _ZglThrowErrorOccuried();
            }
            else
            {
                if (!rasBound)
                {
                    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, TRUE); // bind
                    rasSynced = TRUE;
                    rasBound = TRUE;
                }
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, rgn->origin.x); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, rgn->origin.y); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, rgn->origin.z); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, rgn->whd.w); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, rgn->whd.h); _ZglThrowErrorOccuried();
                gl->GetTexImage(glTarget, rgn->lodIdx, ras->glFmt, ras->glType, &dst[op->offset]); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, 0); _ZglThrowErrorOccuried();
            }
        }
    }

#if UNBIND_GL_TEX
    if (rasBound)
    {
        DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, NIL, TRUE);
    }
#endif
    return err;
}

_ZGL afxError DpuUnpackRaster(zglDpu* dpu, avxRaster ras, avxBuffer buf, afxUnit opCnt, avxRasterIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    AFX_ASSERT(dpu->inDrawScope == FALSE); // This is a transfer operation.

    GLenum glTarget = ras->glTarget;
    GLenum glType = ras->glType;
    GLenum glFmt = ras->glFmt;

    afxBool rasSynced = FALSE;
    afxBool bufSynced = FALSE;
    afxBool rasBound = FALSE;
    afxBool bufBound = FALSE;

    //DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE);
    DpuBindAndSyncBuf(dpu, GL_PIXEL_UNPACK_BUFFER, buf, TRUE);
    bufBound = TRUE; // required for all ops.

    avxFormatDescription pfd;
    AvxDescribeFormats(1, &ras->m.fmt, &pfd);

    for (afxUnit i = 0; i < opCnt; i++)
    {
        avxRasterIo const* op = &ops[i];
        avxRasterRegion const* rgn = &op->rgn;

        // GL_UNPACK_ROW_LENGTH is NOT row stride. It is the number of texel blocks per row of a image.
        afxUnit rowsPerImg = op->rowsPerImg && (op->rowsPerImg != op->rgn.whd.h) ? op->rowsPerImg : 0;
        afxUnit rowLen = 0;// op->rowStride ? op->rowStride / (op->rowStride / op->rgn.whd.w) : 0;

        rowLen = op->rowStride ? op->rowStride / pfd.stride : 0;

        if (rowLen == op->rgn.whd.w)
            rowLen = 0; // zero it to let the implementation handling with default behavior.

        gl->PixelStorei(GL_UNPACK_ALIGNMENT, 1); _ZglThrowErrorOccuried();
        gl->PixelStorei(GL_UNPACK_ROW_LENGTH, rowLen); _ZglThrowErrorOccuried();
        gl->PixelStorei(GL_UNPACK_IMAGE_HEIGHT, rowsPerImg); _ZglThrowErrorOccuried();

        // load from PBO

        if (!AfxIsPixelFormatCompressed(ras->m.fmt))
        {
            switch (glTarget)
            {
            case GL_TEXTURE_2D:
            {
                AFX_ASSERT(1 >= rgn->whd.d);

                if (gl->TextureSubImage2D)
                {
                    if (!rasSynced)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->TextureSubImage2D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, ras->glFmt, ras->glType, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    gl->TexSubImage2D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, ras->glFmt, ras->glType, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                break;
            }
            case GL_TEXTURE_2D_ARRAY:
            case GL_TEXTURE_3D:
            {
                if (gl->TextureSubImage3D)
                {
                    if (!rasSynced)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->TextureSubImage3D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z, rgn->whd.w, rgn->whd.h, rgn->whd.d, ras->glFmt, ras->glType, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    gl->TexSubImage3D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z, rgn->whd.w, rgn->whd.h, rgn->whd.d, ras->glFmt, ras->glType, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                break;
            }
            case GL_TEXTURE_1D_ARRAY:
            {
                AFX_ASSERT(1 >= rgn->whd.h);

                if (gl->TextureSubImage2D)
                {
                    if (!rasSynced)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->TextureSubImage2D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.z, rgn->whd.w, rgn->whd.d, ras->glFmt, ras->glType, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    gl->TexSubImage2D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.z, rgn->whd.w, rgn->whd.d, ras->glFmt, ras->glType, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                break;
            }
            case GL_TEXTURE_1D:
            {
                AFX_ASSERT(1 >= rgn->whd.h);
                AFX_ASSERT(1 >= rgn->whd.d);

                if (gl->TextureSubImage1D)
                {
                    if (!rasSynced)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->TextureSubImage1D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->whd.w, ras->glFmt, ras->glType, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    gl->TexSubImage1D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->whd.w, ras->glFmt, ras->glType, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                break;
            }
            case GL_TEXTURE_CUBE_MAP:
            {
                if (gl->TextureSubImage2D)
                {
                    if (!rasSynced)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->TextureSubImage2D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, ras->glFmt, ras->glType, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    for (afxUnit i = 0; i < rgn->whd.d; i++)
                    {
                        gl->TexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + rgn->origin.z + i, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, ras->glFmt, ras->glType, (void const*)op->offset); _ZglThrowErrorOccuried();
                        AFX_ASSERT(1 >= rgn->whd.d);
                    }
                }
                break;
            }
            default:
                break;
            }
        }
        else
        {
            afxUnit compressedSiz = AFX_OR(rowLen, op->rgn.whd.w) * AFX_OR(rowsPerImg, op->rgn.whd.h) * AFX_OR(op->rgn.whd.d, 1);
            
            switch (glTarget)
            {
            case GL_TEXTURE_2D:
            {
                AFX_ASSERT(1 >= op->rgn.whd.d);

                if (gl->CompressedTextureSubImage2D)
                {
                    if (!rasSynced)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->CompressedTextureSubImage2D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, glFmt, compressedSiz, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    gl->CompressedTexSubImage2D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, glFmt, compressedSiz, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                break;
            }
            case GL_TEXTURE_2D_ARRAY:
            case GL_TEXTURE_3D:
            {
                if (gl->CompressedTextureSubImage3D)
                {
                    if (!rasSynced)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->CompressedTextureSubImage3D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z, rgn->whd.w, rgn->whd.h, rgn->whd.d, glFmt, compressedSiz, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    gl->CompressedTexSubImage3D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z, rgn->whd.w, rgn->whd.h, rgn->whd.d, glFmt, compressedSiz, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                break;
            }
            case GL_TEXTURE_1D_ARRAY:
            {
                AFX_ASSERT(1 >= op->rgn.whd.h);

                if (gl->CompressedTextureSubImage2D)
                {
                    if (!rasSynced)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->CompressedTextureSubImage2D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, glFmt, compressedSiz, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    gl->CompressedTexSubImage2D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, glFmt, compressedSiz, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                break;
            }
            case GL_TEXTURE_1D:
            {
                AFX_ASSERT(1 >= op->rgn.whd.h);
                AFX_ASSERT(1 >= op->rgn.whd.d);

                if (gl->CompressedTextureSubImage1D)
                {
                    if (!rasSynced)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->CompressedTextureSubImage1D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->whd.w, glFmt, compressedSiz, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    gl->CompressedTexSubImage1D(glTarget, rgn->lodIdx, rgn->origin.x, rgn->whd.w, glFmt, compressedSiz, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                break;
            }
            case GL_TEXTURE_CUBE_MAP:
            {
                if (gl->CompressedTextureSubImage2D)
                {
                    if (!rasSynced)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, FALSE); // sync
                        rasSynced = TRUE;
                    }
                    gl->CompressedTextureSubImage2D(ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, glFmt, compressedSiz, (void const*)op->offset); _ZglThrowErrorOccuried();
                }
                else
                {
                    if (!rasBound)
                    {
                        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE); // bind
                        rasSynced = TRUE;
                        rasBound = TRUE;
                    }
                    for (afxUnit i = 0; i < rgn->whd.d; i++)
                    {
                        gl->CompressedTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + rgn->origin.z + i, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, glFmt, compressedSiz, (void const*)op->offset); _ZglThrowErrorOccuried();
                        AFX_ASSERT(1 >= op->rgn.whd.d);
                    }
                }
                break;
            }
            default: AfxThrowError(); break;
            }
        }
    }
#if UNBIND_GL_TEX
    DpuBindAndSyncBuf(dpu, GL_PIXEL_UNPACK_BUFFER, NIL, TRUE);
    if (rasBound)
    {
        DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, NIL, TRUE);
    }
#endif
    return err;
}

_ZGL afxError DpuPackRaster(zglDpu* dpu, avxRaster ras, avxBuffer buf, afxUnit opCnt, avxRasterIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    AFX_ASSERT(dpu->inDrawScope == FALSE); // This is a transfer operation.

    GLenum glTarget = ras->glTarget;
    GLenum glType = ras->glType;
    GLenum glFmt = ras->glFmt;

    afxBool rasSynced = FALSE;
    afxBool rasBound = FALSE;
    DpuBindAndSyncBuf(dpu, GL_PIXEL_PACK_BUFFER, buf, TRUE);
    //DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, TRUE);

    for (afxUnit i = 0; i < opCnt; i++)
    {
        avxRasterIo const* op = &ops[i];
        avxRasterRegion const* rgn = &op->rgn;

        // GL_PACK_ROW_LENGTH is NOT row stride. It is the number of texel blocks per row of a image.
        afxUnit rowsPerImg = op->rowsPerImg && (op->rowsPerImg != op->rgn.whd.h) ? op->rowsPerImg : 0;
        afxUnit rowLen = op->rowStride ? op->rowStride / (op->rowStride / op->rgn.whd.w) : 0;

        if (rowLen == op->rgn.whd.w)
            rowLen = 0; // zero it to let the implementation handling with default behavior.

        gl->PixelStorei(GL_PACK_ALIGNMENT, 1); _ZglThrowErrorOccuried();
        gl->PixelStorei(GL_PACK_ROW_LENGTH, rowLen); _ZglThrowErrorOccuried();
        gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, rowsPerImg); _ZglThrowErrorOccuried();

        // store into PBO at @op.offset
        afxUnit bufSiz = op->rowStride * op->rowStride;
        // TODO: obrigar a especifica��o de um tamanho sempre que for exportar?

        if (AfxIsPixelFormatCompressed(ras->m.fmt))
        {
            if (gl->GetCompressedTextureSubImage)
            {
                if (!rasSynced)
                {
                    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, FALSE); // sync
                    rasSynced = TRUE;
                }
                gl->GetCompressedTextureSubImage(ras->glHandle, op->rgn.lodIdx, op->rgn.origin.x, op->rgn.origin.y, op->rgn.origin.z, op->rgn.whd.w, op->rgn.whd.h, op->rgn.whd.d, bufSiz, (void*)op->offset); _ZglThrowErrorOccuried();
            }
            else if (gl->GetnCompressedTexImage)
            {
                if (!rasBound)
                {
                    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, TRUE); // bind
                    rasSynced = TRUE;
                    rasBound = TRUE;
                }
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, op->rgn.origin.x); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, op->rgn.origin.y); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, op->rgn.origin.z); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, op->rgn.whd.w); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, op->rgn.whd.h); _ZglThrowErrorOccuried();
                gl->GetnCompressedTexImage(glTarget, op->rgn.lodIdx, bufSiz, (void*)op->offset); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, 0); _ZglThrowErrorOccuried();
            }
            else
            {
                if (!rasBound)
                {
                    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, TRUE); // bind
                    rasSynced = TRUE;
                    rasBound = TRUE;
                }
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, op->rgn.origin.x); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, op->rgn.origin.y); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, op->rgn.origin.z); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, op->rgn.whd.w); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, op->rgn.whd.h); _ZglThrowErrorOccuried();
                gl->GetCompressedTexImage(glTarget, op->rgn.lodIdx, (void*)op->offset); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, 0); _ZglThrowErrorOccuried();
            }
        }
        else
        {
            if (gl->GetTextureSubImage)
            {
                if (!rasSynced)
                {
                    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, FALSE); // sync
                    rasSynced = TRUE;
                }
                gl->GetTextureSubImage(ras->glHandle, op->rgn.lodIdx, op->rgn.origin.x, op->rgn.origin.y, op->rgn.origin.z, op->rgn.whd.w, op->rgn.whd.h, op->rgn.whd.d, ras->glFmt, ras->glType, bufSiz, (void*)op->offset); _ZglThrowErrorOccuried();
            }
            else if (gl->GetTextureImage)
            {
                if (!rasSynced)
                {
                    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, FALSE); // sync
                    rasSynced = TRUE;
                }
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, op->rgn.origin.x); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, op->rgn.origin.y); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, op->rgn.origin.z); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, op->rgn.whd.w); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, op->rgn.whd.h); _ZglThrowErrorOccuried();
                gl->GetTextureImage(ras->glHandle, op->rgn.lodIdx, ras->glFmt, ras->glType, bufSiz, (void*)op->offset); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, 0); _ZglThrowErrorOccuried();
            }
            else if (gl->GetnTexImage)
            {
                if (!rasBound)
                {
                    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, TRUE); // bind
                    rasSynced = TRUE;
                    rasBound = TRUE;
                }
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, op->rgn.origin.x); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, op->rgn.origin.y); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, op->rgn.origin.z); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, op->rgn.whd.w); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, op->rgn.whd.h); _ZglThrowErrorOccuried();
                gl->GetnTexImage(glTarget, op->rgn.lodIdx, ras->glFmt, ras->glType, bufSiz, (void*)op->offset); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, 0); _ZglThrowErrorOccuried();
            }
            else
            {
                if (!rasBound)
                {
                    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, TRUE); // bind
                    rasSynced = TRUE;
                    rasBound = TRUE;
                }
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, op->rgn.origin.x); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, op->rgn.origin.y); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, op->rgn.origin.z); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, op->rgn.whd.w); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, op->rgn.whd.h); _ZglThrowErrorOccuried();
                gl->GetTexImage(ras->glHandle, op->rgn.lodIdx, ras->glFmt, ras->glType, (void*)op->offset); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_PIXELS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_ROWS, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_SKIP_IMAGES, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_ROW_LENGTH, 0); _ZglThrowErrorOccuried();
                gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, 0); _ZglThrowErrorOccuried();
            }
        }
    }

#if UNBIND_GL_TEX
    DpuBindAndSyncBuf(dpu, GL_PIXEL_PACK_BUFFER, NIL, TRUE);
    if (rasBound)
    {
        DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, NIL, TRUE);
    }
#endif
    return err;
}

_ZGL afxError _DpuDownloadRaster(zglDpu* dpu, avxRaster ras, afxStream out, afxUnit opCnt, avxRasterIo const* ops)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

    AFX_ASSERT(dpu->inDrawScope == FALSE); // This is a transfer operation.

    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, ras, TRUE);
    afxBool is3d = AvxTestRasterFlags(ras, avxRasterFlag_3D);

    avxFormatDescription pfd;
    AvxDescribeFormats(1, &ras->m.fmt, &pfd);

    for (afxUnit i = 0; i < opCnt; i++)
    {
        avxRasterIo const* op = &ops[i];

        // GL_PACK_ROW_LENGTH is NOT row stride. It is the number of texel blocks per row of a image.
        afxUnit rowsPerImg = op->rowsPerImg && (op->rowsPerImg != op->rgn.whd.h) ? op->rowsPerImg : 0;
        afxUnit rowLen = op->rowStride ? op->rowStride / (op->rowStride / op->rgn.whd.w) : 0;

        if (rowLen == op->rgn.whd.w)
            rowLen = 0; // zero it to let the implementation handling with default behavior.

        gl->PixelStorei(GL_PACK_ALIGNMENT, 1); _ZglThrowErrorOccuried();
        gl->PixelStorei(GL_PACK_ROW_LENGTH, rowLen); _ZglThrowErrorOccuried();
        gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, rowsPerImg); _ZglThrowErrorOccuried();

        GLuint pbo;
        zglCreateBuffers(gl, GL_PIXEL_PACK_BUFFER, 1, &pbo); _ZglThrowErrorOccuried();
        
        avxRasterLayout lay;
        AvxQueryRasterLayout(ras, op->rgn.lodIdx, (is3d ? 0 : op->rgn.origin.z), &lay);

        GLint lodSiz = 0;

        afxUnit bufSiz = op->rowStride * op->rowStride;
        // TODO: obrigar a especifica��o de um tamanho sempre que for exportar?

        if (AfxIsPixelFormatCompressed(ras->m.fmt))
        {
            GLint compressed;
            gl->GetTexLevelParameteriv(ras->glTarget, op->rgn.lodIdx, GL_TEXTURE_COMPRESSED, &compressed);
            AFX_ASSERT(compressed);
            gl->GetTexLevelParameteriv(ras->glTarget, op->rgn.lodIdx, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &lodSiz); _ZglThrowErrorOccuried();

            zglMakeBufferStorage(gl, GL_PIXEL_PACK_BUFFER, pbo, lodSiz * 2, NIL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT); _ZglThrowErrorOccuried();

            // pack into PBO
            if (zglPackTextureSubImageCompressed(gl, ras->glTarget, ras->glHandle, op->rgn.lodIdx, op->rgn.origin.x, op->rgn.origin.y, op->rgn.origin.z, op->rgn.whd.w, op->rgn.whd.h, op->rgn.whd.d, bufSiz, pbo, 0))
                AfxThrowError();

            afxByte* bytemap;
            zglMapBuffer(gl, GL_PIXEL_PACK_BUFFER, pbo, GL_READ_ONLY, (void**)&bytemap); _ZglThrowErrorOccuried();
            
            AfxWriteStreamAt(out, op->offset, op->rgn.whd.d * op->rgn.whd.h * AFX_ALIGN_SIZE(op->rgn.whd.w * pfd.stride, AFX_SIMD_ALIGNMENT), 0, bytemap);
        }
        else
        {
            lodSiz = lay.size;
            zglMakeBufferStorage(gl, GL_PIXEL_PACK_BUFFER, pbo, lodSiz * 2, NIL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT); _ZglThrowErrorOccuried();

            // pack into PBO
            if (zglPackTextureSubImage(gl, ras->glTarget, ras->glHandle, op->rgn.lodIdx, op->rgn.origin.x, op->rgn.origin.y, op->rgn.origin.z, op->rgn.whd.w, op->rgn.whd.h, op->rgn.whd.d, ras->glFmt, ras->glType, bufSiz, pbo, 0))
                AfxThrowError();
            
            afxByte* bytemap;
            zglMapBuffer(gl, GL_PIXEL_PACK_BUFFER, pbo, GL_READ_ONLY, (void**)&bytemap); _ZglThrowErrorOccuried();

            AfxWriteStreamAt(out, op->offset, op->rgn.whd.d * op->rgn.whd.h * AFX_ALIGN_SIZE(op->rgn.whd.w * pfd.stride, AFX_SIMD_ALIGNMENT), 0, bytemap);
        }

        gl->UnmapBuffer(GL_PIXEL_PACK_BUFFER); _ZglThrowErrorOccuried();
        gl->BindBuffer(GL_PIXEL_PACK_BUFFER, 0); _ZglThrowErrorOccuried();
        gl->DeleteBuffers(1, &pbo); _ZglThrowErrorOccuried();
    }

    // Restore pixel pack settings to avoid troubles.
    gl->PixelStorei(GL_PACK_SKIP_PIXELS, 0); _ZglThrowErrorOccuried();
    gl->PixelStorei(GL_PACK_SKIP_ROWS, 0); _ZglThrowErrorOccuried();
    gl->PixelStorei(GL_PACK_SKIP_IMAGES, 0); _ZglThrowErrorOccuried();

    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, NIL, TRUE);
    return err;
}

_ZGL afxError _DpuUploadRaster(zglDpu* dpu, avxRaster ras, afxStream in, afxUnit opCnt, avxRasterIo const* ops)
{
    afxError err = AFX_ERR_NONE;

    AFX_ASSERT(dpu->inDrawScope == FALSE); // This is a transfer operation.

    glVmt const* gl = dpu->gl;
    DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, ras, TRUE);
    afxBool is3d = AvxTestRasterFlags(ras, avxRasterFlag_3D);

    GLenum glTarget = ras->glTarget;
    GLenum glType = ras->glType;
    GLenum glFmt = ras->glFmt;

    for (afxUnit i = 0; i < opCnt; i++)
    {
        avxRasterIo const* op = &ops[i];
        avxRasterRegion const* rgn = &op->rgn;

        // GL_UNPACK_ROW_LENGTH is NOT row stride. It is the number of texel blocks per row of a image.
        afxUnit rowsPerImg = op->rowsPerImg && (op->rowsPerImg != op->rgn.whd.h) ? op->rowsPerImg : 0;
        afxUnit rowLen = op->rowStride ? op->rowStride / (op->rowStride / op->rgn.whd.w) : 0;

        if (rowLen == op->rgn.whd.w)
            rowLen = 0; // zero it to let the implementation handling with default behavior.

        if (rowsPerImg == op->rgn.whd.h)
            rowsPerImg = 0;

        avxRasterLayout lay;
        AvxQueryRasterLayout(ras, op->rgn.lodIdx, (is3d ? 0 : op->rgn.origin.z), &lay);
        afxUnit bufSiz = lay.size;

        GLuint pbo;
        gl->GenBuffers(1, &pbo); _ZglThrowErrorOccuried();
        gl->BindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo); _ZglThrowErrorOccuried();
        gl->BufferStorage(GL_PIXEL_UNPACK_BUFFER, bufSiz, NIL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT); _ZglThrowErrorOccuried();

        afxByte* bytemap = gl->MapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY); _ZglThrowErrorOccuried();
        AfxReadStreamAt(in, op->offset, op->encSiz ? op->encSiz : bufSiz, 0, bytemap);
        gl->UnmapBuffer(GL_PIXEL_UNPACK_BUFFER); _ZglThrowErrorOccuried();

        gl->PixelStorei(GL_UNPACK_ALIGNMENT, 1); _ZglThrowErrorOccuried();
        gl->PixelStorei(GL_UNPACK_ROW_LENGTH, rowLen); _ZglThrowErrorOccuried();
        gl->PixelStorei(GL_UNPACK_IMAGE_HEIGHT, rowsPerImg); _ZglThrowErrorOccuried();

        if (!AfxIsPixelFormatCompressed(ras->m.fmt))
        {
            if (zglUnpackTextureSubImage(gl, ras->glTarget, ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z, rgn->whd.w, rgn->whd.h, rgn->whd.d, ras->glFmt, ras->glType, pbo, 0))
                AfxThrowError();
#if 0
            case GL_TEXTURE_CUBE_MAP:
            case GL_TEXTURE_CUBE_MAP_ARRAY:
            {
                for (afxUnit i = 0; i < rgn->whd.d; i++)
                {
                    if (zglTextureSubImage2D(gl, GL_TEXTURE_CUBE_MAP_POSITIVE_X + rgn->origin.z + i, ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, ras->glFmt, ras->glType, (void const*)0))
                        AfxThrowError();
                }
                break;
            }
#endif
        }
        else
        {
            GLint compressed;
            GLint compressedSiz;
            gl->GetTexLevelParameteriv(ras->glTarget, op->rgn.lodIdx, GL_TEXTURE_COMPRESSED, &compressed);
            gl->GetTexLevelParameteriv(ras->glTarget, op->rgn.lodIdx, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressedSiz); _ZglThrowErrorOccuried();
            AFX_ASSERT(compressed);

            if (zglUnpackTextureSubImageCompressed(gl, ras->glTarget, ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->origin.z, rgn->whd.w, rgn->whd.h, rgn->whd.d, glFmt, compressedSiz, pbo, 0))
                AfxThrowError();

#if 0
            case GL_TEXTURE_CUBE_MAP:
            case GL_TEXTURE_CUBE_MAP_ARRAY:
            {
                for (afxUnit i = 0; i < rgn->whd.d; i++)
                {
                    if (zglCompressedTextureSubImage2D(gl, GL_TEXTURE_CUBE_MAP_POSITIVE_X + rgn->origin.z + i, ras->glHandle, rgn->lodIdx, rgn->origin.x, rgn->origin.y, rgn->whd.w, rgn->whd.h, glFmt, compressedSiz, (void const*)0))
                        AfxThrowError();
                }
                break;
            }
#endif
        }
        
        gl->BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0); _ZglThrowErrorOccuried();
        gl->DeleteBuffers(1, &pbo); _ZglThrowErrorOccuried();
    }

    DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, NIL, TRUE);
    return err;
}

_ZGL afxError DpuCopyRaster(zglDpu* dpu, avxRaster src, avxRaster dst, afxUnit opCnt, avxRasterCopy const ops[])
{
    afxError err = AFX_ERR_NONE;

    AFX_ASSERT(dpu->inDrawScope == FALSE); // This is a transfer operation.

    glVmt const* gl = dpu->gl;
    AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &dst);
    AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &src);

    avxFormatDescription srcPfd, dstPfd;
    AvxDescribeFormats(1, &src->m.fmt, &srcPfd);
    AvxDescribeFormats(1, &dst->m.fmt, &dstPfd);

    for (afxUnit i = 0; i < opCnt; i++)
    {
        avxRasterCopy const* op = &ops[i];

        avxRasterLayout lay;
        AvxQueryRasterLayout(src, 0, 0, &lay);
        
        GLuint pbo;
        gl->GenBuffers(1, &pbo); _ZglThrowErrorOccuried();
        gl->BindBuffer(GL_PIXEL_PACK_BUFFER, pbo); _ZglThrowErrorOccuried();
        gl->BufferStorage(GL_PIXEL_PACK_BUFFER, lay.size, NIL, GL_DYNAMIC_STORAGE_BIT); _ZglThrowErrorOccuried();

        gl->PixelStorei(GL_PACK_ALIGNMENT, 1); _ZglThrowErrorOccuried();
        gl->PixelStorei(GL_PACK_ROW_LENGTH, 0); _ZglThrowErrorOccuried();
        gl->PixelStorei(GL_PACK_IMAGE_HEIGHT, 0); _ZglThrowErrorOccuried();

        gl->PixelStorei(GL_UNPACK_ALIGNMENT, 1); _ZglThrowErrorOccuried();
        gl->PixelStorei(GL_UNPACK_ROW_LENGTH, 0); _ZglThrowErrorOccuried();
        gl->PixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0); _ZglThrowErrorOccuried();

        if ((srcPfd.flags & avxFormatFlag_COMPRESSED) == avxFormatFlag_COMPRESSED)
        {
            if (zglPackTextureSubImageCompressed(gl, src->glTarget, src->glHandle, op->src.lodIdx, op->src.origin.x, op->src.origin.y, op->src.origin.z, op->src.whd.w, op->src.whd.h, op->src.whd.d, lay.size, pbo, 0))
                AfxThrowError();

            if (zglUnpackTextureSubImageCompressed(gl, dst->glTarget, dst->glHandle, op->dstLodIdx, op->dstOrigin.x, op->dstOrigin.y, op->dstOrigin.z, op->src.whd.w, op->src.whd.h, op->src.whd.d, /*dst->glFmt*/src->glFmt, lay.size, pbo, 0))
                AfxThrowError();
        }
        else
        {
            if (zglPackTextureSubImage(gl, src->glTarget, src->glHandle, op->src.lodIdx, op->src.origin.x, op->src.origin.y, op->src.origin.z, op->src.whd.w, op->src.whd.h, op->src.whd.d, src->glFmt, src->glType, lay.size, pbo, 0))
                AfxThrowError();

            if (zglUnpackTextureSubImage(gl, src->glTarget, src->glHandle, op->src.lodIdx, op->src.origin.x, op->src.origin.y, op->src.origin.z, op->src.whd.w, op->src.whd.h, op->src.whd.d, src->glFmt, src->glType, pbo, 0))
                AfxThrowError();
        }

        gl->BindBuffer(GL_PIXEL_PACK_BUFFER, 0); _ZglThrowErrorOccuried();
        gl->BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0); _ZglThrowErrorOccuried();
        gl->DeleteBuffers(1, &pbo); _ZglThrowErrorOccuried();
    }
    return err;
}

_ZGL afxError _ZglDpuResolveRaster(zglDpu* dpu, avxRaster src, avxRaster dst, afxUnit opCnt, avxRasterCopy const ops[])
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &dst);
    AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &src);
    
    AFX_ASSERT(dpu->inDrawScope == FALSE); // This is a transfer operation.

    /*
        vkCmdResolveImage
        Purpose: Resolves a multisampled image to a non-multisampled image.
        Filter: Always uses nearest filtering (VK_FILTER_NEAREST).
        Restrictions:
            Source must be multisampled.
            Destination must be single-sampled.
            Format must match.
            Only supports resolving a 1:1 region (no scaling or filtering choice).

        To mimic vkCmdResolveImage, we should:
            Use nearest filtering only (to match Vulkan's behavior).
            Ensure that we resolve from a multisample framebuffer into a single-sample texture.
            Avoid any format conversion or scaling.
    */
    AFX_ASSERT(src->m.lodCnt > 1);
    AFX_ASSERT(AvxTestRasterFlags(src, avxRasterFlag_MULTISAMP));
    AFX_ASSERT(src->m.lodCnt == 1);
    AFX_ASSERT(!AvxTestRasterFlags(dst, avxRasterFlag_MULTISAMP));
    //AFX_ASSERT(src->m.fmt == dst->m.fmt); // GL can deal with some potential convertions.

    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, src, FALSE); // sync
    DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, dst, FALSE); // sync
    
    GLenum filter = ZglToGlTexelFilterMode(avxTexelFilter_NEAREST);

    GLuint fboOpSrc, fboOpDst;
    GLuint tempFbos[2];
    afxBool useTempFbo = _RESOLVE_RASTER_WITH_GEN_FBOS;
    if (useTempFbo)
    {
        gl->GenFramebuffers(2, tempFbos); _ZglThrowErrorOccuried();
        fboOpSrc = tempFbos[0];
        fboOpDst = tempFbos[1];
    }
    else
    {
        fboOpSrc = dpu->fboOpSrc;
        fboOpDst = dpu->fboOpDst;
    }

    gl->BindFramebuffer(GL_READ_FRAMEBUFFER, fboOpSrc);
    gl->BindFramebuffer(GL_DRAW_FRAMEBUFFER, fboOpDst);

    GLbitfield blitMask = 0;

    switch (src->glAttachment)
    {
    case GL_DEPTH_ATTACHMENT: blitMask |= GL_DEPTH_BUFFER_BIT; break;
    case GL_DEPTH_STENCIL_ATTACHMENT: blitMask |= GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT; break;
    case GL_STENCIL_ATTACHMENT: blitMask |= GL_STENCIL_BUFFER_BIT; break;
    case GL_COLOR_ATTACHMENT0:
    default: blitMask |= GL_COLOR_BUFFER_BIT; break;
    }
    switch (dst->glAttachment)
    {
    case GL_DEPTH_ATTACHMENT: blitMask |= GL_DEPTH_BUFFER_BIT; break;
    case GL_DEPTH_STENCIL_ATTACHMENT: blitMask |= GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT; break;
    case GL_STENCIL_ATTACHMENT: blitMask |= GL_STENCIL_BUFFER_BIT; break;
    case GL_COLOR_ATTACHMENT0:
    default: blitMask |= GL_COLOR_BUFFER_BIT; break;
    }

    for (afxUnit i = 0; i < opCnt; i++)
    {
        avxRasterCopy const* op = &ops[i];

        for (afxUnit srcZ = 0, dstZ = 0; (srcZ < op->src.whd.d) && (dstZ < op->src.whd.d); srcZ++, dstZ++)
        {
            if (gl->BlitNamedFramebuffer)
            {
                _ZglBindFboAttachment(gl, GL_READ_FRAMEBUFFER, fboOpSrc, src->glAttachment, src->glTarget, src->glHandle, op->src.lodIdx, src->m.baseLayer + op->src.origin.z + srcZ, FALSE);
                _ZglBindFboAttachment(gl, GL_DRAW_FRAMEBUFFER, fboOpDst, dst->glAttachment, dst->glTarget, dst->glHandle, op->dstLodIdx, dst->m.baseLayer + op->dstOrigin.z + dstZ, FALSE);

                gl->BlitNamedFramebuffer(fboOpSrc, fboOpDst, op->src.origin.x, op->src.origin.y, op->src.whd.w, op->src.whd.h, op->dstOrigin.x, op->dstOrigin.y, op->src.whd.w, op->src.whd.h, blitMask, filter); _ZglThrowErrorOccuried();
            }
            else
            {
                _ZglBindFboAttachment(gl, GL_READ_FRAMEBUFFER, NIL, src->glAttachment, src->glTarget, src->glHandle, op->src.lodIdx, src->m.baseLayer + op->src.origin.z + srcZ, FALSE);
                _ZglBindFboAttachment(gl, GL_DRAW_FRAMEBUFFER, NIL, dst->glAttachment, dst->glTarget, dst->glHandle, op->dstLodIdx, dst->m.baseLayer + op->dstOrigin.z + dstZ, FALSE);

                gl->BlitFramebuffer(op->src.origin.x, op->src.origin.y, op->src.whd.w, op->src.whd.h, op->dstOrigin.x, op->dstOrigin.y, op->src.whd.w, op->src.whd.h, blitMask, filter); _ZglThrowErrorOccuried();
            }
        }
    }

    if (useTempFbo)
    {
        gl->BindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        gl->BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        gl->DeleteFramebuffers(2, tempFbos); _ZglThrowErrorOccuried();
    }
    else
    {
        if (gl->BlitNamedFramebuffer)
        {
            _ZglBindFboAttachment(gl, GL_READ_FRAMEBUFFER, fboOpSrc, src->glAttachment, src->glTarget, NIL, 0, 0, FALSE);
            _ZglBindFboAttachment(gl, GL_DRAW_FRAMEBUFFER, fboOpDst, dst->glAttachment, dst->glTarget, NIL, 0, 0, FALSE);
        }
        else
        {
            _ZglBindFboAttachment(gl, GL_READ_FRAMEBUFFER, NIL, src->glAttachment, src->glTarget, NIL, 0, 0, FALSE);
            _ZglBindFboAttachment(gl, GL_DRAW_FRAMEBUFFER, NIL, dst->glAttachment, dst->glTarget, NIL, 0, 0, FALSE);
        }

        gl->BindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        gl->BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
    return err;
}

_ZGL afxError _ZglDpuBlitRaster(zglDpu* dpu, avxRaster src, avxRaster dst, afxUnit opCnt, avxRasterBlit const ops[], avxTexelFilter flt)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &dst);
    AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &src);

    AFX_ASSERT(dpu->inDrawScope == FALSE); // This is a transfer operation.

    /*
        vkCmdBlitImage
        Purpose: General-purpose image copy, potentially with format conversion and filtering.
        Filter: User-specified --- VK_FILTER_NEAREST or VK_FILTER_LINEAR.
        More flexible, can scale images and convert formats.

        To match Vulkan's vkCmdBlitImage, OpenGL's glBlitFramebuffer can be used similarly, 
        but with either GL_LINEAR or GL_NEAREST as needed. Or, for more general flexibility 
        (e.g., for mipmap generation or format conversion), a shader-based blit might be required.
    */

    DpuBindAndSyncRas(dpu, ZGL_COPY_READ_RASTER, src, FALSE); // sync
    DpuBindAndSyncRas(dpu, ZGL_COPY_WRITE_RASTER, dst, FALSE); // sync

    GLenum filter = ZglToGlTexelFilterMode(flt);

    GLuint fboOpSrc, fboOpDst;
    GLuint tempFbos[2];
    afxBool useTempFbo = _BLIT_RASTER_WITH_GEN_FBOS;
    if (useTempFbo)
    {
        gl->GenFramebuffers(2, tempFbos); _ZglThrowErrorOccuried();
        fboOpSrc = tempFbos[0];
        fboOpDst = tempFbos[1];
    }
    else
    {
        fboOpSrc = dpu->fboOpSrc;
        fboOpDst = dpu->fboOpDst;
    }

    gl->BindFramebuffer(GL_READ_FRAMEBUFFER, fboOpSrc);
    gl->BindFramebuffer(GL_DRAW_FRAMEBUFFER, fboOpDst);

    GLbitfield blitMask = 0;

    switch (src->glAttachment)
    {
    case GL_DEPTH_ATTACHMENT: blitMask |= GL_DEPTH_BUFFER_BIT; break;
    case GL_DEPTH_STENCIL_ATTACHMENT: blitMask |= GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT; break;
    case GL_STENCIL_ATTACHMENT: blitMask |= GL_STENCIL_BUFFER_BIT; break;
    case GL_COLOR_ATTACHMENT0:
    default: blitMask |= GL_COLOR_BUFFER_BIT; break;
    }
    switch (dst->glAttachment)
    {
    case GL_DEPTH_ATTACHMENT: blitMask |= GL_DEPTH_BUFFER_BIT; break;
    case GL_DEPTH_STENCIL_ATTACHMENT: blitMask |= GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT; break;
    case GL_STENCIL_ATTACHMENT: blitMask |= GL_STENCIL_BUFFER_BIT; break;
    case GL_COLOR_ATTACHMENT0:
    default: blitMask |= GL_COLOR_BUFFER_BIT; break;
    }

    for (afxUnit i = 0; i < opCnt; i++)
    {
        avxRasterBlit const* op = &ops[i];

        for (afxUnit srcZ = 0, dstZ = 0; (srcZ < op->src.whd.d) && (dstZ < op->dst.whd.d); srcZ++, dstZ++)
        {
            if (gl->BlitNamedFramebuffer)
            {
                _ZglBindFboAttachment(gl, GL_READ_FRAMEBUFFER, fboOpSrc, src->glAttachment, src->glTarget, src->glHandle, op->src.lodIdx, src->m.baseLayer + op->src.origin.z + srcZ, FALSE);
                _ZglBindFboAttachment(gl, GL_DRAW_FRAMEBUFFER, fboOpDst, dst->glAttachment, dst->glTarget, dst->glHandle, op->dst.lodIdx, dst->m.baseLayer + op->dst.origin.z + dstZ, FALSE);

                gl->BlitNamedFramebuffer(fboOpSrc, fboOpDst, op->src.origin.x, op->src.origin.y, op->src.whd.w, op->src.whd.h, op->dst.origin.x, op->dst.origin.y, op->dst.whd.w, op->dst.whd.h, blitMask, filter); _ZglThrowErrorOccuried();
            }
            else
            {
                _ZglBindFboAttachment(gl, GL_READ_FRAMEBUFFER, NIL, src->glAttachment, src->glTarget, src->glHandle, op->src.lodIdx, src->m.baseLayer + op->src.origin.z + srcZ, FALSE);
                _ZglBindFboAttachment(gl, GL_DRAW_FRAMEBUFFER, NIL, dst->glAttachment, dst->glTarget, dst->glHandle, op->dst.lodIdx, dst->m.baseLayer + op->dst.origin.z + dstZ, FALSE);

                gl->BlitFramebuffer(op->src.origin.x, op->src.origin.y, op->src.whd.w, op->src.whd.h, op->dst.origin.x, op->dst.origin.y, op->dst.whd.w, op->dst.whd.h, blitMask, filter); _ZglThrowErrorOccuried();
            }
        }
    }

    if (useTempFbo)
    {
        gl->BindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        gl->BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        gl->DeleteFramebuffers(2, tempFbos); _ZglThrowErrorOccuried();
    }
    else
    {
        if (gl->BlitNamedFramebuffer)
        {
            _ZglBindFboAttachment(gl, GL_READ_FRAMEBUFFER, fboOpSrc, src->glAttachment, src->glTarget, NIL, 0, 0, FALSE);
            _ZglBindFboAttachment(gl, GL_DRAW_FRAMEBUFFER, fboOpDst, dst->glAttachment, dst->glTarget, NIL, 0, 0, FALSE);
        }
        else
        {
            _ZglBindFboAttachment(gl, GL_READ_FRAMEBUFFER, NIL, src->glAttachment, src->glTarget, NIL, 0, 0, FALSE);
            _ZglBindFboAttachment(gl, GL_DRAW_FRAMEBUFFER, NIL, dst->glAttachment, dst->glTarget, NIL, 0, 0, FALSE);
        }

        gl->BindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        gl->BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
    return err;
}

_ZGL afxError _ZglRasDtor(avxRaster ras)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_RAS, 1, &ras);

    afxDrawSystem dsys = AfxGetProvider(ras);

    if (ras->glHandle)
    {
        if (ras->glTarget == GL_RENDERBUFFER)
            _ZglDsysEnqueueDeletion(dsys, 0, GL_RENDERBUFFER, (afxSize)ras->glHandle);
        else
            _ZglDsysEnqueueDeletion(dsys, 0, GL_TEXTURE, (afxSize)ras->glHandle);

        ras->glHandle = 0;
    }

    if (_AVX_RAS_CLASS_CONFIG.dtor(ras))
        AfxThrowError();

    return err;
}

_ZGL afxError _ZglRasCtor(avxRaster ras, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;

    if (_AVX_RAS_CLASS_CONFIG.ctor(ras, args, invokeNo)) AfxThrowError();
    else
    {
        afxDrawSystem dsys = AfxGetProvider(ras);
        ras->rasUniqueId = ++dsys->rasUniqueId;

        ras->updFlags = ZGL_UPD_FLAG_DEVICE_INST;

        GLint glIntFmt;
        ZglDetermineGlTargetInternalFormatType(ras, &ras->glTarget, &ras->glIntFmt, &ras->glFmt, &ras->glType);
        
        if ((ras->m.usage & avxRasterUsage_DRAW) == avxRasterUsage_DRAW)
        {
            avxFormatDescription pfd;
            AvxDescribeRasterFormat(ras, 0, &pfd);

            if ((pfd.flags & (avxFormatFlag_DEPTH | avxFormatFlag_STENCIL)) == (avxFormatFlag_DEPTH | avxFormatFlag_STENCIL))
                ras->glAttachment = GL_DEPTH_STENCIL_ATTACHMENT;
            if ((pfd.flags & avxFormatFlag_DEPTH) == avxFormatFlag_DEPTH)
                ras->glAttachment = GL_DEPTH_ATTACHMENT;
            else if ((pfd.flags & avxFormatFlag_STENCIL) == avxFormatFlag_STENCIL)
                ras->glAttachment = GL_STENCIL_ATTACHMENT;
            else if (pfd.flags & avxFormatFlag_COLOR)
                ras->glAttachment = GL_COLOR_ATTACHMENT0;

#ifdef RENDERBUFFER_ENABLED
            if (((ras->m.usage & avxRasterUsage_ALL) == avxRasterUsage_DRAW) && // RBO can not be read/written out of a draw pass.
                (ras->m.whd.d == 1) && // RBO can not be layered
                (!(ras->m.flags & avxRasterFlag_MIPMAP))) // RBO can not be subsampled
            {
                if (ras->glTarget == GL_TEXTURE_2D)
                {
                    ras->glTarget = GL_RENDERBUFFER;
                }
            }
#endif
        }

        if (err && _AVX_RAS_CLASS_CONFIG.dtor(ras))
            AfxThrowError();
    }
    return err;
}
