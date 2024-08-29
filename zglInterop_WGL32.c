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

// Eben'ezer


#include "zglCommands.h"
#include "zglObjects.h"
#include "zglInterop_WGL32.h"
#include "qwadro/exec/afxSystem.h"
//#pragma comment(lib, "opengl32")

_ZGL afxString const sgl2Signature = AFX_STRING(
    "           :::     :::::::::   ::::::::  :::              :::  ::::::::        \n"
    "          :+:      :+:    :+: :+:    :+: :+:             :+:  :+:    :+:       \n"
    "         +:+ +:+   +:+    +:+ +:+        +:+            +:+         +:+        \n"
    "        +#+  +:+   +#+    +:+ :#:        +#+           +#+        +#+          \n"
    "       +#+#+#+#+#+ +#+    +#+ +#+   +#+# +#+          +#+       +#+            \n"
    "             #+#   #+#    #+# #+#    #+# #+#         #+#       #+#             \n"
    "             ###   #########   ########  ########## ###       ##########       \n"
    "                                                                               \n"
    "              Q W A D R O   E X E C U T I O N   E C O S Y S T E M              \n"
    "                                                                               \n"
    "                               Public Test Build                               \n"
    "                           (c) 2017 SIGMA FEDERATION                           \n"
    "                                www.sigmaco.org                                \n"
    "                                                                               \n"
);

