
#include "app_smoke.h"
#include "common.h"
// #include "app_rf.h"
#include "modules_config.h"
#include "bsp_config.h"
#ifdef SUPPORT_CO_FUNCTION
#include "app_co.h"
#endif

#ifdef SUPPORT_SMOKE_FUNCTION
#define FLASH_MAP SmokeFlashMap
#define FLASH_MAP_ADDR_POS FLASH_MAP_SMOKE_ADDR_POS

#define SMOKE_CHECK_TIME (3000)
#define GET_TEMP() SensorGetTemperature()

static BOOL AlarmPreFlg = true; // 预报警
static SmokeTag SmokeObject;

const uint16_t SmokeTempRate[] = {
    SM_TN_20, SM_TN_19, SM_TN_18, SM_TN_17, SM_TN_16,
    SM_TN_15, SM_TN_14, SM_TN_13, SM_TN_12, SM_TN_11,
    SM_TN_10, SM_TN_9, SM_TN_8, SM_TN_7, SM_TN_6,
    SM_TN_5, SM_TN_4, SM_TN_3, SM_TN_2, SM_TN_1,
    SM_TP_0,
    SM_TP_1, SM_TP_2, SM_TP_3, SM_TP_4, SM_TP_5,
    SM_TP_6, SM_TP_7, SM_TP_8, SM_TP_9, SM_TP_10,
    SM_TP_11, SM_TP_12, SM_TP_13, SM_TP_14, SM_TP_15,
    SM_TP_16, SM_TP_17, SM_TP_18, SM_TP_19, SM_TP_20,
    SM_TP_21, SM_TP_22, SM_TP_23, SM_TP_24, SM_TP_25,
    SM_TP_26, SM_TP_27, SM_TP_28, SM_TP_29, SM_TP_30,
    SM_TP_31, SM_TP_32, SM_TP_33, SM_TP_34, SM_TP_35,
    SM_TP_36, SM_TP_37, SM_TP_38, SM_TP_39, SM_TP_40,
    SM_TP_41, SM_TP_42, SM_TP_43, SM_TP_44, SM_TP_45,
    SM_TP_45, SM_TP_47, SM_TP_48, SM_TP_49, SM_TP_50,
    SM_TP_51, SM_TP_52, SM_TP_53, SM_TP_54, SM_TP_55,
    SM_TP_56, SM_TP_57, SM_TP_58, SM_TP_59, SM_TP_60,
    SM_TP_61, SM_TP_62, SM_TP_63, SM_TP_64, SM_TP_65,
    SM_TP_66, SM_TP_67, SM_TP_68, SM_TP_69, SM_TP_70};

static void PollingSmokeBeep(TmrHdl Handle, eTmrCmd eCmd);
static void PollingSmokeTimeOut(TmrHdl Handle, eTmrCmd eCmd);

uint16_t SmokeBatteryCompensate(uint16_t temp)
{
    // uint16_t battery = temp;

    temp += (int16_t)temp * (((int16_t)SmokeObject.SmokeCailBattery - getBatteryVol())) / 10000;

    // debug("bat = %d, smoke = %d, comp = %d", getBatteryVol(), battery, temp);

    return temp;
}

/**
 * @brief Smoke 参数初始化
 *
 * @return SmokeTag*
 */
SmokeTag *SmokeInit(void)
{
    SmokeTag *me = &SmokeObject;

    me->State = eSmokeNO;
    me->SmokeDetCnt = 0;
    me->SmokeInactCnt = 0;
    me->SmokeTransFaultCnt = 0;
    me->SmokeSensortive = 0;
    me->SmokingCnt = 0;
    me->SmokeDustFlag = 0;

    FlashGetData(SmokeCariBattery, SmokeObject.SmokeCailBattery);
    if (SmokeObject.SmokeCailBattery == 0xffff || SmokeObject.SmokeCailBattery == 0)
        SmokeObject.SmokeCailBattery = 3200;

    // FlashSetData(SmokeCariFlag, SmokeObject.SmokeDetCnt);
    return me;
}
REG_APP_INIT(SmokeInit);

/**
 * @brief smoke-数据获取
 *
 * @param State
 * @return uint16_t
 */
