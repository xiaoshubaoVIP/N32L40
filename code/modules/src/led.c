#include "inc/led.h"
#include "common.h"
#include "bsp_config.h"
#include "inc/lcd.h"

static void PollingLedOff(TmrHdl Handle, eTmrCmd eCmd)
{
    uint32_t LedType = (uint32_t)TmrGetCtx(Handle);

    switch (eCmd)
    {
    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagLed);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagLed);
        break;

    case eTimeOut:
        TmrDeleteTimer(Handle);
        LedSetState((eLed_Type)LedType, LED_OFF);
        break;

    default:
        break;
    }
}

void LedPollingHandle(eLed_Type LedType, uint32_t Time)
{
    if (SystemGetActiveFlag(eActiveFlagLed))
    {
        LedSetState(LED_TYPE_ALL, LED_OFF);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingLedOff));
    }

    LedSetState(LedType, LED_ON);
    TmrInsertTimer(GetTmrHd(), TMR_PERIOD(Time), PollingLedOff, (void *)LedType);
}

/**********************************************************************************************
**Description:			LedSetState
** Parameters:       pLed:LED AO POIONTER      LedType : which led  State :on/off/toggle
**     Return:   			none
***********************************************************************************************/
void LedSetState(eLed_Type LedType, eLed_State State)
{
    pin_status_t pin_state = (pin_status_t)State;

    switch (LedType)
    {
    case LED_TYPE_GREEN:
        hal_gpio_write(PIN_LED_G, pin_state);
        break;

    case LED_TYPE_RED:
        hal_gpio_write(PIN_LED_R, pin_state);
        break;
#ifdef BLUE_LED_EN
    case LED_TYPE_BLUE:
        hal_gpio_write(PIN_LED_B, pin_state);
        break;
#endif
    case LED_TYPE_YELLOW:
#ifdef Independent_YellowLed
        hal_gpio_write(PIN_LED_Y, pin_state);
#else
        hal_gpio_write(PIN_LED_G, pin_state);
        hal_gpio_write(PIN_LED_R, pin_state);
#endif        
        break;

    default:
#ifdef Independent_YellowLed
        hal_gpio_write(PIN_LED_Y, PIN_RESET);
#endif   
        hal_gpio_write(PIN_LED_G, PIN_RESET);
        hal_gpio_write(PIN_LED_R, PIN_RESET);
#ifdef BLUE_LED_EN
        hal_gpio_write(PIN_LED_B, PIN_RESET);
#endif        
        break;
    }
}

#if 0
void LedTest(void)
{
	static eLed_Type i = LED_TYPE_RED;
	static eLed_State x = LED_OFF;

	if (i < LED_TYPE_MAX)
	{ 
		if (x < LED_TOGGLE)
		{
			LedSetState(&LedObject,i, x);
		 	++x;
		}
		else
		{
			x = LED_OFF;
			++i;
		}	
	}
	else
	{
		i = LED_TYPE_RED;
		x = LED_OFF;
	}
	//printf("%s,i = %d,x = %d\n",__FUNCTION__,i,x);	
}
#endif
/********************END OF FILE**********************/
