/****************************************************************************/
/*	File   :             Beep.c               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/4/25,12:22:14                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/
#include "inc/Beep.h"
#include "common.h"
#include "bsp_config.h"

#define BEEP_ON()                           hal_gpio_write(PIN_BEEP, PIN_SET)
#define BEEP_OFF()                          hal_gpio_write(PIN_BEEP, PIN_RESET)

void BeepSetState(eBeepState State)
{
    if (State == eBeepOn) {
        BEEP_ON();
    } else {
        BEEP_OFF();
    }
}

static void PollingBeepOff(TmrHdl Handle, eTmrCmd eCmd)
{
    switch(eCmd)
    {
    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagBeep);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagBeep);
        break;

    case eTimeOut:
        BEEP_OFF();
        TmrDeleteTimer(Handle);
        break;

        default:
            break;
    }
}

void BeepPollingHandle(uint32_t Time)
{
    BEEP_ON();
    TmrInsertTimer(GetTmrHd(), TMR_PERIOD(Time), PollingBeepOff, NULL);
}