uint16_t SmokeGetData(uint8_t State)
{
    uint32_t DiffData = 0;
    SmokeTag *pSmoke = &SmokeObject;

    pSmoke->SmokeData = SmokeGetLdData(IRCTR_ON_TEST_OFF);
    pSmoke->SmokeData.Diff = SmokeBatteryCompensate(pSmoke->SmokeData.Diff);
#ifdef SUPPORT_SMOKETEMPCOMPENSATION
    DiffData = (uint32_t)pSmoke->SmokeData.Diff * SmokeTempRate[GET_TEMP()];
    pSmoke->SmokeData.Diff = DiffData / 1000;
#endif

    debug_uart("\nT:%dC\nD:%d, DVol:%.3f   L:%d, LVol:%.3f   L-D:%d, D_LVol:%.3f\nA%d, Sen = %d\n",
               SensorGetTemperature() - 20, pSmoke->SmokeData.Dark, (float)pSmoke->SmokeData.Dark / 1000,
               pSmoke->SmokeData.Light, (float)pSmoke->SmokeData.Light / 1000,
               pSmoke->SmokeData.Diff, (float)pSmoke->SmokeData.Diff / 1000,
               SmokeObject.AlarmLevel, SmokeObject.SmokeSensortive);

    return pSmoke->SmokeData.Diff;
}

/**
 * @brief smoke-校机数据检查
 *
 * @param SmokeData 差值
 */
static void SmokeCalibrationCheck(uint16_t SmokeData)
{
    if ((SmokeData > SMOKE_CALIBRATION_SUCESS_MIN) &&
        (SmokeData < SMOKE_CALIBRATION_SUCESS_MAX) &&
        (SmokeObject.SmokeData.Dark < SMOKE_DARK_AVERAGE) &&
        (SmokeObject.SmokeData.Light < SMOKE_LIGHT_AVERAGE))
    {
        SmokeObject.State = eSmokeCailOk;
        SmokeObject.SmokeSensortive = SmokeData + SmokeObject.AlarmLevel;
        FlashSetData(SmokeCariData, SmokeData);
        SystemPostFIFI(eSystemEvenCalibrationFinish);

        SmokeObject.SmokeCailBattery = getBatteryVol();
        FlashSetData(SmokeCariBattery, SmokeObject.SmokeCailBattery);
        log_noinfo("avg=%d,bat=%d", SmokeData, getBatteryVol());
    }
    else
    {
        SmokeObject.State = eSmokeCailFail;
        SystemPostFIFI(eSystemEvenCalibrationFinish);
        log_noinfo("smoke cail fail");
        log_noinfo("avg=%d(%d~%d)", SmokeData, SMOKE_CALIBRATION_SUCESS_MIN, SMOKE_CALIBRATION_SUCESS_MAX);
        log_noinfo("d=%d(<%d)", SmokeObject.SmokeData.Dark, SMOKE_DARK_AVERAGE);
        log_noinfo("l=%d(<%d)", SmokeObject.SmokeData.Light, SMOKE_LIGHT_AVERAGE);
    }
}

/**
 * @brief 定时器-预报警加速检测
 *
 * @param Handle
 * @param eCmd
 */
void PollingSmokePreAlarmCheck(TmrHdl Handle, eTmrCmd eCmd)
{
    switch (eCmd)
    {
    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagSmokePreCheck);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagSmokePreCheck);
        break;

    case eTimeOut:
        SmokeCheckEvt();
        break;

    default:
        break;
    }
}

/**
 * @brief Smoke 报警控制 -T3
 *
 * @param Handle
 * @param eCmd
 */
