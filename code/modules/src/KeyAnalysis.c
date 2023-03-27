/*************************************************************************
 * Copyright (c) 2018 Shenzhen Xiaojun technology co., Ltd.All right reserved.
 * Author: chaofa.yan
 * Date: 2018/07/04
 * History Information
 * Author 		Date 		Description
 * chaofa.yan	2018/07/04 	Created the file
 *************************************************************************/

#include "inc/KeyAnalysis.h"
#include "common.h"

#define CLEAN_KEY_INFO(KeyInfoVar)        \
    do                                    \
    {                                     \
        KeyInfoVar.KeyName = eKeyNameMax; \
        KeyInfoVar.KeyStat = eKeyStatMax; \
        KeyInfoVar.Counter = 0;           \
    } while (0)

static KeyInfo gSysKeyInfo =
    {
        eKeyNameMax,
        eKeyStatMax,
        0};

KeyInfo GetAndCleanSysKeyInfo(void)
{
    KeyInfo Ret = gSysKeyInfo;

    CLEAN_KEY_INFO(gSysKeyInfo);

    return (Ret);
}

static KeyInfo AnalyzedKey(eKeyName CurKey)
{
    static KeyInfo LastKeyInfo =
        {
            eKeyNameMax,
            eKeyStatMax,
            0};

    KeyInfo RetKeyInfo =
        {
            eKeyNameMax,
            eKeyStatMax,
            0};

    if (CurKey >= eKeyNameMax)
    {
        if (LastKeyInfo.KeyName < eKeyNameMax)
        {
            // last key != eKeyNameMax, cur key == eKeyNameMax. Key release
            RetKeyInfo = LastKeyInfo;
            CLEAN_KEY_INFO(LastKeyInfo);
        }
        else
        {
            // last key == eKeyNameMax, cur key == eKeyNameMax. No Key
            // do nothing currently
        }
        RetKeyInfo.KeyStat = eKeyStatRelease;
    }
    else
    {
        // last key == eKeyNameMax, cur key != eKeyNameMax. New Key
        if (LastKeyInfo.KeyName >= eKeyNameMax)
        {
            RetKeyInfo.KeyName = CurKey;
            RetKeyInfo.Counter = 1;
        }
        else if (LastKeyInfo.KeyName == CurKey)
        {
            // last key==cur key!=eKeyNameMax, Repeat Key
            RetKeyInfo = LastKeyInfo;

            if (RetKeyInfo.Counter < MAX_OF_TYPE(uint16_t)) // Avoid overflow
            {
                RetKeyInfo.Counter++;
            }
        }
        else
        {
            // last key != eKeyNameMax, cur key != eKeyNameMax, last key != cur key.
            // Another Key pressed without last key releaseed
            RetKeyInfo.KeyName = CurKey;
            RetKeyInfo.KeyStat = eKeyStatAnotherKey;
            RetKeyInfo.Counter = 1;
        }

        LastKeyInfo = RetKeyInfo;
    }

    return (RetKeyInfo);
}

void keyInfoHandle(pfGetKey key)
{
    gSysKeyInfo = AnalyzedKey(key());
}

void PollingKey(TmrHdl Handle, eTmrCmd eCmd)
{
    pfGetKey pfKey = (pfGetKey)TmrGetCtx(Handle);

    switch (eCmd)
    {
    case eTimeOut:
        gSysKeyInfo = AnalyzedKey(pfKey());
        break;

    default:
        break;
    }
}
