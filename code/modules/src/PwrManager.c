/****************************************************************************/
/*	File   :             PwrManager.c               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/4/24,19:36:20                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/
#include "inc/PwrManager.h"
#include "common.h"
#include "bsp_config.h"
#include "modules_config.h"
#include "app_main.h"

#define BAT_DET_DELAY() //DelayUs(35)

struct
{
    // uint8_t State;//
    uint16_t LowBatCheckCnt;
} BatteryAO;

uint8_t BatteryState = eBatteryLevelThree;
static uint16_t BatteryVol;

// extern void SytemTimeOutHandle(TmrHdl Handle, eTmrCmd eCmd);
extern TmrHdl SytemTimeOutTimer;
/**
 * @brief  Entery Low power mode
 * @param  None
 * @retval None
 */
void EnteryLowPower(void)
{
    // static uint32_t i = 0;
    hal_watchdog_feed();
    if (SystemGetActiveFlag(eActiveFlagAll) == 0)
    {   
        #if (defined SUPPORT_LCD_FUNCTION) && (defined IRREPLACEABLE_BATTARY)
            Lcd_CloseCheck();
        #endif
        TmrResetCurCnt(SytemTimeOutTimer); // for no idle input
        enter_lowpower();
        exit_lowpower();
    }
    else
    {
        // if(i++ > 1000)
        // {
        //     printf("ideflg = %d,led=%d\n",SystemGetActiveFlag(eActiveFlagAll),(int)GetLcdSwitchFlag());
        //     i = 0;
        // }     
        enter_offcpu();
        exit_offcpu();
    }
}

BOOL IsLowPwrErr(void)
{
    return (BatteryState == eBatteryLevelNull) ? true : false;
}

uint16_t getBatteryVol(void)
{
    return BatteryVol;
}

uint16_t BatteryGetData(void)
{
    hal_gpio_write(PIN_BAT_DET, PIN_SET);
    // DelayUs(35);
    BatteryVol = hal_adc_read_vol(ADC_BATTERY); // read adc vol
    hal_gpio_write(PIN_BAT_DET, PIN_RESET);

#ifdef  SUPPORT_LCD_FUNCTION
    hal_lcd_calibration(BatteryVol);
#endif
//    debug("bat vol=%d", BatteryVol);
    return BatteryVol;
}

uint8_t BatteryGetState(void)
{
    uint8_t BatteryStateCurrent = eBatteryLevelThree;
    uint16_t BatterySample = BatteryGetData();

    if (BatterySample < BatteryState_ONE_THIRED)
    {
        BatteryStateCurrent = eBatteryLevelNull;
    }
    else if (BatterySample < BatteryState_TWO_THIRED)
    {
        BatteryStateCurrent = eBatteryLevelOne;
    }
    else if (BatterySample < BatteryState_FULL)
    {
        BatteryStateCurrent = eBatteryLevelTwo;
    }
    else
    {
        BatteryStateCurrent = eBatteryLevelThree;
    }

    debug_uart("bat = %d,lv = %d\n",BatterySample,BatteryStateCurrent);
    return BatteryStateCurrent;
}

void BatteryMcuInit(void)
{
    BatteryState = BatteryGetState();
}

void BatteryCheckHandle(void)
{
    uint8_t BatteryStateCurrent = BatteryGetState();
    

    if (BatteryState != BatteryStateCurrent) // state change
    {
        if (BatteryAO.LowBatCheckCnt >= 2) // 3 time
        {
            BatteryAO.LowBatCheckCnt = 0;
            BatteryState = BatteryStateCurrent;
            BatteryChagne();
        }
        else
        {
            BatteryAO.LowBatCheckCnt++;
        }
    }
    else
    {
        BatteryAO.LowBatCheckCnt = 0; // for emc
    }

#if defined SUPPORT_LCD_FUNCTION
    BatteryLcdDisplay(BatteryState);
#endif
}

unsigned char GetBatterLevel(void)
{
    return BatteryState;
}

void BatterCheck(void)
{
    if ((SystemGetIdleNUm() % BATTERY_CHECK_COUNT) == (BATTERY_CHECK_COUNT - 1))
        BatteryCheckHandle();
}