_ZGL const char *glVmtNames[] =
{
    // 100
    "glCullFace", // 0
    "glFrontFace",
    "glHint",
    "glLineWidth",
    "glPointSize",
    "glPolygonMode",
    "glScissor",
    "glTexParameterf",
    "glTexParameterfv",
    "glTexParameteri",
    "glTexParameteriv",
    "glTexImage1D",
    "glTexImage2D",
    "glDrawBuffer",
    "glClear",
    "glClearColor",
    "glClearStencil",
    "glClearDepth",
    "glStencilMask",
    "glColorMask",
    "glDepthMask",
    "glDisable",
    "glEnable",
    "glFinish",
    "glFlush",
    "glBlendFunc",
    "glLogicOp",
    "glStencilFunc",
    "glStencilOp",
    "glDepthFunc",
    "glPixelStoref",
    "glPixelStorei",
    "glReadBuffer",
    "glReadPixels",
    "glGetBooleanv",
    "glGetDoublev",
    "glGetError",
    "glGetFloatv",
    "glGetIntegerv",
    "glGetString",
    "glGetTexImage",
    "glGetTexParameterfv",
    "glGetTexParameteriv",
    "glGetTexLevelParameterfv",
    "glGetTexLevelParameteriv",
    "glIsEnabled",
    "glDepthRange",
    "glViewport", // 47
    // 110
    "glDrawArrays", // 0
    "glDrawElements",
    "glGetPointerv",
    "glPolygonOffset",
    "glCopyTexImage1D",
    "glCopyTexImage2D",
    "glCopyTexSubImage1D",
    "glCopyTexSubImage2D",
    "glTexSubImage1D",
    "glTexSubImage2D",
    "glBindTexture",
    "glDeleteTextures",
    "glGenTextures",
    "glIsTexture", // 13
    // 120
    "glDrawRangeElements", // 0
    "glTexImage3D",
    "glTexSubImage3D",
    "glCopyTexSubImage3D", // 3
    // 130
    "glActiveTexture", // 0
    "glSampleCoverage",
    "glCompressedTexImage3D",
    "glCompressedTexImage2D",
    "glCompressedTexImage1D",
    "glCompressedTexSubImage3D",
    "glCompressedTexSubImage2D",
    "glCompressedTexSubImage1D",
    "glGetCompressedTexImage", // 8
    // 140
    "glBlendFuncSeparate", // 0
    "glMultiDrawArrays",
    "glMultiDrawElements",
    "glPointParameterf",
    "glPointParameterfv",
    "glPointParameteri",
    "glPointParameteriv",
    "glBlendColor",
    "glBlendEquation", // 8
    // 150
    "glGenQueries", // 0
    "glDeleteQueries",
    "glIsQuery",
    "glBeginQuery",
    "glEndQuery",
    "glGetQueryiv",
    "glGetQueryObjectiv",
    "glGetQueryObjectuiv",
    "glBindBuffer",
    "glDeleteBuffers",
    "glGenBuffers",
    "glIsBuffer",
    "glBufferData",
    "glBufferSubData",
    "glGetBufferSubData",
    "glMapBuffer",
    "glUnmapBuffer",
    "glGetBufferParameteriv",
    "glGetBufferPointerv", // 18
    // 200
    "glBlendEquationSeparate", // 0
    "glDrawBuffers",
    "glStencilOpSeparate",
    "glStencilFuncSeparate",
    "glStencilMaskSeparate",
    "glAttachShader",
    "glBindAttribLocation",
    "glCompileShader",
    "glCreateProgram",
    "glCreateShader",
    "glDeleteProgram",
    "glDeleteShader",
    "glDetachShader",
    "glDisableVertexAttribArray",
    "glEnableVertexAttribArray",
    "glGetActiveAttrib",
    "glGetActiveUniform",
    "glGetAttachedShaders",
    "glGetAttribLocation",
    "glGetProgramiv",
    "glGetProgramInfoLog",
    "glGetShaderiv",
    "glGetShaderInfoLog",
    "glGetShaderSource",
    "glGetUniformLocation",
    "glGetUniformfv",
    "glGetUniformiv",
    "glGetVertexAttribdv",
    "glGetVertexAttribfv",
    "glGetVertexAttribiv",
    "glGetVertexAttribPointerv",
    "glIsProgram",
    "glIsShader",
    "glLinkProgram",
    "glShaderSource",
    "glUseProgram",
    "glUniform1f",
    "glUniform2f",
    "glUniform3f",
    "glUniform4f",
    "glUniform1i",
    "glUniform2i",
    "glUniform3i",
    "glUniform4i",
    "glUniform1fv",
    "glUniform2fv",
    "glUniform3fv",
    "glUniform4fv",
    "glUniform1iv",
    "glUniform2iv",
    "glUniform3iv",
    "glUniform4iv",
    "glUniformMatrix2fv",
    "glUniformMatrix3fv",
    "glUniformMatrix4fv",
    "glValidateProgram",
    "glVertexAttrib1d",
    "glVertexAttrib1dv",
    "glVertexAttrib1f",
    "glVertexAttrib1fv",
    "glVertexAttrib1s",
    "glVertexAttrib1sv",
    "glVertexAttrib2d",
    "glVertexAttrib2dv",
    "glVertexAttrib2f",
    "glVertexAttrib2fv",
    "glVertexAttrib2s",
    "glVertexAttrib2sv",
    "glVertexAttrib3d",
    "glVertexAttrib3dv",
    "glVertexAttrib3f",
    "glVertexAttrib3fv",
    "glVertexAttrib3s",
    "glVertexAttrib3sv",
    "glVertexAttrib4Nbv",
    "glVertexAttrib4Niv",
    "glVertexAttrib4Nsv",
    "glVertexAttrib4Nub",
    "glVertexAttrib4Nubv",
    "glVertexAttrib4Nuiv",
    "glVertexAttrib4Nusv",
    "glVertexAttrib4bv",
    "glVertexAttrib4d",
    "glVertexAttrib4dv",
    "glVertexAttrib4f",
    "glVertexAttrib4fv",
    "glVertexAttrib4iv",
    "glVertexAttrib4s",
    "glVertexAttrib4sv",
    "glVertexAttrib4ubv",
    "glVertexAttrib4uiv",
    "glVertexAttrib4usv",
    "glVertexAttribPointer", // 92
    // 210
    "glUniformMatrix2x3fv", // 0
    "glUniformMatrix3x2fv",
    "glUniformMatrix2x4fv",
    "glUniformMatrix4x2fv",
    "glUniformMatrix3x4fv",
    "glUniformMatrix4x3fv", // 5
    // 300
    "glColorMaski", // 0
    "glGetBooleani_v",
    "glGetIntegeri_v",
    "glEnablei",
    "glDisablei",
    "glIsEnabledi",
    "glBeginTransformFeedback",
    "glEndTransformFeedback",
    "glBindBufferRange",
    "glBindBufferBase",
    "glTransformFeedbackVaryings",
    "glGetTransformFeedbackVarying",
    "glClampColor",
    "glBeginConditionalRender",
    "glEndConditionalRender",
    "glVertexAttribIPointer",
    "glGetVertexAttribIiv",
    "glGetVertexAttribIuiv",
    "glVertexAttribI1i",
    "glVertexAttribI2i",
    "glVertexAttribI3i",
    "glVertexAttribI4i",
    "glVertexAttribI1ui",
    "glVertexAttribI2ui",
    "glVertexAttribI3ui",
    "glVertexAttribI4ui",
    "glVertexAttribI1iv",
    "glVertexAttribI2iv",
    "glVertexAttribI3iv",
    "glVertexAttribI4iv",
    "glVertexAttribI1uiv",
    "glVertexAttribI2uiv",
    "glVertexAttribI3uiv",
    "glVertexAttribI4uiv",
    "glVertexAttribI4bv",
    "glVertexAttribI4sv",
    "glVertexAttribI4ubv",
    "glVertexAttribI4usv",
    "glGetUniformuiv",
    "glBindFragDataLocation",
    "glGetFragDataLocation",
    "glUniform1ui",
    "glUniform2ui",
    "glUniform3ui",
    "glUniform4ui",
    "glUniform1uiv",
    "glUniform2uiv",
    "glUniform3uiv",
    "glUniform4uiv",
    "glTexParameterIiv",
    "glTexParameterIuiv",
    "glGetTexParameterIiv",
    "glGetTexParameterIuiv",
    "glClearBufferiv",
    "glClearBufferuiv",
    "glClearBufferfv",
    "glClearBufferfi",
    "glGetStringi",
    "glIsRenderbuffer",
    "glBindRenderbuffer",
    "glDeleteRenderbuffers",
    "glGenRenderbuffers",
    "glRenderbufferStorage",
    "glGetRenderbufferParameteriv",
    "glIsFramebuffer",
    "glBindFramebuffer",
    "glDeleteFramebuffers",
    "glGenFramebuffers",
    "glCheckFramebufferStatus",
    "glFramebufferTexture1D",
    "glFramebufferTexture2D",
    "glFramebufferTexture3D",
    "glFramebufferRenderbuffer",
    "glGetFramebufferAttachmentParameteriv",
    "glGenerateMipmap",
    "glBlitFramebuffer",
    "glRenderbufferStorageMultisample",
    "glFramebufferTextureLayer",
    "glMapBufferRange",
    "glFlushMappedBufferRange",
    "glBindVertexArray",
    "glDeleteVertexArrays",
    "glGenVertexArrays",
    "glIsVertexArray", // 83
    // 310
    "glDrawArraysInstanced", // 0
    "glDrawElementsInstanced",
    "glTexBuffer",
    "glPrimitiveRestartIndex",
    "glCopyBufferSubData",
    "glGetUniformIndices",
    "glGetActiveUniformsiv",
    "glGetActiveUniformName",
    "glGetUniformBlockIndex",
    "glGetActiveUniformBlockiv",
    "glGetActiveUniformBlockName",
    "glUniformBlockBinding", // 11
    // 320
    "glDrawElementsBaseVertex", // 0
    "glDrawRangeElementsBaseVertex",
    "glDrawElementsInstancedBaseVertex",
    "glMultiDrawElementsBaseVertex",
    "glProvokingVertex",
    "glFenceSync",
    "glIsSync",
    "glDeleteSync",
    "glClientWaitSync",
    "glWaitSync",
    "glGetInteger64v",
    "glGetSynciv",
    "glGetInteger64i_v",
    "glGetBufferParameteri64v",
    "glFramebufferTexture",
    "glTexImage2DMultisample",
    "glTexImage3DMultisample",
    "glGetMultisamplefv",
    "glSampleMaski", // 18
    // 330
    "glBindFragDataLocationIndexed", // 0
    "glGetFragDataIndex",
    "glGenSamplers",
    "glDeleteSamplers",
    "glIsSampler",
    "glBindSampler",
    "glSamplerParameteri",
    "glSamplerParameteriv",
    "glSamplerParameterf",
    "glSamplerParameterfv",
    "glSamplerParameterIiv",
    "glSamplerParameterIuiv",
    "glGetSamplerParameteriv",
    "glGetSamplerParameterIiv",
    "glGetSamplerParameterfv",
    "glGetSamplerParameterIuiv",
    "glQueryCounter",
    "glGetQueryObjecti64v",
    "glGetQueryObjectui64v",
    "glVertexAttribDivisor",
    "glVertexAttribP1ui",
    "glVertexAttribP1uiv",
    "glVertexAttribP2ui",
    "glVertexAttribP2uiv",
    "glVertexAttribP3ui",
    "glVertexAttribP3uiv",
    "glVertexAttribP4ui",
    "glVertexAttribP4uiv", // 27
    // 400
    "glMinSampleShading", // 0
    "glBlendEquationi",
    "glBlendEquationSeparatei",
    "glBlendFunci",
    "glBlendFuncSeparatei",
    "glDrawArraysIndirect",
    "glDrawElementsIndirect",
    "glUniform1d",
    "glUniform2d",
    "glUniform3d",
    "glUniform4d",
    "glUniform1dv",
    "glUniform2dv",
    "glUniform3dv",
    "glUniform4dv",
    "glUniformMatrix2dv",
    "glUniformMatrix3dv",
    "glUniformMatrix4dv",
    "glUniformMatrix2x3dv",
    "glUniformMatrix2x4dv",
    "glUniformMatrix3x2dv",
    "glUniformMatrix3x4dv",
    "glUniformMatrix4x2dv",
    "glUniformMatrix4x3dv",
    "glGetUniformdv",
    "glGetSubroutineUniformLocation",
    "glGetSubroutineIndex",
    "glGetActiveSubroutineUniformiv",
    "glGetActiveSubroutineUniformName",
    "glGetActiveSubroutineName",
    "glUniformSubroutinesuiv",
    "glGetUniformSubroutineuiv",
    "glGetProgramStageiv",
    "glPatchParameteri",
    "glPatchParameterfv",
    "glBindTransformFeedback",
    "glDeleteTransformFeedbacks",
    "glGenTransformFeedbacks",
    "glIsTransformFeedback",
    "glPauseTransformFeedback",
    "glResumeTransformFeedback",
    "glDrawTransformFeedback",
    "glDrawTransformFeedbackStream",
    "glBeginQueryIndexed",
    "glEndQueryIndexed",
    "glGetQueryIndexediv", // 45
    // 410
    "glReleaseShaderCompiler", // 0
    "glShaderBinary",
    "glGetShaderPrecisionFormat",
    "glDepthRangef",
    "glClearDepthf",
    "glGetProgramBinary",
    "glProgramBinary",
    "glProgramParameteri",
    "glUseProgramStages",
    "glActiveShaderProgram",
    "glCreateShaderProgramv",
    "glBindProgramPipeline",
    "glDeleteProgramPipelines",
    "glGenProgramPipelines",
    "glIsProgramPipeline",
    "glGetProgramPipelineiv",
    "glProgramUniform1i",
    "glProgramUniform1iv",
    "glProgramUniform1f",
    "glProgramUniform1fv",
    "glProgramUniform1d",
    "glProgramUniform1dv",
    "glProgramUniform1ui",
    "glProgramUniform1uiv",
    "glProgramUniform2i",
    "glProgramUniform2iv",
    "glProgramUniform2f",
    "glProgramUniform2fv",
    "glProgramUniform2d",
    "glProgramUniform2dv",
    "glProgramUniform2ui",
    "glProgramUniform2uiv",
    "glProgramUniform3i",
    "glProgramUniform3iv",
    "glProgramUniform3f",
    "glProgramUniform3fv",
    "glProgramUniform3d",
    "glProgramUniform3dv",
    "glProgramUniform3ui",
    "glProgramUniform3uiv",
    "glProgramUniform4i",
    "glProgramUniform4iv",
    "glProgramUniform4f",
    "glProgramUniform4fv",
    "glProgramUniform4d",
    "glProgramUniform4dv",
    "glProgramUniform4ui",
    "glProgramUniform4uiv",
    "glProgramUniformMatrix2fv",
    "glProgramUniformMatrix3fv",
    "glProgramUniformMatrix4fv",
    "glProgramUniformMatrix2dv",
    "glProgramUniformMatrix3dv",
    "glProgramUniformMatrix4dv",
    "glProgramUniformMatrix2x3fv",
    "glProgramUniformMatrix3x2fv",
    "glProgramUniformMatrix2x4fv",
    "glProgramUniformMatrix4x2fv",
    "glProgramUniformMatrix3x4fv",
    "glProgramUniformMatrix4x3fv",
    "glProgramUniformMatrix2x3dv",
    "glProgramUniformMatrix3x2dv",
    "glProgramUniformMatrix2x4dv",
    "glProgramUniformMatrix4x2dv",
    "glProgramUniformMatrix3x4dv",
    "glProgramUniformMatrix4x3dv",
    "glValidateProgramPipeline",
    "glGetProgramPipelineInfoLog",
    "glVertexAttribL1d",
    "glVertexAttribL2d",
    "glVertexAttribL3d",
    "glVertexAttribL4d",
    "glVertexAttribL1dv",
    "glVertexAttribL2dv",
    "glVertexAttribL3dv",
    "glVertexAttribL4dv",
    "glVertexAttribLPointer",
    "glGetVertexAttribLdv",
    "glViewportArrayv",
    "glViewportIndexedf",
    "glViewportIndexedfv",
    "glScissorArrayv",
    "glScissorIndexed",
    "glScissorIndexedv",
    "glDepthRangeArrayv",
    "glDepthRangeIndexed",
    "glGetFloati_v",
    "glGetDoublei_v", // 87
    // 420
    "glDrawArraysInstancedBaseInstance", // 0
    "glDrawElementsInstancedBaseInstance",
    "glDrawElementsInstancedBaseVertexBaseInstance",
    "glGetInternalformativ",
    "glGetActiveAtomicCounterBufferiv",
    "glBindImageTexture",
    "glMemoryBarrier",
    "glTexStorage1D",
    "glTexStorage2D",
    "glTexStorage3D",
    "glDrawTransformFeedbackInstanced",
    "glDrawTransformFeedbackStreamInstanced", // 11
    // 430
    "glClearBufferData", // 0
    "glClearBufferSubData",
    "glDispatchCompute",
    "glDispatchComputeIndirect",
    "glCopyImageSubData",
    "glFramebufferParameteri",
    "glGetFramebufferParameteriv",
    "glGetInternalformati64v",
    "glInvalidateTexSubImage",
    "glInvalidateTexImage",
    "glInvalidateBufferSubData",
    "glInvalidateBufferData",
    "glInvalidateFramebuffer",
    "glInvalidateSubFramebuffer",
    "glMultiDrawArraysIndirect",
    "glMultiDrawElementsIndirect",
    "glGetProgramInterfaceiv",
    "glGetProgramResourceIndex",
    "glGetProgramResourceName",
    "glGetProgramResourceiv",
    "glGetProgramResourceLocation",
    "glGetProgramResourceLocationIndex",
    "glShaderStorageBlockBinding",
    "glTexBufferRange",
    "glTexStorage2DMultisample",
    "glTexStorage3DMultisample",
    "glTextureView",
    "glBindVertexBuffer",
    "glVertexAttribFormat",
    "glVertexAttribIFormat",
    "glVertexAttribLFormat",
    "glVertexAttribBinding",
    "glVertexBindingDivisor",
    "glDebugMessageControl",
    "glDebugMessageInsert",
    "glDebugMessageCallback",
    "glGetDebugMessageLog",
    "glPushDebugGroup",
    "glPopDebugGroup",
    "glObjectLabel",
    "glGetObjectLabel",
    "glObjectPtrLabel",
    "glGetObjectPtrLabel", // 42
    // 440
    "glBufferStorage", // 0
    "glClearTexImage",
    "glClearTexSubImage",
    "glBindBuffersBase",
    "glBindBuffersRange",
    "glBindTextures",
    "glBindSamplers",
    "glBindImageTextures",
    "glBindVertexBuffers", // 8
    // 450
    "glClipControl", // 0
    "glCreateTransformFeedbacks",
    "glTransformFeedbackBufferBase",
    "glTransformFeedbackBufferRange",
    "glGetTransformFeedbackiv",
    "glGetTransformFeedbacki_v",
    "glGetTransformFeedbacki64_v",
    "glCreateBuffers",
    "glNamedBufferStorage",
    "glNamedBufferData",
    "glNamedBufferSubData",
    "glCopyNamedBufferSubData",
    "glClearNamedBufferData",
    "glClearNamedBufferSubData",
    "glMapNamedBuffer",
    "glMapNamedBufferRange",
    "glUnmapNamedBuffer",
    "glFlushMappedNamedBufferRange",
    "glGetNamedBufferParameteriv",
    "glGetNamedBufferParameteri64v",
    "glGetNamedBufferPointerv",
    "glGetNamedBufferSubData",
    "glCreateFramebuffers",
    "glNamedFramebufferRenderbuffer",
    "glNamedFramebufferParameteri",
    "glNamedFramebufferTexture",
    "glNamedFramebufferTextureLayer",
    "glNamedFramebufferCanvas",
    "glNamedFramebufferCanvass",
    "glNamedFramebufferReadBuffer",
    "glInvalidateNamedFramebufferData",
    "glInvalidateNamedFramebufferSubData",
    "glClearNamedFramebufferiv",
    "glClearNamedFramebufferuiv",
    "glClearNamedFramebufferfv",
    "glClearNamedFramebufferfi",
    "glBlitNamedFramebuffer",
    "glCheckNamedFramebufferStatus",
    "glGetNamedFramebufferParameteriv",
    "glGetNamedFramebufferAttachmentParameteriv",
    "glCreateRenderbuffers",
    "glNamedRenderbufferStorage",
    "glNamedRenderbufferStorageMultisample",
    "glGetNamedRenderbufferParameteriv",
    "glCreateTextures",
    "glTextureBuffer",
    "glTextureBufferRange",
    "glTextureStorage1D",
    "glTextureStorage2D",
    "glTextureStorage3D",
    "glTextureStorage2DMultisample",
    "glTextureStorage3DMultisample",
    "glTextureSubImage1D",
    "glTextureSubImage2D",
    "glTextureSubImage3D",
    "glCompressedTextureSubImage1D",
    "glCompressedTextureSubImage2D",
    "glCompressedTextureSubImage3D",
    "glCopyTextureSubImage1D",
    "glCopyTextureSubImage2D",
    "glCopyTextureSubImage3D",
    "glTextureParameterf",
    "glTextureParameterfv",
    "glTextureParameteri",
    "glTextureParameterIiv",
    "glTextureParameterIuiv",
    "glTextureParameteriv",
    "glGenerateTextureMipmap",
    "glBindTextureUnit",
    "glGetTextureImage",
    "glGetCompressedTextureImage",
    "glGetTextureLevelParameterfv",
    "glGetTextureLevelParameteriv",
    "glGetTextureParameterfv",
    "glGetTextureParameterIiv",
    "glGetTextureParameterIuiv",
    "glGetTextureParameteriv",
    "glCreateVertexArrays",
    "glDisableVertexArrayAttrib",
    "glEnableVertexArrayAttrib",
    "glVertexArrayElementBuffer",
    "glVertexArrayVertexBuffer",
    "glVertexArrayVertexBuffers",
    "glVertexArrayAttribBinding",
    "glVertexArrayAttribFormat",
    "glVertexArrayAttribIFormat",
    "glVertexArrayAttribLFormat",
    "glVertexArrayBindingDivisor",
    "glGetVertexArrayiv",
    "glGetVertexArrayIndexediv",
    "glGetVertexArrayIndexed64iv",
    "glCreateSamplers",
    "glCreateProgramPipelines",
    "glCreateQueries",
    "glGetQueryBufferObjecti64v",
    "glGetQueryBufferObjectiv",
    "glGetQueryBufferObjectui64v",
    "glGetQueryBufferObjectuiv",
    "glMemoryBarrierByRegion",
    "glGetTextureSubImage",
    "glGetCompressedTextureSubImage",
    "glGetGraphicsResetStatus",
    "glGetnCompressedTexImage",
    "glGetnTexImage",
    "glGetnUniformdv",
    "glGetnUniformfv",
    "glGetnUniformiv",
    "glGetnUniformuiv",
    "glReadnPixels",
    "glTextureBarrier", // 109
    // 460
    "glSpecializeShader", // 0
    "glMultiDrawArraysIndirectCount",
    "glMultiDrawElementsIndirectCount",
    "glPolygonOffsetClamp" // 3
};

