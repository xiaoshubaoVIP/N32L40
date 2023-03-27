#include <stdlib.h>
#include <string.h>
#include "app_rf.h"
#include "common.h"
#include "app_main.h"
#include "app_co.h"
#include "modules_config.h"
#include "bsp_config.h"
#include "app_smoke.h"

#ifdef SUPPORT_RADIO_FUNCTION
#include "radio/radio_api.h"
#include "radio/inc/radio_2300a_spi.h"
#include "radio/inc/radio_protocol.h"

#define POLLING_SEND_MSG_INTERVAL_TIME 50
#define DELAY_SEND_MSG_INTERVAL_TIME 100
#define TX_DELAY_SHORT_TIME 25
#define TX_DELAY_LONG_TIME 5200

// polling
#define TimeOut10ms ((uint32_t)10)
#define TimeOut50ms ((uint32_t)50)
#define TimeOut200ms ((uint32_t)200)
#define TimeOut500ms ((uint32_t)500)
#define TimeOut1s ((uint32_t)1000)
#define TimeOut2_5s ((uint32_t)2500)
#define TimeOut3s ((uint32_t)3000)
#define TimeOut4s ((uint32_t)4000)
#define TimeOut5s ((uint32_t)5000)
#define TimeOut10s ((uint32_t)10000)
#define TimeOut30s ((uint32_t)30000)
#define TimeOut60s ((uint32_t)60000)
#define TimeOut120s ((uint32_t)120000)
#define TimeOut180s ((uint32_t)180000)
#define TimeOut300s ((uint32_t)300000)
#define Timeout1200s ((uint32_t)1200000) // 20min
#define Timeout1hour ((uint32_t)3600000) // 60min

#define FLASH_MAP_RADIO_ADDR_POS 6144
#define FLASH_MAP RadioFlashMap
#define FLASH_MAP_ADDR_POS FLASH_MAP_RADIO_ADDR_POS
#pragma pack(1)
struct RadioFlashMap
{
    uint8_t Udid[4];       // udid 滚码烧录
    uint8_t IntranetId[4]; // 自组网id
    uint8_t NetModeFlag;   // 配网模式
};
#pragma pack()

struct RadioFlashMap RadioFlashMap;

static NetTag NetAO;

/**
 * @brief 加载flash存储数据
 *
 */
void LoadRadioFlash(void)
{
    /*获取RF Flash信息*/
    hal_flash_read(flash_data_start_physical_address() - FLASH_MAP_RADIO_ADDR_POS,
                   (uint8_t *)&RadioFlashMap, sizeof(RadioFlashMap));

    printbuff(((uint8_t *)&RadioFlashMap), sizeof(struct RadioFlashMap), "RF Flash:%x\n",
              flash_data_start_physical_address() - FLASH_MAP_RADIO_ADDR_POS);

    NetAO.RadioState = eSystemState_Idle;
    NetAO.PairMode = RadioFlashMap.NetModeFlag;
    printf("RF_PairMode = %d\n", NetAO.PairMode);

    DataXor(NetAO.udid, RadioFlashMap.Udid, sizeof(RadioFlashMap.Udid));
    printbuff(NetAO.udid, sizeof(NetAO.udid), "Udid: ");

    switch (NetAO.PairMode)
    {
    case RF_PAIR_MODE_INTRANET:
        // DataXor(tmpid,RadioFlashMap.IntranetId,sizeof(RadioFlashMap.IntranetId));
        SetRadioID(RadioFlashMap.IntranetId);
        printbuff(RadioFlashMap.IntranetId, sizeof(RadioFlashMap.IntranetId), "Intranet_ID = ");
        break;

    case RF_PAIR_MODE_NONE:
        SetRadioID(NetAO.udid);
        debug_warning("not pair!\n");
        break;

    case RF_PAIR_MODE_HUB:
        break;

    default:
        break;
    }

#if defined(SUPPORT_X_SENSE_EN)
    printf("<X-Sense> [Intranet_Freq=869.275M]\n");
#elif defined(SUPPORT_X_SENSE_UL)
    printf("<X-Sense> [Intranet_Freq=915.275M]\n");
#elif defined(SUPPORT_AEGISLINK_EN)
    printf("<Aegislink> [Intranet_Freq=869.675M]\n");
#elif defined(SUPPORT_AEGISLINK_UL)
    printf("<Aegislink> [Intranet_Freq=915.675M]\n");
#endif
}

/**
 * @brief 配网信息flash存储
 *
 * @param mode
 */
void AppRF_SaveFlashInfo(uint8_t mode)
{
    uint8_t netid[4] = {0};

    switch (mode)
    {
    case RF_PAIR_MODE_INTRANET:
        NetAO.PairMode = RF_PAIR_MODE_INTRANET;
        FlashSetData(NetModeFlag, NetAO.PairMode);
        memcpy(netid, GetRadioID(), sizeof(netid));
        FlashSetData(IntranetId, netid);
        break;

    case RF_PAIR_MODE_NONE:
        NetAO.PairMode = RF_PAIR_MODE_NONE;
        FlashSetData(NetModeFlag, NetAO.PairMode);
        FlashSetData(IntranetId, NetAO.udid);
        break;

    default:
        break;
    }

    // reload data
    LoadRadioFlash();
}

/**
 * @brief 模块初始化
 *
 */
void RadioInit(void)
{
    RadioWakeUpPinCfg();
    SetRadioMode(LowPowerIntranetMode);
    LoadRadioFlash();
}
REG_APP_INIT(RadioInit);

/**
 * @brief 设置模块状态
 *
 * @param state
 */
void SetRadioState(eRadioState state)
{

    NetAO.RadioState = state;
}

/**
 * @brief 获取模块状态
 *
 * @param state
 */
uint8_t GetRadioState(void)
{
    return NetAO.RadioState;
}

/**
 * @brief 设置模块组网运行模式
 *
 * @param State
 */
