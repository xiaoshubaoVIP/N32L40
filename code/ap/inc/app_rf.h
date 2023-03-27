/****************************************************************************/
/*	File   :             Net.h               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/8/08,14:13:32                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/

#ifndef __APP_RF_H__
#define __APP_RF_H__

/* 
    日志等级
    0:ERROR
    1:WARN
    2:INFO
    3:DEBUG
 */
#undef LOG_LEVEL
#define LOG_LEVEL 3

#include "common.h"

#ifdef SUPPORT_RADIO_FUNCTION

#define HEART_BEAT_TIME (900)

#define RF_PAIR_MODE_NONE 0
#define RF_PAIR_MODE_HUB 1
#define RF_PAIR_MODE_INTRANET 2

#define SmokeAlarm				(0x01)
#define CoAlarm					(0x02)
#define GasAlarm				(0x03)

/***************************************************变量******************************************/

typedef struct Net
{
	uint8_t udid[4];
    uint8_t PairMode;                 // Net PairMode
    uint8_t RadioState;                
    // uint32_t NetDisconnectTime;    // RX offset
    // uint16_t NetTestLedFlickerNum; // add for test
    // eProductType ProType;
    // eRfDeviceType DevType;
}NetTag;

typedef enum
{
	eSystemState_Idle = 0x00,
	eSystemState_Pair,
	eSystemState_PairSlave,
	eSystemState_PairMaster,
	eSystemState_PairFactory,
	eSystemState_ExitPair,
	eSystemState_RadioTest,
	eSystemState_Test,
	eSystemState_CancelTest,
	eSystemState_Alarm,
	eSystemState_Mute,
	eSystemState_CancelMute,
	eSystemState_CancelAlarm,
}eRadioState;

//Rand Delay Time mode
typedef enum _eDelayMode{
	eNullTimeMode = 0,
	eNormalTimeMode,
	eLowpowerTimeMode,
	eNormalIntranetMode,
	eLowpowerIntranetMode,
}eDelayMode;

enum
{
	NeedDelTmr = 0x00,
	ReserveTmr,
};


/***************************************************函数声明******************************************/
void RfObjectInit(SystemTag *me, QEvent const *e);
void RadioInit(void);
uint8_t GetRadioPairMode(void);
void SetRadioState(eRadioState state);
uint8_t GetRadioState(void);
void RadioIdleEnter(void);
void AppPairSlaveMode(SystemTag *me, QEvent *e);
void AppPairMasterMode(SystemTag *me, QEvent *e);
void AppSendRfTestMode(SystemTag *me, QEvent *e);
void AppResponeRfTestMode(SystemTag *me, QEvent *e);

void IntranetPairSlaveMode(SystemTag *me, QEvent *e);
void IntranetPairMasterMode(SystemTag *me, QEvent *e);
void IntranetPairFactoryMode(SystemTag *me, QEvent *e);
void IntranetBroadcastTestModeEvent(SystemTag *me, QEvent *e);
void IntranetBroadcastCancelTestModeEvent(SystemTag *me, QEvent *e);
void IntranetBroadcastAlarmModeEvent(SystemTag *me, QEvent *e);
void IntranetForwardTestModeEvent(SystemTag *me, QEvent *e);
void IntranetForwardCancelAlarmModeEvent(SystemTag *me, QEvent *e);

void RadioTestHandle(SystemTag *me, QEvent *e);
void RfIdeHandle(SystemTag *me, QEvent *e);

void NetSendTestMode(SystemTag *me, QEvent *e);
void NetTestTimeout(TmrHdl Handle, eTmrCmd eCmd);
void NetProductTestHandle(SystemTag *me, QEvent *e);
void NetStartHandle(void);
void AppRF_SaveFlashInfo(uint8_t mode);

void AppNetAlarmFunc(SystemTag *me, QEvent *e);
void NetSetCoAlarmParm(void);
void NetSetSmokeAlarmParm(void);

void AppExitPair(SystemTag *me, QEvent *e);
#endif


#endif
