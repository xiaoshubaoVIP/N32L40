#ifndef __HAL_GPIO_H__
#define __HAL_GPIO_H__

typedef unsigned char pin_id_t;

/**
 * @brief 引脚模式
 */
typedef enum
{
    PIN_MODE_OUTPUT,         /* 推挽输出 */
    PIN_MODE_INPUT,          /* 浮空输入 */
    PIN_MODE_OUTPUT_OD,      /* 开漏输出 */
    PIN_MODE_INPUT_PULLUP,   /* 上拉输入 */
    PIN_MODE_INPUT_PULLDOWN, /* 下拉输入 */
    PIN_MODE_INPUT_INTURPT,  /* 输入中断 */
} pin_mode_t;

/**
 * @brief 引脚状态
 */
typedef enum
{
    PIN_RESET, /* 低电平 */
    PIN_SET,   /* 高电平 */
    PIN_TOGGLE,
    PIN_ERROR,
} pin_status_t;

typedef enum
{
    IT_TRIGGER_RISING ,
    IT_TRIGGER_FALLING,
    IT_TRIGGER_RISING_FALLING,
} exti_trigger_t;

void hal_gpio_write(pin_id_t pin, pin_status_t status);
pin_status_t hal_gpio_read(pin_id_t pin);
void hal_gpio_mode(pin_id_t pin, pin_mode_t mode);
void hal_gpio_setirq(pin_id_t pin, exti_trigger_t Trigger);
#endif