void SetRadioPairMode(uint8_t State)
{
    NetAO.PairMode = State;
}

/**
 * @brief 获取当前配网模式
 *
 * @return uint8_t
 */
uint8_t GetRadioPairMode(void)
{
    return NetAO.PairMode;
}

uint16_t GetRandDelayTime(eDelayMode DelayMode)
{
    uint16_t Seed = 0;
    uint16_t RandValue1, RandValue2 = 0;
    uint16_t RandValue = 0;

    // Seed = Bt_Cnt16Get(TIM0);
    // Delay(Seed);
    Seed = hal_get_tim_cnt(TIM_LPTIM);
    srand(Seed);

    if (DelayMode == eLowpowerTimeMode)
    {
        RandValue = (rand() % 35 + 5) * DELAY_SEND_MSG_INTERVAL_TIME + TX_DELAY_LONG_TIME;
    }
    else if (DelayMode == eNormalTimeMode)
    {
        RandValue = (rand() % 50 + 5) * POLLING_SEND_MSG_INTERVAL_TIME + 2000; // 25*(5~50)
    }
    else if (DelayMode == eLowpowerIntranetMode)
    {
        RandValue1 = rand() % 400 + 1;
        DelayUs(Seed);
        Seed = hal_get_tim_cnt(TIM_LPTIM);
        RandValue2 = rand() % 300 + 1;

        RandValue = (RandValue1 * 5) + (RandValue2 * 10);
    }
    else if (DelayMode == eNormalIntranetMode)
    {
        RandValue = (rand() % 200 + 1) * 5;
    }

    printf("T' = %d\n", RandValue);
    return RandValue;
}

void RadioIdleEnter(void)
{
    if (GetRadioPairMode() == ePairedHub)
        SetRadioMode(LowPowerHubMode);
    else
        SetRadioMode(LowPowerIntranetMode);

    SetRadioState(eSystemState_Idle);
}

void RadioIdleExit(void)
{
    if (GetRadioPairMode() == ePairedHub)
        SetRadioMode(NormalHubMode);
    else
        SetRadioMode(NormalIntranetMode);
}

void AppTimeout(TmrHdl Handle, eTmrCmd eCmd)
{
    uint32_t state = (uint32_t)TmrGetCtx(Handle);

    switch (eCmd)
    {
    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagAppTimeOut);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagAppTimeOut);
        break;

    case eTimeOut:
        SystemPostFIFI(Q_eIntranetEventAppTimeOut);
        TmrDeleteTimer(Handle);
        break;

    default:
        break;
    }
}

/**
 * @brief 随机定时器-用于错开时间转发数据，防止数据冲突
 *
 * @param Handle
 * @param eCmd
 */
void IntranetPollingForwardTest(TmrHdl Handle, eTmrCmd eCmd)
{
    uint32_t state = (uint32_t)TmrGetCtx(Handle);

    switch (eCmd)
    {
    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagNetForWordTimeOut);
        break;

    case eTimeOut:
        QEP_Post(Q_eIntranetEventForwardTimeout);
        TmrDeleteTimer(Handle);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagNetForWordTimeOut);
        break;

    default:
        break;
    }
}

void PollingNetLedFlicker(TmrHdl Handle, eTmrCmd eCmd)
{
    uint32_t state = (uint32_t)TmrGetCtx(Handle);

    switch (eCmd)
    {
    case eInsertTimer:
        LedPollingHandle(LED_TYPE_RED, state);
        SystemSetActiveFlag(eActiveFlagNetLed);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagNetLed);
        break;

    case eTimeOut:
        LedPollingHandle(LED_TYPE_RED, state);
        break;

    default:
        break;
    }
}

static void PollingLedGreen(TmrHdl Handle, eTmrCmd eCmd)
{
    uint32_t Time = (uint32_t)TmrGetCtx(Handle);
    switch (eCmd)
    {
    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagNetLed);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagNetLed);
        break;

    case eTimeOut:
        LedPollingHandle(LED_TYPE_GREEN, Time);
        TmrDeleteTimer(Handle);
        break;

    default:
        break;
    }
}

void PollingNetTestAction(TmrHdl Handle, eTmrCmd eCmd)
{
    uint32_t state = (uint32_t)TmrGetCtx(Handle);

    switch (eCmd)
    {

    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagNetBeepLed);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagNetBeepLed);
        break;

    case eTimeOut:
        BeepPollingHandle(200);
        LedPollingHandle(LED_TYPE_RED, 200);
        switch (state)
        {
        case 1:
            TmrInsertTimer(GetTmrHd(), TMR_PERIOD(210), PollingLedGreen, ((void *)200));
            break;
        }
        break;

    default:
        break;
    }
}

/**
 * @brief 等待其余操作打断当前准备要发送的数据
 *
 * @param Handle
 * @param eCmd
 */
void IntranetPollingWaitCmd(TmrHdl Handle, eTmrCmd eCmd)
{
    uint32_t state = (uint32_t)TmrGetCtx(Handle);

    switch (eCmd)
    {
    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagWiatCmdTimeOut);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagWiatCmdTimeOut);
        break;

    case eTimeOut:
        SystemPostFIFI(Q_eIntranetEventWaitCmdTimeOut);
        if (state == NeedDelTmr)
        {
            TmrDeleteTimer(Handle);
        }
        break;

    default:
        break;
    }
}

/**
 * @brief 超时强制进入待机模式
 *
 * @param Handle
 * @param eCmd
 */
void IntranetPollingForcedEntryIde(TmrHdl Handle, eTmrCmd eCmd)
{
    switch (eCmd)
    {
    case eTimeOut:
        SystemPostFIFI(Q_eIntranetEventEntryIde);
        TmrDeleteTimer(Handle);
        break;

    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagForceIde);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagForceIde);
        break;

    default:
        break;
    }
}

