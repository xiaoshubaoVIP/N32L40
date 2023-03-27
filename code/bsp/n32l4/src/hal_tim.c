#include "n32l40x.h"
#include "n32l4/inc/hal_tim.h"

#define LPTIM_MS(ms) ((uint32_t)((16000000 >> 2) * ms) / 1000)
extern uint32_t SystemCoreClock;
static tim_inter_callback tim1_callback;

/**
 * @brief LPTIM Interrupt.
 * @param Cmd Interrupt enable or disable
 */
void LPTIMNVIC_Config(FunctionalState Cmd)
{
    EXTI_InitType EXTI_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    EXTI_ClrITPendBit(EXTI_LINE24);
    EXTI_InitStructure.EXTI_Line = EXTI_LINE24;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    /* Enable the RTC Alarm Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = LPTIM_WKUP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = Cmd;
    NVIC_Init(&NVIC_InitStructure);
}

void hal_tim_init(void)
{
    /* LPTIM */
    /* Enable the LSI source */
    LPTIM_DeInit(LPTIM);
    LPTIMNVIC_Config(ENABLE);
    RCC_ConfigLPTIMClk(RCC_LPTIMCLK_SRC_HSI);
    RCC_EnableRETPeriphClk(RCC_RET_PERIPH_LPTIM, ENABLE);

    /* Enable interrupt   */
    LPTIM_SetPrescaler(LPTIM, LPTIM_PRESCALER_DIV4);
    LPTIM_EnableIT_CMPM(LPTIM);
    /* config lptim ARR and compare register */
    LPTIM_Enable(LPTIM);
    LPTIM_SetAutoReload(LPTIM, LPTIM_MS(10));
    LPTIM_StartCounter(LPTIM, LPTIM_OPERATING_MODE_CONTINUOUS);
}

void hal_tim_write(tim_id_t tim, uint32_t ms)
{
    if (tim == TIM_LPTIM)
    {
        LPTIM_Enable(LPTIM);
        LPTIM_SetAutoReload(LPTIM, LPTIM_MS(ms));
        LPTIM_StartCounter(LPTIM, LPTIM_OPERATING_MODE_CONTINUOUS);
    }
}

void hal_tim_set_callback(tim_id_t tim, tim_inter_callback func)
{
    tim1_callback = func;
}

tim_inter_callback hal_tim_get_callback(tim_id_t tim)
{
    return tim1_callback;
}

uint32_t hal_get_tim_cnt(tim_id_t tim)
{
    uint32_t cnt = 0;
    if (tim == TIM_LPTIM)
    {
        cnt = LPTIM_GetCounter(LPTIM);
    }

    return cnt;
}

void hal_tim_enable(tim_id_t tim)
{
    if (tim == TIM_LPTIM)
    {
        SET_BIT(LPTIM->CTRL, LPTIM_CTRL_LPTIMEN);
        LPTIM->ARR = LPTIM_MS(10);
        LPTIM_StartCounter(LPTIM, LPTIM_OPERATING_MODE_CONTINUOUS);
    }
}

void hal_tim_disable(tim_id_t tim)
{
    if (tim == TIM_LPTIM)
    {
        CLEAR_BIT(LPTIM->CTRL, LPTIM_CTRL_LPTIMEN);
    }
}

/**
 * @brief  This function handles LLPTIM interrupt request.
 */
void LPTIM_WKUP_IRQHandler(void)
{
    if ((READ_BIT(LPTIM->INTSTS,LPTIM_INTSTS_CMPM) == LPTIM_INTSTS_CMPM))
    {
        SET_BIT(LPTIM->INTCLR, LPTIM_INTCLR_CMPMCF);
        EXTI_ClrITPendBit(EXTI_LINE24);
        if (tim1_callback != 0)
            tim1_callback();
    }
}