_ZGL afxError __stdcall wglLoadSymbolsSIG(HMODULE opengl32, afxNat base, afxNat cnt, void* vmt[], afxBool echo)
{
    afxError err = AFX_ERR_NONE;
    afxString128 tmp;
    AfxMakeString128(&tmp, 0);
    afxString name;
    afxString arbAndExt[2];
    AfxMakeString(&arbAndExt[0], 0, "ARB", 3);
    AfxMakeString(&arbAndExt[1], 0, "EXT", 3);
    
    char const **names = glVmtNames;

    AfxAssert(names);
    AfxAssert(vmt);
    AfxAssert(opengl32);
    PROC(WINAPI *f)(LPCSTR) = (void*)GetProcAddress(opengl32, "wglGetProcAddress");
    PROC(WINAPI *f2)(LPCSTR) = NIL;

    if (!f)
        f = (void*)GetProcAddress(opengl32, "DrvGetProcAddress");
    else
        if ((f2 = (void*)f("wglGetProcAddress")))
            f = f2;

    afxNat baseAddonIdx = offsetof(glVmt, ReleaseShaderCompiler) / sizeof(void*);
    
    if (baseAddonIdx >= cnt)
        baseAddonIdx = 0;

    for (afxNat i = 0; i < cnt; i++)
    {
        AfxAssert(names[base + i]);
        AfxMakeString(&name, 0, names[base + i], 0);
        void *sym = NIL;

        if (f)
        {
            if ((sym = f((void*)name.start)))
            {
                if (i >= baseAddonIdx && echo)
                {
                    AfxLogEcho("%.3u %.*s core found.", i, AfxPushString(&name));
                }
            }
            else
            {
                for (afxNat j = 0; j < 2; j++)
                {
                    AfxFormatString(&tmp.str, "%.*s%.*s", AfxPushString(&name), AfxPushString(&arbAndExt[j]));

                    if ((sym = f((void*)tmp.buf)))
                    {
                        if (i >= baseAddonIdx && echo)
                        {
                            AfxLogY("%.3u %.*s ext found.", i, AfxPushString(&tmp.str));
                        }
                        break;
                    }
                    else if ((sym = GetProcAddress(opengl32, (void*)tmp.buf)))
                    {
                        if (i >= baseAddonIdx && echo)
                        {
                            AfxLogY("%.3u %.*s fallback found.", i, AfxPushString(&tmp.str));
                        }
                        break;
                    }
                }
            }
        }

        if (!sym && (sym = GetProcAddress(opengl32, (void*)name.start)))
        {
            if (i >= baseAddonIdx && echo)
            {
                AfxLogEcho("%.3u %.*s fallback found.", i, AfxPushString(&name));
            }
        }

        vmt[i] = sym;
    }
    return err;
}

wglVmt wgl;

_ZGL PROC __stdcall wglGetProcAddressSIG(HMODULE opengl32, LPCSTR lpProcName)
{
    afxError err = NIL;
    PROC f;

    if (/*(!wgl.GetProcAddress) || */!(f = wgl.GetProcAddress(lpProcName)))
        f = GetProcAddress(opengl32, lpProcName);

    return f;
}

_ZGL afxBool __stdcall wglHasExtensionSIG(HDC hDc, afxChar const* ext)
{
    const char *extensions;

    if (wgl.GetExtensionsStringEXT == NULL && wgl.GetExtensionsStringARB == NULL)
        return 0;

    if (wgl.GetExtensionsStringARB == NULL || hDc == INVALID_HANDLE_VALUE)
        extensions = wgl.GetExtensionsStringEXT();
    else
        extensions = wgl.GetExtensionsStringARB(hDc);

    if (extensions == NULL || ext == NULL)
        return 0;

    char const *s, *t;

    while (1)
    {
        if (!(s = strstr(extensions, ext)))
            break;

        t = s + strlen(ext);

        if ((s == extensions || *(s - 1) == ' ') && (*t == ' ' || *t == '\0'))
            return TRUE;

        extensions = t;
    }
    return FALSE;
}

HMODULE GetModuleHandleFromFunctionAddress(void* funcAddress)
{
    MEMORY_BASIC_INFORMATION memInfo;
    if (VirtualQuery(funcAddress, &memInfo, sizeof(memInfo)) == 0)
    {
        return NIL;
    }
    return (HMODULE)memInfo.BaseAddress;
}

#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

typedef struct
{
    const char *dli_fname;  // Pathname of shared object that contains address
    void       *dli_fbase;  // Base address at which shared object is loaded
    const char *dli_sname;  // Name of symbol whose definition overlaps addr
    void       *dli_saddr;  // Exact address of symbol named in dli_sname
} Dl_info;

