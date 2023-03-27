#include "n32l40x.h"
#include "config.h"
#include "n32l4/xc/xc01_describe.h"
#include "n32l4/inc/hal_adc.h"
#include "n32l4/inc/hal_gpio.h"

#if defined BOARD_XC01

#define RESOLUTION 12 // 12bit分辨率

/* LCD对比度等级电压 */
#define LCD_CONTRAST_LEVEL_1 3000
#define LCD_CONTRAST_LEVEL_2 2900

typedef const struct
{
    uint8_t SamplingTime;
    uint8_t Channels;
} hal_adc;

typedef const struct
{
    GPIO_Module *port;
    uint16_t pin;
} hal_gpio;

const hal_adc adc_describe[] = {
    [ADC_BATTERY] = {ADC_SAMP_TIME_41CYCLES5, ADC_CH_18},
    [ADC_CO] = {ADC_SAMP_TIME_28CYCLES5, ADC_CH_1_PA0},
    [ADC_TEMP] = {ADC_SAMP_TIME_13CYCLES5, ADC_CH_10_PB1},
};

const hal_gpio gpio_describe[] = {
    [PIN_LED_G] = {GPIOB, GPIO_PIN_11},
    [PIN_LED_R] = {GPIOB, GPIO_PIN_10},
    [PIN_CO_TEST] = {GPIOA, GPIO_PIN_1},
    [PIN_NTC_POWER] = {GPIOB, GPIO_PIN_2},
    [PIN_BEEP] = {GPIOB, GPIO_PIN_13},
    [PIN_BACKLIGHT] = {GPIOA, GPIO_PIN_2},
    [PIN_TEST] = {GPIOB, GPIO_PIN_0},
    [PIN_SWIM] = {GPIOA, GPIO_PIN_13},
    [PIN_BAT_DET] = {GPIOB, GPIO_PIN_12},
    [PIN_FCSB] = {GPIOA, GPIO_PIN_3},
    [PIN_CSB] = {GPIOA, GPIO_PIN_4},
    [PIN_SDIO] = {GPIOA, GPIO_PIN_5},
    [PIN_SCLK] = {GPIOA, GPIO_PIN_6},
    [PIN_WAKEUP] = {GPIOA, GPIO_PIN_7},
    [PIN_RADIOTEST] = {GPIOA, GPIO_PIN_14},
};

static uint16_t vref_value;             // 基准电压
uint32_t lcd_deadtime = LCD_DEADTIME_0; // lcd死区等级，用于调节电压

/**
 * @brief lcd引脚初始化
 *
 */
void lcd_gpio_init(void)
{
    GPIO_InitType gpioinitstruct;

    /*Init GPIO init struct*/
    GPIO_InitStruct(&gpioinitstruct);

    /*Configure peripheral GPIO output for LCD*/
    gpioinitstruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_15;
    gpioinitstruct.GPIO_Mode = GPIO_Mode_Analog;
    gpioinitstruct.GPIO_Pull = GPIO_No_Pull;
    gpioinitstruct.GPIO_Alternate = GPIO_AF10_LCD;
    GPIO_InitPeripheral(GPIOA, &gpioinitstruct);

    gpioinitstruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitPeripheral(GPIOB, &gpioinitstruct);

    gpioinitstruct.Pin = GPIO_PIN_0;
    GPIO_InitPeripheral(GPIOD, &gpioinitstruct);
}

uint8_t lcd_com_conversion(uint8_t com)
{
    uint8_t temp[] = {3, 0, 1, 2};

    if (com < sizeof(temp))
        return temp[com];
    else
        return com;
}

/**
 * @brief lcd屏内存转换实际硬件脚
 *
 * @param ram lcd内存
 * @return uint32_t
 */
uint64_t lcd_ram_conversion(uint32_t ram)
{
    uint64_t temp = 0;
    uint64_t mask[8] = {(1 << 16), ((uint64_t)1 << 32), (1 << 9), (1 << 8), (1 << 7), (1 << 17), (1 << 15), (1 << 14)};

    for (uint8_t i = 0; i < 8; i++)
        if ((ram & (1 << i)) > 0)
            temp |= mask[i];

    return temp;
}

/**
 * @brief gpio初始化
 *
 */
