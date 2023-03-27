/**
 * @file hd50.h
 * @brief 合德传感器
 * @version 0.1
 * @date 2022-12-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __HD50_H__
#define __HD50_H__

// CO传感器自检参数
#define SENSOR_OPEN_FAULT_SAMPLE 1800 // 2100//2300//2.3v
#define SENSOR_OPEN_SAMPLE 600        // 600mv
#define SENSOR_SHORT_SAMPLE_MAX 2200  // 2600//2.6v
#define SENSOR_SHORT_SAMPLE_MIN 1000  // 1v
// CO校机参数
#define PPM_CARIBRATON_STEP0_THRESHOLD_MIN (80) // mv
#define PPM_CARIBRATON_STEP0_THRESHOLD_MAX (250) // 300     // mv
#define PPM_CARIBRATON_STEP1_THRESHOLD_MIN (150) // mv
#define PPM_CARIBRATON_STEP1_THRESHOLD_MAX (500) // 600     // mv
#if defined STANDARD_UL2034_UL217
    #define PPM_CARIBRATON_STEP2_THRESHOLD_MIN (30) // 400PPM
    #define PPM_CARIBRATON_STEP2_THRESHOLD_MAX (50)
#elif defined STANDARD_EN50291_EN14604
    #define PPM_CARIBRATON_STEP2_THRESHOLD_MIN (23) // 300PPM
    #define PPM_CARIBRATON_STEP2_THRESHOLD_MAX (37)
#endif
#endif
