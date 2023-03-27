#ifndef __XC01_CONFIG_H__
#define __XC01_CONFIG_H__

/* LCD功能 */
#define SUPPORT_LCD_FUNCTION
#define LCD_SIMPLE // 小板LCD

/* 不可更换电池 */
// #define IRREPLACEABLE_BATTARY

/* 闪灯 */
#define LED_IDLE_TICK 3

/* CO功能 */
#define SUPPORT_CO_FUNCTION
/* CO peak */
#define SUPPORT_PEAK_FUNCTION
/* CO参数 */
#include "config/co/tgs5141.h"

#if defined SUPPORT_RADIO_FUNCTION
    #define BRAND_X_SENSE "X-Sense"
#endif

#define BOARD_TYPE BOARD_XC01
#endif