void gpio_init(void)
{
    GPIO_InitType gpioinitstruct;

    /*Init GPIO init struct*/
    GPIO_InitStruct(&gpioinitstruct);

    /* 输出引脚初始化 */
    gpioinitstruct.GPIO_Mode = GPIO_Mode_Out_PP;
    for (uint8_t i = 0; i < PIN_NUM; i++)
    {
        gpioinitstruct.Pin = gpio_describe[i].pin;
        GPIO_InitPeripheral(gpio_describe[i].port, &gpioinitstruct);
    }

    // GPIO_WriteBit(gpio_describe[PIN_LCDPWR].port, gpio_describe[PIN_LCDPWR].pin, Bit_SET);

    /* ADC引脚初始化 */
    gpioinitstruct.GPIO_Mode = GPIO_Mode_Input;
    gpioinitstruct.Pin = GPIO_PIN_0;
    GPIO_InitPeripheral(GPIOA, &gpioinitstruct);

    gpioinitstruct.Pin = GPIO_PIN_1;
    GPIO_InitPeripheral(GPIOB, &gpioinitstruct);

    /* 输入引脚初始化 */
    gpioinitstruct.GPIO_Pull = GPIO_Pull_Up;
    gpioinitstruct.Pin = gpio_describe[PIN_TEST].pin;
    GPIO_InitPeripheral(gpio_describe[PIN_TEST].port, &gpioinitstruct);

    hal_gpio_setirq(PIN_TEST, IT_TRIGGER_FALLING);
}

/**
 * @brief 获取系统参数保存地址
 *
 * @return uint32_t
 */
uint32_t flash_data_start_physical_address(void)
{
    /* 最后一页（2kb） */
    return ((DBG->ID & 0x000F0000) >> 1) + FLASH_BASE - 2048;
}

#define CO_ANLOG_MODE 0xc00UL
/**
 * @brief 读取adc电压值
 *
 * @param id 设备号
 * @return unsigned short 返回电压值mV
 */
uint16_t hal_adc_read_vol(adc_id_t id)
{
    uint32_t value;
    GPIO_InitType gpioinitstruct;
    uint16_t vol;

    /* CO口启用 */
    if (id == ADC_CO)
    {
        // GPIOA->PMODE |= CO_ANLOG_MODE;
        GPIO_InitStruct(&gpioinitstruct);
        gpioinitstruct.Pin = GPIO_PIN_0;
        gpioinitstruct.GPIO_Mode = GPIO_Mode_Input;
        GPIO_InitPeripheral(GPIOA, &gpioinitstruct);
    }

    value = hal_adc_read(id);
    if (id == ADC_BATTERY)
    {
        vref_value = (uint32_t)(2048 << RESOLUTION) / value;
        vol = vref_value;
    }
    else
    {
        vol = (uint32_t)(value * vref_value) >> RESOLUTION;
    }

    /* CO口禁用，防止电流倒灌 */
    if (id == ADC_CO)
    {
        // GPIOA->PMODE &= ~CO_ANLOG_MODE;
        gpioinitstruct.GPIO_Mode = GPIO_Mode_Analog;
        GPIO_InitPeripheral(GPIOA, &gpioinitstruct);
    }

    return vol;
}

/**
 * @brief 更改LCD对比度
 *
 * @param vol 当前电压
 */
void change_lcd_contrast(uint16_t vol)
{
    LCD->CTRL &= ~LCD_CTRL_LCDEN_Msk;
    if (vol > LCD_CONTRAST_LEVEL_1)
        lcd_deadtime = LCD_DEADTIME_1;
    else
        lcd_deadtime = LCD_DEADTIME_0;

    __LCD_DEADTIME_CONFIG(lcd_deadtime);
    LCD->CTRL |= LCD_CTRL_LCDEN_Msk;
}

/**
 * @brief  External lines 5 to 9 interrupt.
 */
void EXTI0_IRQHandler(void)
{
    if (RESET != EXTI_GetITStatus(EXTI_LINE0))
    {
        EXTI_ClrITPendBit(EXTI_LINE0);
    }
}

/**
 * @brief  External lines 5 to 9 interrupt.
 */
void EXTI15_10_IRQHandler(void)
{
    if (RESET != EXTI_GetITStatus(EXTI_LINE14))
    {
        EXTI_ClrITPendBit(EXTI_LINE14);
    }
}

/**
 * @brief  External lines 5 to 9 interrupt.
 */
void EXTI9_5_IRQHandler(void)
{
    if (RESET != EXTI_GetITStatus(EXTI_LINE7))
    {
        EXTI_ClrITPendBit(EXTI_LINE7);
#ifdef SUPPORT_RADIO_FUNCTION
        extern void SetRadioRxIntFlag(void);
        SetRadioRxIntFlag();
#endif
    }
}

#endif