static void PollingSmokeBeep(TmrHdl Handle, eTmrCmd eCmd)
{
    uint32_t SmokeMode = (uint32_t)TmrGetCtx(Handle);
    static uint8_t i = 0;

    switch (eCmd)
    {
    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagSmokeBeep);
        break;

    case eDelTimer:
        i = 0;
        SystemClearActiveFlag(eActiveFlagSmokeBeep);
        break;

    case eTimeOut:
        if (i++ < 3)
        {
            switch (SmokeMode)
            {
            case eSmokeNetAlarm:
                TmrInsertTimer(GetTmrHd(), TMR_PERIOD(510), PollingSmokeTimeOut, (void *)eSmokeNetAlarm);
            case eSmoked:
                BeepPollingHandle(500);
            case eSmokeMute:
                LedPollingHandle(LED_TYPE_RED, 500);
                break;
            }
        }
        else
        {
            i = 0;
            // TmrDeleteTimer(Handle);
            SystemPostFIFI(eSystemEventSmokeBeepOK);
            SmokeCheckEvt();
            printf("D = %d,L = %d\n", SmokeObject.SmokeData.Dark, SmokeObject.SmokeData.Light);
        }
        break;

    default:
        break;
    }
}

/**
 * @brief smoke 时间控制
 *
 * @param Handle
 * @param eCmd
 */
static void PollingSmokeTimeOut(TmrHdl Handle, eTmrCmd eCmd)
{
    uint32_t state = (uint32_t)TmrGetCtx(Handle);
    static uint32_t SmokeData = 0;
    static uint16_t cnt = 0;

    switch (eCmd)
    {
    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagSmoke);
        break;

    case eDelTimer:
        cnt = 0;
        SystemClearActiveFlag(eActiveFlagSmoke);
        break;

    case eTimeOut:
        switch (state)
        {

        case eSmokeCancel:
            if (cnt < 5)
            {
                LedPollingHandle(LED_TYPE_GREEN, 500);
                cnt++;
            }
            else
            {
                SystemPostFIFI(eSystemEventSmokeAlarmCancelExit);
                TmrDeleteTimer(Handle);
            }
            break;

        case eSmokeMuteTimeOut:
            if (cnt < 3)
            {
                LedPollingHandle(LED_TYPE_GREEN, 500);
                cnt++;
            }
            else
            {
                SystemPostFIFI(eSystemEventSmokeMuteExitTimeout);
                TmrDeleteTimer(Handle);
            }
            break;

        case eSmokeMute:
            cnt++;
            debug_uart("smoke mute = %d\n", cnt);
            if (cnt >= SMOKE_MUTE_TIME_NUM)
            {
                cnt = 0;
                TmrDeleteTimer(Handle);
                TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingSmokeBeep));
                SystemPostFIFI(eSystemEventSmokeMuteTimeout);
            }
#ifdef SUPPORT_LCD_FUNCTION
            else if (1 == cnt) // 关闭背光
            {
                LCD_BACK_LIGHT_OFF();
            }
#endif
            break;

        case eSmokeNetAlarm:
            LedPollingHandle(LED_TYPE_GREEN, 400);
            TmrDeleteTimer(Handle);
            break;

        case eSmokeTest:
            if (++cnt > 3)
            {
                cnt = 0;
                TmrDeleteTimer(Handle);
                TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), PollingSmokeBeep, (void *)eSmoked);
            }
            break;

        case eSmokeCailEnumMax: // cail smoke
            (SmokeObject.State != eSmokeCailOk) ? (LedPollingHandle(LED_TYPE_RED, 50)) : (LedPollingHandle(LED_TYPE_GREEN, 50));
            SmokeGetData(IRCTR_ON_TEST_OFF);
            break;

        case eSmokeCail:
            if (SensorTemperatureCheck())
            {
                if (cnt < 16)
                {
                    LedPollingHandle(LED_TYPE_YELLOW, 50);
                    SmokeData += SmokeGetData(IRCTR_ON_TEST_OFF);
                    log_noinfo("d=%d, l=%d, l-d=%d", SmokeObject.SmokeData.Dark, SmokeObject.SmokeData.Light, SmokeObject.SmokeData.Diff);
                    cnt++;
                }
                else
                {
                    cnt = 0;
                    SmokeData = SmokeData >> 4;
                    SmokeCalibrationCheck((uint16_t)SmokeData);
                    SmokeData = 0;
                    TmrDeleteTimer(Handle);
                }
            }
            else
            {
                log_noinfo("temp err=%d", (int8_t)SensorGetTemperature() - 20);
                SmokeObject.State = eSmokeCailTempErr;
                SystemPostFIFI(eSystemEvenCalibrationFinish);
                TmrDeleteTimer(Handle);
            }
            break;
        }
        break;

    default:
        break;
    }
}