static void IntranetPollingCancelTest(TmrHdl Handle, eTmrCmd eCmd)
{
    switch (eCmd)
    {
    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagForwardCancle);
        break;

    case eTimeOut:
        SystemPostFIFI(Q_eIntranetEventForwaedCancelTestTimeout);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagForwardCancle);
        break;

    default:
        break;
    }
}

void IntranetPollingPair(TmrHdl Handle, eTmrCmd eCmd)
{
    static uint8_t TimeCnt = 0;
    static uint8_t ReTxCnt = 0;
    uint32_t state = (uint32_t)TmrGetCtx(Handle);

    switch (eCmd)
    {
    case eInsertTimer:
        TimeCnt = 0;
        ReTxCnt = 0;
        SystemSetActiveFlag(eActiveFlagNetTimeOut);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagNetTimeOut);
        break;

    case eTimeOut:
        TimeCnt++;
        switch (state)
        {
        case eSystemState_PairFactory:
            if (ReTxCnt++ > 10)
            {
                SystemPostFIFI(eNETEventPairTimeout);
                TmrDeleteTimer(Handle);
            }
            else
            {
                SendFactoryPair();
            }
            break;

        case eSystemState_PairSlave:
            if (TimeCnt < 10)
            {
                SendStartPair();
            }
            else if (TimeCnt == 10)
            {
                SetRadioMode(LowPowerIntranetMode);
            }
            else if (TimeCnt == 20)
            {
                SetRadioMode(NormalIntranetMode);
                TimeCnt = 0;
            }

        case eSystemState_PairMaster:
            if (ReTxCnt++ > 60)
            {
                SystemPostFIFI(eNETEventPairTimeout);
                TmrDeleteTimer(Handle);
            }
            break;

        default:
            break;
        }

        break;

    default:
        break;
    }
}

void IntranetPollingStartAlarm(TmrHdl Handle, eTmrCmd eCmd)
{
    static uint8_t TimeCnt = 0;
    uint32_t Ctx = (uint32_t)TmrGetCtx(Handle);

    switch (eCmd)
    {
    case eInsertTimer:
        TimeCnt = 0;
        SystemSetActiveFlag(eActiveFlagNetAlarm);
        break;

    case eTimeOut:
#ifdef SUPPORT_SPEAK_LOCATION
        IntranetSetRadioParamer(&SendMsgParamer, (eInternatMsgType)AlarmLocation, NormalIntranetMode, PackNum_2, false);
        IntranetSendRadioMsg(&SendMsgParamer, true);
        HalDelayMs(20);
#endif

        if (Ctx == SmokeAlarm)
        {
            IntranetSend(NormalIntranetMode, PackNum_1, eIntranetRadioMsgSmokeAlarm);
        }
        else if (Ctx == CoAlarm)
        {
            IntranetSend(NormalIntranetMode, PackNum_1, eIntranetRadioMsgCoAlarm);
        }

        if (++TimeCnt > 2)
        {
            TmrDeleteTimer(Handle);
            //     PollingStartAlarm.sig = Q_eIntranetEventStartEntryAlarmTimeout;
            //     QEP_Post(&PollingStartAlarm);
        }
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagNetAlarm);
        break;

    default:
        break;
    }
}

/**
 * @brief 应用显示-配对状态
 *
 * @param me
 * @param e
 */
void AppPairSlaveMode(SystemTag *me, QEvent *e)
{

    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
#ifdef SUPPORT_LCD_FUNCTION
        LCDStandyDisplay(0, false);
#endif
        BeepPollingHandle(100);
        SystemPostFIFI(Q_eIntranetEventPairSlave);

        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(60000), AppTimeout, NULL);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(250), PollingNetLedFlicker, (void *)50);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingNetLedFlicker));
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), AppTimeout));
        break;

    case Q_eSystemEventPairSuccess:
        AppRF_SaveFlashInfo(RF_PAIR_MODE_INTRANET);
        BeepPollingHandle(200);
    case eSystemEventKey:
    case eSystemEventPairClik:
    case Q_eIntranetEventAppTimeOut:
        Q_TRAN(SystemIdle);
        break;
    }
}

#if defined PAIR_KEY
void AppExitPair(SystemTag *me, QEvent *e)
{
    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
        BeepPollingHandle(200);
        LedPollingHandle(LED_TYPE_RED, 200);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(10000), AppTimeout, NULL);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        break;

    case eSystemEventPairLongPressKey:
        AppRF_SaveFlashInfo(RF_PAIR_MODE_NONE);
        LedPollingHandle(LED_TYPE_GREEN, 500);
        BeepPollingHandle(500);
        Q_TRAN(SystemIdle);
        break;

    case Q_eIntranetEventAppTimeOut:
        Q_TRAN(SystemIdle);
        break;
    }
}
#endif

void AppPairMasterMode(SystemTag *me, QEvent *e)
{

    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
#ifdef SUPPORT_LCD_FUNCTION
        LCDStandyDisplay(0, false);
#endif
        BeepPollingHandle(100);
        SystemPostFIFI(Q_eIntranetEventPairMaster);

        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(60000), AppTimeout, NULL);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(2000), PollingNetLedFlicker, (void *)1000);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingNetLedFlicker));
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), AppTimeout));
        break;

#if !(defined PAIR_KEY)
    case eSystemEventLongPressKey:

        AppRF_SaveFlashInfo(RF_PAIR_MODE_NONE);
        LedPollingHandle(LED_TYPE_GREEN, 500);
        BeepPollingHandle(500);
        Q_TRAN(SystemIdle);
        break;
#endif

    case Q_eSystemEventPairSuccess:
        AppRF_SaveFlashInfo(RF_PAIR_MODE_INTRANET);
        BeepPollingHandle(200);
    case eSystemEventKey:
    case eSystemEventPairClik:
    case Q_eIntranetEventAppTimeOut:
        Q_TRAN(SystemIdle);
        break;
    }
}

