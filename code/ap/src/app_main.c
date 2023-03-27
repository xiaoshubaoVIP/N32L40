/****************************************************************************/
/*	File   :             F:\Smoker\XP01\branches\src\Ap\main.c               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2020/4/29,10:50:29                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      XP01                                    */
/****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "app_main.h"
#include "common.h"
#include "modules_config.h"
#include "bsp_config.h"
#include "app_rf.h"

#ifdef SUPPORT_SMOKE_FUNCTION
#include "app_smoke.h"
#endif

#ifdef SUPPORT_CO_FUNCTION
#include "app_co.h"
#endif

#if defined SUPPORT_RADIO_FUNCTION
#include "app_rf.h"
#endif

#define FLASH_MAP SystemFlashMap
#define FLASH_MAP_ADDR_POS FLASH_MAP_SYSTEM_ADDR_POS

// product life time
#define LIFE_TIME (3650 + 100)     // 10*365   10 years+100 day(safe victor)
#define LIFE_TIME_MUTE (30)        //  30 day
#define LCD_SHOW_TOTAL_TIME 30     // 10*30 =5min
#define ENDOFLIFE_MUTE_TIME (1320) //    //22h = 60*22 = 1320

#ifndef LED_IDLE_TICK
#define LED_IDLE_TICK 1
#endif

typedef enum
{
    eDeviceOK = 0,
    eDeviceErr,
    eDevicePreEndofLife,
    eDevicePreEndofLifeMute,
    eDeviceEndofLife,
    eDeviceStarting,
    eDeviceLowBattery,
    eDeviceLowBatteryMute,
    eDeviceMax,

} eDeviceState;

struct System
{
#ifdef SUPPORT_LCD_FUNCTION
    BOOL LcdSwitchFlag;
#endif
    BOOL UartEnableFlag;            // uart switch OPEN WHEN KEY TEST  LONG PRESS
    BOOL PowerConsumptionCheckFlag; // factory  test  flag
    uint16_t DeviceState;           //
    uint16_t SilenceTime;           // Life end of time silence
    uint16_t LowPwrSilenceFlag;     // Low silence Flag
    uint16_t LowPwrSilence;         // Low silence Time
    uint32_t ActiveFlag;            // active or idle flag
    tick_sec_t led_tick;
};

/* Private define ------------------------------------------------------------*/
#define SYSTEM_TIMEOUT_TIME (10800000) // 3h
#define SYSTEM_TIMEOUT_NUM (1)         // 3*Num
#define SYSTEM_LED_TIME (500)
#define SYSTEM_CANCEL_ALARM_TIME (1000)
#define SYSTEM_SMOKE_CHECK_TIME (3000)

#define KEY_TEST_DELAY_TIME (1) // S
#define ENDLIFE_MODE_CYCLE_COUNT (60)

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static struct System SystemObject;
TmrHdl SytemTimeOutTimer;

static ShellNodeVarAttr shelluart = {
    .get = (int (*)())IsUartOpen,
    .set = (int (*)())SetUartOpen};

/* Private function prototypes -----------------------------------------------*/
void SystemIdle(SystemTag *me, QEvent *e);
void SystemKeyTestHandle(SystemTag *me, QEvent *e);
void SytemTimeOutHandle(TmrHdl Handle, eTmrCmd eCmd);
/* Private functions ---------------------------------------------------------*/

/******************************************************************************
**Description:		OPEN UART FLAG
** Parameters:            NONE
**     Return:   		UART STATE
*******************************************************************************/
BOOL IsUartOpen(void)
{
    return SystemObject.UartEnableFlag;
}

void SetUartOpen(uint8_t flg)
{
    SystemObject.UartEnableFlag = flg;
}

