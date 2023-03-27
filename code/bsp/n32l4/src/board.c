#include "bsp_config.h"
#include "n32l40x.h"
#include <stdio.h>

#define SYSTICK_CONVER_MS(ms) ((SystemCoreClock >> 3) / 1000 * (ms))
#define SYSTICK_CONVER_US(ms) ((SystemCoreClock >> 3) / 1000000 * (ms))

extern uint32_t SystemCoreClock;
static __IO uint16_t CaptureNumber = 0;
uint32_t LsiFreq;
static volatile uint8_t systick_timeout;
static uint8_t dis_low_power;

/**
 * @brief  This function handles SysTick Handler.
 */
void SysTick_Handler(void)
{
    systick_timeout = true;
}

void Delay(unsigned long nTime)
{
    uint32_t ms;
    while (nTime)
    {
        ms = nTime > 1000 ? 1000 : nTime;
        SysTick_Config(SYSTICK_CONVER_MS(ms));
        systick_timeout = false;

        while (systick_timeout == false)
        {
            // PWR_EnterSLEEPMode(SLEEP_OFF_EXIT, PWR_SLEEPENTRY_WFI);
        }
        SysTick->CTRL = 0;
        nTime -= ms;
    }
}

void DelayUs(unsigned long TimeCnt)
{
    if (TimeCnt == 0)
        return;

    SysTick_Config(SYSTICK_CONVER_US(TimeCnt));
    systick_timeout = false;

    do
    {
        PWR_EnterSLEEPMode(SLEEP_OFF_EXIT, PWR_SLEEPENTRY_WFI);
    } while (systick_timeout == false);

    SysTick->CTRL = 0;
}

/**
 * @brief 粗略延迟 -us
 *
 * @param u32Us
 */
void SystemDelayUs(uint32_t u32Us)
{
    while (u32Us--)
    {
        __NOP();
    }
}

void DelayMs(unsigned long nTime)
{
    uint32_t ms;
    while (nTime)
    {
        ms = nTime > 1000 ? 1000 : nTime;
        DelayUs(ms * 1000);
        nTime -= ms;
    }
}

void enter_lowpower(void)
{
    if (dis_low_power == true)
        return;

    hal_uart_disable();
    hal_adc_disable();
    hal_tim_disable(TIM_LPTIM);
    PWR_EnterSTOP2Mode(PWR_STOPENTRY_WFI, PWR_CTRL3_RAM1RET);
}

void exit_lowpower(void)
{
    if (dis_low_power == true)
        return;

    hal_uart_enable();
    hal_adc_enable();
    hal_tim_enable(TIM_LPTIM);
}

void enter_offcpu(void)
{
    if (dis_low_power == true)
        return;

    PWR_EnterSLEEPMode(SLEEP_OFF_EXIT, PWR_SLEEPENTRY_WFI);
}

void exit_offcpu(void)
{
    if (dis_low_power == true)
        return;
}

/**
 * @brief  This function handles TIM9 global interrupt request.
 */
void TIM9_IRQHandler(void)
{
    static uint32_t Capture;
    static uint16_t IC1ReadValue1;
    static uint16_t IC1ReadValue2;

    if (TIM_GetIntStatus(TIM9, TIM_INT_CC3) != RESET)
    {
        if (CaptureNumber == 0)
        {
            /* Get the Input Capture value */
            IC1ReadValue1 = TIM_GetCap3(TIM9);
        }
        else if (CaptureNumber == 2)
        {
            RCC_ClocksType clks;
            /* Get the Input Capture value */
            IC1ReadValue2 = TIM_GetCap3(TIM9);

            /* Capture computation */
            if (IC1ReadValue2 > IC1ReadValue1)
            {
                Capture = (IC1ReadValue2 - IC1ReadValue1);
            }
            else
            {
                Capture = ((0xFFFF - IC1ReadValue1) + IC1ReadValue2);
            }
            RCC_GetClocksFreqValue(&clks);
            /* Frequency computation */
            LsiFreq = (uint32_t)clks.Pclk1Freq / Capture * 32;
        }

        CaptureNumber++;
        /* Clear TIM9 Capture compare interrupt pending bit */
        TIM_ClrIntPendingBit(TIM9, TIM_INT_CC3);
    }
}

