#include "inc/temp.h"
#include "common.h"
#include "bsp_config.h"

BOOL IsUartOpen(void);
#define debug_uart(fmt,arg...) do{ if(IsUartOpen()){ printf(fmt, ##arg); } }while(0)

#define TEMP_DETECT_ON() hal_gpio_write(PIN_NTC_POWER, PIN_SET)
#define TEMP_DETECT_OFF() hal_gpio_write(PIN_NTC_POWER, PIN_RESET)
#define read_adc() hal_adc_read(ADC_TEMP)

#define TEMPERATURE_FACTORY_START           (35)//(40)                  // 20 C
#define TEMPERATURE_FACTORY_END             (56)//(55)                  // 30 C

typedef struct {
    uint16_t TempAdcSampe;
    uint8_t Temp;// offset +20
}SensorTag;

static SensorTag SensorObject;

static const uint16_t NTC_Table[91]={ 
    3741, 3721, 3699, 3677, 3654,  	//-20℃ ~ -16℃
    3630, 3605, 3535, 3552, 3525,  	//-15℃ ~ -11℃
    3496, 3466, 3435, 3404, 3360,  	//-10℃ ~ -6℃ 
    3338, 3303, 3268, 3232, 3195,  	//-5℃ ~ 1℃
    3157, 						    //0℃ 
    3118, 3079, 3038, 2998, 2956,	// 1 //℃ ~ 5℃
    2914, 2871, 2827, 2783, 2738, 	//6℃ ~ 10℃
    2694, 2648, 2603, 2557, 2511, 	//11℃ ~ 15℃
    2464, 2418, 2371, 2325, 2279, 	//16℃ ~ 20℃
    2232, 2186, 2140, 2094, 2048, 	//21℃ ~ 25℃
    2003, 1958, 1913, 1881, 1825, 	//26℃ ~ 30℃
    1782, 1739, 1697, 1655, 1614, 	//31℃ ~ 35℃
    1574, 1534, 1495, 1457, 1419,	//36℃ ~ 40℃
    1383, 1345, 1310, 1275, 1241, 	//41℃ ~ 45℃
    1208, 1175, 1143, 1112, 1082, 	//46℃ ~ 50℃
    1052, 1023, 995,  967,  940,  	//51℃ ~ 55℃
    913,  888,  863,  839,  816,  	//56℃ ~ 60℃
    793,  770,  749,  728,  707,  	//61℃ ~ 65℃
    687,  668,  649,  631,  613,	//66℃ ~ 70℃
};


/**
 * @brief Get the Temp Data object
 * 
 * @return uint8_t 返回温度偏移-20℃
 */
uint8_t GetTempData(void)
{ 
    uint8_t Index = 0;
    uint16_t TempAdcValue = 0;

    TEMP_DETECT_ON();
    DelayUs(50);
    TempAdcValue = read_adc();
    TEMP_DETECT_OFF();

    #if 1
    /* 加速查找 */
    if (TempAdcValue < NTC_Table[40]) { // 在20C以上
        Index = 40;
    } else if (TempAdcValue < NTC_Table[25]) { // 在5C以上
        Index = 25;
    }
    #endif

    while(TempAdcValue < NTC_Table[Index])
    {
        if(Index >= 90)
        {
            Index = 90;
            break;
        }
        else
        {
            Index++;
        }
    }

    SensorObject.Temp = Index;

    debug_uart("T:%dC", SensorObject.Temp - 20);

    return Index;
}

uint8_t SensorGetTemperature(void)
{
    return SensorObject.Temp; 
}

/**
 * @brief 温度数据范围检测
 * 
 * @return Ret： 1正常 0错误 
 */
uint8_t SensorTemperatureCheck(void)
{
    uint8_t Ret;
    uint8_t Temp = GetTempData();

    if (Temp > TEMPERATURE_FACTORY_START 
    && Temp < TEMPERATURE_FACTORY_END)
    {
        Ret = 1;
    }
    else
    {
        Ret = 0;
    }

    return Ret;
}
