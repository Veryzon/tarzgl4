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
#include "zglUtils.h"

// OpenGL/Vulkan Continuous Integration

_ZGL afxError _DpuBindPipeline(zglDpu* dpu, avxPipeline pip, avxVertexInput vin, afxFlags dynamics)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;

    if (!pip)
    {
        if (dpu->activePip || dpu->activePipGpuHandle)
        {
            dpu->nextPip = NIL;
            dpu->activePip = NIL;
            dpu->activePipGpuHandle = NIL;
            dpu->nextLiga = NIL;
            dpu->activeLiga = NIL;
            gl->UseProgram(0); _SglThrowErrorOccuried();
            //gl->BindProgramPipeline(0); _SglThrowErrorOccuried();
            //gl->BindVertexArray(0); _SglThrowErrorOccuried();
        }
    }
    else
    {
        AfxAssertObjects(1, &pip, afxFcc_PIP);

        GLuint glHandle = pip->glHandle;
        zglUpdateFlags devUpdReq = (pip->updFlags & ZGL_UPD_FLAG_DEVICE);
        afxBool applyStates = FALSE;
        afxBool bound = FALSE;

        if (!glHandle || (devUpdReq & ZGL_UPD_FLAG_DEVICE_INST))
        {
            //afxArray code;
            afxChar errStr[1024];
            afxUnit tmpShdGlHandleCnt = 0;
            GLuint tmpShdGlHandles[5];
            //AfxMakeArray(&code, 2048, sizeof(afxChar), NIL);

            afxArray code;
            AfxMakeArray(&code, 4096, sizeof(afxByte), NIL);

            for (afxUnit stageIdx = 0; stageIdx < pip->m.stageCnt; stageIdx++)
            {
                AfxEmptyArray(&code);
                avxShader shd = pip->m.stages[stageIdx].shd;

                afxString4096 tmps;
                AfxMakeString4096(&tmps, NIL);
                AfxFormatString(&tmps.str, "#version %d%d0 %s // QWADRO (c) 2017 SIGMA FEDERATION \n", AfxMax(ZGL_DEFAULT_CTX_VER_MAJOR, shd->m.verMajor), shd->m.verMajor ? shd->m.verMinor : AfxMax(ZGL_DEFAULT_CTX_VER_MINOR, shd->m.verMinor), shd->m.extended ? " " : " core");

                afxUnit arrel;
                void* room = AfxInsertArrayUnits(&code, tmps.str.len, &arrel, NIL);
                AfxDumpString(&tmps.str, 0, tmps.str.len, room);

                afxChar const stdInc[] =
                    " \
                \n#define afxInt int \
                \n#define afxUnit uint \
                \n#define afxReal float \
                \n#define afxV2d vec2 \
                \n#define afxV3d vec3 \
                \n#define afxV4d vec4 \
                \n#define afxM2d mat2 \
                \n#define afxM3d mat3 \
                \n#define afxM4d mat4 \
                \n#define float2 vec2 \
                \n#define float3 vec3 \
                \n#define float4 vec4 \
                 \
                \n#define IN(_location_, _type_, _name_) layout(location = _location_) in _type_ _name_ \
                \n#define OUT(_location_, _type_, _name_) layout(location = _location_) out _type_ _name_ \
                // enforce early tests \
                \n#define IN_EARLY_FRAGMENT_TESTS() layout(early_fragment_tests) in \
                 \
                \n#define IN_CANVAS_COORD() in vec4 gl_FragCoord \
                \n#define IN_WINDOW_COORD() layout(origin_upper_left) in vec4 gl_FragCoord \
                 \
                // ubo = std140, ssbo = std430 \
                \n#define UNIFORM_UNIT(_set_, _binding_, _name_) layout(std140) uniform _name_ \
                \n#define STORAGE_UNIT(_set_, _binding_, _access_, _name_) layout(std430) _access_ buffer _name_ \
                \n#define SAMPLING_UNIT(_set_, _binding_, _type_, _name_) uniform _type_ _name_ \
                \n#define TEXTURE(_set_, _binding_, _type_, _name_) uniform sampler##_type_ _name_ \
                \n#define PUSHABLE(_name_) layout(std140, location = 1023) uniform /*_name_*/ pushable \
                \n#define SPECIALIZED(_specid_, _type_, _name_) layout(_specid_) const _type_ _name_ \
                 \
                ";
                room = AfxInsertArrayUnits(&code, sizeof(stdInc) - 1, &arrel, NIL);
                AfxCopy(room, stdInc, sizeof(stdInc) - 1);

                if (!AfxIsStringEmpty(&pip->m.stages[stageIdx].fn.str))
                {
                    AfxFormatString(&tmps.str, "\n#define main %.*s \n", AfxPushString(&pip->m.stages[stageIdx].fn.str));
                    void* room = AfxInsertArrayUnits(&code, tmps.str.len, &arrel, NIL);
                    AfxDumpString(&tmps.str, 0, tmps.str.len, room);
                }

                switch (pip->m.stages[stageIdx].stage)
                {
                case avxShaderStage_VERTEX:
                {
                    AfxFormatString(&tmps.str, "\n#define _VERTEX_ ");
                    void* room = AfxInsertArrayUnits(&code, tmps.str.len, &arrel, NIL);
                    AfxDumpString(&tmps.str, 0, tmps.str.len, room);
                    break;
                }
                case avxShaderStage_FRAGMENT:
                {
                    AfxFormatString(&tmps.str, "\n#define _FRAGMENT_ ");
                    void* room = AfxInsertArrayUnits(&code, tmps.str.len, &arrel, NIL);
                    AfxDumpString(&tmps.str, 0, tmps.str.len, room);
                    break;
                }
                case avxShaderStage_COMPUTE:
                {
                    AfxFormatString(&tmps.str, "\n#define _COMPUTE_ ");
                    void* room = AfxInsertArrayUnits(&code, tmps.str.len, &arrel, NIL);
                    AfxDumpString(&tmps.str, 0, tmps.str.len, room);
                    break;
                }
                case avxShaderStage_PRIMITIVE:
                {
                    AfxFormatString(&tmps.str, "\n#define _PRIMITIVE_ ");
                    void* room = AfxInsertArrayUnits(&code, tmps.str.len, &arrel, NIL);
                    AfxDumpString(&tmps.str, 0, tmps.str.len, room);
                    break;
                }
                case avxShaderStage_TESS_EVAL:
                {
                    AfxFormatString(&tmps.str, "\n#define _TESS_EVAL_ ");
                    void* room = AfxInsertArrayUnits(&code, tmps.str.len, &arrel, NIL);
                    AfxDumpString(&tmps.str, 0, tmps.str.len, room);
                    break;
                }
                case avxShaderStage_TESS_CTRL:
                {
                    AfxFormatString(&tmps.str, "\n#define _TESS_CTRL_ ");
                    void* room = AfxInsertArrayUnits(&code, tmps.str.len, &arrel, NIL);
                    AfxDumpString(&tmps.str, 0, tmps.str.len, room);
                    break;
                }
                default: AfxThrowError(); break;
                }

                afxUnit nullTermArrel;
                AfxDumpShaderCode(pip->m.stages[stageIdx].shd, &code);
                AfxInsertArrayUnits(&code, 1, &nullTermArrel, NIL);

                avxShaderStage stage = pip->m.stages[stageIdx].stage;

                GLuint shader;

                if (!(shader = gl->CreateShader(AfxToGlShaderStage(stage))))
                {
                    _SglThrowErrorOccuried();
                }
                else
                {
                    GLint compiled = 0;
                    //gl->ShaderSource(shader, 1, (GLchar const*const[]) { (void*)code.bytemap }, (GLint const[]) { code.cnt }); _SglThrowErrorOccuried();
#if 0
                    AfxLogComment("%.*s", code.cnt, code.bytemap);
#endif
                    gl->ShaderSource(shader, 1, (GLchar const*const[]) { (void*)code.bytemap }, (GLint const[]) { code.pop }); _SglThrowErrorOccuried();
                    gl->CompileShader(shader); _SglThrowErrorOccuried();
                    gl->GetShaderiv(shader, GL_COMPILE_STATUS, &compiled); _SglThrowErrorOccuried();

                    if (compiled == GL_FALSE)
                    {
                        AfxThrowError();
                        gl->GetShaderInfoLog(shader, sizeof(errStr), NIL, (GLchar*)errStr); _SglThrowErrorOccuried();
                        AfxLogError(errStr);
                        gl->DeleteShader(shader); _SglThrowErrorOccuried();
                    }
                    else
                    {
                        tmpShdGlHandles[tmpShdGlHandleCnt] = shader;
                        tmpShdGlHandleCnt++;
                    }
                }

                if (err)
                {
                    for (afxUnit i = tmpShdGlHandleCnt; i-- > 0;)
                    {
                        gl->DeleteShader(tmpShdGlHandles[tmpShdGlHandleCnt]); _SglThrowErrorOccuried();
                        tmpShdGlHandles[tmpShdGlHandleCnt] = NIL;
                    }
                    break;
                }
            }

            AfxCleanUpArray(&code);

            if (!err)
            {
                if (glHandle)
                {
                    gl->DeleteProgram(glHandle), glHandle = NIL; _SglThrowErrorOccuried();
                }

                if (!(glHandle = gl->CreateProgram()))
                {
                    _SglThrowErrorOccuried();
                }
                else
                {
                    afxBool linked;

                    for (afxUnit i = 0; i < tmpShdGlHandleCnt; i++)
                    {
                        gl->AttachShader(glHandle, tmpShdGlHandles[i]); _SglThrowErrorOccuried();
                    }

                    gl->LinkProgram(glHandle); _SglThrowErrorOccuried();
                    gl->GetProgramiv(glHandle, GL_LINK_STATUS, &linked); _SglThrowErrorOccuried();

                    if (linked == GL_FALSE)
                    {
                        AfxThrowError();
                        gl->GetProgramInfoLog(glHandle, sizeof(errStr), NIL, (GLchar*)errStr); _SglThrowErrorOccuried();
                        AfxLogError(errStr);
                    }

                    // required bind due to issue with Intel Graphics Drivers no allowing retrieve of uniform locations only after assembling.
                    AfxAssert(gl->IsProgram(glHandle));
                    gl->UseProgram(glHandle); _SglThrowErrorOccuried();
                    bound = TRUE;

                    //AfxGetPipelineLigature(&pip, &liga);

                    if (_DpuBindAndResolveLiga(dpu, pip->m.liga, glHandle))
                        AfxThrowError();

                    for (afxUnit i = tmpShdGlHandleCnt; i-- > 0;)
                    {
                        gl->DetachShader(glHandle, tmpShdGlHandles[i]); _SglThrowErrorOccuried();
                    }

                    if (err)
                    {
                        gl->DeleteProgram(glHandle); _SglThrowErrorOccuried();
                        glHandle = NIL;
                    }
                }

                for (afxUnit i = tmpShdGlHandleCnt; i-- > 0;)
                {
                    gl->DeleteShader(tmpShdGlHandles[i]); _SglThrowErrorOccuried();
                    tmpShdGlHandles[i] = NIL;
                }

                if (!err)
                {
                    AfxLogEcho("avxPipeline %p hardware-side data instanced.", pip);
                    pip->updFlags &= ~(ZGL_UPD_FLAG_DEVICE);
                }

                pip->glHandle = glHandle;
                dpu->nextPip = NIL; // force update in "next first time".
                dpu->activePip = pip;
                dpu->activePipGpuHandle = glHandle;

                dpu->nextLiga = NIL;
                AfxGetPipelineLigature(dpu->activePip, &dpu->activeLiga);

                applyStates = TRUE;
            }
        }
        else
        {
            if ((dpu->activePip != pip) ||
                (dpu->activePipGpuHandle != glHandle)
                )
            {
                dpu->nextPip = NIL; // force update in "next first time".
                dpu->activePip = pip;
                dpu->activePipGpuHandle = glHandle;

                dpu->nextLiga = NIL;
                AfxGetPipelineLigature(dpu->activePip, &dpu->activeLiga);

                AfxAssert(gl->IsProgram(glHandle));
                gl->UseProgram(glHandle); _SglThrowErrorOccuried();
                applyStates = TRUE;
                bound = TRUE;
            }
        }

        if (applyStates)
        {
            avxPipelineInfo config;
            AfxDescribePipeline(pip, &config);

            dpu->nextTs.primTop = pip->m.primTop;
            dpu->nextTs.primRestartEnabled = pip->m.primRestartEnabled;
            dpu->nextTs.depthClampEnabled = pip->m.depthClampEnabled;

            if ((dpu->nextTs.cullMode = pip->m.cullMode))
            {
                dpu->nextTs.cwFrontFacing = pip->m.frontFacingInverted;
            }

            // RASTERIZATION

            dpu->nextRs.rasterizationDisabled = FALSE;

            if ((dpu->nextRs.outCnt = config.colorOutCnt))
            {
                AfxCopy2(dpu->nextRs.outs, config.colorOuts, sizeof(config.colorOuts[0]), config.colorOutCnt);

                AfxColorCopy(dpu->nextRs.blendConstants, config.blendConstants);
                //dpu->nextBlendConstUpd = 1;
            }

            if ((dpu->nextRs.depthBiasEnabled = config.depthBiasEnabled))
            {
                dpu->nextRs.depthBiasClamp = config.depthBiasClamp;
                dpu->nextRs.depthBiasConstFactor = config.depthBiasConstFactor;
                dpu->nextRs.depthBiasSlopeScale = config.depthBiasSlopeScale;
            }

            if ((dpu->nextRs.depthBoundsTestEnabled = config.depthBoundsTestEnabled))
                AfxV2dCopy(dpu->nextRs.depthBounds, config.depthBounds);

            if ((dpu->nextRs.depthTestEnabled = config.depthTestEnabled))
            {
                dpu->nextRs.depthCompareOp = config.depthCompareOp;
            }

            dpu->nextRs.depthWriteDisabled = config.depthWriteDisabled;

            dpu->nextRs.fillMode = config.fillMode;
            dpu->nextRs.lineWidth = config.lineWidth;

            if ((dpu->nextRs.minSampleShadingValue = config.minSampleShadingValue))
            {
                dpu->nextRs.sampleShadingEnabled = config.sampleShadingEnabled;
            }

            dpu->nextRs.msEnabled = config.msEnabled;

            dpu->nextRs.alphaToCoverageEnabled = config.alphaToCoverageEnabled;
            dpu->nextRs.alphaToOneEnabled = config.alphaToOneEnabled;

            if ((dpu->nextRs.logicOpEnabled = config.pixelLogicOpEnabled))
            {
                dpu->nextRs.logicOp = config.pixelLogicOp;
            }

            if ((dpu->nextRs.sampleCnt = config.sampleCnt))
            {
                AfxCopy2(dpu->nextRs.sampleMasks, config.sampleMasks, sizeof(config.sampleMasks), config.sampleCnt);
            }

            if ((dpu->nextRs.stencilTestEnabled = config.stencilTestEnabled))
            {
                dpu->nextRs.stencilBack = config.stencilBack;
                dpu->nextRs.stencilFront = config.stencilFront;
            }
        }
    }

    _DpuBindVertexInput(dpu, pip && !vin ? pip->m.vin : vin);

    return err;
}

