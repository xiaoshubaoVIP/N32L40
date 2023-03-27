#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdint.h>
#include <stdbool.h>
#include "qep_port.h"

#define SystemPostFIFI QEP_Post
#define FLASH_MAP_SMOKE_ADDR_POS 0
#define FLASH_MAP_CO_ADDR_POS 2048
#define FLASH_MAP_SYSTEM_ADDR_POS 4096

#define GET_FLASH_MEMBER_ADDR(me) (uint32_t)(&(((struct FLASH_MAP *)0)->me))
#define FlashSetData(name, buf)                                                                \
    do                                                                                         \
    {                                                                                          \
        for (uint8_t i = 0; i < sizeof(buf); i++)                                              \
            ((uint8_t *)(&FLASH_MAP))[GET_FLASH_MEMBER_ADDR(name) + i] = ((uint8_t *)&buf)[i]; \
        WriteSystemParmData(-FLASH_MAP_ADDR_POS, (uint8_t *)&FLASH_MAP, sizeof(FLASH_MAP));    \
    } while (0)

#define FlashGetData(name, buf)                                                                \
    do                                                                                         \
    {                                                                                          \
        for (uint8_t i = 0; i < sizeof(buf); i++)                                              \
            ((uint8_t *)&buf)[i] = ((uint8_t *)(&FLASH_MAP))[GET_FLASH_MEMBER_ADDR(name) + i]; \
    } while (0)

