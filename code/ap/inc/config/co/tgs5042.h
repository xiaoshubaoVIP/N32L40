/**
 * @file tgs5042.h
 * @brief figaro �󴫸���
 * @version 0.1
 * @date 2022-12-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __TGS5042_H__
#define __TGS5042_H__

// CO�������Լ����
#define SENSOR_OPEN_FAULT_SAMPLE 1800 // 2100//2300//2.3v
#define SENSOR_OPEN_SAMPLE 600        // 600mv
#define SENSOR_SHORT_SAMPLE_MAX 2200  // 2600//2.6v
#define SENSOR_SHORT_SAMPLE_MIN 1000  // 1v
// COУ������
#define PPM_CARIBRATON_STEP0_THRESHOLD_MIN (100) // mv
#define PPM_CARIBRATON_STEP0_THRESHOLD_MAX (200) // 300     // mv
#define PPM_CARIBRATON_STEP1_THRESHOLD_MIN (225) // mv
#define PPM_CARIBRATON_STEP1_THRESHOLD_MAX (450) // 600     // mv
#if defined STANDARD_UL2034_UL217
    #define PPM_CARIBRATON_STEP2_THRESHOLD_MIN (34) // 400PPM
    #define PPM_CARIBRATON_STEP2_THRESHOLD_MAX (46)
#elif defined STANDARD_EN50291_EN14604
    #define PPM_CARIBRATON_STEP2_THRESHOLD_MIN (25) // 300PPM
    #define PPM_CARIBRATON_STEP2_THRESHOLD_MAX (35)
#endif
#endif