#if 0
_ZGL afxResult _AfxRegisterOpenGlResourcesToQwadroDrawPipeline(avxPipeline pip)
{
    glVmt* gl = &(((afxDrawSystem)AfxGetProvider(&pip->obj))->vmt);
#if 0
    avxShaderStage                      stages;
    avxShaderParam              type;
    avxPipelineSubresType           baseType;
    afxUnit32                            set,
        binding,
        loc,
        inputAttachIdx,
        vecSiz,
        columns,
        arrSiz,
        off,
        siz;
    afxEmbeddedString(name, 16);
    avxPipelineSubresource const    *subresources;
#endif
    _DrawPipelineResource res, *resptr;
    GLuint gpuHandle = pip->gpuHandle;

    afxInt cnt = 0;
    gl->GetProgramiv(gpuHandle, GL_ACTIVE_UNIFORM_BLOCKS, &cnt); _SglThrowErrorOccuried();

    for (afxInt i = 0; i < cnt; i++)
    {
        AFX_ZERO(&res);

        GLint binding, dataSiz, nameLen, subresCnt, subresIndices[1], refByVsh, refByFsh, refByGsh;
        gl->GetActiveUniformBlockiv(gpuHandle, i, GL_UNIFORM_BLOCK_BINDING, &binding);  _SglThrowErrorOccuried();// The current block binding, as set either within the shader or from gl->UniformBlockBinding.
        gl->GetActiveUniformBlockiv(gpuHandle, i, GL_UNIFORM_BLOCK_DATA_SIZE, &dataSiz);  _SglThrowErrorOccuried();// The buffer object storage size needed for this block.
        gl->GetActiveUniformBlockiv(gpuHandle, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);  _SglThrowErrorOccuried();// The length of this block's name.
        gl->GetActiveUniformBlockiv(gpuHandle, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &subresCnt);  _SglThrowErrorOccuried();// The number of active uniforms within this block.
        //glGetActiveUniformBlockiv(gpuHandle, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, &subresIndices); // params​ will be filled in with the uniform indices of all uniforms that are stored in this block.It will receive GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS number of uniforms.
        gl->GetActiveUniformBlockiv(gpuHandle, i, GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER, &refByVsh);  _SglThrowErrorOccuried();// GL_FALSE if the uniform block is not referenced by an active * shader, where * is the particular shader stage in question. It canv be VERTEX_SHADER, FRAGMENT_SHADER, or GEOMETRY_SHADER.
        gl->GetActiveUniformBlockiv(gpuHandle, i, GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER, &refByFsh);  _SglThrowErrorOccuried();// GL_FALSE if the uniform block is not referenced by an active * shader, where * is the particular shader stage in question. It canv be VERTEX_SHADER, FRAGMENT_SHADER, or GEOMETRY_SHADER.
        gl->GetActiveUniformBlockiv(gpuHandle, i, GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER, &refByGsh);  _SglThrowErrorOccuried();// GL_FALSE if the uniform block is not referenced by an active * shader, where * is the particular shader stage in question. It canv be VERTEX_SHADER, FRAGMENT_SHADER, or GEOMETRY_SHADER.


        res.binding = binding;
        res.resType = avxShaderParam_UNIFORM_UNIT;
        res.set = 0;
        res.location = i;
        res.arraySize = 0;
        res.vecSize = 0;
        res.inputAttachmentIndex = 0;
        res.size = dataSiz;
        res.stages = 0;
        gl->GetActiveUniformBlockName(gpuHandle, i, sizeof(res.name), NIL, res.name); _SglThrowErrorOccuried();
        res.name[15] = '\0';
        //res.loc = gl->GetUniformLocation(gpuHandle, res.nameBuf); _SglThrowErrorOccuried();

        afxResult j = AfxPipeline.RegisterResource(pip, &res);
        AfxPipeline.GetResource(pip, j, &res);
        AfxAssert(i == j);
        AfxLogEcho("%d %d %d %d", res.loc, res.binding, i, j);
    }

    gl->GetProgramiv(gpuHandle, GL_ACTIVE_UNIFORMS, &cnt);

    for (GLuint i = 0; i < (GLuint)cnt; i++)
    {
        AFX_ZERO(&res);
        AfxDeployString(&res.name, res.nameBuf);

        afxBool valid = FALSE;

        GLint type, blockIdx;
        gl->GetActiveUniformsiv(gpuHandle, 1, &i, GL_UNIFORM_TYPE, &type); _SglThrowErrorOccuried();
        gl->GetActiveUniformsiv(gpuHandle, 1, &i, GL_UNIFORM_BLOCK_INDEX, &blockIdx); _SglThrowErrorOccuried();

        if (blockIdx == -1)
        {
            switch (type)
            {
            case GL_SAMPLER_1D:
                gl->GetActiveUniformName(gpuHandle, i, sizeof(res.name), NIL, res.name); _SglThrowErrorOccuried();
                res.resType = avxShaderParam_COMBINED_IMAGE_SAMPLER;
                valid = TRUE;
                break;
            case GL_SAMPLER_2D:
                gl->GetActiveUniformName(gpuHandle, i, sizeof(res.name), NIL, res.name); _SglThrowErrorOccuried();
                res.resType = avxShaderParam_COMBINED_IMAGE_SAMPLER;
                valid = TRUE;
                break;
            case GL_SAMPLER_3D:
                gl->GetActiveUniformName(gpuHandle, i, sizeof(res.name), NIL, res.name); _SglThrowErrorOccuried();
                res.resType = avxShaderParam_COMBINED_IMAGE_SAMPLER;
                valid = TRUE;
                break;
            case GL_SAMPLER_CUBE:
                gl->GetActiveUniformName(gpuHandle, i, sizeof(res.name), NIL, res.name); _SglThrowErrorOccuried();
                res.resType = avxShaderParam_COMBINED_IMAGE_SAMPLER;
                valid = TRUE;
                break;
            default:
                AfxLogError("%X is a unhandled type", type);
                valid = FALSE;
                break;
            }

            if (valid)
            {
                res.location = gl->GetUniformLocation(gpuHandle, res.name); _SglThrowErrorOccuried();
                //res.loc = i;
                afxSupplyDescriptor entry[1];
                afxResult j = AfxPipeline.FindEntries(pip, 1, &res.name, entry);
                //AfxPipeline.GetResource(pip, j, &res);

                if (j == 1)
                {
                    AfxAssert(i == j);
                    gl->BindUniform(gpuHandle, );
                    AfxLogEcho("%d %d %d %d", res.location, res.binding, i, j);
                }
            }
        }
    }
    return 0;
}
#endif

_ZGL afxError _SglPipDtor(avxPipeline pip)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &pip, afxFcc_PIP);

    afxDrawContext dctx = AfxGetProvider(pip);

    if (pip->glHandle)
    {
        _SglDctxEnqueueDeletion(dctx, 0, GL_PROGRAM, (afxSize)pip->glHandle);
        pip->glHandle = 0;
    }

    if (_AvxPipStdImplementation.dtor(pip))
        AfxThrowError();

    return err;
}

_ZGL afxError _SglPipCtor(avxPipeline pip, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &pip, afxFcc_PIP);

    if (_AvxPipStdImplementation.ctor(pip, args, invokeNo)) AfxThrowError();
    else
    {
        afxDrawContext dctx = args[0];
        avxPipelineBlueprint const *pipb = ((avxPipelineBlueprint const*)args[1]) + invokeNo;
        //AfxAssertType(pipb, afxFcc_PIPB);

        pip->glHandle = 0;
        pip->updFlags = ZGL_UPD_FLAG_DEVICE_INST;

        if (err && _AvxPipStdImplementation.dtor(pip))
            AfxThrowError();
    }
    AfxAssertObjects(1, &pip, afxFcc_PIP);
    return err;
}
