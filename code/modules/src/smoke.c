

#include "inc/smoke.h"
#include "common.h"
#include "modules_config.h"
#include "bsp_config.h"

#ifdef SUPPORT_SMOKE_FUNCTION

#define SMOKE_OP_POWER_ON() hal_gpio_write(PIN_SMOKE_EN, PIN_SET)
#define SMOKE_OP_POWER_OFF() hal_gpio_write(PIN_SMOKE_EN, PIN_RESET)
#define SMOKE_IR_CTR_ON() hal_gpio_write(PIN_SMOKE_IR, PIN_SET)
#define SMOKE_IR_CTR_OFF() hal_gpio_write(PIN_SMOKE_IR, PIN_RESET)
#define SMOKE_IR_TEST_ON()    // hal_gpio_write(PIN_SMOKE_IR_TEST, PIN_SET)
#define SMOKE_IR_TEST_OFF()   // hal_gpio_write(PIN_SMOKE_IR_TEST, PIN_RESET)
#define SMOKE_DEBUG_PIN_ON()  // hal_gpio_write(PIN_SWIM, PIN_SET)
#define SMOKE_DEBUG_PIN_OFF() // hal_gpio_write(PIN_SWIM, PIN_RESET)


/*
***********************************************************************************************************************
*  功能: Smoke 获取亮暗光值
*
*  描述: [in] pSmoke smoke结构体对象
*        [in] State  smoke状态
*
*  返回: Smoke D-L差值
*
***********************************************************************************************************************
*/
SmokeDataTag SmokeGetLdData(uint8_t State)
{
    SmokeDataTag SmokeData;

    // NOT TO ADD ANY THINGS FOLLOW  SECTION
    /*---------------------------------FIRST--------------------------------------------*/
    SMOKE_OP_POWER_ON(); // smoke op on
    DelayUs(SMOKE_CHECK_DARK_DELAY);

    SMOKE_DEBUG_PIN_ON();
    SmokeData.Dark = hal_adc_read_vol(ADC_SMOKE);
    SMOKE_DEBUG_PIN_OFF();
    switch (State)
    {
    case IRCTR_ON_TEST_OFF:
    {
        SMOKE_IR_CTR_ON(); // smoke ir control on
    }
    break;
    case IR_ALL_ON:
    {
        SMOKE_IR_CTR_ON();  // smoke ir control on
        SMOKE_IR_TEST_ON(); // smoke ir test control on
    }
    break;
    case IR_ALL_OFF:
    {
        SMOKE_IR_CTR_OFF();  // smoke ir control off
        SMOKE_IR_TEST_OFF(); // smoke ir test off
    }
    break;

    case IR_TEST_ON_IR_OFF:
    {
        SMOKE_IR_TEST_ON(); // smoke ir test control on
    }
    break;

    default:
        break;
    }
    DelayUs(SMOKE_CHECK_LIGHT_DELAY);
    SMOKE_DEBUG_PIN_ON();
    SmokeData.Light = hal_adc_read_vol(ADC_SMOKE);
    SMOKE_DEBUG_PIN_OFF();
    SMOKE_IR_CTR_OFF();
    SMOKE_IR_TEST_OFF();
    SMOKE_OP_POWER_OFF();
    SmokeData.Diff = SmokeData.Light - SmokeData.Dark;
    if (SmokeData.Diff > 6000)
    {
        SmokeData.Diff = 0;
    }

    // printf("d= %d,l=%d,dif=%d\n",SmokeData.Dark,SmokeData.Light,SmokeData.Diff);
    return SmokeData;
}

#endif /* end of SUPPORT_SMOKE_FUNCTION */