/******************************************************************************
**Description:			smoke environment compensation every 3hours
** Parameters:                        light minus dark
**     Return:   						none
*******************************************************************************/
static void SmokeEnvironmentCompensation(uint16_t SmokeData)
{
    static uint8_t SmokeCompensationCnt = 0;
    static uint16_t SmokeCompensationData = 0;
    uint16_t Sensortive = 0;

    SmokeCompensationCnt++;
    SmokeCompensationData += SmokeData;

    debug_uart("SmokeCompensationData: %d,Cnt: %d\n",
               SmokeCompensationData / SmokeCompensationCnt, SmokeCompensationCnt);

    if (SmokeCompensationCnt >= 16)
    {
        SmokeCompensationCnt = 0;
        SmokeCompensationData >>= 4;
        Sensortive = SmokeObject.SmokeSensortive - SmokeObject.AlarmLevel;
        if ((SmokeCompensationData < (Sensortive + (SmokeObject.AlarmLevel >> SMOKE_CALIBRATION_DUST_INCREASE))) &&
            (SmokeCompensationData > (Sensortive - (SmokeObject.AlarmLevel >> SMOKE_CALIBRATION_DUST_INCREASE))))
        {

            SmokeObject.SmokeSensortive = SmokeCompensationData + SmokeObject.AlarmLevel;

            debug_uart("compensation success!Sensortive:%d", SmokeObject.SmokeSensortive);

            if (SmokeObject.SmokeSensortive < SYSTEM_LDO_VOL)
            {
                SmokeObject.SmokeDustFlag = 0;
            }
            else
            {
                SmokeObject.SmokeSensortive = SYSTEM_LDO_VOL;
                SmokeCompensationData = SmokeObject.SmokeSensortive - SmokeObject.AlarmLevel;
            }
            FlashSetData(SmokeCariData, SmokeCompensationData);
        }

        SmokeCompensationData = 0;
    }
}

/******************************************************************************
**Description:				check if 	alarm	one time
** Parameters:                  none
**     Return:   					smoke state
*******************************************************************************/
static uint16_t SmokeAlarmCheck(void)
{
    uint16_t ret = eSmokeNO; // 0 :normal 1:smoke 2:err
    uint16_t SmokeData = SmokeGetData(IRCTR_ON_TEST_OFF);

#if !defined TEMPERATURE_HIGH_LOW_TEST
    /* 加速检测 */
    if (SmokeData >= SmokeObject.SmokeSensortive - (SmokeObject.AlarmLevel >> SMOKE_CALIBRATION_DUST_INCREASE))
    {
        ret = eSmoking;
        if (AlarmPreFlg && (SmokeObject.SmokeDustFlag == 0) && (SmokeObject.State != eSmoked))
        {
            AlarmPreFlg = false;
            TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_CHECK_TIME), PollingSmokePreAlarmCheck, NULL);
        }
    }
    else
    {
        if (!AlarmPreFlg)
        {
            AlarmPreFlg = true;
            TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingSmokePreAlarmCheck));
        }
    }

    if (SmokeData >= SmokeObject.SmokeSensortive)
    {
        ret = eSmoked;
    }
    else
    {

        if ((SmokeData < SMOKE_ERR_VALUE) || (SmokeObject.SmokeData.Dark > SMOKE_CALIBRATION_SUCESS_MAX) ||
            (SmokeObject.SmokeData.Dark > SmokeObject.SmokeData.Light))
        {
            ret = eSmokeErr;
        }
        else
        {
            uint32_t SlpTimeCnt = SystemGetIdleNUm();
            if ((SlpTimeCnt % SMOKE_EnvironmentCompensation_COUNT) == (SMOKE_EnvironmentCompensation_COUNT - 1))
            {
                SmokeEnvironmentCompensation(SmokeData);
            }
        }
    }
#else
#warning smoke alarm
#endif

    return ret;
}

