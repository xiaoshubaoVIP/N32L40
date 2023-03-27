#ifndef __CONFIG_H__
#define __CONFIG_H__

/*****************************************************************************
 * @brief 软件版本
 */
#define VERSION "0.1.3"

/*****************************************************************************
 * @brief 地区标准
 */
//#define STANDARD_UL2034_UL217 "UL"
 #define STANDARD_EN50291_EN14604 "EN"

/*****************************************************************************
 * @brief 板级宏开关
 */
// #define BOARD_SC01 "SC01"
// #define BOARD_XC01 "XC01"
#define BOARD_SC200 "SC200"

//=============================================================================
//RF功能开关
//=============================================================================
// #define SUPPORT_RADIO_FUNCTION "RF"
/**
 * @brief 品牌频点选择
 * 
 */
#define BRAND_X_SENSE "X-Sense"
// #define BRAND_AEGISLINK "Aegislink"

// #define SUPPORT_RADIO_TEST //测试频点
//#define SUPPORT_HUB_FUNC
//=============================================================w================
//芯片宏开关
//=============================================================================
// #define CPU_STM8L51X "STM8"
#define CHIP_N32L406X "N32L"

/**
 * @brief 开启/关闭TUV认证算法
 */
// #define TUV_RENGZHENG

/**
 * @brief 睡眠时间
 */
#define SLEEP_TIME (10)                           // 10s
#define SYSTEM_ONE_DAY_COUNT (86400 / SLEEP_TIME) // 24h=24*60*60=86400/SLEEP_TIME=8640


#if defined STANDARD_EN50291_EN14604     //欧标频点
    #if defined BRAND_X_SENSE
        #define SUPPORT_X_SENSE_EN
    #else
        #define SUPPORT_AEGISLINK_EN 
    #endif
#elif defined  STANDARD_UL2034_UL217     //美标频点
    #if defined BRAND_X_SENSE
        #define SUPPORT_X_SENSE_UL
    #else
        #define SUPPORT_AEGISLINK_UL 
    #endif
#else
    #error
#endif

#if defined BOARD_SC01
    #include "config/sc01_config.h"
#elif defined  BOARD_XC01
    #include "config/xc01_config.h"
#elif defined BOARD_SC200
    #include "config/sc200_config.h"
#else
    #error
#endif

#if defined CHIP_N32L406X
    #define CPU_TYPE CHIP_N32L406X
#elif defined  CPU_STM8L51X
    #define CPU_TYPE CPU_STM8L51X
#else
    #error
#endif
#endif
