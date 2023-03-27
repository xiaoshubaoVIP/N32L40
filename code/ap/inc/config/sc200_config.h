#ifndef __SC200_CONFIG_H__
#define __SC200_CONFIG_H__

/* LCD功能 */
#define SUPPORT_LCD_FUNCTION
#define LCD_SIMPLE // 小板LCD

/* 不可更换电池 */
// #define IRREPLACEABLE_BATTARY

/* SMOKE功能 */
#define SUPPORT_SMOKE_FUNCTION
/* smoke温度补偿*/
#define SUPPORT_SMOKETEMPCOMPENSATION
#include "config/smoke/sc200_temp_comp.h"

/* 迷宫参数 */
#include "config/smoke/AC_small_maze.h"

/* CO功能 */
#define SUPPORT_CO_FUNCTION
/* CO参数 */
#include "config/co/hd50.h"

#if defined SUPPORT_RADIO_FUNCTION
    #define PAIR_KEY
    #define BRAND_AEGISLINK "Aegislink"
#endif

#define BOARD_TYPE BOARD_SC200
#endif