/**
 * @brief 应用层ui-响应测试
 *
 * @param me
 * @param e
 */
void AppResponeRfTestMode(SystemTag *me, QEvent *e)
{
    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
#ifdef SUPPORT_LCD_FUNCTION
        LCDStandyDisplay(0, false);
#endif
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(120000), AppTimeout, NULL);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), PollingNetTestAction, (void *)1);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingNetTestAction));
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), AppTimeout));
        break;

    case Q_eIntranetEventCancelTest:
    case eSystemEventKey:
    case Q_eIntranetEventAppTimeOut:
        Q_TRAN(SystemIdle);
        break;
    }
}

/**
 * @brief 应用层ui-联动测试
 *
 * @param me
 * @param e
 */
void AppSendRfTestMode(SystemTag *me, QEvent *e)
{
    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
        SystemPostFIFI(Q_AppStartRfTest);
#ifdef SUPPORT_LCD_FUNCTION
        LCDStandyDisplay(0, false);
        LCD_BACK_LIGHT_ON();
#endif
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(120000), AppTimeout, NULL);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), PollingNetTestAction, (void *)0);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
#ifdef SUPPORT_LCD_FUNCTION
        LCD_BACK_LIGHT_OFF();
#endif
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingNetTestAction));
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), AppTimeout));
        break;

    case eSystemEventReleaseKey:
        SystemPostFIFI(Q_AppStopRfTest);
    case Q_eIntranetEventAppTimeOut:
        Q_TRAN(SystemIdle);
        break;
    }
}

/**
 * @brief 广播测试
 *
 * @param me
 * @param e
 */
void IntranetMasterTestMode(SystemTag *me, QEvent *e)
{
    printf("sig=%d\n", e->sig);

    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
        SetRadioState(eSystemState_Test);
        SendStartTest();
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(TimeOut120s), IntranetPollingForcedEntryIde, NULL);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        SetRadioState(eSystemState_Idle);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingForcedEntryIde));
        break;

    case Q_AppStopRfTest:
        Q_TRAN(IntranetBroadcastCancelTestModeEvent);
        break;

    case Q_eIntranetEventMasterTest:
        Q_TRAN(IntranetForwardTestModeEvent);
        break;

    case Q_AppStartRfTest:
        Q_TRAN(IntranetBroadcastTestModeEvent);
        break;

    case Q_eIntranetEventEntryIde:
        Q_TRAN(RfIdeHandle);
        break;

    case Q_eIntranetEventCancelSync:
        if (GetRadioState() == eSystemState_CancelTest)
        {
            SendCancelSyncTest();
        }
        break;

    default:
        break;
    }
}

/**
 * @brief 广播取消报警
 *
 * @param me
 * @param e
 */
void IntranetBroadcastCancelAlarmModeEvent(SystemTag *me, QEvent *e)
{
    static uint8_t TimeCnt = 0;

    printf("sig=%d\n", e->sig);

    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
        TimeCnt = 0;
        SetRadioState(eSystemState_Alarm);
        CancelAlarm();
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(GetRandDelayTime(eNormalIntranetMode) +
                                     TX_DELAY_LONG_TIME), IntranetPollingWaitCmd, (void*)ReserveTmr);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        SetRadioState(eSystemState_Idle);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingWaitCmd));
        break;

    case Q_AppStartRfTest:
        Q_TRAN(IntranetBroadcastTestModeEvent);
        break;

    case Q_eIntranetEventMasterTest:
        Q_TRAN(IntranetForwardTestModeEvent);
        break;

#if defined SUPPORT_SMOKE_FUNCTION
    case Q_AppRfSmokeAlarm:
        SystemPostFIFI(Q_eIntranetEventBroadcastSmokeAlarm);
        Q_TRAN(IntranetBroadcastAlarmModeEvent);
        break;
#endif

#if defined SUPPORT_CO_FUNCTION
    case Q_AppRfCoAlarm:
        SystemPostFIFI(Q_eIntranetEventBroadcastCoAlarm);
        Q_TRAN(IntranetBroadcastAlarmModeEvent);
        break;
#endif

    case Q_eIntranetEventWaitCmdTimeOut:
        BroadcastSendCancelAlarm();
        if (TimeCnt++ > 2)
        {
            Q_TRAN(RfIdeHandle);
        }
        break;

    default:
        break;
    }
}

/**
 * @brief 广播报警
 *
 * @param me
 * @param e
 */
void IntranetBroadcastAlarmModeEvent(SystemTag *me, QEvent *e)
{
    printf("sig=%d\n", e->sig);

    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
        SetRadioState(eSystemState_Alarm);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(Timeout1hour), IntranetPollingForcedEntryIde, NULL);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        SetRadioState(eSystemState_Idle);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingStartAlarm));
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingForcedEntryIde));
        break;

    case Q_AppRfSmokeAlarm:
        SystemPostFIFI(Q_eIntranetEventBroadcastSmokeAlarm);
        Q_TRAN(IntranetBroadcastAlarmModeEvent);
        break;

    /*广播 smoke 报警*/
    case Q_eIntranetEventBroadcastSmokeAlarm:
        IntranetSend(LowPowerIntranetMode, PackNum_1, eIntranetRadioMsgSmokeAlarm);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(TimeOut200ms), IntranetPollingStartAlarm, (void *)(SmokeAlarm));
        printf("BroadCast SmokeAlram\n");
        break;

    /*广播 co 报警*/
    case Q_eIntranetEventBroadcastCoAlarm:
        IntranetSend(LowPowerIntranetMode, PackNum_1, eIntranetRadioMsgCoAlarm);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(TimeOut200ms), IntranetPollingStartAlarm, (void *)(CoAlarm));
        printf("BroadCast CoAlram\n");
        break;

    // case Q_eIntranetEventBroadcastMute:
    case Q_AppCancelRfCoAlarm:
#if defined SUPPORT_EN50291_EN14604
        if (getCoAppPpm() < COPPMLEVEL3)
#endif
        case Q_AppCancelRfSmokeAlarm:
        {
            Q_TRAN(IntranetBroadcastCancelAlarmModeEvent);
        }
        break;

        case Q_eIntranetEventMuteMasterAlarm:
            // UartDownLoadHandleFunc(eUartMsgFireAlarm, UartFireAlarmMsgMute, 0);
            SystemPostFIFI(eSystemEventKey);
        Q_TRAN(RfIdeHandle);
        break;

    default:
        break;
    }
}

void IntranetBroadcastCancelTestModeEvent(SystemTag *me, QEvent *e)
{
    static uint8_t TimeCnt1 = 0;
    static uint8_t TimeCnt2 = 0;

    printf("sig=%d\n", e->sig);

    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
        TimeCnt1 = 0;
        TimeCnt2 = 0;

        SetRadioState(eSystemState_CancelTest);
        SendCancelTest();
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(TimeOut5s), IntranetPollingCancelTest, NULL);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(TimeOut120s), IntranetPollingForcedEntryIde, NULL);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        SetRadioState(eSystemState_Idle);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingCancelTest));
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingForcedEntryIde));
        break;

    case Q_AppStartRfTest:
        Q_TRAN(IntranetBroadcastTestModeEvent);
        break;

    case Q_eIntranetEventMasterTest:
        SystemPostFIFI(Q_eIntranetEventReceiveTest);
        Q_TRAN(&IntranetForwardTestModeEvent);
        break;

    case Q_eIntranetEventForwaedCancelTestTimeout:
        IntranetSend(NormalIntranetMode, PackNum_4, eIntranetRadioMsgCancleTest);
        if (TimeCnt1++ > 2)
        {
            Q_TRAN(RfIdeHandle);
        }
        break;

    case Q_eIntranetEventCancelSync:
        SendCancelSyncTest();
        if (TimeCnt2++ > 2)
        {
            Q_TRAN(RfIdeHandle);
        }
        break;

    case Q_eIntranetEventPairMaster:
        Q_TRAN(IntranetPairMasterMode);
        break;

    case Q_eIntranetEventPairSlave:
        Q_TRAN(IntranetPairSlaveMode);
        break;

    default:
        break;
    }
}

/**
 * @brief 广播-联动测试
 *
 * @param me
 * @param e
 */
void IntranetBroadcastTestModeEvent(SystemTag *me, QEvent *e)
{
    printf("sig=%d\n", e->sig);

    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
        SetRadioMode(NormalIntranetMode);
        SetRadioState(eSystemState_Test);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), IntranetPollingWaitCmd, NULL);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        SetRadioState(eSystemState_Idle);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingWaitCmd));
        break;

    case Q_eIntranetEventWaitCmdTimeOut:
        Q_TRAN(IntranetMasterTestMode);
        break;

    case Q_AppStartRfTest:
        Q_TRAN(IntranetBroadcastTestModeEvent);
        break;

    case Q_AppStopRfTest:
        Q_TRAN(RfIdeHandle);
        break;

    case Q_eIntranetEventMasterTest:
        Q_TRAN(IntranetForwardTestModeEvent);
        break;

    /*快速取消联动测试*/
    case Q_eIntranetEventCancelTest:
        SendQuicklyCancelCmd();
        // DelayMs(2);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingWaitCmd));
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), IntranetPollingWaitCmd, NULL);
        break;
    }
}

void IntranetAlarmModeEvent(SystemTag *me, QEvent *e)
{

    printf("sig=%d\n", e->sig);

    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
        SetRadioState(eSystemState_Alarm);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(Timeout1hour), IntranetPollingForcedEntryIde, NULL);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        SetRadioState(eSystemState_Idle);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingForcedEntryIde));
        break;

    case Q_eIntranetEventEntryIde:
        Q_TRAN(RfIdeHandle);
        break;

    case Q_AppCancelRfForwardAlarm:
    case eSystemEventSmokeAlarmCancel:
    case eSystemEventCOAlarmCancel:
        Q_TRAN(IntranetBroadcastCancelAlarmModeEvent);
        break;

    case Q_eIntranetEventCancelAlarm:
        Q_TRAN(&IntranetForwardCancelAlarmModeEvent);
        break;

    default:
        break;
    }
}

/**
 * @brief 转发取消报警
 *
 * @param me
 * @param e
 */
void IntranetForwardCancelAlarmModeEvent(SystemTag *me, QEvent *e)
{
    printf("sig=%d\n", e->sig);

    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
        SetRadioState(eSystemState_CancelAlarm);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(GetRandDelayTime(eLowpowerIntranetMode) + TX_DELAY_SHORT_TIME), IntranetPollingWaitCmd, NULL);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        SetRadioState(eSystemState_Idle);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingWaitCmd));
        break;

    case Q_AppStartRfTest:
        Q_TRAN(&IntranetBroadcastTestModeEvent);
        break;

    case Q_eIntranetEventMasterTest:
        Q_TRAN(&IntranetForwardTestModeEvent);
        break;

    /* 本机触发smoke报警 */
    case Q_AppRfSmokeAlarm:
        SystemPostFIFI(Q_eIntranetEventBroadcastSmokeAlarm);
        Q_TRAN(IntranetBroadcastAlarmModeEvent);
        break;

    /* 本机触发co报警 */
    case Q_AppRfCoAlarm:
        SystemPostFIFI(Q_eIntranetEventBroadcastCoAlarm);
        Q_TRAN(IntranetBroadcastAlarmModeEvent);
        break;

    case Q_eIntranetEventSync:
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingWaitCmd));
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(GetRandDelayTime(eLowpowerIntranetMode) + TX_DELAY_LONG_TIME),
                       IntranetPollingWaitCmd, NULL);
        break;

    case Q_eIntranetEventCancelSync:
        SendCancelAlarmSync();
        Q_TRAN(RfIdeHandle);
        break;

    case Q_eIntranetEventWaitCmdTimeOut:
        ForwardCancelAlarm();
        Q_TRAN(RfIdeHandle);
        break;

    default:
        break;
    }
}