static void SystemGetVersion(void)
{
#if defined SUPPORT_RADIO_FUNCTION
    printf("\n%s-RF_%s_V%s\n", BOARD_TYPE, CPU_TYPE, mcu_version);
#else
    printf("\n%s_%s_V%s\n", BOARD_TYPE, CPU_TYPE, mcu_version);
#endif
// model info
#if defined STANDARD_EN50291_EN14604
    printf("Standard: EN50291_EN14604\n");
#elif defined STANDARD_UL2034_UL217
    printf("Standard: UL2034_UL217\n");
#else
#err
#endif
}

#ifdef SUPPORT_LCD_FUNCTION

void PollingLineDisplay(TmrHdl Handle, eTmrCmd eCmd)
{
    // static uint8_t i = 0;
    static uint8_t j = 0;

    switch (eCmd)
    {
    case eInsertTimer:
        j = 0;
        SystemSetActiveFlag(eActiveFlagLcd);
        LCD_Clear();
        break;

    case eTimeOut:
        if (++j == 4)
        {
            LCD_Clear();
            j = 0;
        }
        else
        {
            LCD_SystemStartAnimation(j - 1);
        }

        break;

    case eDelTimer:
        LCD_Clear();
        SystemClearActiveFlag(eActiveFlagLcd);
        break;
    default:
        break;
    }
}

BOOL SetLcdSwitchFlag(BOOL Flag)
{
    return SystemObject.LcdSwitchFlag = Flag;
}

BOOL GetLcdSwitchFlag(void)
{
    return SystemObject.LcdSwitchFlag;
}

void Lcd_CloseCheck(void)
{
    if (!(GetLcdSwitchFlag()) && (!IsCOPPM()))
    {
        LCD_Control(eLcdOFF);
        LCD_BACK_LIGHT(0);
    }
}

#endif /* end of SUPPORT_LCD_FUNCTION */

/**
 * @brief 重置LED时间
 */
void ResetLedTick(void)
{
    SystemObject.led_tick = GetSystemSecondTick(); /* 重置led闪灯tick */
}

unsigned char GetEndLifeTime(void)
{
    return (SystemGetLifeDays() >= LIFE_TIME) ? 1 : 0;
}

void check_end_life(void)
{
    if (SystemGetLifeDays() >= LIFE_TIME)
    {
        SystemPostFIFI(eSystemEventEndLife); // for system start
        if (SystemGetLifeDays() >= LIFE_TIME + LIFE_TIME_MUTE)
        {
            SystemObject.DeviceState = eDeviceEndofLife;
        }
        else
        {
            if (SystemObject.DeviceState != eDevicePreEndofLifeMute)
            {
                SystemObject.DeviceState = eDevicePreEndofLife;
            }
        }
    }
}

void SysTimeUpdate(void)
{
    SystemUpdateTime();
    check_end_life();
}

static void showFlash(void)
{
    printbuff(((uint8_t*)&SmokeFlashMap), sizeof(struct SmokeFlashMap),
                    "Smoke Flash:%x\n", 
                    flash_data_start_physical_address() - FLASH_MAP_SMOKE_ADDR_POS);

    printbuff(((uint8_t*)&CoFlashMap), sizeof(struct CoFlashMap),
                    "Co Flash:%x\n", 
                    flash_data_start_physical_address() - FLASH_MAP_CO_ADDR_POS);

    printbuff(((uint8_t*)&SystemFlashMap), sizeof(struct SystemFlashMap),
                    "System Flash:%x\n", 
                    flash_data_start_physical_address() - FLASH_MAP_SYSTEM_ADDR_POS);

    debug_noinfo("log Flash:%x-%x\n", LOG_FLASH_START_ADDRESS, LOG_FLASH_START_ADDRESS + LOG_FLASH_LENGHT);
}