HMODULE GetSymbolInfoFromAddress(void* addr, Dl_info* info)
{
    HANDLE process = GetCurrentProcess();
    DWORD64 address = (DWORD64)(addr);

    // Initialize the symbol handler for the process
    SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME);
    if (!SymInitialize(process, NULL, TRUE))
    {
        return 0;
    }

    // Get symbol information
    SYMBOL_INFO* symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + 256);
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = 255;

    if (SymFromAddr(process, address, NULL, symbol))
    {
        info->dli_fbase = (void*)symbol->ModBase;
        info->dli_saddr = (void*)symbol->Address;
        info->dli_sname = (void*)symbol->Name;
        return (void*)symbol->ModBase;
    }
    else
    {
        return 0;
        // err
    }

    free(symbol);
    SymCleanup(process);
    return 0;
}

_ZGL LRESULT WINAPI _SglTmpWndPrcCbW32(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    afxError err = AFX_ERR_NONE;
    //afxObject obj = (afxObject)(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    if (message == WM_SHOWWINDOW)
    {
        // Sent to a window when the window is about to be hidden or shown.
        return 0; // If an application processes this message, it should return zero.
    }
    return DefWindowProcA(hWnd, message, wParam, lParam);
}

_ZGL void __stdcall wglLoadWsiSymbolsSIG(HMODULE opengl32, afxNat* verMajor, afxNat* verMinor, afxNat* verPatch)
{
    afxError err = NIL;
    wgl.GetProcAddress = (void*)GetProcAddress(opengl32, "wglGetProcAddress");

    wgl.CreateContext = (void*)GetProcAddress(opengl32, "wglCreateContext");
    wgl.DeleteContext = (void*)GetProcAddress(opengl32, "wglDeleteContext");
    wgl.GetCurrentContext = (void*)GetProcAddress(opengl32, "wglGetCurrentContext");
    wgl.GetCurrentDC = (void*)GetProcAddress(opengl32, "wglGetCurrentDC");
    wgl.MakeCurrent = (void*)GetProcAddress(opengl32, "wglMakeCurrent");
    wgl.ChoosePixelFormat = (void*)GetProcAddress(opengl32, "wglChoosePixelFormat"); // "wglChoosePixelFormat" funciona com Intel mas não com AMD.
    wgl.SetPixelFormat = (void*)GetProcAddress(opengl32, "wglSetPixelFormat"); // "wglSetPixelFormat" funciona com Intel mas não com AMD.
    wgl.DescribePixelFormat = (void*)GetProcAddress(opengl32, "wglDescribePixelFormat");    
    wgl.GetPixelFormat = (void*)GetProcAddress(opengl32, "wglGetPixelFormat");
    wgl.SwapBuffers = (void*)GetProcAddress(opengl32, "wglSwapBuffers");
    wgl.SwapLayerBuffers = (void*)GetProcAddress(opengl32, "wglSwapLayerBuffers");

    HDC bkpHdc = wgl.GetCurrentDC();
    HGLRC bkpHglrc = wgl.GetCurrentContext();
    static afxBool loaded = FALSE;

    HDC tmpHdc = NIL;
    HWND tmpHwnd = NIL;
    HGLRC tmpHrc = NIL;
    WNDCLASSEXA wndClss;

    if (!loaded)
    {
        loaded = TRUE;
        wgl.ChoosePixelFormatARB = NIL;
        wgl.CreateContextAttribsARB = NIL;
        wgl.MakeContextCurrentARB = NIL;

        AfxZero(&wndClss, sizeof(wndClss));
        wndClss.cbSize = sizeof(wndClss); // only on EX
        wndClss.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wndClss.lpfnWndProc = _SglTmpWndPrcCbW32;
        wndClss.cbClsExtra = 0;
        wndClss.cbWndExtra = 0;
        wndClss.hInstance = GetModuleHandleA(NULL);
        wndClss.hIcon = LoadIconA(NULL, IDI_SHIELD);
        wndClss.hCursor = LoadCursorA(NULL, IDC_ARROW);
        wndClss.hbrBackground = NULL;
        wndClss.lpszMenuName = NULL;
        wndClss.lpszClassName = "wglLoadWsiSymbolsSIG";
        wndClss.hIconSm = LoadIconA(NULL, IDI_SHIELD);

        if (!(RegisterClassExA(&wndClss))) AfxThrowError();
        else
        {
            DWORD pfdFlags = PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL
#if 0
                | PFD_SWAP_EXCHANGE
#endif
#if !0
                | PFD_DIRECT3D_ACCELERATED
#endif
#if 0
                | PFD_SUPPORT_COMPOSITION
#endif
                ;
            DWORD dwExStyle = WS_EX_APPWINDOW;
            DWORD wndStyles = WS_POPUP;

            if (!(tmpHwnd = CreateWindowExA(dwExStyle, wndClss.lpszClassName, "", wndStyles | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1, NIL, NIL, wndClss.hInstance, NIL))) AfxThrowError();
            else
            {
                if (!(tmpHdc = GetDC(tmpHwnd))) AfxThrowError();
                else
                {
                    PIXELFORMATDESCRIPTOR pfd = { 0 };
                    pfd.nSize = sizeof(pfd);
                    pfd.nVersion = 1;
                    pfd.dwFlags = pfdFlags;
                    pfd.iPixelType = PFD_TYPE_RGBA;
                    pfd.cColorBits = GetDeviceCaps(tmpHdc, BITSPIXEL);
                    pfd.cAlphaBits = 8;
                    pfd.cDepthBits = 24;
                    pfd.cStencilBits = 8;
                    int pxlfmt;

                    if (!(pxlfmt = wglChoosePixelFormatSIG(tmpHdc, &(pfd))))
                        AfxThrowError();

                    if (!(wglDescribePixelFormatSIG(tmpHdc, pxlfmt, sizeof(pfd), &pfd)))
                        AfxThrowError();

                    if (!wglSetPixelFormatSIG(tmpHdc, pxlfmt, tmpHwnd, &(pfd)))
                        AfxThrowError();

                    if (!(tmpHrc = wgl.CreateContext(tmpHdc))) AfxThrowError();
                    else
                    {
                        if (!(wgl.MakeCurrent(tmpHdc, tmpHrc))) AfxThrowError();
                        else
                        {
                            afxString ver;
                            afxNat vMajor, vMinor, vPatch;
                            PFNGLGETSTRINGPROC _glGetString = (void*)wgl.GetProcAddress("glGetString");
                            AfxMakeString(&ver, 0, (afxChar const*)_glGetString(GL_VERSION), 0);
                            AfxScanString(&ver, "%u.%u.%u", &vMajor, &vMinor, &vPatch);

                            if (verMajor)
                                *verMajor = vMajor;

                            if (verMinor)
                                *verMinor = vMinor;

                            if (verPatch)
                                *verPatch = vPatch;
                        }

                        if (err)
                            wgl.DeleteContext(tmpHrc), tmpHrc = NIL;
                    }

                    if (err)
                        ReleaseDC(tmpHwnd, tmpHdc), tmpHdc = NIL;
                }

                if (err)
                    DestroyWindow(tmpHwnd), tmpHwnd = NIL;
            }

            if (err)
                UnregisterClassA(wndClss.lpszClassName, wndClss.hInstance), AfxZero(&wndClss, sizeof(wndClss));
        }
    }

    // ARB/EXT

    wgl.GetPixelFormatAttribivARB = (void*)wgl.GetProcAddress("wglGetPixelFormatAttribivARB");
    wgl.GetPixelFormatAttribfvARB = (void*)wgl.GetProcAddress("wglGetPixelFormatAttribfvARB");
    wgl.ChoosePixelFormatARB = (void*)wgl.GetProcAddress("wglChoosePixelFormatARB");
    
    wgl.GetCurrentReadDCARB = (void*)wgl.GetProcAddress("wglGetCurrentReadDCARB");
    wgl.MakeContextCurrentARB = (void*)wgl.GetProcAddress("wglMakeContextCurrentARB");
    wgl.CreateContextAttribsARB = (void*)wgl.GetProcAddress("wglCreateContextAttribsARB");
    
    wgl.GetExtensionsStringARB = (void*)wgl.GetProcAddress("wglGetExtensionsStringARB");
    wgl.GetExtensionsStringEXT = (void*)wgl.GetProcAddress("wglGetExtensionsStringEXT");

    wgl.SwapIntervalEXT = (void*)wgl.GetProcAddress("wglSwapIntervalEXT");

    wgl.DXOpenDeviceNV = (void*)wgl.GetProcAddress("wglDXOpenDeviceNV");
    wgl.DXCloseDeviceNV = (void*)wgl.GetProcAddress("wglDXCloseDeviceNV");
    wgl.DXRegisterObjectNV = (void*)wgl.GetProcAddress("wglDXRegisterObjectNV");
    wgl.DXUnregisterObjectNV = (void*)wgl.GetProcAddress("wglDXUnregisterObjectNV");
    wgl.DXLockObjectsNV = (void*)wgl.GetProcAddress("wglDXLockObjectsNV");
    wgl.DXUnlockObjectsNV = (void*)wgl.GetProcAddress("wglDXUnlockObjectsNV");

    // DRV internals
#if 0
    Dl_info nfo = { 0 };
    HMODULE mod = GetModuleHandleFromFunctionAddress(&wgl.CreateContextAttribsARB);
    afxChar buf[256];
    GetModuleFileNameA(mod, buf, sizeof(buf));
    mod = LoadLibraryA("ig7icd32");
#else
    HMODULE mod = opengl32;
#endif

    wgl.DrvCopyContext = (void*)wglGetProcAddressSIG(mod, "DrvCopyContext");
    wgl.DrvCreateContext = (void*)wglGetProcAddressSIG(mod, "DrvCreateContext");
    wgl.DrvCreateLayerContext = (void*)wglGetProcAddressSIG(mod, "DrvCreateLayerContext");
    wgl.DrvDeleteContext = (void*)wglGetProcAddressSIG(mod, "DrvDeleteContext");
    wgl.DrvDescribePixelFormat = (void*)wglGetProcAddressSIG(mod, "DrvDescribePixelFormat");
    wgl.DrvGetProcAddress = (void*)wglGetProcAddressSIG(mod, "DrvGetProcAddress");
    wgl.DrvReleaseContext = (void*)wglGetProcAddressSIG(mod, "DrvReleaseContext");
    wgl.DrvSetContext = (void*)wglGetProcAddressSIG(mod, "DrvSetContext");
    wgl.DrvSetPixelFormat = (void*)GetProcAddress(mod, "DrvSetPixelFormat");
    wgl.DrvSwapBuffers = (void*)GetProcAddress(mod, "DrvSwapBuffers");
    wgl.DrvPresentBuffers = (void*)wglGetProcAddressSIG(mod, "DrvPresentBuffers");

    int a = 0;

    if (((tmpHrc || tmpHdc || tmpHwnd)))
    {
        wgl.MakeCurrent(NIL, NIL);
        wgl.DeleteContext(tmpHrc), tmpHrc = NIL;
        ReleaseDC(tmpHwnd, tmpHdc), tmpHdc = NIL;
        DestroyWindow(tmpHwnd), tmpHwnd = NIL;
        UnregisterClassA(wndClss.lpszClassName, wndClss.hInstance), AfxZero(&wndClss, sizeof(wndClss));
    }
    wgl.MakeCurrent(bkpHdc, bkpHglrc);
}

