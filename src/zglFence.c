/*
 *           ::::::::    :::::::::::    ::::::::    ::::     ::::       :::
 *          :+:    :+:       :+:       :+:    :+:   +:+:+: :+:+:+     :+: :+:
 *          +:+              +:+       +:+          +:+ +:+:+ +:+    +:+   +:+
 *          +#++:++#++       +#+       :#:          +#+  +:+  +#+   +#++:++#++:
 *                 +#+       +#+       +#+   +#+#   +#+       +#+   +#+     +#+
 *          #+#    #+#       #+#       #+#    #+#   #+#       #+#   #+#     #+#
 *           ########    ###########    ########    ###       ###   ###     ###
 *
 *                     S I G M A   T E C H N O L O G Y   G R O U P
 *
 *                                   Public Test Build
 *                               (c) 2017 SIGMA FEDERATION
 *                             <https://sigmaco.org/qwadro/>
 */


#include "zglUtils.h"
#include "zglCommands.h"
#include "zglObjects.h"

_ZGL afxError _DpuBindAndSyncFenc(zglDpu* dpu, afxBool syncOnly, avxFence fenc)
{
    //AfxEntry("pip=%p", pip);
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = dpu->gl;
    
    gl->FenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

    return err;
}

_ZGL afxError _ZglResetFence(zglDpu* dpu, avxFence fenc)
{
    afxError err = AFX_ERR_NONE;
    AfxStoreAtom32(&fenc->m.signaled, 0);
    return err;
}

_ZGL afxError _ZglSignalFence(zglDpu* dpu, avxFence fenc)
{
    afxError err = AFX_ERR_NONE;
    afxInt32 expected = 0;
    //AfxCasAtom32(&fenc->m.signaled, &expected, 1);
    AfxStoreAtom32(&fenc->m.signaled, 1);
    return err;
}

_ZGL afxError _ZglWaitFenc(afxBool waitAll, afxUnit64 timeout, afxUnit cnt, avxFence const fences[])
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_FENC, cnt, fences);
    afxDrawSystem dsys = (void*)AvxGetFenceHost(fences[0]);
    afxUnit txuIdx = 0;
    glVmt const* gl = NIL;// &ddev->idd->dpus[txuIdx].gl;

    afxClock startClock, currClock;
    AfxGetClock(&startClock);

    do
    {

        for (afxUnit i = 0; i < cnt; i++)
        {
            avxFence fenc = fences[i];
            AFX_ASSERT_OBJECTS(afxFcc_FENC, 1, fenc);

            // To block all CPU operations until a sync object is signaled, you call this function:
            GLenum rslt = gl->ClientWaitSync(fenc->glHandle, GL_SYNC_FLUSH_COMMANDS_BIT, /*timeout*/0); _ZglThrowErrorOccuried();

            /*
                This function will not return until one of two things happens: the sync​ object parameter becomes signaled, or a number of nanoseconds greater than or equal to the timeout​ parameter passes.
                If timeout​ is zero, the function will simply check to see if the sync object is signaled and return immediately.
                Note that the fact that timeout​ is in nanoseconds does not imply that this function has true nanosecond granularity in its timeout; you are only guaranteed that at least that much time will pass.
            */

            switch (rslt)
            {
            case GL_ALREADY_SIGNALED:
            {
                // the sync object was signaled before the function was called.
                AfxStoreAtom32(&fenc->m.signaled, 1);
                break;
            }
            case GL_TIMEOUT_EXPIRED:
            {
                // the sync object did not signal within the given timeout period.
                //fenc->m.signaled = FALSE;
                break;
            }
            case GL_CONDITION_SATISFIED:
            {
                // the sync object was signaled within the given timeout period.
                AfxStoreAtom32(&fenc->m.signaled, 1);
                break;
            }
            case GL_WAIT_FAILED:
            default:
            {
                // If an OpenGL Error occurred, then GL_WAIT_FAILED will be returned in addition to raising an error.
                //fenc->m.signaled = FALSE;
                break;
            }
            }

            if (fenc->m.signaled && !waitAll)
                break;
        }

        /*
            There is another function for waiting on sync objects:

            void glWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)

            Recall the discussion in the article on Synchronization about the difference between the GPU's command queue and the driver's internal command buffer.
            What glWaitSync does is prevent the driver from adding any commands to the GPU's command queue until this sync object is signaled.
            This function does not halt the CPU.

            The driver will still put commands in its internal buffer. But none of them will be seen by the GPU until this sync object is signaled.

            You need to ensure that the sync object is in the GPU's command queue.
            If you don't, then you may create an infinite loop.
            Since glWaitSync prevents the driver from adding any commands to the GPU command queue, this would include the sync object itself if it has not yet been added to the queue.
            This function does not take the GL_SYNC_FLUSH_COMMANDS_BIT, so you have to do it with a manual glFlush call.


        */

        AfxGetClock(&currClock);
    } while (timeout > AfxGetSecondsElapsed(&startClock, &currClock));
    return err;
}

_ZGL afxError _ZglResetFenc(afxUnit cnt, avxFence const fences[])
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_FENC, cnt, fences);
    
    for (afxUnit i = 0; i < cnt; i++)
    {
        avxFence fenc = fences[i];
        AFX_ASSERT_OBJECTS(afxFcc_FENC, 1, fenc);
        AfxStoreAtom32(&fenc->m.signaled, 0);
    }
    return err;
}

_ZGL afxError _ZglFencDtorCb(avxFence fenc)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_FENC, 1, &fenc);
    afxDrawSystem dsys = (void*)AvxGetFenceHost(fenc);

    if (fenc->glHandle)
    {
        _ZglDsysEnqueueDeletion(dsys, 0, GL_SYNC_FENCE, (afxSize)fenc->glHandle);
        fenc->glHandle = 0;
    }

    if (_AVX_FENC_CLASS_CONFIG.dtor(fenc))
        AfxThrowError();

    return err;
}

_ZGL afxError _ZglFencCtorCb(avxFence fenc, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_FENC, 1, &fenc);

    if (_AVX_FENC_CLASS_CONFIG.ctor(fenc, args, invokeNo)) AfxThrowError();
    else
    {
        afxDrawSystem dsys = AfxGetHost(fenc);
        fenc->fencUniqueId = ++dsys->fencUniqueId;

        fenc->glHandle = 0;
        fenc->updFlags = ZGL_UPD_FLAG_DEVICE_INST;

        if (_AVX_FENC_CLASS_CONFIG.dtor(fenc))
            AfxThrowError();
    }
    return err;
}

_ZGL afxError _ZglSemDtorCb(afxSemaphore sem)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_SEM, 1, &sem);

    if (_AfxSemStdImplementation.dtor(sem))
        AfxThrowError();

    return err;
}

_ZGL afxError _ZglSemCtorCb(afxSemaphore sem, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_SEM, 1, &sem);

    if (_AfxSemStdImplementation.ctor(sem, args, invokeNo)) AfxThrowError();
    else
    {

        if (_AfxSemStdImplementation.dtor(sem))
            AfxThrowError();
    }
    return err;
}
