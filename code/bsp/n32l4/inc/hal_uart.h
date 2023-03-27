#ifndef __HAL_UART_H__
#define __HAL_UART_H__

#include <stdint.h>

#define UART_RX_BUF_LEN 128 /* 接收长度 */
#define UART_TX_BUF_LEN 512 /* 发送长度 */

/**
 * @brief uart设备号
 */
typedef unsigned char uart_id_t;

void hal_uart_init(uart_id_t dev, uint32_t bound);
uint8_t hal_uart_write(uart_id_t dev, uint8_t buff[], uint16_t len);
uint16_t hal_uart_receive(uart_id_t dev, uint8_t buff[], uint16_t len);
void hal_uart_disable(void);
void hal_uart_enable(void);
#endif
