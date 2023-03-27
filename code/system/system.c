#include "system.h"
#include "common.h"
#include "bsp_config.h"

#define FLASH_MAP SystemFlashMap
#define FLASH_MAP_ADDR_POS FLASH_MAP_SYSTEM_ADDR_POS

static struct
{
    uint16_t LifeTimeDays; // system Life time day
    uint32_t SysTemTick;   // polling timer tick
    uint32_t SlpTimeCnt;   // system idle num
    tick_sec_t sec_tick;
} System;

static unsigned long activeflag;
struct SmokeFlashMap SmokeFlashMap;
struct CoFlashMap CoFlashMap;
struct SystemFlashMap SystemFlashMap;

/******************************************************************************
**Description:			SystemSetTick	 10ms
** Parameters:          none
**     Return:   		none
*******************************************************************************/
void SystemSetTick(void)
{
    System.SysTemTick++;
}

/******************************************************************************
**Description:	SystemSetTick	 10ms
** Parameters:     none
**     Return:   	tick   for  polling used
*******************************************************************************/
uint32_t SystemGetTick(void)
{
    return System.SysTemTick;
}

void SystemSetIdleNUm(uint32_t cnt)
{
    System.SlpTimeCnt = cnt;
}

/******************************************************************************
**Description:					SystemGetIdleNUm    10s
** Parameters:                        none
**     Return:   					sleep counter
*******************************************************************************/
uint32_t SystemGetIdleNUm(void)
{
    return System.SlpTimeCnt;
}

/******************************************************************************
**Description:		 	SystemGetLifeDays
** Parameters:             none
**     Return:   		LifeTimeDays
*******************************************************************************/
uint16_t SystemGetLifeDays(void)
{
    return System.LifeTimeDays;
}

tick_sec_t GetSystemSecondTick(void)
{
    return System.sec_tick;
}

void SystemUpdateTime(void)
{
    System.sec_tick += SLEEP_TIME;
    if (System.SlpTimeCnt < SYSTEM_ONE_DAY_COUNT)
    {
        System.SlpTimeCnt++;
    }
    /* 达到一天的秒数 */
    else
    {
        System.LifeTimeDays++;
        FlashSetData(LifeTimeDays, System.LifeTimeDays);
        System.SlpTimeCnt = 0;
    }
}

void SystemClearActiveFlag(eActiveFlag flag)
{
    activeflag &= ~flag;
}

void SystemSetActiveFlag(eActiveFlag flag)
{
    activeflag |= flag;
}

uint32_t SystemGetActiveFlag(uint32_t Flag)
{
    return activeflag & Flag;
}

void SystemTimeEvent(void)
{
    SystemPostFIFI(eSystemEventWakeup);
}

void System_init(void)
{
    hal_tim_set_callback(TIM_1, SystemSetTick);
    hal_rtc_set_callback(SystemTimeEvent);
    hal_flash_read(flash_data_start_physical_address() - FLASH_MAP_SMOKE_ADDR_POS, (uint8_t *)&SmokeFlashMap, sizeof(SmokeFlashMap));
    hal_flash_read(flash_data_start_physical_address() - FLASH_MAP_CO_ADDR_POS, (uint8_t *)&CoFlashMap, sizeof(CoFlashMap));
    hal_flash_read(flash_data_start_physical_address() - FLASH_MAP_SYSTEM_ADDR_POS, (uint8_t *)&SystemFlashMap, sizeof(SystemFlashMap));
    FlashGetData(LifeTimeDays, System.LifeTimeDays);
}

/******************************************************************************
**Description:				SystemPowerConsumptionCheck		 for factory test power custom
** Parameters:                none
**     Return:   				none
*******************************************************************************/
BOOL SystemPowerConsumptionCheck(void)
{
    BOOL ret = false;
    hal_gpio_mode(PIN_SWIM, PIN_MODE_INPUT_PULLUP);

    DelayUs(5);
    if (hal_gpio_read(PIN_SWIM) == PIN_RESET)
    {
        DelayMs(10);
        if (hal_gpio_read(PIN_SWIM) == PIN_RESET)
        {
            printf("Power Check in");
            ret = true;
        }
    }

    hal_gpio_mode(PIN_SWIM, PIN_MODE_OUTPUT);
    hal_gpio_write(PIN_SWIM, PIN_RESET);

    return ret;
}

/**
 * @brief 写入系统数据
 * 
 * @param pos 数据偏移地址，参考FlashMap定义
 * @param buf 要写入数据
 * @param buf_size 要写入大小
 */
void WriteSystemParmData(int32_t pos, uint8_t *buf, uint16_t buf_size)
{
    hal_flash_write(flash_data_start_physical_address() + pos, buf, buf_size);
}

// /**
//  * @brief 读出系统数据
//  * 
//  * @param pos 数据偏移地址，参考FlashMap定义
//  * @param buf 要写入数据
//  * @param buf_size 要写入大小
//  */
// void ReadSystemParmData(int32_t pos, uint8_t *buf, uint16_t buf_size)
// {
//     for (uint8_t i = 0; i < buf_size; i++)
//         buf[i] = ((uint8_t*)(&SystemParm))[pos + i];
// }

/**
 * @brief flash存储数据取反
 * 
 * @param dest 
 * @param src 
 * @param n 
 */
void DataXor(void *dest,const void *src,uint8_t n)
{
    if (src == NULL)
        return;    

    uint8_t *d = (uint8_t*)dest;
    uint8_t *s = (uint8_t*)src;

    while (n--)
    {
        *d = *s ^ 0xff;
        d++;
        s++;
    }
}
