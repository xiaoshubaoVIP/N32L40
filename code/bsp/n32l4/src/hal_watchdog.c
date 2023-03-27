#include "n32l4/inc/hal_watchdog.h"
#include "n32l40x.h"

extern uint32_t LsiFreq;

// MAX : 26S
#define WATCHDOG_S(S) (LsiFreq / 256 * S)
#define KEY_ReloadKey ((uint16_t)0xAAAA)

void hal_watchdog_init(void)
{
    /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency dispersion) */
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteConfig(IWDG_WRITE_ENABLE);
    /* IWDG counter clock: LSI/32 */
    IWDG_SetPrescalerDiv(IWDG_PRESCALER_DIV256);
    /* Set counter reload value to obtain 250ms IWDG TimeOut.
       Counter Reload Value = 250ms/IWDG counter clock period
                            = 250ms / (LSI/32)
                            = 0.25s / (LsiFreq/32)
                            = LsiFreq/(32 * 4)
                            = LsiFreq/128 */
    IWDG_CntReload(WATCHDOG_S(20));
    /* Reload IWDG counter */
    IWDG_ReloadKey();
    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
}

void hal_watchdog_feed(void)
{
    /* Reload IWDG counter */
    // IWDG_ReloadKey();
    IWDG->KEY = KEY_ReloadKey;
}

/**
 * @brief 看门狗是否产生复位
 * 
 * @return true 
 * @return false 
 */
BOOL hal_watchdog_isreset(void)
{
   return (BOOL)RCC_GetFlagStatus(RCC_CTRLSTS_FLAG_IWDGRSTF);
}

/**
 * @brief 通过置位该位来清除所有复位标志
 * 
 */
void hal_watchdog_rcc_clrflg(void)
{
    RCC_ClrFlag();
}