/******************************************************************************
**Description:			syetem init all models
** Parameters:                  object and evet
**     Return:   					none
*******************************************************************************/
void SystemObjectInit(SystemTag *me, QEvent const *e)
{
    System_init();

    BatteryMcuInit(); // mcu battery init

    SECTION_CALL(APP_INIT);

    SystemObject.PowerConsumptionCheckFlag = SystemPowerConsumptionCheck();
    SystemObject.DeviceState = eDeviceOK;

    SystemObject.SilenceTime = 0;
    SystemPostFIFI(eSystemEventWakeup);
    // add  for  System no idle
    SytemTimeOutTimer = TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SYSTEM_TIMEOUT_TIME), SytemTimeOutHandle, NULL);
#if defined SUPPORT_RADIO_FUNCTION
    if (hal_gpio_read(PIN_TEST) == PIN_RESET)
    {
        Q_INIT(IntranetPairFactoryMode); 
    }
    else
#endif
    {

#if defined SUPPORT_SMOKE_FUNCTION
        Q_INIT(&SmokeFactoryMode); // init over ,tran to factory mode
#elif defined SUPPORT_CO_FUNCTION
        Q_INIT(&COFactoryMode); // init over ,tran to factory mode
#else
        Q_INIT(&SystemStartHandle); // init
#endif
    }

    SensorTemperatureCheck();
    hal_rtc_write(SLEEP_TIME);

    printf("\n\n");
    SystemGetVersion(); // SYSTEM VERSION
    printf("=>\n");
    printf("Days = %d\n", SystemGetLifeDays());
    printf("bat = %d mV\n", getBatteryVol());
    printf("Temp = %dC\n", SensorGetTemperature() - 20);
    printf("=>\n");
    showFlash();
}

/******************************************************************************
**Description:			for led and beep control
** Parameters:             led type on/off flag and num,time
**     Return:   				none
*******************************************************************************/

void EndLifeIndex(uint8_t LedFlag, eLed_Type LedType, uint8_t BeepFlag, uint8_t Num, uint8_t Time)
{
    uint8_t i = 0;
    for (i = 0; i < Num; i++) // beep num times
    {
        if (LedFlag)
        {
            LedSetState(LedType, LED_ON);
        }
        if (BeepFlag)
        {
            BeepSetState(eBeepOn);
        }
        DelayMs(Time);
        if (LedFlag)
        {
            LedSetState(LedType, LED_OFF);
        }
        if (BeepFlag)
        {
            BeepSetState(eBeepOff);
        }
        DelayMs(5 * Time); // 150ms
        IWG_FEED();
    }
}

/******************************************************************************
**Description:				 led flick for idle and low battery
** Parameters:                 none
**     Return:   			   none
*******************************************************************************/
static void LedFlickerCheck(void)
{
    if (IsLowPwrErr())
    {
        SystemPostFIFI(eSystemEventLowPwr);
    }
    else
    {
        if (SystemGetActiveFlag(eActiveFlagLed) == 0)
        {
            LedSetState(LED_TYPE_GREEN, LED_ON);
            DelayMs(LED_IDLE_TICK);
            LedSetState(LED_TYPE_GREEN, LED_OFF);
        }
#if defined SUPPORT_WIFI_NET_FUNCTION
        WifiIdleLedHandle();
#endif
    }
}

static void LedFlickerEndLife(void)
{
    if (SystemObject.DeviceState == eDevicePreEndofLifeMute)
    {
        if (SystemObject.SilenceTime >= ENDOFLIFE_MUTE_TIME)
        {
            SystemObject.DeviceState = eDevicePreEndofLife;
            SystemObject.SilenceTime = 0;
        }
        else
        {
            ++SystemObject.SilenceTime;
        }
        EndLifeIndex(1, LED_TYPE_YELLOW, 0, 3, 50);
    }
    else
    {
        EndLifeIndex(1, LED_TYPE_YELLOW, 1, 3, 50);
    }
}

