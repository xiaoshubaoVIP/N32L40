#ifndef __HAL_WATCHDOG_H__
#define __HAL_WATCHDOG_H__
#include "system.h"

void hal_watchdog_init(void);
void hal_watchdog_feed(void);
BOOL hal_watchdog_isreset(void);

void hal_watchdog_rcc_clrflg(void);
#endif
