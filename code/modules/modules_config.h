#ifndef __MODULES_CONFIG_H__
#define __MODULES_CONFIG_H__

#include "config.h"

#include "inc/temp.h"
#include "inc/key.h"
#include "inc/led.h"
#include "inc/PwrManager.h"
#include "inc/Beep.h"

#if defined SUPPORT_CO_FUNCTION
    #include "inc/co.h"
#endif

#if defined SUPPORT_SMOKE_FUNCTION
    #include "inc/smoke.h"
#endif

#if defined SUPPORT_LCD_FUNCTION
    #include "inc/lcd.h"
#endif

#if defined SUPPORT_WIFI_NET_FUNCTION
    #include "wifi/wifi_api.h"
#elif defined SUPPORT_RF_NET_FUNCTION
    #include "rf/rf_api.h"
#elif defined SUPPORT_RADIO_FUNCTION
    #include "radio/radio_api.h"
#endif

#include "inc/net_api.h"
#endif