static void LedFlickerLowPower(void)
{
    if (IsLowPwrErr())
    {
        if (SystemObject.LowPwrSilenceFlag)
        {
            if (SystemObject.LowPwrSilence >= LOW_BATTERY_MUTE_TIME)
            {
                SystemObject.LowPwrSilenceFlag = 0;
                SystemObject.LowPwrSilence = 0;
            }
            else
            {
                SystemObject.LowPwrSilence++;
            }
            EndLifeIndex(1, LED_TYPE_YELLOW, 0, 1, 50);
        }
        else
        {
            LedPollingHandle(LED_TYPE_YELLOW, 100);
            BeepPollingHandle(100);
        }
    }
    else
    {
        SystemPostFIFI(eSystemEventLowPwrCancel);
    }
}

/**
 * @brief  系统待机led显示
 *
 */
void SystemLedFlicker(uint8_t state)
{
    tick_sec_t sys_tick = GetSystemSecondTick();

    if (sys_tick - SystemObject.led_tick >= ENDLIFE_MODE_CYCLE_COUNT)
    {
        SystemObject.led_tick = sys_tick;
        
        switch (state)
        {
        case SysLedIde:
            LedFlickerCheck();
            break;

        case SysLedLowPower:
            LedFlickerLowPower();
            break;

        case SysLedErr:
            EndLifeIndex(1, LED_TYPE_YELLOW, 1, 2, 50);
            break;

        case SysLedEndLife:
            LedFlickerEndLife();
            break;

        default:
            break;
        }
        SensorTemperatureCheck();
    }
}

/**
 * @brief 定时器 - 开机动画
 *
 * @param Handle
 * @param eCmd
 */
void PollingStartNormal(TmrHdl Handle, eTmrCmd eCmd)
{
    static uint8_t i = 0;

    switch (eCmd)
    {
    case eInsertTimer:
#if defined SUPPORT_LCD_FUNCTION
        LCD_BACK_LIGHT_ON();
        LCD_Clear();
#endif        
        BeepPollingHandle(200); // start beep
        SystemSetActiveFlag(eActiveFlagLcd);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagLcd);
        break;

    case eTimeOut:
        if (i < 32)
        {
            uint8_t ring_count = i % 4;
            LedPollingHandle((eLed_Type)ring_count, SYSTEM_LED_TIME);
#if defined SUPPORT_LCD_FUNCTION
            LCD_Clear();
            if ((ring_count != 3))
            {
                LCD_SystemStartAnimation(ring_count);
            }
#endif

            i++;
        }
        else
        {
            i = 0;
#if defined SUPPORT_LCD_FUNCTION
            LCD_BACK_LIGHT_OFF();
            LCD_Clear();
#endif            
            TmrDeleteTimer(Handle);
            SystemPostFIFI(eSystemEventStartIndexEnd);
        }
        break;

    default:
        break;
    }
}

/**
 * @brief 时间控制
 *
 * @param Handle
 * @param eCmd
 */
void PollingAppTimeOut(TmrHdl Handle, eTmrCmd eCmd)
{
    uint32_t state = (uint32_t)TmrGetCtx(Handle);
    switch (eCmd)
    {
    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagTimeout);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagTimeout);
        break;

    case eTimeOut:
        switch (state)
        {
        case eTestSmokeOverWait:
            SystemPostFIFI(eSystemEventTestSmokeOverWait);
            break;

        case eTestCoOverWait:
            SystemPostFIFI(eSystemEventTestCoOverWait);
            break;

        case eTestFinish:
            SystemPostFIFI(eSystemEventTestOver);
            break;

        case eTestCoPeakDisplay:
            SystemPostFIFI(eSystemEventPeakDisplay);
            break;
        default:
            break;
        }

        TmrDeleteTimer(Handle);
        break;
    }
}

void StartHandle(void)
{
    if (SystemObject.PowerConsumptionCheckFlag || hal_watchdog_isreset())
    {
        printf("iwdg reset\n");
        SystemPostFIFI(eSystemEventStartIndexEnd);
    }
    else
    {
        SystemSetActiveFlag(eActiveFlagLcd);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SYSTEM_LED_TIME), PollingStartNormal, NULL);
        SystemObject.DeviceState = eDeviceStarting;
    }
}

