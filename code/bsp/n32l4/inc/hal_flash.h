#ifndef __HAL_FLASH_H__
#define __HAL_FLASH_H__

#include <stdint.h>


void hal_flash_init(void);
void hal_flash_write(uint32_t addr, const uint8_t* buf, uint16_t len);
void hal_flash_read(uint32_t addr, uint8_t* buf, uint16_t len);
void hal_flash_erase(uint32_t addr);
#endif
