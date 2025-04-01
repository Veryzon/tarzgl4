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

_ZGL afxError DpuBindPipeline(zglDpu* dpu, avxPipeline pip, avxVertexInput vin, afxFlags dynamics)
{
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;

    if (!pip)
    {
        if (dpu->activePip || dpu->activePipGpuHandle)
        {
            dpu->nextPip = NIL;
            dpu->activePip = NIL;
            dpu->activePipGpuHandle = NIL;
            dpu->nextLiga = NIL;
            dpu->activeLiga = NIL;
            gl->UseProgram(0); _ZglThrowErrorOccuried();
            //gl->BindProgramPipeline(0); _ZglThrowErrorOccuried();
            //gl->BindVertexArray(0); _ZglThrowErrorOccuried();
        }
    }
    else
    {
        AFX_ASSERT_OBJECTS(afxFcc_PIP, 1, &pip);

        GLuint glHandle = pip->glHandle[dpu->m.exuIdx];
        zglUpdateFlags devUpdReq = (pip->updFlags & ZGL_UPD_FLAG_DEVICE);
        afxBool applyStates = FALSE;
        afxBool bound = FALSE;

        if (!glHandle || (devUpdReq & ZGL_UPD_FLAG_DEVICE_INST))
        {
            //afxArray code;
            afxChar errStr[1024];
            afxUnit tmpShdGlHandleCnt = 0;
            GLuint tmpShdGlHandles[5];
            //AfxMakeArray(&code, sizeof(afxChar), 2048, NIL, 0);

            afxArray code;
            AfxMakeArray(&code, sizeof(afxByte), 4096, NIL, 0);

            for (afxUnit stageIdx = 0; stageIdx < pip->m.stageCnt; stageIdx++)
            {
                AfxEmptyArray(&code);
                avxShader shd = pip->m.stages[stageIdx].shd;

                afxString4096 tmps;
                AfxMakeString4096(&tmps, NIL);
                AfxFormatString(&tmps.str, "#version %d%d0 // QWADRO (c) 2017 SIGMA FEDERATION \n", AfxMax(ZGL_DEFAULT_CTX_VER_MAJOR, shd->m.verMajor), shd->m.verMajor ? shd->m.verMinor : AfxMax(ZGL_DEFAULT_CTX_VER_MINOR, shd->m.verMinor), shd->m.extended ? " " : " core");

                afxUnit arrel;
                void* room = AfxPushArrayUnits(&code, tmps.str.len, &arrel, NIL);
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
                \n#define IN(_at_, _type_, _name_) layout(location = _at_) in _type_ _name_ \
                \n#define OUT(_at_, _type_, _name_) layout(location = _at_) out _type_ _name_ \
                // enforce early tests \
                \n#define IN_EARLY_FRAGMENT_TESTS() layout(early_fragment_tests) in \
                 \
                \n#define IN_CANVAS_COORD() in vec4 gl_FragCoord \
                \n#define IN_WINDOW_COORD() layout(origin_upper_left) in vec4 gl_FragCoord \
                 \
                // ubo = std140, ssbo = std430 \
                \n#define UNIFORM(_set_, _binding_, _layout_, _name_) layout(_layout_) uniform _name_ \
                \n#define BUFFER(_set_, _binding_, _layout_, _access_, _name_) layout(_layout_) _access_ buffer _name_ \
                \n#define TEXTURE(_set_, _binding_, _type_, _name_) uniform _type_ _name_ \
                \n#define PUSH(_name_) layout(std140, location = 1023) uniform /*_name_*/ pushable \
                \n#define SPECIALIZED(_specid_, _type_, _name_) layout(_specid_) const _type_ _name_ \
                \n#define FETCH(_set_, _binding_, _name_) uniform usamplerBuffer _name_ \
                \n \
                \n#define fetch(_bufName_, _elemIdx_) texelFetch(_bufName_, _elemIdx_) \
                \n \
                 \
                ";

                afxChar const stdIncVs[] =
                    "#extension ARB_shader_draw_parameters : enable\n"
                    "#extension ARB_indirect_parameters : enable\n";

                room = AfxPushArrayUnits(&code, sizeof(stdInc) - 1, &arrel, NIL);
                AfxCopy(room, stdInc, sizeof(stdInc) - 1);

                if (!AfxIsStringEmpty(&pip->m.stages[stageIdx].fn.str))
                {
                    AfxFormatString(&tmps.str, "\n#define main %.*s \n", AfxPushString(&pip->m.stages[stageIdx].fn.str));
                    void* room = AfxPushArrayUnits(&code, tmps.str.len, &arrel, NIL);
                    AfxDumpString(&tmps.str, 0, tmps.str.len, room);
                }

                switch (pip->m.stages[stageIdx].stage)
                {
                case avxShaderStage_VERTEX:
                {
                    AfxFormatString(&tmps.str, "\n#define _VERTEX_ ");
                    void* room = AfxPushArrayUnits(&code, tmps.str.len, &arrel, NIL);
                    AfxDumpString(&tmps.str, 0, tmps.str.len, room);
                    break;
                }
                case avxShaderStage_FRAGMENT:
                {
                    AfxFormatString(&tmps.str, "\n#define _FRAGMENT_ ");
                    void* room = AfxPushArrayUnits(&code, tmps.str.len, &arrel, NIL);
                    AfxDumpString(&tmps.str, 0, tmps.str.len, room);
                    break;
                }
                case avxShaderStage_COMPUTE:
                {
                    AfxFormatString(&tmps.str, "\n#define _COMPUTE_ ");
                    void* room = AfxPushArrayUnits(&code, tmps.str.len, &arrel, NIL);
                    AfxDumpString(&tmps.str, 0, tmps.str.len, room);
                    break;
                }
                case avxShaderStage_PRIMITIVE:
                {
                    AfxFormatString(&tmps.str, "\n#define _PRIMITIVE_ ");
                    void* room = AfxPushArrayUnits(&code, tmps.str.len, &arrel, NIL);
                    AfxDumpString(&tmps.str, 0, tmps.str.len, room);
                    break;
                }
                case avxShaderStage_TESS_EVAL:
                {
                    AfxFormatString(&tmps.str, "\n#define _TESS_EVAL_ ");
                    void* room = AfxPushArrayUnits(&code, tmps.str.len, &arrel, NIL);
                    AfxDumpString(&tmps.str, 0, tmps.str.len, room);
                    break;
                }
                case avxShaderStage_TESS_CTRL:
                {
                    AfxFormatString(&tmps.str, "\n#define _TESS_CTRL_ ");
                    void* room = AfxPushArrayUnits(&code, tmps.str.len, &arrel, NIL);
                    AfxDumpString(&tmps.str, 0, tmps.str.len, room);
                    break;
                }
                default: AfxThrowError(); break;
                }

                afxUnit nullTermArrel;
                AvxDumpShaderCode(pip->m.stages[stageIdx].shd, &code);
                AfxPushArrayUnits(&code, 1, &nullTermArrel, NIL);

                avxShaderStage stage = pip->m.stages[stageIdx].stage;

                GLuint shader;

                if (!(shader = gl->CreateShader(AfxToGlShaderStage(stage))))
                {
                    _ZglThrowErrorOccuried();
                }
                else
                {
                    GLint compiled = 0;

                    if (pip->m.stages[stageIdx].shd->m.tag.len)
                    {
                        gl->ObjectLabel(GL_SHADER, shader, pip->m.stages[stageIdx].shd->m.tag.len, (GLchar const*)pip->m.stages[stageIdx].shd->m.tag.start); _ZglThrowErrorOccuried();
                    }

                    //gl->ShaderSource(shader, 1, (GLchar const*const[]) { (void*)code.bytemap }, (GLint const[]) { code.cnt }); _ZglThrowErrorOccuried();
#if 0
                    AfxReportComment("%.*s", code.cnt, code.bytemap);
#endif
                    gl->ShaderSource(shader, 1, (GLchar const*const[]) { (void*)code.bytemap }, (GLint const[]) { code.pop }); _ZglThrowErrorOccuried();
                    gl->CompileShader(shader); _ZglThrowErrorOccuried();
                    gl->GetShaderiv(shader, GL_COMPILE_STATUS, &compiled); _ZglThrowErrorOccuried();

                    if (compiled == GL_FALSE)
                    {
                        AfxThrowError();
                        gl->GetShaderInfoLog(shader, sizeof(errStr), NIL, (GLchar*)errStr); _ZglThrowErrorOccuried();
                        AfxReportError(errStr);
                        gl->DeleteShader(shader); _ZglThrowErrorOccuried();
                        AfxReportError((afxChar const*)code.bytemap);
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
                        gl->DeleteShader(tmpShdGlHandles[tmpShdGlHandleCnt]); _ZglThrowErrorOccuried();
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
                    gl->DeleteProgram(glHandle), glHandle = NIL; _ZglThrowErrorOccuried();
                }

                if (!(glHandle = gl->CreateProgram()))
                {
                    _ZglThrowErrorOccuried();
                }
                else
                {
                    afxBool linked;

                    if (pip->m.tag.len)
                    {
                        gl->ObjectLabel(GL_PROGRAM, glHandle, pip->m.tag.len, (GLchar const*)pip->m.tag.start); _ZglThrowErrorOccuried();
                    }

                    for (afxUnit i = 0; i < tmpShdGlHandleCnt; i++)
                    {
                        gl->AttachShader(glHandle, tmpShdGlHandles[i]); _ZglThrowErrorOccuried();
                    }

                    gl->LinkProgram(glHandle); _ZglThrowErrorOccuried();
                    gl->GetProgramiv(glHandle, GL_LINK_STATUS, &linked); _ZglThrowErrorOccuried();

                    if (linked == GL_FALSE)
                    {
                        AfxThrowError();
                        gl->GetProgramInfoLog(glHandle, sizeof(errStr), NIL, (GLchar*)errStr); _ZglThrowErrorOccuried();
                        AfxReportError(errStr);
                    }

                    // required bind due to issue with Intel Graphics Drivers no allowing retrieve of uniform locations only after assembling.
                    AFX_ASSERT(gl->IsProgram(glHandle));
                    gl->UseProgram(glHandle); _ZglThrowErrorOccuried();
                    bound = TRUE;

                    //AvxGetPipelineLigature(&pip, &liga);

                    if (_DpuBindAndResolveLiga(dpu, pip->m.liga, glHandle))
                        AfxThrowError();

                    for (afxUnit i = tmpShdGlHandleCnt; i-- > 0;)
                    {
                        gl->DetachShader(glHandle, tmpShdGlHandles[i]); _ZglThrowErrorOccuried();
                    }

                    if (err)
                    {
                        gl->DeleteProgram(glHandle); _ZglThrowErrorOccuried();
                        glHandle = NIL;
                    }
                }

                for (afxUnit i = tmpShdGlHandleCnt; i-- > 0;)
                {
                    gl->DeleteShader(tmpShdGlHandles[i]); _ZglThrowErrorOccuried();
                    tmpShdGlHandles[i] = NIL;
                }

                if (!err)
                {
                    //AfxReportMessage("avxPipeline %p hardware-side data instanced.", pip);
                    pip->updFlags &= ~(ZGL_UPD_FLAG_DEVICE);
                }

                pip->glHandle[dpu->m.exuIdx] = glHandle;
                dpu->nextPip = NIL; // force update in "next first time".
                dpu->activePip = pip;
                dpu->activePipGpuHandle = glHandle;

                dpu->nextLiga = NIL;
                AvxGetPipelineLigature(dpu->activePip, &dpu->activeLiga);

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
                AvxGetPipelineLigature(dpu->activePip, &dpu->activeLiga);

                AFX_ASSERT(gl->IsProgram(glHandle));
                gl->UseProgram(glHandle); _ZglThrowErrorOccuried();
                applyStates = TRUE;
                bound = TRUE;
            }
        }

#if 0
        if (applyStates)
#endif
        {
            avxPipelineInfo config;
            AvxDescribePipeline(pip, &config);

            dpu->nextVpCnt = pip->m.vpCnt;
            dpu->vpCnt = pip->m.vpCnt;

            dpu->nextPrimTop = pip->m.primTop;
            dpu->nextPrimRestartEnabled = pip->m.primRestartEnabled;
            dpu->nextDepthClampEnabled = pip->m.depthClampEnabled;

            if ((dpu->nextCullMode = pip->m.cullMode))
            {
                dpu->nextFrontFaceCw = pip->m.frontFacingInverted;
            }

            // RASTERIZATION

            dpu->nextRasterizationDisabled = pip->m.rasterizationDisabled;

            //if ((dpu->nextOutCnt = config.colorOutCnt))
            if ((dpu->outCnt = config.colorOutCnt))
            {
                AfxCopy2(dpu->nextOuts, config.colorOuts, sizeof(config.colorOuts[0]), config.colorOutCnt);

                AvxCopyColor(dpu->nextBlendConstants, pip->m.blendConstants);
                //dpu->nextBlendConstUpd = 1;
            }

            if ((dpu->nextDepthBiasEnabled = pip->m.depthBiasEnabled))
            {
                dpu->nextDepthBiasClamp = pip->m.depthBiasClamp;
                dpu->nextDepthBiasConstFactor = pip->m.depthBiasConstFactor;
                dpu->nextDepthBiasSlopeScale = pip->m.depthBiasSlopeScale;
            }

            if ((dpu->nextDepthBoundsTestEnabled = pip->m.depthBoundsTestEnabled))
                AfxV2dCopy(dpu->nextDepthBounds, pip->m.depthBounds);

            if ((dpu->nextDepthTestEnabled = pip->m.depthTestEnabled))
            {
                dpu->nextDepthCompareOp = pip->m.depthCompareOp;
            }

            dpu->nextDepthWriteDisabled = pip->m.depthWriteDisabled;

            dpu->nextFillMode = pip->m.fillMode;
            dpu->nextLineWidth = pip->m.lineWidth;

            if ((dpu->nextMinSampleShadingValue = pip->m.minSampleShadingValue))
            {
                dpu->nextSampleShadingEnabled = pip->m.sampleShadingEnabled;
            }

            dpu->nextMsEnabled = pip->m.msEnabled;

            dpu->nextAlphaToCoverageEnabled = pip->m.alphaToCoverageEnabled;
            dpu->nextAlphaToOneEnabled = pip->m.alphaToOneEnabled;

            if ((dpu->nextLogicOpEnabled = pip->m.logicOpEnabled))
            {
                dpu->nextLogicOp = pip->m.logicOp;
            }

            if ((dpu->nextSampleLvl = pip->m.sampleLvl))
            {
                AfxCopy2(dpu->nextSampleMasks, pip->m.sampleMasks, sizeof(pip->m.sampleMasks), pip->m.sampleLvl);
            }

            if ((dpu->nextStencilTestEnabled = pip->m.stencilTestEnabled))
            {
                dpu->nextStencilBack = pip->m.stencilBack;
                dpu->nextStencilFront = pip->m.stencilFront;
            }
        }
    }

    DpuBindVertexInput(dpu, pip && !vin ? pip->m.vin : vin);

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
    gl->GetProgramiv(gpuHandle, GL_ACTIVE_UNIFORM_BLOCKS, &cnt); _ZglThrowErrorOccuried();

    for (afxInt i = 0; i < cnt; i++)
    {
        AFX_ZERO(&res);

        GLint binding, dataSiz, nameLen, subresCnt, subresIndices[1], refByVsh, refByFsh, refByGsh;
        gl->GetActiveUniformBlockiv(gpuHandle, i, GL_UNIFORM_BLOCK_BINDING, &binding);  _ZglThrowErrorOccuried();// The current block binding, as set either within the shader or from gl->UniformBlockBinding.
        gl->GetActiveUniformBlockiv(gpuHandle, i, GL_UNIFORM_BLOCK_DATA_SIZE, &dataSiz);  _ZglThrowErrorOccuried();// The buffer object storage size needed for this block.
        gl->GetActiveUniformBlockiv(gpuHandle, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);  _ZglThrowErrorOccuried();// The length of this block's name.
        gl->GetActiveUniformBlockiv(gpuHandle, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &subresCnt);  _ZglThrowErrorOccuried();// The number of active uniforms within this block.
        //glGetActiveUniformBlockiv(gpuHandle, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, &subresIndices); // params​ will be filled in with the uniform indices of all uniforms that are stored in this block.It will receive GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS number of uniforms.
        gl->GetActiveUniformBlockiv(gpuHandle, i, GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER, &refByVsh);  _ZglThrowErrorOccuried();// GL_FALSE if the uniform block is not referenced by an active * shader, where * is the particular shader stage in question. It canv be VERTEX_SHADER, FRAGMENT_SHADER, or GEOMETRY_SHADER.
        gl->GetActiveUniformBlockiv(gpuHandle, i, GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER, &refByFsh);  _ZglThrowErrorOccuried();// GL_FALSE if the uniform block is not referenced by an active * shader, where * is the particular shader stage in question. It canv be VERTEX_SHADER, FRAGMENT_SHADER, or GEOMETRY_SHADER.
        gl->GetActiveUniformBlockiv(gpuHandle, i, GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER, &refByGsh);  _ZglThrowErrorOccuried();// GL_FALSE if the uniform block is not referenced by an active * shader, where * is the particular shader stage in question. It canv be VERTEX_SHADER, FRAGMENT_SHADER, or GEOMETRY_SHADER.


        res.binding = binding;
        res.resType = avxShaderParam_UNIFORM;
        res.set = 0;
        res.location = i;
        res.arraySize = 0;
        res.vecSize = 0;
        res.inputAttachmentIndex = 0;
        res.size = dataSiz;
        res.stages = 0;
        gl->GetActiveUniformBlockName(gpuHandle, i, sizeof(res.name), NIL, res.name); _ZglThrowErrorOccuried();
        res.name[15] = '\0';
        //res.loc = gl->GetUniformLocation(gpuHandle, res.nameBuf); _ZglThrowErrorOccuried();

        afxResult j = AfxPipeline.RegisterResource(pip, &res);
        AfxPipeline.GetResource(pip, j, &res);
        AFX_ASSERT(i == j);
        AfxReportMessage("%d %d %d %d", res.loc, res.binding, i, j);
    }

    gl->GetProgramiv(gpuHandle, GL_ACTIVE_UNIFORMS, &cnt);

    for (GLuint i = 0; i < (GLuint)cnt; i++)
    {
        AFX_ZERO(&res);
        AfxDeployString(&res.name, res.nameBuf);

        afxBool valid = FALSE;

        GLint type, blockIdx;
        gl->GetActiveUniformsiv(gpuHandle, 1, &i, GL_UNIFORM_TYPE, &type); _ZglThrowErrorOccuried();
        gl->GetActiveUniformsiv(gpuHandle, 1, &i, GL_UNIFORM_BLOCK_INDEX, &blockIdx); _ZglThrowErrorOccuried();

        if (blockIdx == -1)
        {
            switch (type)
            {
            case GL_SAMPLER_1D:
                gl->GetActiveUniformName(gpuHandle, i, sizeof(res.name), NIL, res.name); _ZglThrowErrorOccuried();
                res.resType = avxShaderParam_TEXTURE;
                valid = TRUE;
                break;
            case GL_SAMPLER_2D:
                gl->GetActiveUniformName(gpuHandle, i, sizeof(res.name), NIL, res.name); _ZglThrowErrorOccuried();
                res.resType = avxShaderParam_TEXTURE;
                valid = TRUE;
                break;
            case GL_SAMPLER_3D:
                gl->GetActiveUniformName(gpuHandle, i, sizeof(res.name), NIL, res.name); _ZglThrowErrorOccuried();
                res.resType = avxShaderParam_TEXTURE;
                valid = TRUE;
                break;
            case GL_SAMPLER_CUBE:
                gl->GetActiveUniformName(gpuHandle, i, sizeof(res.name), NIL, res.name); _ZglThrowErrorOccuried();
                res.resType = avxShaderParam_TEXTURE;
                valid = TRUE;
                break;
            default:
                AfxReportError("%X is a unhandled type", type);
                valid = FALSE;
                break;
            }

            if (valid)
            {
                res.location = gl->GetUniformLocation(gpuHandle, res.name); _ZglThrowErrorOccuried();
                //res.loc = i;
                afxSupplyDescriptor entry[1];
                afxResult j = AfxPipeline.FindEntries(pip, 1, &res.name, entry);
                //AfxPipeline.GetResource(pip, j, &res);

                if (j == 1)
                {
                    AFX_ASSERT(i == j);
                    gl->BindUniform(gpuHandle, );
                    AfxReportMessage("%d %d %d %d", res.location, res.binding, i, j);
                }
            }
        }
    }
    return 0;
}
#endif

_ZGL afxError _ZglPipDtor(avxPipeline pip)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_PIP, 1, &pip);

    afxDrawSystem dsys = AfxGetProvider(pip);

    for (afxUnit i = 0; i < ZGL_MAX_PSO_HANDLES; i++)
    {
        if (pip->glHandle[i])
        {
            _ZglDsysEnqueueDeletion(dsys, i, GL_PROGRAM, (afxSize)pip->glHandle[i]);
            pip->glHandle[i] = 0;
        }
    }

    if (_AVX_PIP_CLASS_CONFIG.dtor(pip))
        AfxThrowError();

    return err;
}

_ZGL afxError _ZglPipCtor(avxPipeline pip, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_PIP, 1, &pip);

    if (_AVX_PIP_CLASS_CONFIG.ctor(pip, args, invokeNo)) AfxThrowError();
    else
    {
        afxDrawSystem dsys = args[0];
        avxPipelineBlueprint const *pipb = ((avxPipelineBlueprint const*)args[1]) + invokeNo;
        //AfxAssertType(pipb, afxFcc_PIPB);

        AfxZero(pip->glHandle, sizeof(pip->glHandle));
        pip->updFlags = ZGL_UPD_FLAG_DEVICE_INST;


        pip->pipUniqueId = ++dsys->pipUniqueId;

        if (err && _AVX_PIP_CLASS_CONFIG.dtor(pip))
            AfxThrowError();
    }
    AFX_ASSERT_OBJECTS(afxFcc_PIP, 1, &pip);
    return err;
}