/******************************************************************************
**Description:				product power on start index
** Parameters:                     object and event
**     Return:   						none
*******************************************************************************/
void SystemStartHandle(SystemTag *me, QEvent *e)
{
    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        StartHandle();
        break;

    case Q_EXIT_SIG:
        SystemObject.DeviceState = eDeviceOK;
#if defined TEMPERATURE_HIGH_LOW_TEST
        SystemObject.UartEnableFlag = true; // open uart switch
#endif
        break;

    case eSystemEventStartIndexEnd:
        Q_TRAN(SystemIdle);
        break;

    default:
        break;
    }
}

/******************************************************************************
**Description:				product endoftime state handle
** Parameters:                     object and event
**     Return:   						none
*******************************************************************************/
void SystemEndofTimeHandle(SystemTag *me, QEvent *e)
{
    uint32_t SlpTimeCnt = SystemGetIdleNUm();

    switch (e->sig)
    {
    case eSystemEventWakeup:
        BatterCheck();
        if (SlpTimeCnt % 3 == 2) // 30s time  check
        {
            BatteryCheckHandle();
        }
        SystemPollingIdle();
        SysTimeUpdate();
        SystemLedFlicker(SysLedEndLife);
        SECTION_CALL(APP_WAKE_PROC);
        break;

    case Q_ENTRY_SIG:
        SystemObject.led_tick = GetSystemSecondTick(); /* 重置led闪灯tick */
        EndLife();
#if defined SUPPORT_LCD_FUNCTION
        LCD_Control(eLcdON);
        LCD_Clear();
        SetLcdSwitchFlag(true);
        LCD_DisplayStr(eStr_End);
#endif
        printf("%s,in\n", __FUNCTION__);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        break;

    case eSystemEventKey:
    case eSystemEventLongPressKey:
    case eSystemEventNetCheckSelf:
        if (SystemObject.DeviceState == eDevicePreEndofLife)
        {
            SystemObject.DeviceState = eDevicePreEndofLifeMute;
            SystemObject.SilenceTime = 0;
        }
        else if (SystemObject.DeviceState == eDevicePreEndofLifeMute)
        {
            // Q_TRAN(SystemKeyTestHandle);
            SystemObject.SilenceTime = ENDOFLIFE_MUTE_TIME;
        }
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

/******************************************************************************
**Description:			    syetem Common handle
** Parameters:               object and event
**     Return:   				none
*******************************************************************************/

static void SystemCommonhandle(SystemTag *me, QEvent *e)
{
    switch (e->sig)
    {

    case eSystemEventKey:
#ifndef SUPPORT_RADIO_FUNCTION
    case eSystemEventLongPressKey:
#endif
        printf("key \n");
#ifdef SUPPORT_LCD_FUNCTION
        if (GetLcdSwitchFlag())
        {
            Q_TRAN(SystemKeyTestHandle);
        }
        else
        {
            SetLcdSwitchFlag(true);
            LCDStandyDisplay(0, true);
        }
#else
        Q_TRAN(SystemKeyTestHandle);
#endif
        break;

    case eSystemEventEndLife:
        Q_TRAN(SystemEndofTimeHandle);
        break;

#if defined SUPPORT_SMOKE_FUNCTION
    case eSystemEventSmokeAlarm:
        Q_TRAN(SystemSmokeAlarm);
        break;

    case eSystemEventSmokeErr:
        Q_TRAN(SystemSmokeError);
        break;
#endif

#if defined SUPPORT_CO_FUNCTION
    case eSystemEventCOAlarm:
        Q_TRAN(SystemCOAlarm);
        break;
    case eSystemEventCOErr:
        Q_TRAN(SystemCOError);
        break;
#endif

#ifdef SUPPORT_WIFI_NET_FUNCTION
    case eSystemEventFourKey:
        Q_TRAN(NetDelPairHandle);
        break;

    case eSystemEventLongPressKey:
        Q_TRAN(NetPairHandle);
        break;
#endif

/*****************************************NET FUNCTION****************************************************/
#if defined SUPPORT_RADIO_FUNCTION

#if defined PAIR_KEY
    case eSystemEventPairClik:
        Q_TRAN(AppPairMasterMode);
        break;

    case eSystemEventPairLongPressKey:
        Q_TRAN(AppPairSlaveMode);
        break;

    case eSystemEventPairFourKey:
        Q_TRAN(AppExitPair);
        break;
#else
    case eSystemEventDoubleKey:
    case eSystemEventPairClik:
        Q_TRAN(AppPairSlaveMode);
        break;

    case eSystemEventFourKey:
        Q_TRAN(AppPairMasterMode);
        break;
#endif

    case eSystemEventLongPressKey:
        if (GetRadioPairMode() == RF_PAIR_MODE_INTRANET)
        {
            Q_TRAN(AppSendRfTestMode);
        }
        else if (GetRadioPairMode() == RF_PAIR_MODE_NONE)
        {
            Q_TRAN(SystemKeyTestHandle);
        }
        break;

    case Q_eIntranetEventRadioRxTest:
        Q_TRAN(AppResponeRfTestMode);
    break;

    case Q_eIntranetEventReceiveTest:
        if (GetRadioPairMode() != RF_PAIR_MODE_NONE)
        {
            Q_TRAN(AppResponeRfTestMode);
        }
        break;
    
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

#endif /* end of SUPPORT_RADIO_FUNCTION */

    default:

        break;
    }
}

void SystemKeyTestHandle(SystemTag *me, QEvent *e)
{
    static uint8_t i = 0;
    static uint8_t UartEnableNum = 0;
#ifdef SUPPORT_PEAK_FUNCTION
    static uint8_t ClearOneTime = 0;
#endif

    switch (e->sig)
    {
    case Q_ENTRY_SIG:
#if defined SUPPORT_LCD_FUNCTION
        LCD_BACK_LIGHT_ON();
#endif

#ifdef SUPPORT_SMOKE_FUNCTION
        BeepPollingHandle(50);
        SmokeKeyTestHandle(0);
#else
        BeepPollingHandle(200);
        LedPollingHandle(LED_TYPE_RED, 200);
        COKeyTestDelay(0);
#endif
        break;

    case Q_EXIT_SIG:

        i = 0;
        UartEnableNum = 0;
#ifdef SUPPORT_PEAK_FUNCTION
        ClearOneTime = 0;
#endif
#if defined SUPPORT_LCD_FUNCTION
        LCD_BACK_LIGHT_OFF();
#endif
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingAppTimeOut));
        break;

#if defined SUPPORT_SMOKE_FUNCTION
    case eSystemEventSmokeBeepOK:
        if (i++ > 0)
        {
            i = 0;
            SmokeKeyTestHandle(1);
        }
        break;
    /* 烟感测试完成，进入等待时长 */
    case eSystemEventSmokeTestOver:
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(3500), PollingAppTimeOut, (void *)eTestSmokeOverWait);
        break;

    /* 进入Co测试 */
    case eSystemEventTestSmokeOverWait:
#if defined SUPPORT_CO_FUNCTION
        COKeyTestDelay(0);
#else
        Q_TRAN(SystemIdle);
#endif
        break;
#endif
    case eSystemEventCOBeepOK:
        if (i++ > 0)
        {
            i = 0;
            COKeyTestDelay(1);
        }
        else
        {
#if !(defined SUPPORT_SMOKE_FUNCTION) && (defined SUPPORT_LCD_FUNCTION)
#define CO_TEST_LCD_TIME (450)
            TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_TEST_LCD_TIME), PollingLineDisplay, NULL);
