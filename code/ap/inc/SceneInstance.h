/*************************************************************************
* Copyright (c) 2018 Shenzhen Xiaojun technology co., Ltd.All right reserved.
* Author: chaofa.yan
* Date: 2018/07/04
* History Information
* Author 		Date 		Description
* chaofa.yan	2018/07/04 	Created the file
*************************************************************************/

#ifndef __SCENE_INSTANCE_H__
#define __SCENE_INSTANCE_H__

#include "polling.h"
#if 0
//#define TICK_UNIT 10 //10ms
#define TICK_UNIT 5 //ms

//unit of Period is ms
#define TMR_PERIOD(Period) \
	((Period)<TICK_UNIT? 1: (((Period)+TICK_UNIT/2)/TICK_UNIT)) 
#endif
#define TIMER_MAX_NUM    (16)
typedef enum{
 TimeHeader = 0,
 TimeHeaderIdle,
}eTimeType;


extern TmrHeader* GetTmrHd(void);
extern TmrHeader* GetTmrIdleHd(void);

extern void PollingModuleInit(void);
extern void SystemPolling(void);
extern void SystemPollingIdle(void);

#endif

