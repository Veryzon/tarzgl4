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
#include "qwadro/afxQwadro.h"

_ZGL afxError _SglCmdbResetCb(avxCmdb cmdb)
{
    afxError err = AFX_ERR_NONE;
    AfxAssert(cmdb->m.state != avxCmdbState_PENDING);

    _sglCmd const* cmdHdr;
    AfxChainForEveryLinkageB2F(&cmdb->commands, _sglCmd, script, cmdHdr)
    {
        if (cmdHdr->id == NIL/*ZGL_CMD_END*/)
            break;

        //if (cmdHdr->id == ZGL_CMD_BEGIN_SYNTHESIS)
        {
            //_sglCmdBeginSynthesis const *cmd = AFX_REBASE(cmdHdr, _sglCmdBeginSynthesis, cmd);
        }
    }

    AfxZero(cmdb->levelCaches, sizeof(cmdb->levelCaches[0]));

#if 0
    while (1)
    {
        afxChain *commands = &idd->commands;
        afxLinkage *first = AfxGetLastLinkage(commands), *dummy = AfxGetAnchor(commands);

        if (first == dummy) break;
        else
        {
            _sglCmd *cmd = AFX_REBASE(first, _sglCmd, script);
            AfxPopLinkage(&cmd->script);

            //if (cmd != &cmdb->m.cmdEnd)
            {
                //AfxDeallocate(all, cmd);
                //AfxAssertType(idd->cmdArena, afxFcc_AREN);
                AfxRecycleArena(&idd->cmdArena, cmd, cmd->siz);
            }
        }
    }
#else
    AfxExhaustArena(&cmdb->m.cmdArena);
#endif

    AfxDeployChain(&cmdb->commands, cmdb);

    return err;
}

ZGL afxError _SglCmdbEndCb(avxCmdb cmdb); // no sglDrawCommands.c

_ZGL afxError _SglCmdbBeginCb(avxCmdb cmdb, afxBool permanent)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &cmdb, afxFcc_CMDB);
    cmdb->m.disposable = !permanent;
    return err;
}

_ZGL afxError _SglCmdbDtor(avxCmdb cmdb)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &cmdb, afxFcc_CMDB);

    while (AfxLoadAtom32(&cmdb->m.submCnt))
    {
        AfxYield();
    }

    if (_AvxCmdbStdImplementation.dtor(cmdb))
        AfxThrowError();

    return err;
}

_ZGL afxError _SglCmdbCtor(avxCmdb cmdb, void** args, afxNat invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AfxAssertObjects(1, &cmdb, afxFcc_CMDB);

    if (_AvxCmdbStdImplementation.ctor(cmdb, args, invokeNo)) AfxThrowError();
    else
    {
        AfxDeployChain(&cmdb->commands, cmdb);
        AfxDeployChain(&cmdb->echoes, cmdb);

        cmdb->m.beginCb = _SglCmdbBeginCb;
        cmdb->m.endCb = _SglCmdbEndCb;
        cmdb->m.resetCb = _SglCmdbResetCb;

        cmdb->m.stdCmds = &_SglEncodeCmdVmt;

        if (err && _AvxCmdbStdImplementation.dtor(cmdb))
            AfxThrowError();
    }
    return err;
}

_ZGL afxClassConfig _SglCmdbMgrCfg =
{
    .fcc = afxFcc_CMDB,
    .name = "CmdBuffer",
    .desc = "Draw I/O Bufferization",
    .fixedSiz = sizeof(AFX_OBJECT(avxCmdb)),
    .ctor = (void*)_SglCmdbCtor,
    .dtor = (void*)_SglCmdbDtor
};
