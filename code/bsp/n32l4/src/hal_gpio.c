#include "n32l4/inc/hal_gpio.h"
#include "bsp_config.h"
#include "n32l40x.h"

typedef const struct
{
    GPIO_Module *port;
    uint16_t pin;
} hal_gpio;

extern const hal_gpio gpio_describe[];

/**
 * @brief 设置gpio模式
 *
 * @param pin 引脚号
 * @param mode 模式
 */
void hal_gpio_mode(pin_id_t pin, pin_mode_t mode)
{
    GPIO_InitType gpioinitstruct;

    if (pin > PIN_NUM)
        return;

    /*Init GPIO init struct*/
    GPIO_InitStruct(&gpioinitstruct);

    gpioinitstruct.Pin = gpio_describe[pin].pin;

    switch (mode)
    {
    case PIN_MODE_OUTPUT:
        gpioinitstruct.GPIO_Mode = GPIO_Mode_Out_PP;
        break;

    case PIN_MODE_INPUT:
        gpioinitstruct.GPIO_Mode = GPIO_Mode_Input;
        break;

    case PIN_MODE_INPUT_PULLUP:
        gpioinitstruct.GPIO_Pull = GPIO_Pull_Up;
        gpioinitstruct.GPIO_Mode = GPIO_Mode_Input;
        break;

    case PIN_MODE_INPUT_PULLDOWN:
        gpioinitstruct.GPIO_Pull = GPIO_Pull_Down;
        gpioinitstruct.GPIO_Mode = GPIO_Mode_Input;
        break;

    case PIN_MODE_OUTPUT_OD:
        gpioinitstruct.GPIO_Mode = GPIO_Mode_Out_OD;
        break;

    case PIN_MODE_INPUT_INTURPT:
        gpioinitstruct.GPIO_Pull = GPIO_Pull_Up;
        gpioinitstruct.GPIO_Mode = GPIO_Mode_IT_Falling;
        break;

    default:
        return;
    }
    GPIO_InitPeripheral(gpio_describe[pin].port, &gpioinitstruct);
}

void hal_gpio_write(pin_id_t pin, pin_status_t status)
{
    if (pin > PIN_NUM)
        return;

    switch (status)
    {
    case PIN_SET:
        gpio_describe[pin].port->PBSC = gpio_describe[pin].pin;
        break;
    case PIN_RESET:
        gpio_describe[pin].port->PBC = gpio_describe[pin].pin;
        break;

    case PIN_TOGGLE:
        (gpio_describe[pin].port->POD & gpio_describe[pin].pin) ? (gpio_describe[pin].port->PBC = gpio_describe[pin].pin)
                                                                : (gpio_describe[pin].port->PBSC = gpio_describe[pin].pin);
        break;

    default:
        return;
    }
}

void hal_gpio_setirq(pin_id_t pin, exti_trigger_t Trigger)
{
    EXTI_InitType EXTI_InitStructure;
    NVIC_InitType NVIC_InitStructure;
    uint8_t port_source = 0;
    uint8_t pin_source = 0;
    EXTI_TriggerType triger = EXTI_Trigger_Rising;

    if (pin > PIN_NUM)
        return;

    switch (Trigger)
    {
    case IT_TRIGGER_RISING :
        triger = EXTI_Trigger_Rising;
        break;

    case IT_TRIGGER_FALLING:
        triger = EXTI_Trigger_Falling;
        break;

    case IT_TRIGGER_RISING_FALLING:
        triger = EXTI_Trigger_Rising_Falling;
        break;

    default:
        break;
    }

    if (gpio_describe[pin].port == GPIOA)
    {
        port_source = GPIOA_PORT_SOURCE;
    }
    else if (gpio_describe[pin].port == GPIOB)
    {
        port_source = GPIOB_PORT_SOURCE;
    }
    else if (gpio_describe[pin].port == GPIOC)
    {
        port_source = GPIOC_PORT_SOURCE;
    }
    else if (gpio_describe[pin].port == GPIOD)
    {
        port_source = GPIOD_PORT_SOURCE;
    }

    uint32_t data = 0x01;
    for (uint8_t i = 0; i <= GPIO_PIN_SOURCE15; i++)
    {
        if (gpio_describe[pin].pin & data)
        {
            pin_source = i;
            break;
        }
        else
        {
            data = data << 1;
        }
    }

    uint8_t Irq_Channel = 0;
    if (pin_source == GPIO_PIN_SOURCE0)
    {
        Irq_Channel = EXTI0_IRQn;
    }
    else if (pin_source == GPIO_PIN_SOURCE1)
    {
        Irq_Channel = EXTI1_IRQn;
    }
    else if (pin_source == GPIO_PIN_SOURCE2)
    {
        Irq_Channel = EXTI2_IRQn;
    }
    else if (pin_source == GPIO_PIN_SOURCE3)
    {
        Irq_Channel = EXTI3_IRQn;
    }
    else if (pin_source == GPIO_PIN_SOURCE4)
    {
        Irq_Channel = EXTI4_IRQn;
    }
    else if ((pin_source >= GPIO_PIN_SOURCE4) && (pin_source <= GPIO_PIN_SOURCE9)) // 5-9
    {
        Irq_Channel = EXTI9_5_IRQn;
    }
    else if ((pin_source >= GPIO_PIN_SOURCE10) && (pin_source <= GPIO_PIN_SOURCE15)) // 5-9
    {
        Irq_Channel = EXTI15_10_IRQn;
    }


    GPIO_ConfigEXTILine(port_source, pin_source);
    /*Configure key EXTI line*/
    EXTI_InitStructure.EXTI_Line = gpio_describe[pin].pin;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = triger; // EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    /*Set key input interrupt priority*/
    NVIC_InitStructure.NVIC_IRQChannel = Irq_Channel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

pin_status_t hal_gpio_read(pin_id_t pin)
{
    if (pin > PIN_NUM)
        return PIN_ERROR;
    return (((gpio_describe[pin].port->PID & gpio_describe[pin].pin)) > 0) ? PIN_SET : PIN_RESET;
}
