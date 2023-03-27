#ifndef __HAL_GPIO_H__
#define __HAL_GPIO_H__

typedef unsigned char pin_id_t;

/**
 * @brief ����ģʽ
 */
typedef enum
{
    PIN_MODE_OUTPUT,         /* ������� */
    PIN_MODE_INPUT,          /* �������� */
    PIN_MODE_OUTPUT_OD,      /* ��©��� */
    PIN_MODE_INPUT_PULLUP,   /* �������� */
    PIN_MODE_INPUT_PULLDOWN, /* �������� */
    PIN_MODE_INPUT_INTURPT,  /* �����ж� */
} pin_mode_t;

/**
 * @brief ����״̬
 */
typedef enum
{
    PIN_RESET, /* �͵�ƽ */
    PIN_SET,   /* �ߵ�ƽ */
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
