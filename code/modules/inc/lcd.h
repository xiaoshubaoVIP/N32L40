/**
 ******************************************************************************
 * @file    lcd.h
 * @author  zhenrong.peng
 * @version V1.0
 * @date    25-05-2017
 * @brief   This file contains definitions for HT1621B
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_H
#define __LCD_H

#include <stdint.h>
#include "bsp_config.h"

#define DISPLAY_CLEAR (0x00)
#define DISPLAY_ALL (0xFF)

/* Exported types ------------------------------------------------------------*/

#define ASCII_NUM_0 ((uint8_t)0x30)
#define LCD_DIS_NULL (ASCII_NUM_0 + 10)
#define LCD_DIS_LINE (ASCII_NUM_0 + 11)
#define LCD_DIS_A (ASCII_NUM_0 + 12)
#define LCD_DIS_B (ASCII_NUM_0 + 13)
#define LCD_DIS_L (ASCII_NUM_0 + 14)
#define LCD_DIS_E (ASCII_NUM_0 + 15)
#define LCD_DIS_R (ASCII_NUM_0 + 16)
#define LCD_DIS_P (ASCII_NUM_0 + 17)
#define LCD_DIS_S (ASCII_NUM_0 + 18)
#define LCD_DIS_n (ASCII_NUM_0 + 19)
#define LCD_DIS_d (ASCII_NUM_0 + 20)

#define NUM_1 (ASCII_NUM_0 + 1)
#define NUM_2 (ASCII_NUM_0 + 2)
#define NUM_3 (ASCII_NUM_0 + 3)
#define NUM_4 (ASCII_NUM_0 + 4)
#define NUM_5 (ASCII_NUM_0 + 5)
#define NUM_6 (ASCII_NUM_0 + 6)
#define NUM_7 (ASCII_NUM_0 + 7)
#define NUM_8 (ASCII_NUM_0 + 8)
#define NUM_9 (ASCII_NUM_0 + 9)

#define LCD_START (0)
#define LCD_END (2)

#define LCD_BACK_LIGHT_ON() LCD_BACK_LIGHT(1)
#define LCD_BACK_LIGHT_OFF() LCD_BACK_LIGHT(0)

typedef enum
{
    LCD_BATTERY_SEG = 7,
    LCD_BATTERY_LEVEL1_SEG = 7,
    LCD_BATTERY_LEVEL2_SEG = 7,
    LCD_BATTERY_LEVEL3_SEG = 7,
#ifdef LCD_SIMPLE
    LCD_CO_SEG = 2,
#else
    LCD_CO_SEG = 6,
#endif
    LCD_CO_PPM_SEG = 0,

    LCD_PEAK_SEG = 4,
    LCD_OUT_PWR_SEG = 6,
    LCD_WIFI_SEG = 6,

    LCD_TWO_FLOAT_SEG = 4,
    LCD_ONE_FLOAT_SEG = 2,
    LCD_PERCENT_SEG = 6,

    LCD_DIS_LINE1_SEG = 4,
    LCD_DIS_LINE2_SEG = 2,
    LCD_DIS_LINE3_SEG = 0,

    LCD_DIS_2A_SEG = 3,
    LCD_DIS_2B_SEG = 2,
    LCD_DIS_2C_SEG = 2,
    LCD_DIS_2D_SEG = 3,
    LCD_DIS_2E_SEG = 3,
    LCD_DIS_2F_SEG = 3,
    LCD_DIS_2G_SEG = 2,

    LCD_DIS_3A_SEG = 1,
    LCD_DIS_3B_SEG = 0,
    LCD_DIS_3C_SEG = 0,
    LCD_DIS_3D_SEG = 1,
    LCD_DIS_3E_SEG = 1,
    LCD_DIS_3F_SEG = 1,
    LCD_DIS_3G_SEG = 0,

} LCDPointSEGTag;

typedef enum
{
    // BATTERY
    LCD_BATTERY_COM = 3,
    LCD_BATTERY_LEVEL1_COM = 1,
    LCD_BATTERY_LEVEL2_COM = 2,
    LCD_BATTERY_LEVEL3_COM = 0,
#ifdef LCD_SIMPLE
    LCD_CO_COM = 0,
#else
    LCD_CO_COM = 2,
#endif

    LCD_CO_PPM_COM = 0,

    LCD_Data1_COM = 2, // 1G
    LCD_Data2_COM = 2, // 2G
    LCD_Data3_COM = 2, // 3G

    LCD_PEAK_COM = 0,
    LCD_WIFI_COM0 = 0,
    LCD_WIFI_COM1 = 1,
    LCD_WIFI_COM2 = 2,

    LCD_OUT_PWR_COM = 3,

    LCD_TWO_FLOAT_COM = 0,
    LCD_ONE_FLOAT_COM = 0,
    LCD_PERCENT_COM = 0,

    LCD_DIS_LINE_COM = 2,

    LCD_DIS_0_COM = 0,
    LCD_DIS_1_COM = 1,
    LCD_DIS_2_COM = 2,
    LCD_DIS_3_COM = 3,

} LCDPointSCOMTag;

typedef enum
{
    eLcdOFF,
    eLcdON,
    eLcdNULL,
    eLcdUpdate,

} eLcdState;

typedef enum
{
    eStr_End,
    eStr_Err,
    eStr_LINE,
    eStr_Lb,
    eStr_PAS,
    eStr_E03,
    eStr_E04,
    eStr_E06,
    eStr_E07,
    eStr_E08,
    eStr_E09,
    eStr_E10,
    eStr_E11,
    eStr_E12,
} eLcdStr;

typedef enum
{
    eLcdPointOFF,
    eLcdPointON
} eLcdPointState;

/* Exported Functions ------------------------------------------------------------*/

void LcdSetMem(uint8_t data);
int LcdDisplayString(uint8_t *pstr, uint8_t start, uint8_t end);
void LcdDisplayPoint(eLcdPointState eType, uint8_t com, uint8_t seg);
void SetLcdPointData(eLcdPointState eType, uint8_t com, uint8_t seg);
int SetLcdString(uint8_t *pstr, uint8_t start, uint8_t end);
void LcdStoreReg(void);
void LCD_Clear(void);
void LCD_Control(eLcdState state);

void LCD_DisplayNum(uint16_t num);
void LCD_BACK_LIGHT(unsigned char state);
void BatteryLcdDisplay(uint8_t BatteryState);
void LCD_SystemStartAnimation(uint8_t line);
void LCD_DisplayStr(eLcdStr str);
void LCD_DsiplayCoFlg(eLcdState control);
void LCDStandyDisplay(uint16_t ppm, BOOL ClrFlg);

#endif /* __LCD_H */

/************************END OF FILE***********************/