_ZGL BOOL __stdcall wglChooseBestPixelFormatSIG(HDC hDC, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats)
{
    int numFormats;
    int bestFormat = 0;
    int bestMatchScore = -1;

    // Retrieve the number of pixel formats
    int maxPixelFormats;

    if (wgl.GetPixelFormatAttribivARB(hDC, 0, 0, 1, (int[]) { WGL_NUMBER_PIXEL_FORMATS_ARB }, &maxPixelFormats) != TRUE)
    {
        return 0;
    }

    //WGL_NUMBER_PIXEL_FORMATS_ARB, // The number of pixel formats for the device context.The <iLayerPlane> and <iPixelFormat> parameters are ignored if this attribute is specified.

    int intAttrCnt = 0;
    int fltAttrCnt = 0;

    for (int j = 0; piAttribIList && piAttribIList[2 * j]; j++)
        intAttrCnt++;

    for (int j = 0; pfAttribFList && pfAttribFList[2 * j]; j++)
        fltAttrCnt++;

#if 0
    int attrFmtFetchAttrs[] =
    {
        WGL_DRAW_TO_WINDOW_ARB, // True if the pixel format can be used with a window.The <iLayerPlane> parameter is ignored if this attribute is specified.
        WGL_DRAW_TO_BITMAP_ARB, // True if the pixel format can be used with a memory bitmap.The <iLayerPlane> parameter is ignored if this attribute is specified.
        WGL_ACCELERATION_ARB, // Indicates whether the pixel format is supported by the driver. If this is set to WGL_NO_ACCELERATION_ARB then only the software renderer supports this pixel format; if this is set to WGL_GENERIC_ACCELERATION_ARB then the pixel format is supported by an MCD driver; if this is set to WGL_FULL_ACCELERATION_ARB then the pixel format is supported by an ICD driver.
        WGL_NEED_PALETTE_ARB, // A logical palette is required to achieve the best results for this pixel format.The <iLayerPlane> parameter is ignored if this attribute is specified.
        WGL_NEED_SYSTEM_PALETTE_ARB, // The hardware supports one hardware palette in 256-color mode only.The <iLayerPlane> parameter is ignored if this attribute is specified.
        WGL_SWAP_LAYER_BUFFERS_ARB, // True if the pixel format supports swapping layer planes independently of the main planes.If the pixel format does not support a back buffer then this is set to FALSE.The <iLayerPlane> parameter is ignored if this attribute is specified.
        WGL_SWAP_METHOD_ARB, // If the pixel format supports a back buffer, then this indicates how they are swapped.If this attribute is set to WGL_SWAP_EXCHANGE_ARB then swapping exchanges the front and back buffer contents; if it is set to WGL_SWAP_COPY_ARB then swapping copies the back buffer contents to the front buffer; if it is set to WGL_SWAP_UNDEFINED_ARB then the back buffer contents are copied to the front buffer but the back buffer contents are undefined after the operation.If the pixel format does not support a back buffer then this parameter is set to WGL_SWAP_UNDEFINED_ARB.The <iLayerPlane> parameter is ignored if this attribute is specified.
        WGL_NUMBER_OVERLAYS_ARB, // The number of overlay planes.The <iLayerPlane> parameter is ignored if this attribute is specified.
        WGL_NUMBER_UNDERLAYS_ARB, // The number of underlay planes.The <iLayerPlane> parameter is ignored if this attribute is specified.
        WGL_TRANSPARENT_ARB, // True if transparency is supported.
        WGL_TRANSPARENT_RED_VALUE_ARB, // Specifies the transparent red color value.Typically this value is the same for all layer planes.This value is undefined if transparency is not supported.
        WGL_TRANSPARENT_GREEN_VALUE_ARB, // Specifies the transparent green value.Typically this value is the same for all layer planes.This value is undefined if transparency is not supported.
        WGL_TRANSPARENT_BLUE_VALUE_ARB, // Specifies the transparent blue color value.Typically this value is the same for all layer planes.This value is undefined if transparency is not supported.
        WGL_TRANSPARENT_ALPHA_VALUE_ARB, // Specifies the transparent alpha value.This is reserved for future use.
        WGL_TRANSPARENT_INDEX_VALUE_ARB, // Specifies the transparent color index value.Typically this value is the same for all layer planes.This value is undefined if transparency is not supported.
        WGL_SHARE_DEPTH_ARB, // True if the layer plane shares the depth buffer with the main planes.If <iLayerPlane> is zero, this is always true.
        WGL_SHARE_STENCIL_ARB, // True if the layer plane shares the stencil buffer with the main planes.If <iLayerPlane> is zero, this is always true.
        WGL_SHARE_ACCUM_ARB, // True if the layer plane shares the accumulation buffer with the main planes.If <iLayerPlane> is zero, this is always true.
        WGL_SUPPORT_GDI_ARB, // True if GDI rendering is supported.
        WGL_SUPPORT_OPENGL_ARB, // True if OpenGL is supported.
        WGL_DOUBLE_BUFFER_ARB, // True if the color buffer has back / front pairs.
        WGL_STEREO_ARB, // True if the color buffer has left / right pairs.
        WGL_PIXEL_TYPE_ARB, // The type of pixel data.This can be set to WGL_TYPE_RGBA_ARB or WGL_TYPE_COLORINDEX_ARB
        WGL_COLOR_BITS_ARB, // The number of color bitplanes in each color buffer.For RGBA pixel types, it is the size of the color buffer, excluding the alpha bitplanes.For color - index pixels, it is the size of the color index buffer.
        WGL_RED_BITS_ARB, // The number of red bitplanes in each RGBA color buffer.
        WGL_RED_SHIFT_ARB, // The shift count for red bitplanes in each RGBA color buffer.
        WGL_GREEN_BITS_ARB, // The number of green bitplanes in each RGBA color buffer.
        WGL_GREEN_SHIFT_ARB, // The shift count for green bitplanes in each RGBA color buffer.
        WGL_BLUE_BITS_ARB, // The number of blue bitplanes in each RGBA color buffer.
        WGL_BLUE_SHIFT_ARB, // The shift count for blue bitplanes in each RGBA color buffer.
        WGL_ALPHA_BITS_ARB, // The number of alpha bitplanes in each RGBA color buffer.
        WGL_ALPHA_SHIFT_ARB, // The shift count for alpha bitplanes in each RGBA color buffer.
        WGL_ACCUM_BITS_ARB, // The total number of bitplanes in the accumulation buffer.
        WGL_ACCUM_RED_BITS_ARB, // The number of red bitplanes in the accumulation buffer.
        WGL_ACCUM_GREEN_BITS_ARB, // The number of green bitplanes in the accumulation buffer.
        WGL_ACCUM_BLUE_BITS_ARB, // The number of blue bitplanes in the accumulation buffer.
        WGL_ACCUM_ALPHA_BITS_ARB, // The number of alpha bitplanes in the accumulation buffer.
        WGL_DEPTH_BITS_ARB, // The depth of the depth(z - axis) buffer.
        WGL_STENCIL_BITS_ARB, // The depth of the stencil buffer.
        WGL_AUX_BUFFERS_ARB, // The number of auxiliary buffers.
    };

    for (int i = 1; i < maxPixelFormats; i++)
    {
        int score = 0;

        for (int k = 0; k < AFX_COUNTOF(attrFmtFetchAttrs); k++)
        {
            afxBool found = FALSE;

            int valuei = 0;
            wgl->GetPixelFormatAttribivARB(hDC, i, 0, 1, &attrFmtFetchAttrs[k], &valuei);

            for (int j = 0; j < intAttrCnt; j++)
            {
                if (piAttribIList[2 * j] == attrFmtFetchAttrs[i])
                {
                    found = TRUE;

                    int valueb = piAttribIList[2 * j + 1];

                    if (valueb == valuei)
                        score += 10;
                }
            }

            if (!found)
            {
                if (valuei == 0)
                    score += 1;
            }
        }

        if (score > bestMatchScore)
        {
            bestMatchScore = score;
            bestFormat = i;
        }
    }
#else
    for (int i = 1; i < maxPixelFormats; i++)
    {
        int score = 0;

        for (int j = 0; j < intAttrCnt; j++)
        {
            int valuei = 0;
            wgl.GetPixelFormatAttribivARB(hDC, i, 0, 1, &piAttribIList[2 * j], &valuei);

            int valueb = piAttribIList[2 * j + 1];

            if (valueb == valuei)
                score += 2;
            //else if ((valueb && valueb < valuei) || (!valueb && valuei > valueb))
              //  score += 1;
        }

        for (int j = 0; j < fltAttrCnt; j++)
        {
            FLOAT valuef = 0;
            wgl.GetPixelFormatAttribfvARB(hDC, i, 0, 1, (int*)&pfAttribFList[2 * j], &valuef);

            float valuec = pfAttribFList[2 * j + 1];

            if (valuec == valuef)
                score += 2;
            //else if (valuec && valuec < valuef)
                //score += 1;
        }

        if (score > bestMatchScore)
        {
            bestMatchScore = score;
            bestFormat = i;
        }
    }
#endif
    *piFormats = bestFormat;
    *nNumFormats = !!bestFormat;

    return !!bestFormat;
}

