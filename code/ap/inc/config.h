#ifndef __CONFIG_H__
#define __CONFIG_H__

/*****************************************************************************
 * @brief ����汾
 */
#define VERSION "0.1.3"

/*****************************************************************************
 * @brief ������׼
 */
//#define STANDARD_UL2034_UL217 "UL"
 #define STANDARD_EN50291_EN14604 "EN"

/*****************************************************************************
 * @brief �弶�꿪��
 */
// #define BOARD_SC01 "SC01"
// #define BOARD_XC01 "XC01"
#define BOARD_SC200 "SC200"

//=============================================================================
//RF���ܿ���
//=============================================================================
// #define SUPPORT_RADIO_FUNCTION "RF"
/**
 * @brief Ʒ��Ƶ��ѡ��
 * 
 */
#define BRAND_X_SENSE "X-Sense"
// #define BRAND_AEGISLINK "Aegislink"

// #define SUPPORT_RADIO_TEST //����Ƶ��
//#define SUPPORT_HUB_FUNC
//=============================================================w================
//оƬ�꿪��
//=============================================================================
// #define CPU_STM8L51X "STM8"
#define CHIP_N32L406X "N32L"

/**
 * @brief ����/�ر�TUV��֤�㷨
 */
// #define TUV_RENGZHENG

/**
 * @brief ˯��ʱ��
 */
#define SLEEP_TIME (10)                           // 10s
#define SYSTEM_ONE_DAY_COUNT (86400 / SLEEP_TIME) // 24h=24*60*60=86400/SLEEP_TIME=8640


#if defined STANDARD_EN50291_EN14604     //ŷ��Ƶ��
    #if defined BRAND_X_SENSE
        #define SUPPORT_X_SENSE_EN
    #else
        #define SUPPORT_AEGISLINK_EN 
    #endif
#elif defined  STANDARD_UL2034_UL217     //����Ƶ��
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
