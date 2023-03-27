#ifndef __TEMP_H__
#define __TEMP_H__

/* 
    日志等级 LOG_LEVEL
    0:ERROR
    1:WARN
    2:INFO
    3:DEBUG
 */
#undef LOG_LEVEL
#define LOG_LEVEL 2

unsigned char GetTempData(void);
unsigned char SensorGetTemperature(void);
unsigned char SensorTemperatureCheck(void);
#endif