void LsiMeasurment(void)
{
    NVIC_InitType NVIC_InitStructure;
    TIM_ICInitType TIM_ICInitStructure;
    // uint8_t LsiPeriodCounter = 0;
    /* Enable the LSI OSC */
    RCC_EnableLsi(ENABLE);
    /* Enable TIM9 clocks */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM9, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
    /* Enable the TIM9 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM9_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    /* Configure TIM9 prescaler */
    TIM_ConfigPrescaler(TIM9, 0, TIM_PSC_RELOAD_MODE_IMMEDIATE);
    /* Connect internally the TM9_CH3 PB14 Input Capture to the LSI clock output */
    GPIO_ConfigPinRemap(GPIOB_PORT_SOURCE, GPIO_PIN_SOURCE14, GPIO_AF1_TIM9);
    /* TIM9 configuration: Input Capture mode
       The LSI oscillator is connected to TIM9 CH3
       The Rising edge is used as active edge,
       The TIM9 CCDAT3 is used to compute the frequency value */
    TIM_ICInitStructure.Channel = TIM_CH_3;
    TIM_ICInitStructure.IcPolarity = TIM_IC_POLARITY_RISING;
    TIM_ICInitStructure.IcSelection = TIM_IC_SELECTION_DIRECTTI;
    TIM_ICInitStructure.IcPrescaler = TIM_IC_PSC_DIV8;
    TIM_ICInitStructure.IcFilter = 0;
    TIM_ICInit(TIM9, &TIM_ICInitStructure);
    TIM9->CTRL1 |= TIM_CTRL1_C3SEL;
    /* TIM9 Counter Enable */
    TIM_Enable(TIM9, ENABLE);
    /* Reset the flags */
    TIM9->STS = 0;
    /* Enable the CC3 Interrupt Request */
    TIM_ConfigInt(TIM9, TIM_INT_CC3, ENABLE);
    /* Wait until the TIM9 get 3 LSI edges */
    while (CaptureNumber != 3)
        ;

    /* Disable TIM9 CC3 Interrupt Request */
    TIM_ConfigInt(TIM9, TIM_INT_CC3, DISABLE);
    TIM_Enable(TIM9, DISABLE);
    TIM_DeInit(TIM9);
}

extern void gpio_init(void);
extern void lcd_gpio_init(void);

void unused_gpio_init(void)
{
    GPIOA->PMODE = 0xffffffff;
    GPIOB->PMODE = 0xffffffff;
    GPIOC->PMODE = 0xffffffff;
    GPIOD->PMODE = 0xffffffff;
}

/**
 * @brief 初始化板子硬件
 */
void board_init(void)
{
    /* Config NVIC priority group */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /*Enable GPIO Clocks*/
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_GPIOC | RCC_APB2_PERIPH_GPIOD, ENABLE);
    LsiMeasurment();
    Delay(100);
    unused_gpio_init();
    gpio_init();
    lcd_gpio_init();
    #if !defined TEMPERATURE_HIGH_LOW_TEST
    hal_uart_init(UART_DEBUG, 115200);
    #else
    hal_uart_init(UART_DEBUG, 9600);
    #endif
    hal_adc_init();
    hal_flash_init();
    hal_lcd_init();
    hal_tim_init();
    hal_rtc_init();
    hal_watchdog_init();
}

/**
 * @brief 初始化调试功能
 */
void board_debug_init(void)
{
    GPIO_InitType gpioinitstruct;

    /*Init GPIO init struct*/
    GPIO_InitStruct(&gpioinitstruct);

    gpioinitstruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioinitstruct.Pin = GPIO_PIN_13;
    gpioinitstruct.GPIO_Alternate = GPIO_AF0_SW_JTAG;
    gpioinitstruct.GPIO_Pull = GPIO_Pull_Up;
    GPIO_InitPeripheral(GPIOA, &gpioinitstruct);

    gpioinitstruct.Pin = GPIO_PIN_14;
    gpioinitstruct.GPIO_Pull = GPIO_Pull_Down;
    GPIO_InitPeripheral(GPIOA, &gpioinitstruct);

    // DBG_ConfigPeriph(DBG_SLEEP, ENABLE);
    // DBG_ConfigPeriph(DBG_STOP, ENABLE);
    DBG_ConfigPeriph(DBG_IWDG_STOP, ENABLE);

    dis_low_power = true;
}

/**
 * @brief 反初始化调试功能
 */
void board_debug_deinit(void)
{
    GPIO_InitType gpioinitstruct;

    /*Init GPIO init struct*/
    GPIO_InitStruct(&gpioinitstruct);

    gpioinitstruct.GPIO_Mode = GPIO_Mode_Analog;
    gpioinitstruct.Pin = GPIO_PIN_13 | GPIO_PIN_14;
    gpioinitstruct.GPIO_Pull = GPIO_No_Pull;
    GPIO_InitPeripheral(GPIOA, &gpioinitstruct);

    // DBG_ConfigPeriph(DBG_SLEEP, DISABLE);
    // DBG_ConfigPeriph(DBG_STOP, DISABLE);
    DBG_ConfigPeriph(DBG_IWDG_STOP, DISABLE);

    dis_low_power = false;
}

/**
 * @brief  This function handles Hard Fault exception.
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}
