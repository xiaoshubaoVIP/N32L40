#ifndef __SC01_CONFIG_H__
#define __SC01_CONFIG_H__

/* LCD���� */
#define SUPPORT_LCD_FUNCTION

/* ���ɸ������ */
#define IRREPLACEABLE_BATTARY

/* SMOKE���� */
#define SUPPORT_SMOKE_FUNCTION
/* smoke�¶Ȳ��� */
#define SUPPORT_SMOKETEMPCOMPENSATION
#include "config/smoke/sc01_temp_comp.h"

/* �Թ����� */
#include "config/smoke/big_maze.h"

/* CO���� */
#define SUPPORT_CO_FUNCTION
/* CO���� */
#include "config/co/tgs5042.h"

#if defined SUPPORT_RADIO_FUNCTION
    #define BRAND_X_SENSE "X-Sense"
#endif

#define BOARD_TYPE BOARD_SC01
#endif
