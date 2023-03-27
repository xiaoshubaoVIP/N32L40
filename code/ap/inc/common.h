#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

#include "polling.h"
#include "SceneInstance.h"
#include "qep_port.h"

#include "config.h"
#include "system.h"
#include "debug.h"
#include "shell_ext.h"
#include "ext_interface.h"



#define GET_ARRAY_NUM(obj) (sizeof(obj) / sizeof(obj[0]))
#define assert_param(expr) ((void)0)


#ifndef NULL
#define NULL ((void *)0)
#endif

extern const unsigned char mcu_version[];

void DelayMs(unsigned long nTime);
void DelayUs(unsigned long TimeCnt);
void Delays(unsigned long nTime);
void SystemDelayUs(unsigned long nTime);
#endif 
