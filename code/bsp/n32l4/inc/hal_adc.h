#ifndef __HAL_ADC_H__
#define __HAL_ADC_H__

#include <stdint.h>

typedef uint8_t adc_id_t;

void hal_adc_init(void);
uint16_t hal_adc_read(adc_id_t id);
uint16_t hal_adc_read_vol(adc_id_t id);
void hal_adc_enable(void);
void hal_adc_disable(void);
#endif