/******************************************************************************
**Description:			check smoke if error
** Parameters:                      none
**     Return:   				error or right
*******************************************************************************/
uint16_t SmokeCheckEvt(void)
{
    uint16_t Ret = 0;
    uint16_t SmokeSate = SmokeAlarmCheck();

    switch (SmokeSate)
    {
    case eSmokeNO:
        SmokeObject.SmokeInactCnt++;
        SmokeObject.SmokeDetCnt = 0;
        SmokeObject.SmokeTransFaultCnt = 0;
        SmokeObject.SmokingCnt = 0;
        if (SmokeObject.SmokeInactCnt > SMOKE_Cancel_Cnt)
        {
            if (SmokeObject.State != eSmokeNO)
            {
                SystemPostFIFI(eSystemEventSmokeAlarmCancel);
                SmokeObject.State = eSmokeNO;
                SmokeObject.SmokeInactCnt = SMOKE_Check_Cnt;
            }
            if (SmokeObject.SmokeDustFlag)
            {
                SmokeObject.SmokeDustFlag = 0;
            }
        }
        break;

    case eSmoked:
        SmokeObject.SmokeDetCnt++;
        SmokeObject.SmokeTransFaultCnt = 0;
        SmokeObject.SmokeInactCnt = 0;
        SmokeObject.SmokingCnt = 0;
        if (SmokeObject.SmokeDetCnt >= SMOKE_Check_Cnt)
        {
            if (SmokeObject.State != eSmoked)
            {
                SmokeObject.SmokeDetCnt = SMOKE_Check_Cnt;
                SystemPostFIFI(eSystemEventSmokeAlarm);
                SmokeObject.State = eSmoked;

                if (!AlarmPreFlg)
                {
                    AlarmPreFlg = true;
                    TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingSmokePreAlarmCheck));
                }
            }
            Ret = 1;
        }
        break;

    case eSmokeErr:
        SmokeObject.SmokeTransFaultCnt++;
        SmokeObject.SmokeInactCnt = 0;
        SmokeObject.SmokeDetCnt = 0;
        SmokeObject.SmokingCnt = 0;
        if (SmokeObject.SmokeTransFaultCnt > SMOKE_Check_Cnt)
        {
            if (SmokeObject.State != eSmokeErr)
            {
                SystemPostFIFI(eSystemEventSmokeErr);
                SmokeObject.State = eSmokeErr;
                SmokeObject.SmokeTransFaultCnt = SMOKE_Check_Cnt;
            }
        }
        break;

    case eSmoking:
        if (SmokeObject.SmokingCnt > 1000) // 1 hour
        {
            SmokeObject.SmokingCnt = 0;
            SmokeObject.SmokeDustFlag = 1; // 长时间处于预加速监测
            /* 预加速检测超过一定时间删除检测的定时器 */
            if (!AlarmPreFlg)
            {
                AlarmPreFlg = true;
                TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingSmokePreAlarmCheck));
            }
        }
        else
        {
            SmokeObject.SmokingCnt++;
        }
        break;
    }

    return Ret;
}
REG_APP_WAKE_PROC(SmokeCheckEvt);


/**
 * @brief 烟雾自检模式
 *
 * @param handle
 */
void SmokeKeyTestHandle(uint8_t handle)
{
    uint16_t SmokeState = 0;
    switch (handle)
    {
    /*test start*/
    case 0:
        SmokeState = SmokeAlarmCheck();
        {
            debug_uart("D=%d , L=%d , L-D=%d ,Sen = %d , A%d\n", SmokeObject.SmokeData.Dark,
                       SmokeObject.SmokeData.Light, SmokeObject.SmokeData.Diff,
                       SmokeObject.SmokeSensortive, SmokeObject.AlarmLevel);
        }
        if (eSmokeErr == SmokeState)
        {
            SystemPostFIFI(eSystemEventSmokeErr);
        }
        else
        {
            TmrInsertTimer(GetTmrHd(), TMR_PERIOD(450), PollingSmokeTimeOut, (void *)eSmokeTest);
#if defined SUPPORT_LCD_FUNCTION
            TmrInsertTimer(GetTmrHd(), TMR_PERIOD(500), PollingLineDisplay, NULL);
#endif
        }
        break;

    /*test over*/
    case 1:

        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingSmokeBeep));
