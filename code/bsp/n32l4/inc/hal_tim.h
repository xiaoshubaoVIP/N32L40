#ifndef __HAL_TIM_H__
#define __HAL_TIM_H__

#include <stdint.h>

enum 
{
    TIM_1,
    TIM_LPTIM,
};

typedef void (*tim_inter_callback)(void);

typedef unsigned char tim_id_t;

void hal_tim_init(void);
void hal_tim_write(tim_id_t tim, uint32_t ms);
void hal_tim_set_callback(tim_id_t tim, tim_inter_callback func);
void hal_tim_enable(tim_id_t tim);
void hal_tim_disable(tim_id_t tim);
uint32_t hal_get_tim_cnt(tim_id_t tim);
#endif