/**
 * @brief 转发报警
 *
 * @param me
 * @param e
 */
void IntranetForwardSmokeAlarmModeEvent(SystemTag *me, QEvent *e)
{
    uint8_t ForwardAlarmtype = eIntranetRadioMsgSmokeAlarm;

    printf("sig=%d\n", e->sig);

    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
        SetRadioState(eSystemState_Alarm);
        SetRadioMode(NormalIntranetMode);

        TmrInsertTimer(GetTmrHd(),TMR_PERIOD(GetRandDelayTime(eLowpowerIntranetMode) + 
                                    TX_DELAY_LONG_TIME), IntranetPollingWaitCmd, NULL);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        SetRadioState(eSystemState_Idle);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingWaitCmd));
        break;

    case Q_eIntranetEventForwardCoAlarm:
        ForwardAlarmtype = eIntranetRadioMsgCoAlarm;
        break;

    case Q_eIntranetEventForwardSmokeAlarm:
        ForwardAlarmtype = eIntranetRadioMsgSmokeAlarm;
        break;

    /* 本机触发smoke报警 */
    case Q_AppRfSmokeAlarm:
        SystemPostFIFI(Q_eIntranetEventBroadcastSmokeAlarm);
        Q_TRAN(IntranetBroadcastAlarmModeEvent);
        break;

    /* 本机触发co报警 */
    case Q_AppRfCoAlarm:
        SystemPostFIFI(Q_eIntranetEventBroadcastCoAlarm);
        Q_TRAN(IntranetBroadcastAlarmModeEvent);
        break;

    case Q_AppCancelRfCoAlarm:
    case Q_AppCancelRfSmokeAlarm:
        Q_TRAN(&IntranetBroadcastCancelAlarmModeEvent);
        break;

    case Q_AppCancelRfForwardAlarm:
        Q_TRAN(&IntranetBroadcastCancelAlarmModeEvent);
        break;

    case Q_eIntranetEventCancelAlarm:
        Q_TRAN(IntranetForwardCancelAlarmModeEvent);
        break;

    case Q_eIntranetEventSync:
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingWaitCmd));
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(GetRandDelayTime(eLowpowerIntranetMode) + 
                                        TX_DELAY_LONG_TIME), IntranetPollingWaitCmd, NULL);        
        break;			

    case Q_eIntranetEventWaitCmdTimeOut:
        if (ForwardAlarmtype == eIntranetRadioMsgSmokeAlarm)
        {
            ForwardSmokeAlarm();
            printf("forward SmokeAlarm\n");
        }
        else
        {
            ForwardCoAlarm();
            printf("forward CoAlarm\n");
        }
        Q_TRAN(IntranetAlarmModeEvent);
        break;

    default:
        break;
    }
}

/**
 * @brief 转发取消测试
 *
 * @param me
 * @param e
 */
void IntranetForwardCancelTestModeEvent(SystemTag *me, QEvent *e)
{
    printf("sig=%d\n", e->sig);

    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
        SetRadioState(eSystemState_Test);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(TimeOut120s), IntranetPollingForcedEntryIde, NULL);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(GetRandDelayTime(eLowpowerIntranetMode) + TX_DELAY_SHORT_TIME),
                       IntranetPollingWaitCmd, NULL);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        SetRadioState(eSystemState_Idle);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingWaitCmd));
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingForcedEntryIde));
        break;

    case Q_eIntranetEventEntryIde:
        Q_TRAN(RfIdeHandle);
        break;

    case Q_eIntranetEventMasterTest:
        SystemPostFIFI(Q_eIntranetEventReceiveTest);
        Q_TRAN(&IntranetForwardTestModeEvent);
        break;

    case Q_AppStartRfTest:
        Q_TRAN(&IntranetBroadcastTestModeEvent);
        break;

    case Q_eIntranetEventPairMaster:
        Q_TRAN(IntranetPairMasterMode);
        break;

    case Q_eIntranetEventPairSlave:
        Q_TRAN(IntranetPairSlaveMode);
        break;

    case Q_eIntranetEventSync:
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingWaitCmd));
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(GetRandDelayTime(eLowpowerIntranetMode) + TX_DELAY_LONG_TIME),
                       IntranetPollingWaitCmd, NULL);
        break;

    case Q_eIntranetEventCancelSync:
        // DelayMs(2);
        SendCancelSyncTest();
        Q_TRAN(RfIdeHandle);
        break;

    case Q_eIntranetEventWaitCmdTimeOut:
        SendCancelSyncTest();
        Q_TRAN(RfIdeHandle);
        break;

    default:
        break;
    }
}

/**
 * @brief RF转发测试
 *
 * @param me
 * @param e
 */
