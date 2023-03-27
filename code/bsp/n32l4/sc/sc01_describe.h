#ifndef __SC01_DESCRIBE_H__
#define __SC01_DESCRIBE_H__

enum
{
    ADC_BATTERY, /* 电池 */
    ADC_SMOKE,  /* SMOKE传感器 */
    ADC_CO,     /* CO传感器 */
    ADC_TEMP,   /* 温度传感器 */
};

enum
{
    PIN_LED_G,     /* 绿灯 */
    PIN_LED_R,     /* 红灯 */
    PIN_LED_Y,     /* 黄灯 */
    PIN_CO_TEST,   /* CO测试使能 */
    PIN_NTC_POWER, /* ntc电源使能 */
    PIN_BEEP,      /* 蜂鸣器 */
    PIN_BACKLIGHT, /* 显示屏背光 */
    PIN_LCDPWR,    /* LCD使能 */
    PIN_SMOKE_EN,  /* SMOKE使能 */
    PIN_SMOKE_IR,  /* SMOKE 红外使能 */
    PIN_TEST,      /* 输入测试按键 */
    PIN_FCSB,      /* SPI FSB */
    PIN_CSB,       /* SPI CSB */
    PIN_SDIO,      /* SPI SDIO */
    PIN_SCLK,      /* SPI SCLK */
    PIN_WAKEUP,    /* 2300唤醒mcu */
    PIN_BAT_DET,    /* 电池检测使能 */
    PIN_SWIM,       /* 产测使能 */
    PIN_RADIOTEST, /* 2300测试按键 */
    PIN_NUM,


    /* 以下为了兼容增加 */
    PIN_NET_PIN,    /* 通讯模块使能 */
    PIN_SMKOE_OP_1, /* SMOKE 运放使能1 */
    PIN_SMOKE_OP_2, /* SMOKE 运放使能2 */
    PIN_LED_B,      /* 蓝灯 */
    PIN_SMOKE_PWM_TEST,
};

enum
{
    UART_NET,   /* 网络 */
    UART_DEBUG, /* 调试 */
};

/* 独立黄灯 */
#define Independent_YellowLed

void lcd_gpio_init(void);
void gpio_init(void);
uint8_t lcd_com_conversion(uint8_t com);
uint64_t lcd_ram_conversion(uint32_t ram);
uint32_t flash_data_start_physical_address(void);
void change_lcd_contrast(uint16_t vol);
#endif
