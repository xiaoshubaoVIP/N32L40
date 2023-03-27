/**
  ******************************************************************************
  * @file    led.h
  * @author  zhenrong.peng
  * @version V1.0
  * @date    01-04-2017
  * @brief   Header for led.c
  *****************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_H
#define __LED_H

#include <stdint.h>


/* Exported types ------------------------------------------------------------*/
/**
 * @brief LED����
 */
typedef enum
{
    LED_TYPE_YELLOW,
    LED_TYPE_GREEN,
    LED_TYPE_RED,
    LED_TYPE_ALL,
    LED_TYPE_BLUE,
    LED_TYPE_MAX,
}eLed_Type;

/**
 * @brief LED״̬
 */
typedef enum
{
    LED_OFF=0x00,
    LED_ON,
    LED_TOGGLE,
}eLed_State;	

/**
 * @brief ����LED״̬
 * 
 * @param LedType LED����
 * @param State ״̬
 */
void LedSetState(eLed_Type LedType, eLed_State State);

/**
 * @brief ��LED����һ��ʱ��
 * 
 * @param LedType LED����
 * @param Time ����ʱ��
 */
void LedPollingHandle(eLed_Type LedType, uint32_t Time);

#endif /* __LED_H */



/************************END OF FILE**************************/