void IntranetForwardTestModeEvent(SystemTag *me, QEvent *e)
{
    static bool Flag = true;

    printf("sig=%d\n", e->sig);

    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
        Flag = true;
        SetRadioMode(NormalIntranetMode);
        SetRadioState(eSystemState_Test);

        /*随机定时转发数据*/
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(GetRandDelayTime(eLowpowerIntranetMode) + TX_DELAY_LONG_TIME),
                       IntranetPollingForwardTest, NULL);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(TimeOut120s), IntranetPollingForcedEntryIde, NULL);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        SetRadioState(eSystemState_Idle);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingForcedEntryIde));
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingForwardTest));
        break;

    case Q_eIntranetEventForwardTimeout:
        if (GetRadioState() == eSystemState_Test)
        {
            Flag = false;
            SendForwardTest();
        }
        break;

    case Q_eIntranetEventCancelTest:
        Q_TRAN(IntranetForwardCancelTestModeEvent);
        break;

    case Q_eIntranetEventMasterTest:
        Q_TRAN(IntranetForwardTestModeEvent);
        break;

    case Q_AppStartRfTest:
        Q_TRAN(&IntranetBroadcastTestModeEvent);
        break;

    /*同步-收到后重新计算延时再转发，仅转发一次*/
    case Q_eIntranetEventSync:
        if (Flag == true)
        {
            TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingForwardTest));
            TmrInsertTimer(GetTmrHd(), TMR_PERIOD(GetRandDelayTime(eLowpowerIntranetMode) + TX_DELAY_LONG_TIME), IntranetPollingForwardTest, NULL);
        }
        break;

    case eSystemEventKey:
    case Q_eIntranetEventEntryIde:
        Q_TRAN(RfIdeHandle);
        break;
    }
}

/**
 * @brief 配对状态-工厂配工程宝模式
 *
 * @param me
 * @param e
 */
void IntranetPairFactoryMode(SystemTag *me, QEvent *e)
{
    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
        SetRadioMode(FactoryPairMode);
        BeepPollingHandle(100);
        SetRadioState(eSystemState_Pair);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(500), PollingNetLedFlicker, (void *)100);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), IntranetPollingPair, (void *)eSystemState_PairFactory);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        SetRadioState(eSystemState_Idle);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingNetLedFlicker));
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingPair));
        break;

    case Q_eIntranetEventPairAnswer:
        SendFactoryPairACK();
        // save id
        AppRF_SaveFlashInfo(RF_PAIR_MODE_INTRANET);
        BeepPollingHandle(200);
        LedPollingHandle(LED_TYPE_GREEN, 200);
        goto exit;

    case eSystemEventKey:
    case eNETEventPairTimeout:

    exit:
#ifdef SUPPORT_SMOKE_FUNCTION
        Q_TRAN(&SmokeFactoryMode); // init over ,tran to factory mode
#else
#ifdef SUPPORT_CO_FUNCTION
        Q_TRAN(&COFactoryMode); // init over ,tran to factory mode
#else
        Q_TRAN(&SystemStartHandle); // init
#endif
#endif
        break;
    }
}

/**
 * @brief 配对状态-发送模式
 *
 * @param me
 * @param e
 */
void IntranetPairSlaveMode(SystemTag *me, QEvent *e)
{
    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
        // SetRadioMode(NormalIntranetMode);
        SetRadioState(eSystemState_Pair);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), IntranetPollingPair, (void *)eSystemState_PairSlave);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        SetRadioState(eSystemState_Idle);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingPair));
        break;

    case Q_eIntranetEventPairAnswer:
        SystemPostFIFI(Q_eSystemEventPairSuccess);
        // DelayMs(2);
    case eSystemEventKey:
    case eNETEventPairTimeout:
        Q_TRAN(RfIdeHandle);
        break;
    }
}

/**
 * @brief 配对状态-接收模式
 *
 * @param me
 * @param e
 */
void IntranetPairMasterMode(SystemTag *me, QEvent *e)
{
    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
        SetRadioMode(NormalIntranetMode);
        SetRadioState(eSystemState_Pair);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), IntranetPollingPair, (void *)eSystemState_PairMaster);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        SetRadioState(eSystemState_Idle);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), IntranetPollingPair));
        break;

    case Q_eIntranetEventPairRequest:
        SendPairACK();
        SystemPostFIFI(Q_eSystemEventPairSuccess);
        // DelayMs(2);
    case eSystemEventKey:
    case eSystemEventLongPressKey:
    case eNETEventPairTimeout:
        Q_TRAN(RfIdeHandle);
        break;
    }
}

/**
 * @brief 射频性能测试-fsk、ook、rx
 *
 * @param me
 * @param e
 */
