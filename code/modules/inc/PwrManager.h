/****************************************************************************/
/*	File   :             PwrManager.h               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/4/24,19:40:24                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/

#ifndef __PWR_MANAGER_H_
#define __PWR_MANAGER_H_

/* 
    日志等级 LOG_LEVEL
    0:ERROR
    1:WARN
    2:INFO
    3:DEBUG
 */
#undef LOG_LEVEL
#define LOG_LEVEL 2

#include <stdint.h>
#include "system.h"

#define BAT_VALUE                                           (2700)    //3v
#define BATTERY_CHECK_COUNT                                 (6)       // 54s/SLEEP_TIME=6
#define LOW_BATTERY_VOLTAGE_THRESHOLD                       (2600)    //2600mV  

#define LOW_BATTERY_MUTE_TIME                   (600)//    //10h 

#define LOW_BATTERY_THRESHOLD                   (2700)
#define LOW_BATTERY_HUSH_FORBID_THRESHOLD       (2550)

#define BatteryState_FULL				(2900)		// >2.85v
#define BatteryState_TWO_THIRED			(2800)		// >2.80v

#ifndef BatteryState_ONE_THIRED
#define BatteryState_ONE_THIRED			(2650)
#endif

typedef enum{
	eBatteryLevelNull = 0x01,
	eBatteryLevelOne,
	eBatteryLevelTwo,
	eBatteryLevelThree,
	eBatteryLevelMAX,
}eBatteryState;

typedef enum{
	eBatteryMCU,
	eBatteryCO,
}eBatteryType;

extern uint8_t BatteryState;

void EnteryLowPower(void);
uint16_t BatteryGetData(void);
void BatteryMcuInit(void);
void BatteryCheckHandle(void);

void BatterCheck(void);
BOOL IsLowPwrErr(void);
unsigned char GetBatterLevel(void);
uint16_t getBatteryVol(void);
#endif

