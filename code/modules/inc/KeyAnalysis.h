/*************************************************************************
* Copyright (c) 2018 Shenzhen Xiaojun technology co., Ltd.All right reserved.
* Author: chaofa.yan
* Date: 2018/07/04
* History Information
* Author 		Date 		Description
* chaofa.yan	2018/07/04 	Created the file
*************************************************************************/

#ifndef __KEY_ANALYSIS_H__
#define __KEY_ANALYSIS_H__

#include <stdint.h>
#include "polling.h"

#define MAX_OF_TYPE(Type) \
	((Type)-1>0? (Type)-1: (1<<(sizeof(Type)*8-2))-1+(1<<(sizeof(Type)*8-2)))


typedef enum _eKeyName
{
	eKeyMaxMin,
	eKeyPair,
	eKeyTest,
	eKeyRadioTest,
	eKeyNameMax,
}eKeyName;


typedef enum _eKeyStatus
{
	eKeyStatRelease,
	eKeyStatAnotherKey,
	eKeyStatMax,
}eKeyStat;

typedef struct _KeyInfo
{
	uint8_t KeyName; //eKeyName KeyName
	uint8_t KeyStat; //eKeyStat KeyStat
	uint16_t Counter;
}KeyInfo;

typedef eKeyName (*pfGetKey)(void);

extern KeyInfo GetAndCleanSysKeyInfo(void);
extern void PollingKey(TmrHdl Handle, eTmrCmd eCmd);
extern void keyInfoHandle(pfGetKey key);
#endif

