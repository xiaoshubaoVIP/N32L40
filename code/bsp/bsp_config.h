#ifndef __BSP_CONFIG_H__
#define __BSP_CONFIG_H__

#include "config.h"

#include "n32l4/inc/board.h"
#include "n32l4/inc/hal_tim.h"
#include "n32l4/inc/hal_flash.h"
#include "n32l4/inc/hal_adc.h"
#include "n32l4/inc/hal_rtc.h"
#include "n32l4/inc/hal_uart.h"
#include "n32l4/inc/hal_gpio.h"
#include "n32l4/inc/hal_watchdog.h"
#include "n32l4/inc/hal_lcd.h"

#ifdef BOARD_SC01
    #include "n32l4/sc/sc01_describe.h"
#elif defined BOARD_XC01
    #include "n32l4/xc/xc01_describe.h"
#elif defined BOARD_SC200
    #include "n32l4/sc/sc200_describe.h"
#endif

#endif
