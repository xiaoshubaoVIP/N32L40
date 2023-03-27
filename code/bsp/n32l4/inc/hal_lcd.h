#ifndef __HAL_LCD_H__
#define __HAL_LCD_H__

#include <stdint.h>

void hal_lcd_init(void);
void hal_lcd_write(uint32_t data[], uint8_t start, uint8_t end);
void hal_lcd_enble(void);
void hal_lcd_disenble(void);
void hal_lcd_calibration(uint16_t vol);
#endif
