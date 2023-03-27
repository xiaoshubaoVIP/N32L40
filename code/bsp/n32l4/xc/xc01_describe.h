#ifndef __XC01_DESCRIBE_H__
#define __XC01_DESCRIBE_H__

enum
{
    ADC_BATTERY, /* ��� */
    ADC_SMOKE,   /* SMOKE������ */
    ADC_CO,      /* CO������ */
    ADC_TEMP,    /* �¶ȴ����� */
};

enum
{
    PIN_LED_G,     /* �̵� */
    PIN_LED_R,     /* ��� */
    PIN_CO_TEST,   /* CO����ʹ�� */
    PIN_NTC_POWER, /* ntc��Դʹ�� */
    PIN_BEEP,      /* ������ */
    PIN_BACKLIGHT, /* ��ʾ������ */
    PIN_TEST,      /* ������԰��� */
    PIN_SWIM,      /* ����ʹ�� */
    PIN_BAT_DET,   /* ��ؼ��ʹ�� */
    PIN_FCSB,      /* SPI FSB */
    PIN_CSB,       /* SPI CSB */
    PIN_SDIO,      /* SPI SDIO */
    PIN_SCLK,      /* SPI SCLK */
    PIN_WAKEUP,    /* 2300����mcu */
    PIN_RADIOTEST, /* 2300���԰��� */
    PIN_NUM,

    /* ����Ϊ�˼������� */
    PIN_LED_Y,      /* �Ƶ� */
    PIN_LCDPWR,     /* LCDʹ�� */
    PIN_SMOKE_EN,   /* SMOKEʹ�� */
    PIN_SMOKE_IR,   /* SMOKE ����ʹ�� */
    PIN_NET_PIN,    /* ͨѶģ��ʹ�� */
    PIN_SMKOE_OP_1, /* SMOKE �˷�ʹ��1 */
    PIN_SMOKE_OP_2, /* SMOKE �˷�ʹ��2 */
    PIN_MCUBAT_DET, /* MCU��ؼ��ʹ�� */
    PIN_LED_B,      /* ���� */
    PIN_SMOKE_PWM_TEST,
};

enum
{
    UART_NET,   /* ���� */
    UART_DEBUG, /* ���� */
};

/* �����Ƶ� */
// #define Independent_YellowLed

void lcd_gpio_init(void);
void gpio_init(void);
uint8_t lcd_com_conversion(uint8_t com);
uint64_t lcd_ram_conversion(uint32_t ram);
uint32_t flash_data_start_physical_address(void);
void change_lcd_contrast(uint16_t vol);
#endif
