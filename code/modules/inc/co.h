#ifndef __CO_H__
#define __CO_H__

#include <stdint.h>

/* 
    日志等级
    0:ERROR
    1:WARN
    2:INFO
    3:DEBUG
 */
#undef LOG_LEVEL
#define LOG_LEVEL 2

//co Check State
#define CO_NORMAL_CHECK  ((uint8_t)0)
#define CO_OPEN_FAULT_CHECK   ((uint8_t)1)
#define CO_SHORT_FAULT_CHECK   ((uint8_t)2)

/* 获取CO的ADC采样数据 */
uint16_t COAdcGetSample(uint8_t state);
/* 获取CO的ADC采样数据(校机时使用) */
uint16_t COGetData(void);
/* 获取CO的PPM */
uint16_t COGetPPM(uint32_t offset, uint32_t slope);
#endif

