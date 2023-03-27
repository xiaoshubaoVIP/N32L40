#include "n32l4/inc/hal_adc.h"
#include "bsp_config.h"
#include "n32l40x.h"
#include <stdio.h>

/* ADC TSPD mask */
#define CTRL2_TSVREFE_SET ((uint32_t)0x00800000)
#define CTRL2_TSVREFE_RESET ((uint32_t)0xFF7FFFFF)

// #define VREF_VALUE 3000 // mv
#define ADCNUM (1)

/* adc描述，用于将设备号映射相应硬件资源 */
typedef const struct
{
    uint8_t SamplingTime;
    uint8_t Channels;
} hal_adc;

extern const hal_adc adc_describe[];

static void hal_adc_register_init(void)
{
    ADC_InitType ADC_InitStructure;

    /* Enable ADC clocks */
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPH_ADC, ENABLE);

    /* RCC_ADCHCLK_DIV16*/
    ADC_ConfigClk(ADC_CTRL3_CKMOD_AHB, RCC_ADCHCLK_DIV16);
    RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8); // selsect HSE as RCC ADC1M CLK Source

    ADC_DeInit(ADC);
    /* ADC configuration ------------------------------------------------------*/
    ADC_InitStructure.MultiChEn = DISABLE;
    ADC_InitStructure.ContinueConvEn = DISABLE;
    ADC_InitStructure.ExtTrigSelect = ADC_EXT_TRIGCONV_NONE;
    ADC_InitStructure.DatAlign = ADC_DAT_ALIGN_R;
    ADC_InitStructure.ChsNumber = 1;
    ADC_Init(ADC, &ADC_InitStructure);

    /* Enable the temperature sensor and Vrefint channel*/ /* VREFBUFF */
    ADC_EnableTempSensorVrefint(ENABLE);

    /* Enable ADC */
    ADC_Enable(ADC, ENABLE);
    /* Check ADC Ready */
    while (ADC_GetFlagStatusNew(ADC, ADC_FLAG_RDY) == RESET)
        ;
    /* Start ADC1 calibration */
    ADC_StartCalibration(ADC);
    /* Check the end of ADC1 calibration */
    while (ADC_GetCalibrationStatus(ADC))
        ;
}

/**
 * @brief 初始化
 */
void hal_adc_init(void)
{
    /* Register init */
    hal_adc_register_init();
    /* Disable adc */
    ADC_Enable(ADC, DISABLE);
    hal_adc_read_vol(ADC_BATTERY);
}

void hal_adc_enable(void)
{

}

void hal_adc_disable(void)
{

}

/**
 * @brief 读取adc数据
 *
 * @param id 设备号
 * @return unsigned short 返回采集的数据
 */
uint16_t hal_adc_read(adc_id_t id)
{
    uint32_t value = 0;

    /* check adc init ok */
    if ((ADC->CTRL2 & ADC_EXT_TRIGCONV_NONE) == 0)
    {
        hal_adc_register_init();
    }
    else
    {
        // if (id == ADC_BATTERY)
        //     ADC_EnableTempSensorVrefint(ENABLE);

        while (ADC_GetFlagStatusNew(ADC, ADC_FLAG_PD_RDY) == RESET)
            ;
        ADC_Enable(ADC, ENABLE);
        while (ADC_GetFlagStatusNew(ADC, ADC_FLAG_RDY) == RESET)
            ;
    }

    ADC_ConfigRegularChannel(ADC, adc_describe[id].Channels, 1, adc_describe[id].SamplingTime);

    for (uint8_t i = 0; i < (1 << ADCNUM); i++)
    {
        /* Start ADC Software Conversion */
        ADC_EnableSoftwareStartConv(ADC, ENABLE);
        while (ADC_GetFlagStatus(ADC, ADC_FLAG_ENDC) == 0)
            ;
        ADC_ClearFlag(ADC, ADC_FLAG_ENDC);
        ADC_ClearFlag(ADC, ADC_FLAG_STR);
        value += ADC_GetDat(ADC);
    }

    // ADC_EnableTempSensorVrefint(DISABLE);
    ADC_Enable(ADC, DISABLE);

    return value >> ADCNUM;
}