#define STimerEnter(name, time)                              \
    {                                                        \
        static tick_sec_t name##_sec_tick;                   \
        if (GetSystemSecondTick() - name##_sec_tick >= time) \
        {
#define STimerExit(name)                     \
    name##_sec_tick = GetSystemSecondTick(); \
    }                                        \
    }

typedef QFsm SystemTag;
typedef bool BOOL;
typedef uint32_t tick_sec_t;

typedef enum
{
    eSystemEventIdle = Q_USER_SIG,

    // system
    eSystemEvenCalibrationSmoke,
    eSystemEvenCalibrationCO,
    eSystemEvenCalibrationFinish,
    eSystemEventLowPwr,
    eSystemEventEndLife,
    eSystemEventWakeup,
    eSystemEventLowPwrCancel,

    // key
    eSystemEventKey,
    eSystemEventLongPressKey,
    eSystemEventDoubleKey,
    eSystemEventReleaseKey,
    eSystemEventThreeKey,
    eSystemEventFourKey,
    eSystemEventPairClik,
    eSystemEventPairLongPressKey,
    eSystemEventPairFourKey,
    eSystemEventTestOver,
    eSystemEventLcdUpdate,
    eSystemEventRadioKey,

    // smoke
    eSystemEventSmokeAlarm,
    eSystemEventSmokeMuteExitTimeout,
    eSystemEventSmokeAlarmCancel,
    eSystemEventSmokeErr,
    eSystemEventSmokeBeepOK,
    eSystemEventSmokeMuteTimeout,
    eSystemEventSmokeTestOver,
    eSystemEventSmokeAlarmCancelExit,
    eSystemEventTestSmokeOverWait,

    // co
    eSystemEventCOAlarm,
    eSystemEventCOAlarmCancel,
    eSystemEventCOBeepOK,
    eSystemEventCOTestOver,
    eSystemEventCOErr,
    eSystemEventCOMuteTimeout,
    eSystemEventStartIndexEnd,
    eSystemEventSkipBootAnimation,
    eSystemEventPeakDisplay,
    eSystemEventTestCoOverWait,
    eSystemEventCoAlarmCancelExit,
    eSystemEventCoMuteExit,

    // net
    eSystemEventNetMute,
    eNETEventCancleMute,

    //rf
    Q_eSystemEventPairSuccess,
    Q_eIntranetEventPairAnswer,
    Q_eIntranetEventPairRequest,
    Q_eIntranetEventPairOldRequest,
    Q_eIntranetEventMasterTest,
    Q_eIntranetEventRadioRxTest,
    Q_eIntranetEventForwardTest,
    Q_eIntranetEventCancelTest,
    Q_eIntranetEventSync,
    Q_eIntranetEventCancelSync,
    Q_eIntranetEventSmokeAlarm,
    Q_eIntranetEventCoAlarm,
    Q_eIntranetEventCancelAlarm,
    Q_eIntranetEventMute,
    Q_eIntranetEventCancelMute,
    Q_eIntranetEventWaitCmdTimeOut,
    Q_eIntranetEventForwardTimeout,
    Q_eIntranetEventEntryIde,
    Q_eIntranetEventSlavePair,
    Q_eIntranetEventMasterPair,
    Q_eIntranetEventAppTimeOut,
    Q_eIntranetEventBroadcastSmokeAlarm,
    Q_eIntranetEventBroadcastCoAlarm,
    Q_eIntranetEventMuteMasterAlarm,
    Q_eIntranetEventForwardSmokeAlarm,
    Q_eIntranetEventForwardCoAlarm,
    Q_eIntranetEventReceiveTest,
    eNETEventPairTimeout,
    eSystemEventNetCheckSelf,
    Q_eIntranetEventForwaedCancelTestTimeout,
    Q_eIntranetEventPairSlave,
    Q_eIntranetEventPairMaster,
    Q_AppStopRfTest,
    Q_AppStartRfTest,
    Q_AppRfSmokeAlarm,
    Q_AppRfCoAlarm,
    Q_AppCancelRfSmokeAlarm,
    Q_AppCancelRfCoAlarm,
    Q_AppCancelRfForwardAlarm,

} eSystemEvent;



typedef enum
{
    eActiveFlagNull = (uint32_t)0,
    eActiveFlagKey = (uint32_t)1<<0,
    eActiveFlagLcd = (uint32_t)1<<1,
    eActiveFlagLed = (uint32_t)1<<2,
    eActiveFlagCO = (uint32_t)1<<3,
    eActiveFlagTimeout = (uint32_t)1<<4,
    eActiveFlagBeep = (uint32_t)1<<5,
    eActiveFlagSmokeBeep = (uint32_t)1<<6,
    eActiveFlagCOBeep = (uint32_t)1<<7,
    eActiveFlagNetBeepLed = (uint32_t)1<<8,
    eActiveFlagNetTimeOut = (uint32_t)1<<9,
    eActiveFlagWiatCmdTimeOut = (uint32_t)1<<10,
    eActiveFlagKeyDouble = (uint32_t)1<<11,
    eActiveFlagCoMute = (uint32_t)1<<12,
    eActiveFlagForwardCancle = (uint32_t)1<<13,
    eActiveFlagGREEN= (uint32_t)1<<14,
    eActiveFlagSmoke = (uint32_t)1<<15,
    eActiveFlagAppTimeOut = (uint32_t)1<<16,
    eActiveFlagCOBeepDelay = (uint32_t)1<<17,
    eActiveFlagSmokePreCheck = (uint32_t)1<<18,
    eSystemEventUartTx = (uint32_t)1<<19,
    eActiveFlagPair = (uint32_t)1<<20,
    eActiveFlagNetForWordTimeOut = (uint32_t)1<<21,
    eActiveFlagNetBusy = (uint32_t)1<<22,
    eActiveFlagNetLed = (uint32_t)1<<23,
    eActiveFlagForceIde = (uint32_t)1<<24,
    eActiveFlagNetAlarm = (uint32_t)1<<25,
    
    eActiveFlagAll = ~(int)0,
} eActiveFlag;

typedef enum _eEventFlag
{
    eEventFlagLowBat = 1 << 0,
    eEventFlagFault = 1 << 1,
    eEventFlagEndOfLife = 1 << 2,
    eEventFlagMAX = 0xFF,
} eEventFlag;

#pragma pack(1)

struct SmokeFlashMap
{   
    uint16_t SmokeAlarmLevel;
    uint8_t SmokeCariFlag;     // smoke校机标志
    uint16_t SmokeCariData;    // smoke校机数据
    uint16_t SmokeCariBattery; // 校机电量
};

struct CoFlashMap
{
    uint8_t COCariFlag;      // CO校机标志
    uint16_t COCaliGradient; // CO校机斜率
    uint16_t COCaliOffset;   // CO校机B值
};

struct SystemFlashMap
{
    uint16_t LifeTimeDays; // 产品生命时间
    uint8_t NetModeFlag;   // NET Flag
    uint8_t NetFactory;    // NET产测
};
#pragma pack()

extern struct SmokeFlashMap SmokeFlashMap;
extern struct CoFlashMap CoFlashMap;
extern struct SystemFlashMap SystemFlashMap;

void DataXor(void *dest,const void *src,uint8_t n);
void SystemSetIdleNUm(uint32_t cnt);
void SystemSetTick(void);
uint32_t SystemGetTick(void);
uint32_t SystemGetIdleNUm(void);
uint16_t SystemGetLifeDays(void);
tick_sec_t GetSystemSecondTick(void);
void SystemClearActiveFlag(eActiveFlag flag);
void SystemSetActiveFlag(eActiveFlag flag);
uint32_t SystemGetActiveFlag(uint32_t Flag);
void System_init(void);
void SystemUpdateTime(void);
BOOL SystemPowerConsumptionCheck(void);
void WriteSystemParmData(int32_t pos, uint8_t *buf, uint16_t buf_size);
void ReadSystemParmData(int32_t pos, uint8_t *buf, uint16_t buf_size);
#endif
