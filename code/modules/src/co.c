#include "inc/co.h"
#include "common.h"
#include "modules_config.h"
#include "bsp_config.h"

#define CO_DETECT_SHORT_ON() hal_gpio_write(PIN_CO_TEST, PIN_SET)
#define CO_DETECT_SHORT_OFF() hal_gpio_write(PIN_CO_TEST, PIN_RESET)
#define CO_DELAY_MS(ms) DelayMs(ms)
#define READ_VOL() hal_adc_read_vol(ADC_CO)
#define GET_TEMP() SensorGetTemperature()

#if !defined TEMPERATURE_HIGH_LOW_TEST
const uint16_t COTempOffsetRate[91] = {
    930, 930, 930, 930, 930,      // -20 ~ -16
    930, 935, 937, 938, 938,      // -15 ~ -11
    939, 939, 943, 943, 943,      // -10 ~ -6
    946, 948, 947, 949, 950,      // -5 ~ -1
    951,                          // 0
    951, 951, 953, 955, 959,      // 1 ~ 5
    959, 960, 958, 961, 962,      // 6 ~ 10
    963, 965, 967, 968, 972,      // 11 ~ 15
    971, 972, 975, 977, 981,      // 16 ~ 20
    982, 986, 990, 993, 1000,     // 21 ~ 25
    1005, 1010, 1014, 1022, 1027, // 26 ~ 30
    1039, 1047, 1061, 1070, 1083, // 31 ~ 35
    1099, 1110, 1127, 1150, 1168, // 36 ~ 39
    1194, 1216, 1244, 1278, 1311, // 40 ~ 45
    1348, 1390, 1437, 1483, 1536, // 46 ~ 49
    1587, 1642, 1700, 1758, 1822, // 50 ~ 55
    1899, 1987, 2086, 2178, 2251, // 56 ~ 59
    2330, 2414, 2499, 2587, 2671, // 60 ~ 65
    2859, 3042, 3224, 3587, 3800, // 66 ~ 70
};

const uint16_t COTempSlopeRate[91] = {
    655, 664, 674, 684, 694, 704, 714, 723, 733, 742,           // -20 - -11
    752, 761, 771, 780, 789, 799, 808, 817, 826, 835,           // -10 - -1
    844,                                                        // 0
    852, 861, 870, 878, 887, 895, 903, 911, 919, 927,           // 1 - 10
    935, 943, 950, 958, 965, 972, 980, 987, 994, 1000,          // 11 - 20
    1007, 1013, 1020, 1026, 1032, 1038, 1044, 1050, 1055, 1060, // 21 - 30
    1066, 1071, 1076, 1080, 1085, 1089, 1094, 1098, 1101, 1105, // 31 - 40
    1109, 1112, 1115, 1118, 1121, 1124, 1126, 1128, 1130, 1132, // 41 - 50
    1134, 1135, 1136, 1137, 1138, 1139, 1139, 1139, 1139, 1139, // 51 - 60
    1139, 1139, 1139, 1139, 1139, 1139, 1139, 1139, 1139, 1139  // 61 - 70
};
#endif

uint16_t COAdcGetSample(uint8_t state)
{
    /* check co fault */
    if (state == CO_SHORT_FAULT_CHECK)
    {
        CO_DETECT_SHORT_ON();  // enable fault detect
        CO_DELAY_MS(10);       // 10ms  (470mv)
        CO_DETECT_SHORT_OFF(); // disable fault detect
    }

    return READ_VOL();
}

/**
 * @brief 校机时使用
 *
 * @return uint16_t
 */
uint16_t COGetData(void)
{
    uint32_t value = COAdcGetSample(CO_NORMAL_CHECK);

    log_noinfo("co mvol:%ld", value);

    return value;
}

extern BOOL IsUartOpen(void);
uint16_t COGetPPM(uint32_t offset, uint32_t slope)
{
    uint16_t COData = COAdcGetSample(CO_NORMAL_CHECK);
    uint16_t ret;

// co温度补偿
#if !defined TEMPERATURE_HIGH_LOW_TEST
    {
        uint8_t Temp = SensorGetTemperature();

        offset = (offset * COTempOffsetRate[Temp] / 1000); // b
        slope = (slope * COTempSlopeRate[Temp] / 1000);    // k
    }
#endif

    if (COData >= offset)
    {
        ret = (COData - offset) * 10 / slope; // x=(y-b)/k
    }
    else
    {
        ret = 0;
    }

    if (IsUartOpen())
    {
        printf("\nco sample:%.3f(mv),b=%d,k=%d,ppm=%d", (float)COData / 1000, offset, slope, ret);
    }

    return ret;
}