#ifdef SUPPORT_LCD_FUNCTION
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingLineDisplay));
        LCDStandyDisplay(0, true);
#endif

#ifdef SUPPORT_CO_FUNCTION
        SystemPostFIFI(eSystemEventSmokeTestOver);
#else
        SystemPostFIFI(eSystemEventTestCoOverWait);
#endif
        debug_uart("smoke tst ov\n");
        break;
    }
}

/******************************************************************************
**Description:				SystemSmokeError
** Parameters:           me :AO     e : event
**     Return:   			none
*******************************************************************************/
void SystemSmokeError(SystemTag *me, QEvent *e)
{
    switch (e->sig)
    {
    case eSystemEventWakeup:
        SysTimeUpdate();
        SystemPollingIdle();
        SystemLedFlicker(SysLedErr);
        SECTION_CALL(APP_WAKE_PROC);
        break;

    case Q_ENTRY_SIG:
        printf("%s in\n", __FUNCTION__);
        ResetLedTick();
#ifdef SUPPORT_LCD_FUNCTION
        LCD_DisplayStr(eStr_Err); //"Err"
#endif
        SmokeFault();
        EndLifeIndex(1, LED_TYPE_YELLOW, 1, 2, 50);
        SmokeObject.State = eSmokeErr;
        break;

    case Q_EXIT_SIG:
        printf("%s out\n", __FUNCTION__);
        CancelFault();
        SmokeObject.State = eSmokeNO;
        break;

#ifdef SUPPORT_CO_FUNCTION
    case eSystemEventCOAlarm:
        Q_TRAN(SystemCOAlarm);
        break;
#endif

    case eSystemEventSmokeAlarmCancel:
        Q_TRAN(SystemIdle);
        break;

    case eSystemEventKey:
#ifdef SUPPORT_LCD_FUNCTION
        LCD_DisplayStr(eStr_Err); //"Err"
#endif
        EndLifeIndex(1, LED_TYPE_YELLOW, 1, 2, 50);
        break;

    default:
        break;
    }
}

/******************************************************************************
**Description:				smoke mute state handle
** Parameters:                 object and event
**     Return:   					none
*******************************************************************************/
static void SystemSmokeMute(SystemTag *me, QEvent *e)
{
    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s in\n", __FUNCTION__);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_MUTE_TIME), PollingSmokeTimeOut, (void *)eSmokeMute);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), PollingSmokeBeep, (void *)eSmokeMute);
        break;

    case Q_EXIT_SIG:
        printf("%s out\n", __FUNCTION__);
        SmokeObject.State = eSmokeNO;
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingSmokeTimeOut));
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingSmokeBeep));
        break;

#ifdef SUPPORT_NET_FUNCTION
    case eNETEventCancleMute:
        Q_TRAN(SystemKeyTestHandle);
        break;
#endif

    case eSystemEventSmokeMuteTimeout:
#ifdef SUPPORT_LCD_FUNCTION
        LCD_BACK_LIGHT_ON();
#endif
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), PollingSmokeTimeOut, (void *)eSmokeMuteTimeOut);
        break;

    case eSystemEventSmokeMuteExitTimeout:
#ifdef SUPPORT_LCD_FUNCTION
        LCD_BACK_LIGHT_OFF();
#endif
        CancelMute();
        Q_TRAN(SystemIdle);
        break;

#if defined SUPPORT_RADIO_FUNCTION
    case Q_eIntranetEventSmokeAlarm:
        if (GetRadioPairMode() != RF_PAIR_MODE_NONE)
        {
            NetSetSmokeAlarmParm();
            Q_TRAN(AppNetAlarmFunc);
        }
        break;

    case Q_eIntranetEventCoAlarm:
        if (GetRadioPairMode() != RF_PAIR_MODE_NONE)
        {
            NetSetCoAlarmParm();
            Q_TRAN(AppNetAlarmFunc);
        }
        break;
#endif

    default:
        break;
    }
}

/******************************************************************************
**Description:					smoke alarm state handle
** Parameters:                   object and event
**     Return:   					none
*******************************************************************************/