#endif
        }
        break;

    /* Co测试完成，进入等待时长 */
    case eSystemEventCOTestOver:
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), PollingAppTimeOut, (void *)eTestCoOverWait);
        break;

    case eSystemEventTestCoOverWait:
#ifdef SUPPORT_SMOKE_FUNCTION
        LedPollingHandle(LED_TYPE_RED, 50);
        BeepPollingHandle(50);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(2000), PollingAppTimeOut, (void *)eTestFinish);
#else
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), PollingAppTimeOut, (void *)eTestFinish);
#endif
#if defined SUPPORT_LCD_FUNCTION
        LCD_DisplayStr(eStr_PAS);
#endif
        break;

    /* 所有测试完成 */
    case eSystemEventTestOver:
        SelfCheckOk(0x00);
#ifdef SUPPORT_PEAK_FUNCTION
#ifndef SUPPORT_SMOKE_FUNCTION
        LedPollingHandle(LED_TYPE_RED, 50);
        BeepPollingHandle(50);
#endif
        /* Peak值显示 */
        ClearOneTime = 1;
        LcdDisplayPoint(eLcdPointON, LCD_PEAK_COM, LCD_PEAK_SEG);
        LCD_DisplayNum(COGetPeakData());
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(5000), PollingAppTimeOut, (void *)eTestCoPeakDisplay);
#else
        Q_TRAN(SystemIdle);
