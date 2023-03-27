#include "n32l4/inc/hal_lcd.h"
#include "bsp_config.h"
#include "n32l40x.h"

#define COM_NUM 4
#define ENABLE_LCD

static uint8_t lcd_power;
extern uint32_t lcd_deadtime;

void hal_lcd_init(void)
{
#ifdef ENABLE_LCD

#endif
}

/**
 * @brief 更新LCD显示
 *
 * @param data 更新的数据
 * @param start 开始内存
 * @param end 结束内存
 */
void hal_lcd_write(uint32_t data[], uint8_t start, uint8_t end)
{
#ifdef ENABLE_LCD
    if (lcd_power == false)
        return;

    /* wait update display request finish */
    while (RESET == (__LCD_GET_FLAG(LCD_FLAG_UDD)))
        ;

    for (; start < end; start++)
    {
        while (__LCD_GET_FLAG(LCD_FLAG_UDR))
            ;
        LCD->RAM_COM[start] = data[start];
    }

    /*Clear UDD flag*/
    __LCD_CLEAR_FLAG(LCD_FLAG_UDD_CLEAR);

    /* set update display request bit*/
    __LCD_UPDATE_REQUEST();
#endif
}

/**
 * @brief 使能Lcd
 *
 */
void hal_lcd_enble(void)
{
#ifdef ENABLE_LCD
    if (lcd_power == false)
    {
        LCD_InitType Init;

        LCD_DeInit();
        LCD_ClockConfig(LCD_CLK_SRC_LSI);

        /*LCD parameter config*/
        Init.Divider = LCD_DIV_20;
        Init.Prescaler = LCD_PRESCALER_8;
        Init.Duty = LCD_DUTY_1_4;
        Init.Bias = LCD_BIAS_1_3;
        Init.VoltageSource = LCD_VOLTAGESOURCE_EXTERNAL;
        Init.Contrast = LCD_CONTRASTLEVEL_4;
        Init.DeadTime = lcd_deadtime;
        Init.PulseOnDuration = LCD_PULSEONDURATION_0;
        Init.HighDrive = LCD_HIGHDRIVE_DISABLE;
        Init.HighDriveBuffer = LCD_HIGHDRIVEBUFFER_DISABLE;
        Init.BlinkMode = LCD_BLINKMODE_OFF;
        Init.BlinkFreq = LCD_BLINKFREQ_DIV_1024;
        Init.MuxSegment = LCD_MUXSEGMENT_DISABLE;

        /*config and start LCD controller*/
        LCD_Init(&Init);
        LCD_RamClear();
        LCD->CTRL |= LCD_CTRL_LCDEN_Msk;

        lcd_power = true;
    }
#endif
}

/**
 * @brief 关闭Lcd
 *
 */
void hal_lcd_disenble(void)
{
#ifdef ENABLE_LCD
    if (lcd_power == true)
    {
        LCD->CTRL &= ~LCD_CTRL_LCDEN_Msk;
        LCD_DeInit();

        lcd_power = false;
    }
#endif
}

/**
 * @brief lcd切换对比度，用于在不同电压显示效果一致
 *
 * @param vol 当前LCD电压
 */
void hal_lcd_calibration(uint16_t vol)
{
#ifdef ENABLE_LCD
    if (lcd_power == false)
        return;
    LCD->CTRL &= ~LCD_CTRL_LCDEN_Msk;
    change_lcd_contrast(vol);
    LCD->CTRL |= LCD_CTRL_LCDEN_Msk;
#endif
}