void SystemSmokeAlarm(SystemTag *me, QEvent *e)
{
    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        printf("%s in\n", __FUNCTION__);
#ifdef SUPPORT_LCD_FUNCTION
        LCD_BACK_LIGHT_ON();
        LCDStandyDisplay(0, true);
#endif
        SmokeAlarm();
        SystemPostFIFI(Q_AppRfSmokeAlarm);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), PollingSmokeBeep, ((void *)eSmoked));
        break;

    case Q_EXIT_SIG:
        printf("%s out\n", __FUNCTION__);
#ifdef SUPPORT_LCD_FUNCTION
        LCD_BACK_LIGHT_OFF();
#endif
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingSmokeBeep));
        break;

    case eSystemEventKey:
    case eSystemEventNetMute:
    #if defined EnterMute || defined MuteSuccess
        (e->sig == eSystemEventKey) ? (EnterMute()) : (MuteSuccess());
    #endif
        SystemPostFIFI(Q_AppCancelRfSmokeAlarm);
        Q_TRAN(SystemSmokeMute);
        break;

    case eSystemEventSmokeAlarmCancel:
        SystemPostFIFI(Q_AppCancelRfSmokeAlarm);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingSmokeBeep));
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_BEEP_TIME), PollingSmokeTimeOut, (void *)eSmokeCancel);
        break;

    case eSystemEventSmokeAlarmCancelExit:
        CancelAlarm();
        Q_TRAN(SystemIdle);
        break;

    default:
        break;
    }
}

/******************************************************************************
**Description:		SmokeGetyDifferenceMode
** Parameters:         pSmoke:  smokeAO           SmokeData  :  getsmokedata
**     Return:   			ret :state
*******************************************************************************/
uint8_t SmokeGetyDifferenceMode(SmokeTag *pSmoke, uint16_t SmokeData)
{
    uint8_t ret = 0;
    uint16_t Data = 0;

    Data = ((pSmoke->SmokeSensortive - SmokeObject.AlarmLevel) >= SmokeData) ? (pSmoke->SmokeSensortive - SmokeObject.AlarmLevel - SmokeData) : (SmokeData - (pSmoke->SmokeSensortive - SmokeObject.AlarmLevel));

    if (Data < SMOKE_LIGHT_DARK_AVERAGE)
    {
        printf("OK	");
        ret = 1;
    }
    else
    {
        printf("NG	");
        ret = 0;
    }
    printf(" Camp Output Difference= %d\n", Data);
    return ret;
}

/**
 * @brief smoke-校机结果处理
 *
 */
void SmokeCailResHandle()
{
    // printf("Smoke CailRes = %d\n", SmokeObject.State);
#ifdef SUPPORT_LCD_FUNCTION
    TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingLineDisplay));
#endif

    switch (SmokeObject.State)
    {
    case eSmokeCailOk:
        FlashSetData(SmokeCariFlag, SmokeObject.State);
        break;

#ifdef SUPPORT_LCD_FUNCTION
    case eSmokeCailTempErr:
        LCD_DisplayStr(eStr_E06); // temp err
        break;

    case eSmokeCailBatErr:
        LCD_DisplayStr(eStr_E11); // bat err
        break;

    case eSmokeCailFail:
        LCD_DisplayStr(eStr_E07); // err
        break;
#endif
    }

    hal_rtc_write(2);
    TmrInsertTimer(GetTmrIdleHd(), TMR_IDLE_PERIOD(SMOKE_CALIBRATION_ERR_TIME), PollingSmokeTimeOut, (void *)eSmokeCailEnumMax);
}

static int shellAlarmDataSet(int value)
{
    SmokeObject.AlarmLevel = value;

    FlashSetData(SmokeAlarmLevel, SmokeObject.AlarmLevel);

    return SmokeObject.AlarmLevel;
}

static int shellAlarmDataGet()
{
    return SmokeObject.AlarmLevel;
}
static ShellNodeVarAttr shellAlarmData = {
    .get = shellAlarmDataGet,
    .set = shellAlarmDataSet};
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_VAR_NODE),
                 AlarmData, &shellAlarmData, "Alarm Leve");

/**
 * @brief 检查上位机设置报警阈值是否符合设定范围
 *
 * @return true
 * @return false
 */