#endif
        break;

    case eSystemEventLongPressKey: // for	open debug
        if (UartEnableNum >= 1)
        {
            LedPollingHandle(LED_TYPE_GREEN, 50);
            SystemObject.UartEnableFlag = (BOOL)!SystemObject.UartEnableFlag;
        }
        printf("UartFlag:%d\r\n", SystemObject.UartEnableFlag);
#ifdef SUPPORT_PEAK_FUNCTION
        if (ClearOneTime)
        {
            ClearOneTime = 0;
            LedPollingHandle(LED_TYPE_GREEN, 50);
            BeepPollingHandle(50);
            COSetPeakData(0);
            LCD_DisplayNum(0);
        }
        break;

    case eSystemEventPeakDisplay:
        SystemPostFIFI(eSystemEventLcdUpdate);
        Q_TRAN(SystemIdle);
        break;
#endif

    case eSystemEventDoubleKey: // for	open debug
        UartEnableNum++;
        printf("Num:%d\r\n", UartEnableNum);
        break;

#if defined SUPPORT_SMOKE_FUNCTION
    case eSystemEventSmokeErr:
        Q_TRAN(SystemSmokeError);
        break;
#endif

#if defined SUPPORT_CO_FUNCTION
    case eSystemEventCOErr:
        Q_TRAN(SystemCOError);
        break;
#endif

    default:

        break;
    }
}

/******************************************************************************
**Description:			    syetem LowPwr handle
** Parameters:               object and event
**     Return:   				none
*******************************************************************************/