_ZGL HDC __stdcall wglGetCurrentDrawDCSIG(void)
{
    afxError err = NIL;    
    return wgl.GetCurrentDC();
}

_ZGL HDC __stdcall wglGetCurrentReadDCSIG(void)
{
    afxError err = NIL;
    HDC rdc;
    
    if ((!wgl.GetCurrentReadDCARB) || !(rdc = wgl.GetCurrentReadDCARB()))
        rdc = wgl.GetCurrentDC();

    return rdc;
}

_ZGL HGLRC __stdcall wglGetCurrentContextSIG(void)
{
    return wgl.GetCurrentContext();
}

_ZGL BOOL __stdcall wglMakeContextCurrentSIG(HDC hDrawDC, HDC hReadDC, HGLRC hglrc)
{
    afxError err = NIL;
    BOOL rslt;

    //if (1 || /*(hDrawDC == hReadDC) ||*/ (!wgl.MakeContextCurrentARB) || !(rslt = wgl.MakeContextCurrentARB(hDrawDC, hReadDC, hglrc)))
        rslt = wgl.MakeCurrent(hDrawDC, hglrc);

    return rslt;
}

_ZGL HGLRC __stdcall wglCreateContextAttribsSIG(HDC hDC, HGLRC hShareContext, int const attribList[])
{
    afxError err = AFX_ERR_NONE;
    HGLRC rc;

    if (((!attribList) && (!hShareContext)) || (!wgl.CreateContextAttribsARB) || !(rc = wgl.CreateContextAttribsARB(hDC, hShareContext, attribList)))
#ifndef _AVOID_DRV_FUNCS
        if ((!wgl.DrvCreateContext) || !(rc = (HGLRC)wgl.DrvCreateContext(hDC)))
#endif
            rc = wgl.CreateContext(hDC);

    return rc;
}

_ZGL BOOL __stdcall wglDeleteContextSIG(HGLRC hGlrc)
{
    afxError err = AFX_ERR_NONE;
    BOOL rslt = FALSE;

    if (hGlrc == wglGetCurrentContextSIG())
        wglMakeContextCurrentSIG(NIL, NIL, NIL);

#ifndef _AVOID_DRV_FUNCS
    if ((!wgl.DrvDeleteContext) || !(rslt = wgl.DrvDeleteContext(hGlrc)))
#endif
        rslt = wgl.DeleteContext(hGlrc);

    return rslt;
}

_ZGL BOOL __stdcall wglSwapBuffersSIG(HDC hDC, HWND hWnd)
{
    afxError err = AFX_ERR_NONE;
    BOOL rslt = FALSE;
#if 0
#ifndef _AVOID_DRV_FUNCS
    if ((!wgl.DrvSwapBuffers) || !(rslt = wgl.DrvSwapBuffers(hDC, hWnd)))
#endif
#endif
#ifndef _AVOID_WGL_FUNCS
        if ((!wgl.SwapBuffers) || !(rslt = wgl.SwapBuffers(hDC)))
#endif
            rslt = SwapBuffers(hDC);

    return rslt;
}

_ZGL BOOL __stdcall wglPresentBuffersSIG(HDC hDC, int a2)
{
    afxError err = AFX_ERR_NONE;
    BOOL rslt = FALSE;

    // a2 is appearently a HGLRC

#ifndef _AVOID_DRV_FUNCS
    if ((!wgl.DrvPresentBuffers) || !(rslt = wgl.DrvPresentBuffers(hDC, a2)))
#endif
        0;

    return rslt;
}

_ZGL int __stdcall wglChoosePixelFormatSIG(HDC hDC, CONST PIXELFORMATDESCRIPTOR* ppfd)
{
    afxError err = AFX_ERR_NONE;
    int fmt;

#ifndef _AVOID_WGL_FUNCS
    if ((!wgl.ChoosePixelFormat) || !(fmt = wgl.ChoosePixelFormat(hDC, ppfd)))
#endif
        fmt = ChoosePixelFormat(hDC, ppfd);

    return fmt;
}

_ZGL BOOL __stdcall wglSetPixelFormatSIG(HDC hDC, int format, HWND hWnd, CONST PIXELFORMATDESCRIPTOR* ppfd)
{
    afxError err = AFX_ERR_NONE;
    BOOL rslt = FALSE;

#ifndef _AVOID_DRV_FUNCS
    if ((!wgl.DrvSetPixelFormat) || !(rslt = wgl.DrvSetPixelFormat(hDC, format, hWnd)))
#endif
#ifndef _AVOID_WGL_FUNCS
        if ((!wgl.SetPixelFormat) || !(rslt = wgl.SetPixelFormat(hDC, format, ppfd)))
#endif
            rslt = SetPixelFormat(hDC, format, ppfd);
    
    return rslt;
}

_ZGL int __stdcall wglDescribePixelFormatSIG(HDC hDC, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd)
{
    afxError err = AFX_ERR_NONE;
    int rslt = 0;

#ifndef _AVOID_DRV_FUNCS
    if ((!wgl.DrvDescribePixelFormat) || !(rslt = wgl.DrvDescribePixelFormat(hDC, iPixelFormat, nBytes, ppfd)))
#endif
#ifndef _AVOID_WGL_FUNCS
        if ((!wgl.DescribePixelFormat) || !(rslt = wgl.DescribePixelFormat(hDC, iPixelFormat, nBytes, ppfd)))
#endif
            rslt = DescribePixelFormat(hDC, iPixelFormat, nBytes, ppfd);

    return rslt;
}

_ZGL int __stdcall wglGetPixelFormatSIG(HDC hDC)
{
    afxError err = AFX_ERR_NONE;
    int rslt;

#ifndef _AVOID_WGL_FUNCS
    if ((!wgl.GetPixelFormat) || !(rslt = wgl.GetPixelFormat(hDC)))
#endif
        rslt = GetPixelFormat(hDC);

    return rslt;
}

_ZGL BOOL __stdcall wglSwapIntervalSIG(int interval)
{
    return wgl.SwapIntervalEXT ? wgl.SwapIntervalEXT(interval) : FALSE;
}

_ZGL void APIENTRY _glDbgMsgCb(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    afxChar*sourceStr, *typeStr, *severityStr;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API: sourceStr = "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceStr = "WINDOW SYSTEM"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "SHADER COMPILER"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: sourceStr = "THIRD PARTY"; break;
    case GL_DEBUG_SOURCE_APPLICATION: sourceStr = "APPLICATION"; break;
    case GL_DEBUG_SOURCE_OTHER: sourceStr = "OTHER"; break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR: typeStr = "ERROR"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "DEPRECATED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeStr = "UNDEFINED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_PORTABILITY: typeStr = "PORTABILITY"; break;
    case GL_DEBUG_TYPE_PERFORMANCE: typeStr = "PERFORMANCE"; break;
    case GL_DEBUG_TYPE_MARKER: typeStr = "MARKER"; break;
    case GL_DEBUG_TYPE_OTHER: typeStr = "OTHER"; break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_NOTIFICATION: severityStr = "NOTIFICATION"; break;
    case GL_DEBUG_SEVERITY_LOW: severityStr = "LOW"; break;
    case GL_DEBUG_SEVERITY_MEDIUM: severityStr = "MEDIUM"; break;
    case GL_DEBUG_SEVERITY_HIGH: severityStr = "HIGH"; break;
    }
    AfxLogError("%s %s %s %u %s", sourceStr, typeStr, severityStr, id, message);
}

_ZGL void SglDetectDeviceFeatures(glVmt const* gl, HDC hDC, afxDrawFeatures* pFeatures)
{
    static afxDrawFeatures features = { 0 };
    
    features.robustBufAccess = wglHasExtensionSIG(hDC, "GL_ARB_robustness");
    features.primShader = wglHasExtensionSIG(hDC, "GL_ARB_geometry_shader4");
    features.tessShader = wglHasExtensionSIG(hDC, "GL_ARB_tessellation_shader");
    features.sampleRateShading = wglHasExtensionSIG(hDC, "GL_ARB_sample_shading");
    features.dxt = wglHasExtensionSIG(hDC, "GL_ARB_compressed_texture_pixel_storage");
    features.samplerAnisotropy = wglHasExtensionSIG(hDC, "GL_ARB_texture_filter_anisotropic");
    features.multiViewport = wglHasExtensionSIG(hDC, "GL_ARB_viewport_array");
    features.samplerMirrorClampToEdge = wglHasExtensionSIG(hDC, "GL_ARB_texture_mirror_clamp_to_edge");
    features.multiDrawIndirect = wglHasExtensionSIG(hDC, "GL_ARB_multi_draw_indirect");
    features.voidCanvas = wglHasExtensionSIG(hDC, "GL_ARB_framebuffer_no_attachments");
    features.samplerFilterMinMax = wglHasExtensionSIG(hDC, "GL_ARB_texture_filter_minmax");
    features.shaderCullDist = wglHasExtensionSIG(hDC, "GL_ARB_cull_distance");
    features.shaderRasterGatherExt = wglHasExtensionSIG(hDC, "GL_ARB_texture_gather");
    features.shaderBufInt64Atomics = wglHasExtensionSIG(hDC, "GL_ARB_shader_atomic_counters");
    features.vtxPipelineStoresAndAtomics = wglHasExtensionSIG(hDC, "GL_ARB_shader_atomic_counter_ops");
    features.depthClamp = wglHasExtensionSIG(hDC, "GL_ARB_depth_clamp");
    features.depthBiasClamp = wglHasExtensionSIG(hDC, "GL_ARB_polygon_offset_clamp");
    features.depthBounds = wglHasExtensionSIG(hDC, "GL_EXT_depth_bounds_test");
    
    *pFeatures = features;
}