void RadioTestHandle(SystemTag *me, QEvent *e)
{
    static uint8_t TestStep = 0;

    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        TestStep = 0;
        SetRadioState(eSystemState_RadioTest);
        printf("Test OOK\n");
        SetRadioMode(OokTestMode);
        break;

    case Q_EXIT_SIG:
        SetRadioState(eSystemState_Idle);
        break;

    case eSystemEventRadioKey:
        TestStep++;
        switch (TestStep)
        {
        case 1:
            printf("Test Fsk\n");
            SetRadioMode(FskTestMode);
            break;

        case 2:
            printf("Test Rx\n");
            SetRadioMode(RxTestMode);
            break;

        case 3:
            printf("goto ide\n");
            SetRadioMode(LowPowerIntranetMode);
            Q_TRAN(RfIdeHandle);
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}

/**
 * @brief RF 待机模式处理 - 仅处理RF状态
 *
 * @param me
 * @param e
 */
void RfIdeHandle(SystemTag *me, QEvent *e)
{
    switch (e->sig)
    {

        // case eSystemEventWakeup:
        //     printf("rf ide\n");
        //     break;

    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        break;

    case Q_eIntranetEventPairSlave:
        Q_TRAN(IntranetPairSlaveMode); // Slave
        break;

    case Q_eIntranetEventPairMaster:
        Q_TRAN(IntranetPairMasterMode); // Master
        break;

    case Q_AppStartRfTest:
        if (GetRadioPairMode() != RF_PAIR_MODE_NONE)
        {
            Q_TRAN(IntranetBroadcastTestModeEvent);
        }
        else
        {
            // 网关配对
        }
        break;

    case Q_AppRfSmokeAlarm:
        if (GetRadioPairMode() != RF_PAIR_MODE_NONE)
        {
            SystemPostFIFI(Q_eIntranetEventBroadcastSmokeAlarm);
            Q_TRAN(IntranetBroadcastAlarmModeEvent);
        }
        break;

    case Q_AppRfCoAlarm:
        if (GetRadioPairMode() != RF_PAIR_MODE_NONE)
        {
            SystemPostFIFI(Q_eIntranetEventBroadcastCoAlarm);
            Q_TRAN(IntranetBroadcastAlarmModeEvent);
        }
        break;

    case Q_eIntranetEventMasterTest:
    case Q_eIntranetEventForwardTest:
        if (GetRadioPairMode() != RF_PAIR_MODE_NONE)
        {
            SystemPostFIFI(Q_eIntranetEventReceiveTest);
            Q_TRAN(IntranetForwardTestModeEvent);
        }
        break;

    case Q_eIntranetEventSmokeAlarm:
        if (GetRadioPairMode() != RF_PAIR_MODE_NONE)
        {
            SystemPostFIFI(Q_eIntranetEventForwardSmokeAlarm);
            Q_TRAN(IntranetForwardSmokeAlarmModeEvent);
        }
        break;

    case Q_eIntranetEventCoAlarm:
        if (GetRadioPairMode() != RF_PAIR_MODE_NONE)
        {
            SystemPostFIFI(Q_eIntranetEventForwardCoAlarm);
            Q_TRAN(IntranetForwardSmokeAlarmModeEvent);
        }
        break;

    case eSystemEventRadioKey:
        Q_TRAN(RadioTestHandle);
        break;
    }
}

/**
 * @brief 状态机初始化
 *
 * @param me
 * @param e
 */
void RfObjectInit(SystemTag *me, QEvent const *e)
{
    Q_INIT(RfIdeHandle);
}

struct
{
    uint8_t is_alternat_blink;
    uint8_t beep_num;
    uint16_t beep_time;
    uint16_t led_time;
    uint16_t cycle_time;
} net_alarm_parm;

void NetSetCoAlarmParm(void)
{
    net_alarm_parm.is_alternat_blink = 0;
    net_alarm_parm.beep_num = 4;
    net_alarm_parm.beep_time = 100;
    net_alarm_parm.led_time = 100;
    net_alarm_parm.cycle_time = 4700;
}

void NetSetSmokeAlarmParm(void)
{
    net_alarm_parm.is_alternat_blink = 1;
    net_alarm_parm.beep_num = 3;
    net_alarm_parm.beep_time = 500;
    net_alarm_parm.led_time = 500;
    net_alarm_parm.cycle_time = 1000;
}

static void PollingNETBeepHandler(TmrHdl Handle, eTmrCmd eCmd)
{
    static uint8_t count;
    static uint8_t NetAlarmFlg = 0;

    switch (eCmd)
    {
    case eInsertTimer:
        count = 0;
        NetAlarmFlg = 0;
        SystemSetActiveFlag(eActiveFlagNetBeepLed);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagNetBeepLed);
        break;

    case eTimeOut:
        if ((!NetAlarmFlg) && (count++ < net_alarm_parm.beep_num))
        {
            LedPollingHandle(LED_TYPE_RED, net_alarm_parm.led_time);
            BeepPollingHandle(net_alarm_parm.beep_time);

            if (net_alarm_parm.is_alternat_blink == 1)
            {
                TmrInsertTimer(GetTmrHd(), TMR_PERIOD((net_alarm_parm.led_time) + 10), PollingLedGreen, (void *)400);
            }
        }
        else
        {
            count = 0;
            NetAlarmFlg = !NetAlarmFlg;

            if (NetAlarmFlg)
            {
                SystemPostFIFI(eSystemEventCOBeepOK);
                TmrSetCurCnt(Handle, TMR_PERIOD(net_alarm_parm.cycle_time));

                if (net_alarm_parm.is_alternat_blink == 0)
                {
                    TmrInsertTimer(GetTmrHd(), TMR_PERIOD((net_alarm_parm.beep_time << 1) + 10), PollingLedGreen, (void *)net_alarm_parm.led_time);
                }
            }
            else
            {
                TmrSetCurCnt(Handle, TMR_PERIOD(net_alarm_parm.beep_time << 1));
            }
        }

        break;

    default:
        break;
    }
}

void NetAlarmPolling(TmrHdl Handle, eTmrCmd eCmd)
{
    switch (eCmd)
    {
    case eTimeOut:
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(net_alarm_parm.beep_time << 1), PollingNETBeepHandler, NULL);
        break;

    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagNetBeepLed);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagNetBeepLed);
        break;

    default:
        break;
    }
}

void AppNetAlarmFunc(SystemTag *me, QEvent *e)
{
    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
#ifdef SUPPORT_LCD_FUNCTION
        LCDStandyDisplay(getCoAppPpm(), true);
#endif
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(net_alarm_parm.beep_time << 1), PollingNETBeepHandler, NULL);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingNETBeepHandler));
        break;

    case eSystemEventWakeup:
        SysTimeUpdate();
        //        AlarmCollectPpm();
        break;

    case eSystemEventCOBeepOK:
#if defined SUPPORT_SMOKE_FUNCTION
        SmokeCheckEvt();
#endif
#if defined SUPPORT_CO_FUNCTION
        COAlarmCheck();
#endif
        break;

    case eSystemEventKey:
        SystemPostFIFI(Q_AppCancelRfForwardAlarm);
        Q_TRAN(SystemIdle);
        break;

        // case eNETEventMute:
        //     SendMuteResult(DevMuteTypeNonActive);
        //     Q_TRAN(SystemIdle);
        //     break;

    case Q_eIntranetEventCancelAlarm:
        Q_TRAN(SystemIdle);
        break;

#if defined SUPPORT_SMOKE_FUNCTION
    case eSystemEventSmokeAlarm:
        Q_TRAN(SystemSmokeAlarm);
        break;
#endif

#if defined SUPPORT_CO_FUNCTION
    case eSystemEventCOAlarm:
        Q_TRAN(SystemCOAlarm);
        break;
#endif

    default:
        break;
    }
}

#endif
