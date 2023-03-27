#ifndef __SC200_CONFIG_H__
#define __SC200_CONFIG_H__

/* LCD���� */
#define SUPPORT_LCD_FUNCTION
#define LCD_SIMPLE // С��LCD

/* ���ɸ������ */
// #define IRREPLACEABLE_BATTARY

/* SMOKE���� */
#define SUPPORT_SMOKE_FUNCTION
/* smoke�¶Ȳ���*/
#define SUPPORT_SMOKETEMPCOMPENSATION
#include "config/smoke/sc200_temp_comp.h"

/* �Թ����� */
#include "config/smoke/AC_small_maze.h"

/* CO���� */
#define SUPPORT_CO_FUNCTION
/* CO���� */
#include "config/co/hd50.h"

#if defined SUPPORT_RADIO_FUNCTION
    #define PAIR_KEY
    #define BRAND_AEGISLINK "Aegislink"
#endif

#define BOARD_TYPE BOARD_SC200
#endif