void SystemLowPwrHandle(SystemTag *me, QEvent *e)
{
    switch (e->sig)
    {
    case eSystemEventWakeup:
        BatterCheck();
        SystemPollingIdle();
        SysTimeUpdate();
        SystemLedFlicker(SysLedLowPower);
        SECTION_CALL(APP_WAKE_PROC);
        break;

    case Q_ENTRY_SIG:
        printf("%s in\n", __FUNCTION__);
        SystemObject.LowPwrSilenceFlag = 0;
        SystemObject.led_tick = GetSystemSecondTick();  /* 重置led闪灯tick */
        SystemSetIdleNUm(3 * (SystemGetIdleNUm() / 3)); // WAIT 30S FOR  CO CHECK
#if defined SUPPORT_LCD_FUNCTION
        LCD_Control(eLcdON);
        LCD_Clear();
        SetLcdSwitchFlag(true);
        LCD_DisplayStr(eStr_Lb);
#endif
        LedPollingHandle(LED_TYPE_YELLOW, 100);
        BeepPollingHandle(100);
        break;

    case Q_EXIT_SIG:
        printf("%s out\n", __FUNCTION__);
        break;

    case eSystemEventKey:
    case eSystemEventLongPressKey:
        if (SystemObject.LowPwrSilenceFlag == 0) // silence
        {
#if defined STANDARD_UL2034_UL217
            if (BatteryGetData() > LOW_BATTERY_HUSH_FORBID_THRESHOLD)
#endif
            {
                SystemObject.LowPwrSilenceFlag = 1;
                SystemObject.LowPwrSilence = 0;
                LedPollingHandle(LED_TYPE_YELLOW, 100);
            }
#ifdef STANDARD_UL2034_UL217
            else
            {
                Q_TRAN(SystemKeyTestHandle);
            }
#endif
        }
        else
        {
            Q_TRAN(SystemKeyTestHandle);
        }
        break;
    case eSystemEventLowPwrCancel:
        Q_TRAN(SystemIdle);
        break;

    default:
        SystemCommonhandle(me, e);
        break;
    }
}

/******************************************************************************
**Description:			    syetem idle handle
** Parameters:               object and event
**     Return:   				none
*******************************************************************************/

void SystemIdle(SystemTag *me, QEvent *e)
{
#if (defined SUPPORT_LCD_FUNCTION) && (defined IRREPLACEABLE_BATTARY)
    static uint8_t i = 0;
#endif

    switch (e->sig)
    {
    case eSystemEventWakeup:
        SysTimeUpdate();
        SystemPollingIdle();
        BatterCheck();
        SystemLedFlicker(SysLedIde);
        SECTION_CALL(APP_WAKE_PROC);

#if (defined SUPPORT_LCD_FUNCTION) && (defined IRREPLACEABLE_BATTARY)
        if (GetLcdSwitchFlag() == true)
        {
            if (i >= LCD_SHOW_TOTAL_TIME)
            {
                i = 0;
                SetLcdSwitchFlag(false);
            }
            i++;
        }
#endif
        break;

    case Q_ENTRY_SIG:
        printf("%s,in\n", __FUNCTION__);
        SystemObject.led_tick = GetSystemSecondTick();  /* 重置led闪灯tick */
        SystemSetIdleNUm(3 * (SystemGetIdleNUm() / 3)); // WAIT 30S FOR  CO CHECK
#if defined SUPPORT_LCD_FUNCTION
        /* 跳过开机动画并关闭lcd */
        if (SystemObject.PowerConsumptionCheckFlag)
        {
            SystemObject.PowerConsumptionCheckFlag = false;
            SystemPostFIFI(eSystemEventSkipBootAnimation);
            #if defined IRREPLACEABLE_BATTARY
            break;
            #endif
        }
            SetLcdSwitchFlag(true);
            LCDStandyDisplay(0, true);
#endif
        break;

    case Q_EXIT_SIG:
#if (defined SUPPORT_LCD_FUNCTION) && (defined IRREPLACEABLE_BATTARY)
        i = 0;
#endif
        printf("%s,out\n", __FUNCTION__);
        break;

    case eSystemEventSkipBootAnimation:
        printf("skip boot\n");
        break;

    case eSystemEventLowPwr:
        Q_TRAN(SystemLowPwrHandle);
        break;

    default:
        SystemCommonhandle(me, e);
        break;
    }
}

void SytemTimeOutHandle(TmrHdl Handle, eTmrCmd eCmd)
{
    static uint8_t i = 0;

    switch (eCmd)
    {
    case eTimeOut:
        if (i < SYSTEM_TIMEOUT_NUM) //
        {
            i++;
        }
        else
        {
            while (1)
                ;
        }
        break;
    default:
        break;
    }
}

/* shell cmd */
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_VAR_NODE), uart, &shelluart, "open/close uart debug");

/**************END OF FILE***************/
