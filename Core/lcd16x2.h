#ifndef LCD16X2_H
#define LCD16X2_H

#include "stm32f4xx_hal.h"

void lcd16x2_init(void);
void lcd16x2_clear(void);
void lcd16x2_setCursor(uint8_t row,uint8_t col);
void lcd16x2_1stLine(void);
void lcd16x2_2ndLine(void);
void lcd16x2_printf(const char* str,...);

#endif
