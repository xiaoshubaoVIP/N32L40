#ifndef __SC01_CONFIG_H__
#define __SC01_CONFIG_H__

/* LCD功能 */
#define SUPPORT_LCD_FUNCTION

/* 不可更换电池 */
#define IRREPLACEABLE_BATTARY

/* SMOKE功能 */
#define SUPPORT_SMOKE_FUNCTION
/* smoke温度补偿 */
#define SUPPORT_SMOKETEMPCOMPENSATION
#include "config/smoke/sc01_temp_comp.h"

/* 迷宫参数 */
#include "config/smoke/big_maze.h"

/* CO功能 */
#define SUPPORT_CO_FUNCTION
/* CO参数 */
#include "config/co/tgs5042.h"

#if defined SUPPORT_RADIO_FUNCTION
    #define BRAND_X_SENSE "X-Sense"
#endif

#define BOARD_TYPE BOARD_SC01
#endif
