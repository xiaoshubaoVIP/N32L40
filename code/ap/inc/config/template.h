
/* LCD功能 */
#define SUPPORT_LCD_FUNCTION

/* 不可更换电池 */
#define IRREPLACEABLE_BATTARY

/* SMOKE功能 */
#define SUPPORT_SMOKE_FUNCTION

/* smoke温度补偿 */
#define SUPPORT_SMOKETEMPCOMPENSATION

/* 温度补偿参数，以下头文件选择一个 */
    // #include "config/smoke/sc200_temp_comp.h"
    // #include "config/smoke/sc01_temp_comp.h"

/* 迷宫参数，以下头文件选择一个 */
    // #include "config/smoke/big_maze.h"
    // #include "config/smoke/AC_small_maze.h"

/* CO功能 */
#define SUPPORT_CO_FUNCTION

/* CO参数，以下头文件选择一个 */
    // #include "config/co/hd50.h"
    // #include "config/co/tgs5042.h"
    // #include "config/co/tgs5141.h"

/* CO峰值功能 */
#define SUPPORT_PEAK_FUNCTION

#define BOARD_TYPE BOARD_SC01

