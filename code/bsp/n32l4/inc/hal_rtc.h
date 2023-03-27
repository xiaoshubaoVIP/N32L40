#ifndef __HAL_RTC_H__
#define __HAL_RTC_H__

#include <stdint.h>

typedef void (*rtc_inter_callback)(void);

void hal_rtc_init(void);
void hal_rtc_write(uint16_t count);
uint16_t hal_rtc_read(void);
void hal_rtc_set_callback(rtc_inter_callback callback);
#endif
