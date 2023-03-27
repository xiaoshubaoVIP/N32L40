#ifndef __XC01_CONFIG_H__
#define __XC01_CONFIG_H__

/* LCD���� */
#define SUPPORT_LCD_FUNCTION
#define LCD_SIMPLE // С��LCD

/* ���ɸ������ */
// #define IRREPLACEABLE_BATTARY

/* ���� */
#define LED_IDLE_TICK 3

/* CO���� */
#define SUPPORT_CO_FUNCTION
/* CO peak */
#define SUPPORT_PEAK_FUNCTION
/* CO���� */
#include "config/co/tgs5141.h"

#if defined SUPPORT_RADIO_FUNCTION
    #define BRAND_X_SENSE "X-Sense"
#endif

#define BOARD_TYPE BOARD_XC01
#endif