bool CheckSmokeAlramLevel(void)
{
    uint16_t AlarmData = 0;

    FlashGetData(SmokeAlarmLevel, AlarmData);
    DataXor(&SmokeObject.AlarmLevel, &AlarmData, sizeof(AlarmData));

    if ((SmokeObject.AlarmLevel >= SMOKE_SET_ALARMLEVEL_MIN) && (SmokeObject.AlarmLevel <= SMOKE_SET_ALARMLEVEL_MAX))
    {
        // printf("AlarmLevel Ok\n");
        return true;
    }
    else
    {
        printf("AlarmLevel Err = %d!!!\n", SmokeObject.AlarmLevel);
        while (1)
        {
            IWG_FEED();
            EndLifeIndex(1, LED_TYPE_RED, 1, 4, 50);
        }
    }
}

/**
 * @brief smoke -校机状态函数
 *
 * @param me
 * @param e
 */
void SmokeFactoryMode(SystemTag *me, QEvent *e)
{
    uint8_t SmokeCialFlg = 0XFF;
    uint16_t SmokeDifData = 0;

    switch (e->sig)
    {
    case eSystemEventWakeup:
        SysTimeUpdate();
        SystemPollingIdle();
        break;

    case Q_ENTRY_SIG:
        // debug("%s,in\n", __FUNCTION__);

        /*灵敏度阈值设定检查*/
        CheckSmokeAlramLevel();

        SmokeObject.State = eSmokeCailEnumMax;
        FlashGetData(SmokeCariFlag, SmokeCialFlg);
        FlashGetData(SmokeCariData, SmokeDifData);

        if (SmokeCialFlg == SMOKE_CALI_STATE)
        // if(1)
        {
            SmokeObject.SmokeSensortive = SmokeDifData + SmokeObject.AlarmLevel;
            SmokeGetData(IRCTR_ON_TEST_OFF);
            // SmokeGetyDifferenceMode(&SmokeObject,SmokeDifData);
            printf("\nD = %d,L = %d,Diff = %d\n", SmokeObject.SmokeData.Dark, SmokeObject.SmokeData.Light,
                   SmokeObject.SmokeData.Diff);
            printf("\nSmokeCailData:\n");
            printf("AlarmThreshold = %d,CailDiff = %d,Sens = %d,CailVol = %dmv\n", SmokeObject.AlarmLevel,
                   SmokeDifData, SmokeObject.SmokeSensortive, SmokeObject.SmokeCailBattery);

#ifdef SUPPORT_CO_FUNCTION
            SystemPostFIFI(eSystemEvenCalibrationCO);
#else
            Q_TRAN(SystemStartHandle);
#endif
        }
        else
        {
            if ((BatteryGetData() > BATTERY_CHECK_MIN) && (BatteryGetData() < BATTERY_CHECK_MAX)) // 3~3.3V
            {
                log_noinfo("smoke calibration start");
                // SetUartOpen(true);
                /* 进行烟感校机 */
                TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_CALIBRATION_TIME), PollingSmokeTimeOut, (void *)eSmokeCail);
#ifdef SUPPORT_LCD_FUNCTION
                TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_CALIBRATION_LCD_TIME), PollingLineDisplay, NULL);
#endif
            }
            else
            {
                log_noinfo("bat=%dmv. range out %d~%d", getBatteryVol(), BATTERY_CHECK_MIN, BATTERY_CHECK_MAX);
                SmokeObject.State = eSmokeCailBatErr;
                SystemPostFIFI(eSystemEvenCalibrationFinish);
            }
        }
        break;

    case Q_EXIT_SIG:
        SmokeObject.State = eSmokeNO;
        break;

    case eSystemEvenCalibrationFinish:
        SmokeCailResHandle();
        break;

    case eSystemEvenCalibrationCO:
#ifdef SUPPORT_CO_FUNCTION
        Q_TRAN(COFactoryMode); // init over ,tran to factory mode
#else
        Q_TRAN(SystemStartHandle);
#endif
        break;

    default:
        break;
    }
}

#endif /* end of SUPPORT_SMOKE_FUNCTION */
