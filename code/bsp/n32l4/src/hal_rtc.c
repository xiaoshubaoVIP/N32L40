#include "n32l4/inc/hal_rtc.h"
#include "n32l40x.h"

#define ASYNCH_PREDIV 125
#define SYNCH_PREDIV 32

#define RTC_SEC(s) (LsiFreq * s / ASYNCH_PREDIV / SYNCH_PREDIV)

extern uint32_t LsiFreq;
static rtc_inter_callback rtc_callback;

/**
 * @brief  Config RTC wake up Interrupt.
 */
void EXTI20_RTCWKUP_Configuration(FunctionalState Cmd)
{
    EXTI_InitType EXTI_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    EXTI_ClrITPendBit(EXTI_LINE20);
    EXTI_InitStructure.EXTI_Line = EXTI_LINE20;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);
    /* Enable the RTC WakeUp Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = Cmd;
    NVIC_Init(&NVIC_InitStructure);
}

void hal_rtc_init(void)
{
    RTC_InitType RTC_InitStructure;
    /* Enable the PWR clock */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
    /* Allow access to RTC */
    PWR_BackupAccessEnable(ENABLE);
    /* Disable RTC clock */
    RCC_EnableRtcClk(DISABLE);

    while (RCC_GetFlagStatus(RCC_CTRLSTS_FLAG_LSIRD) == RESET)
        ;
    RCC_ConfigRtcClk(RCC_RTCCLK_SRC_LSI);

    /* Enable the RTC Clock */
    RCC_EnableRtcClk(ENABLE);
    RTC_WaitForSynchro();

    RTC_DeInit();

    /* Configure the RTC data register and RTC prescaler */
    RTC_InitStructure.RTC_AsynchPrediv = ASYNCH_PREDIV - 1;
    RTC_InitStructure.RTC_SynchPrediv = SYNCH_PREDIV - 1;
    RTC_InitStructure.RTC_HourFormat = RTC_24HOUR_FORMAT;
    /*RTC init */
    RTC_Init(&RTC_InitStructure);

    /* wake up clock select */
    RTC_ConfigWakeUpClock(RTC_WKUPCLK_CK_SPRE_16BITS);

    EXTI20_RTCWKUP_Configuration(ENABLE);
    /* Enable the RTC Wakeup Interrupt */
    RTC_ConfigInt(RTC_INT_WUT, ENABLE);
    // RTC_EnableWakeUp(ENABLE);
    hal_rtc_write(5);
}

void hal_rtc_write(uint16_t count)
{
    RTC_EnableWakeUp(DISABLE);
    RTC_SetWakeUpCounter(RTC_SEC(count));
    RTC_EnableWakeUp(ENABLE);
}

uint16_t hal_rtc_read(void)
{
    return 0;
}

void hal_rtc_set_callback(rtc_inter_callback callback)
{
    rtc_callback = callback;
}

/**
 * @brief  This function handles RTC WakeUp interrupt request.
 */
void RTC_WKUP_IRQHandler(void)
{
    EXTI_ClrITPendBit(EXTI_LINE20);
    if (RTC_GetITStatus(RTC_INT_WUT) != RESET)
    {
        RTC_ClrIntPendingBit(RTC_INT_WUT);
    }

    if (rtc_callback != NULL)
        rtc_callback();
}