_ZGL void SglDetectDeviceLimits(glVmt const* gl, afxDrawLimits* pLimits)
{
    GLfloat dataf;
    GLfloat dataf2[3];
    GLint datai;
    GLint datai2[3];
    static afxDrawLimits limits = { 0 };

#if 0
    afxNat maxPushConstantsSiz;
    afxNat maxMemAllocCnt;
    afxNat maxSamplerAllocCnt;

    afxSize sparseAddrSpaceSiz;
    afxNat maxBoundDescriptorSets;
    afxNat maxPerStageDescriptorSamplers;
    afxNat maxPerStageDescriptorUniformBuffers;
    afxNat maxPerStageDescriptorStorageBuffers;
    afxNat maxPerStageDescriptorSampledImages;
    afxNat maxPerStageDescriptorStorageImages;
    afxNat maxPerStageDescriptorInputAttachments;
    afxNat maxPerStageResources;

    // the maximum number of uniform blocks per program.The value must be at least 70.
    // GL_MAX_COMBINED_UNIFORM_BLOCKS

    // the maximum supported texture image units that can be used to access texture maps from the vertex shader and the fragment processor combined.
    // If both the vertex shader and the fragment processing stage access the same texture image unit, then that counts as using two texture image units against this limit.
    // The value must be at least 48.
    //MAX_COMBINED_TEXTURE_IMAGE_UNITS
    afxNat maxDescriptorSetSamplers;
    afxNat maxDescriptorSetUniformBuffers;
    afxNat maxDescriptorSetUniformBuffersDynamic;
    afxNat maxDescriptorSetStorageBuffers;
    afxNat maxDescriptorSetStorageBuffersDynamic;
    // the maximum supported texture image units that can be used to access texture maps from the fragment shader. The value must be at least 16.
    //GL_MAX_TEXTURE_IMAGE_UNITS
    afxNat maxDescriptorSetSampledImages;
    afxNat maxDescriptorSetStorageImages;
    afxNat maxDescriptorSetInputAttachments;
    afxNat maxFragCombinedOutputResources;

    afxNat subPixelPrecisionBits;
    afxNat subTexelPrecisionBits;
    afxNat mipmapPrecisionBits;
    afxNat maxDrawIndirectCnt;

    afxFlags storageRasterSampleCnts;
    afxBool timestampComputeAndGraphics;
    afxReal timestampPeriod;
    afxNat discreteQueuePriorities;
    afxBool strictLines;
    afxBool standardSampleLocations;
    afxSize optimalBufCopyOffsetAlign;
    afxSize optimalBufCopyRowPitchAlign;
    afxSize nonCoherentAtomSiz;
#endif

    //////////////////////////////////////
    // VERTEX PROCESSING

    // the maximum number of 4-component generic vertex attributes accessible to a vertex shader. The value must be at least 16.
    gl->GetIntegerv(GL_MAX_VERTEX_ATTRIBS, &datai); _SglThrowErrorOccuried();
    limits.maxVtxInputAttributes = datai; // at least 16
    // the maximum number of components of output written by a vertex shader, which must be at least 64.
    gl->GetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &datai); _SglThrowErrorOccuried();
    limits.maxVtxOutputCompos = datai; // at least 64
    // the maximum number of vertex buffers that may be bound.
    gl->GetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &datai); _SglThrowErrorOccuried();
    limits.maxVtxInputBindings = datai;
    // the maximum offset that may be added to a vertex binding offset.
    gl->GetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, &datai); _SglThrowErrorOccuried();
    limits.maxVtxInputAttributeOffset = datai;
    // the maximum value of stride parameters to vertex array pointer-setting commands.
    gl->GetIntegerv(GL_MAX_VERTEX_ATTRIB_STRIDE, &datai); _SglThrowErrorOccuried();
    limits.maxVtxInputBindingStride = datai;

    /////////////////////////////////////////////
    // PRIMITIVE PROCESSING

    // the maximum number of application-defined clipping distances. The value must be at least 8.
    gl->GetIntegerv(GL_MAX_CLIP_DISTANCES, &datai); _SglThrowErrorOccuried();
    limits.maxClipDistances = datai; // at least 8
    gl->GetIntegerv(GL_MAX_CULL_DISTANCES, &datai); _SglThrowErrorOccuried();
    limits.maxCullDistances = datai; // at least 8
    gl->GetIntegerv(GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES, &datai); _SglThrowErrorOccuried();
    limits.maxCombinedClipAndCullDistances = datai; // at least 8

    // the maximum number of simultaneous viewports that are supported. The value must be at least 16.
    gl->GetIntegerv(GL_MAX_VIEWPORTS, &datai); _SglThrowErrorOccuried();
    limits.maxVpCnt = datai; // at least 16
    // the maximum supported width and height of the viewport. These must be at least as large as the visible dimensions of the display being rendered to.
    gl->GetIntegerv(GL_MAX_VIEWPORT_DIMS, datai2); _SglThrowErrorOccuried();
    limits.maxVpDimensions[0] = datai2[0];
    limits.maxVpDimensions[1] = datai2[1];
    // the minimum and maximum viewport bounds range. The minimum range should be at least [-32768, 32767].
    gl->GetFloatv(GL_VIEWPORT_BOUNDS_RANGE, dataf2); _SglThrowErrorOccuried();
    limits.vpBoundsRange[0] = dataf2[0];  // at least [-32768, 32767]
    limits.vpBoundsRange[1] = dataf2[1];  // at least [-32768, 32767]
    // the number of bits of sub-pixel precision which the GL uses to interpret the floating point viewport bounds. The minimum value is 0.
    gl->GetIntegerv(GL_VIEWPORT_SUBPIXEL_BITS, &datai); _SglThrowErrorOccuried();
    limits.vpSubPixelBits = datai; // at least 0

    // the maximum number of components of inputs read by a geometry shader, which must be at least 64.
    gl->GetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS, &datai); _SglThrowErrorOccuried();
    limits.maxPrimInComps = datai; // at least 64
    // the maximum number of components of outputs written by a geometry shader, which must be at least 128.
    gl->GetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, &datai); _SglThrowErrorOccuried();
    limits.maxPrimOutComps = datai; // at least 128

    // the implementation-dependent maximum tessellation level (the value of MAX_TESS_GEN_LEVEL).
    gl->GetIntegerv(GL_MAX_TESS_GEN_LEVEL, &datai); _SglThrowErrorOccuried();
    limits.maxTessGenLvl = datai;
    // the implementation-dependent maximum patch size (the value of MAX_PATCH_VERTICES). The patch size is initially three vertices.
    gl->GetIntegerv(GL_MAX_PATCH_VERTICES, &datai); _SglThrowErrorOccuried();
    limits.maxTessPatchSiz = datai;
    gl->GetIntegerv(GL_MAX_TESS_CONTROL_INPUT_COMPONENTS, &datai); _SglThrowErrorOccuried();
    limits.maxTessCtrlPerVtxInComps = datai;
    gl->GetIntegerv(GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS, &datai); _SglThrowErrorOccuried();
    limits.maxTessCtrlPerVtxOutComps = datai;
    // The number of components of active per-patch output variables may not exceed the value of MAX_TESS_PATCH_COMPONENTS
    gl->GetIntegerv(GL_MAX_TESS_PATCH_COMPONENTS, &datai); _SglThrowErrorOccuried();
    limits.maxTessCtrlPerPatchOutComps = datai;
    gl->GetIntegerv(GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS, &datai); _SglThrowErrorOccuried();
    limits.maxTessCtrlTotalOutComps = datai;
    gl->GetIntegerv(GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS, &datai); _SglThrowErrorOccuried();
    limits.maxTessEvalInComps = datai;
    gl->GetIntegerv(GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS, &datai); _SglThrowErrorOccuried();
    limits.maxTessEvalOutComps = datai;

    gl->GetIntegerv(GL_MAX_GEOMETRY_SHADER_INVOCATIONS, &datai); _SglThrowErrorOccuried();
    limits.maxPrimShaderInvocations = datai;
    gl->GetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &datai); _SglThrowErrorOccuried();
    limits.maxPrimOutVertices = datai;
    gl->GetIntegerv(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, &datai); _SglThrowErrorOccuried();
    limits.maxPrimTotalOutComps = datai;

    /////////////////////////////////////
    // CANVAS

    gl->GetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &datai); _SglThrowErrorOccuried();
    limits.maxColorAttachments = datai; // at least 16384
    // 4.3 --- the maximum width for a framebuffer that has no attachments, which must be at least 16384.
    gl->GetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &datai); _SglThrowErrorOccuried();
    limits.maxCanvasWhd[0] = datai; // at least 16384
    // 4.3 --- the maximum height for a framebuffer that has no attachments, which must be at least 16384.
    gl->GetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &datai); _SglThrowErrorOccuried();
    limits.maxCanvasWhd[1] = datai; // at least 16384
    // 4.3 --- the maximum number of layers for a framebuffer that has no attachments, which must be at least 2048.
    gl->GetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &datai); _SglThrowErrorOccuried();
    limits.maxCanvasWhd[2] = datai; // at least 2048
     // the maximum samples in a framebuffer that has no attachments, which must be at least 4.
    gl->GetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &datai); _SglThrowErrorOccuried();
    limits.canvasNoAttachmentsSampleCnts = datai;

    ////////////////////////////////////////////
    // MULTISAMPLING

    // the maximum number of sample mask words.
    gl->GetIntegerv(GL_MAX_SAMPLE_MASK_WORDS, &datai); _SglThrowErrorOccuried();
    limits.maxSampleMaskWords = datai;
    // the maximum number of samples supported in integer format multisample buffers.
    gl->GetIntegerv(GL_MAX_INTEGER_SAMPLES, &datai); _SglThrowErrorOccuried();
    limits.sampledRasterIntegerSampleCnts = datai;
    // the maximum number of samples in a color multisample texture.
    gl->GetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &datai); _SglThrowErrorOccuried();
    limits.sampledRasterColorSampleCnts = datai;
    limits.canvasColorSampleCnts = datai;
    // the maximum number of samples in a multisample depth or depth-stencil texture.
    gl->GetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &datai); _SglThrowErrorOccuried();
    limits.sampledRasterDepthSampleCnts = datai;
    limits.sampledRasterStencilSampleCnts = datai;
    limits.canvasDepthSampleCnts = datai;
    limits.canvasStencilSampleCnts = datai;


    ///////////////////////////////////////////
    // BUFFERIZATION

    // the minimum alignment in basic machine units of pointers returned from glMapBuffer and glMapBufferRange. This value must be a power of two and must be at least 64.
    gl->GetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT, &datai); _SglThrowErrorOccuried();
    limits.minMemMapAlign = datai; // at least 64
    // the minimum required alignment for shader storage buffer sizes and offset. The initial value is 1.
    gl->GetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &datai); _SglThrowErrorOccuried();
    limits.minStorageBufOffsetAlign = datai; // at least 64
    // the minimum required alignment for texture buffer sizes and offset. The initial value is 1.
    gl->GetIntegerv(GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT, &datai); _SglThrowErrorOccuried();
    limits.minTexelBufOffsetAlign = datai; // at least 64
    // the minimum required alignment for uniform buffer sizes and offset. The initial value is 1.
    gl->GetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &datai); _SglThrowErrorOccuried();
    limits.minUniformBufOffsetAlign = datai; // at least 64

    // A rough estimate of the largest texture that the GL can handle. The value must be at least 1024.
    gl->GetIntegerv(GL_MAX_TEXTURE_SIZE, &datai); _SglThrowErrorOccuried();
    limits.maxRasterDim1D = datai; // at least 1024
    limits.maxRasterDim2D = datai; // at least 1024
    // a rough estimate of the largest 3D texture that the GL can handle. The value must be at least 64.
    gl->GetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &datai); _SglThrowErrorOccuried();
    limits.maxRasterDim3D = datai; // at least 64
    // the maximum number of layers allowed in an array texture, and must be at least 256.
    gl->GetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &datai); _SglThrowErrorOccuried();
    limits.maxRasterArrayLayers = datai; // at least 256
    // a rough estimate of the largest cube-map texture that the GL can handle. The value must be at least 1024.
    gl->GetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &datai); _SglThrowErrorOccuried();
    limits.maxRasterDimCube = datai; // at least 1024

    // The value gives the maximum number of texels allowed in the texel array of a texture buffer object. Value must be at least 65536.
    gl->GetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &datai); _SglThrowErrorOccuried();
    limits.maxTexelBufElements = datai;
    // the maximum size in basic machine units of a uniform block, which must be at least 16384.
    gl->GetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &datai); _SglThrowErrorOccuried();
    limits.maxUniformBufRange = datai;
    gl->GetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &datai); _SglThrowErrorOccuried();
    limits.maxStorageBufRange = datai;
    // the minimum required alignment for texture buffer sizes and offset. The initial value is 1.
    gl->GetIntegerv(GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT, &datai); _SglThrowErrorOccuried();
    limits.bufferRasterGranularity = datai;

    //////////////////////////////
    // SAMPLING

    // the maximum, absolute value of the texture level-of-detail bias. The value must be at least 2.0.
    gl->GetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &dataf); _SglThrowErrorOccuried();
    limits.maxSamplerLodBias = dataf; // at least 2.0
    gl->GetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &dataf); _SglThrowErrorOccuried();
    limits.maxSamplerAnisotropy = dataf;

    ////////////////////////////////////////////
    // RASTERIZATION

    // the smallest and largest supported sizes for antialiased points. The smallest size must be at most 1, and the largest size must be at least 1.
    gl->GetFloatv(GL_POINT_SIZE_RANGE, dataf2); _SglThrowErrorOccuried();
    limits.pointSizRange[0] = dataf2[0]; // at least 1
    limits.pointSizRange[1] = dataf2[1]; // at least 1
    // the size difference between adjacent supported sizes for antialiased points.
    gl->GetFloatv(GL_POINT_SIZE_GRANULARITY, &dataf); _SglThrowErrorOccuried();
    limits.pointSizGranularity = dataf;
    // the range of widths supported for smooth (antialiased) lines.
    gl->GetFloatv(GL_LINE_WIDTH_RANGE, dataf2); _SglThrowErrorOccuried();
    limits.lineWidthRange[0] = dataf2[0];
    limits.lineWidthRange[1] = dataf2[1];
    // the level of quantization applied to smooth line width parameters.
    gl->GetFloatv(GL_LINE_WIDTH_GRANULARITY, &dataf); _SglThrowErrorOccuried();
    limits.lineWidthGranularity = dataf;

    ///////////////////////////////
    // FRAGMENT PROCESSING

    // the maximum number of components of the inputs read by the fragment shader, which must be at least 128.
    gl->GetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &datai); _SglThrowErrorOccuried();
    limits.maxFragInComps = datai; // at least 128
    // the maximum number of simultaneous outputs that may be written in a fragment shader. The value must be at least 8.
    gl->GetIntegerv(GL_MAX_DRAW_BUFFERS, &datai); _SglThrowErrorOccuried();
    limits.maxFragOutAttachments = datai; // at least 8
    // the maximum number of active draw buffers when using dual-source blending. The value must be at least 1.
    gl->GetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, &datai); _SglThrowErrorOccuried();
    limits.maxFragDualSrcAttachments = datai; // at least 1

    // the minimum texel offset allowed in a texture lookup, which must be at most -8.
    gl->GetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, &datai); _SglThrowErrorOccuried();
    limits.minTexelOffset = datai; // at least 1
    // the maximum texel offset allowed in a texture lookup, which must be at least 7.
    gl->GetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, &datai); _SglThrowErrorOccuried();
    limits.maxTexelOffset = datai; // at least 1
    // For all instructions except textureGather, the limits are the values of MIN_PROGRAM_TEXEL_OFFSET and MAX_PROGRAM_TEXEL_OFFSET. 
    // For the textureGather instruction, the limits are the values of MIN_PROGRAM_TEXTURE_GATHER_OFFSET and MAX_PROGRAM_TEXTURE_GATHER_OFFSET.
    gl->GetIntegerv(GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET, &datai); _SglThrowErrorOccuried();
    limits.minTexelGatherOffset = datai; // at least 1
    gl->GetIntegerv(GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET, &datai); _SglThrowErrorOccuried();
    limits.maxTexelGatherOffset = datai; // at least 1

    // The built-in function interpolateAtOffset will sample variables at a specified(x, y) offset relative to the center of the pixel.The range and granularity of offsets supported by this function is implementation-dependent.
    // If either component of the specified offset is less than the value of MIN_FRAGMENT_INTERPOLATION_OFFSET or greater than the value of MAX_FRAGMENT_INTERPOLATION_OFFSET, the position used to interpolate the variable is undefined.
    // Not all values of offset may be supported; x and y offsets may be rounded to fixed-point values with the number of fraction bits given by the value of the implementation-dependent constant FRAGMENT_INTERPOLATION_OFFSET_BITS.
    gl->GetFloatv(GL_MIN_FRAGMENT_INTERPOLATION_OFFSET, &dataf); _SglThrowErrorOccuried();
    limits.minInterpolationOffset = dataf; // at least 1
    gl->GetFloatv(GL_MAX_FRAGMENT_INTERPOLATION_OFFSET, &dataf); _SglThrowErrorOccuried();
    limits.maxInterpolationOffset = dataf; // at least 1
    gl->GetIntegerv(GL_FRAGMENT_INTERPOLATION_OFFSET_BITS, &datai); _SglThrowErrorOccuried();
    limits.subPixelInterpolationOffsetBits = datai; // at least 1

    ///////////////////////////////////////
    // COMPUTING

    // the number of invocations in a single local work group (i.e., the product of the three dimensions) that may be dispatched to a compute shader.
    gl->GetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &datai); _SglThrowErrorOccuried();
    limits.maxComputeWorkGroupInvocations = datai;
    // the maximum number of work groups that may be dispatched to a compute shader. Indices 0, 1, and 2 correspond to the X, Y and Z dimensions, respectively.
    gl->GetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_COUNT, datai2); _SglThrowErrorOccuried();
    limits.maxComputeWorkGroupCnt[0] = datai2[0];
    limits.maxComputeWorkGroupCnt[1] = datai2[1];
    limits.maxComputeWorkGroupCnt[2] = datai2[2];
    // the maximum size of a work groups that may be used during compilation of a compute shader. Indices 0, 1, and 2 correspond to the X, Y and Z dimensions, respectively.
    gl->GetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_SIZE, datai2); _SglThrowErrorOccuried();
    limits.maxComputeWorkGroupSiz[0] = datai2[0];
    limits.maxComputeWorkGroupSiz[1] = datai2[1];
    limits.maxComputeWorkGroupSiz[2] = datai2[2];

    gl->GetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &datai); _SglThrowErrorOccuried();
    limits.maxComputeSharedMemorySiz = datai;

    // the maximum index that may be specified during the transfer of generic vertex attributes to the GL.
    gl->GetIntegerv(GL_MAX_ELEMENT_INDEX, &datai); _SglThrowErrorOccuried();
    limits.maxDrawIndexedIdxValue = datai;




    limits.maxBoundDescriptorSets = 4;
    limits.maxPushConstantsSiz = 0;

    *pLimits = limits;
}
