#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__

#include "system.h"
#include "polling.h"
#include "inc/led.h"
#include "config.h"

#undef LOG_LEVEL
#define LOG_LEVEL 2

//按键使能debug扩展宏
#define debug_uart(fmt,arg...) do{ if(IsUartOpen()){ printf(fmt, ##arg); } }while(0)

#define IWG_FEED() hal_watchdog_feed()

typedef enum 
{
    eTestSmokeOverWait = 0,
    eTestCoOverWait,
    eTestCoPeakDisplay,
    eTestFinish,

}TestState;

/**
 * @brief 系统提示灯
 * 
 */
typedef enum 
{
    SysLedIde = 0,
    SysLedLowPower,
    SysLedErr,
    SysLedEndLife,

}eSysLedState;

void RfObjectInit(SystemTag *me, QEvent const *e);
void SystemEndofTimeHandle(SystemTag  *me,QEvent *e);
void SystemKeyTestHandle(SystemTag  *me,QEvent *e);
void SystemIdle(SystemTag  *me,QEvent *e);
void SystemLowPwrHandle(SystemTag  *me,QEvent *e);
void SystemStartHandle(SystemTag  *me,QEvent *e);
void SytemTimeOutHandle(TmrHdl Handle, eTmrCmd eCmd);
void SysTimeUpdate(void);
void SystemObjectInit(SystemTag *me, QEvent const *e);
BOOL IsUartOpen(void);
void EndLifeIndex(uint8_t LedFlag, eLed_Type LedType, uint8_t BeepFlag, uint8_t Num, uint8_t Time);
void SystemLedFlicker(uint8_t state);
void ResetLedTick(void);
void SetUartOpen(uint8_t flg);

#ifdef SUPPORT_LCD_FUNCTION
void Lcd_CloseCheck(void);
BOOL SetLcdSwitchFlag(BOOL Flag);
BOOL GetLcdSwitchFlag(void);
void PollingLineDisplay(TmrHdl Handle, eTmrCmd eCmd);
#endif

#endif
