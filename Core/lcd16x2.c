#include "lcd16x2.h"
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
extern I2C_HandleTypeDef hi2c1;

/* Change address if needed */
#define LCD_ADDR (0x27 << 1)

/* PCF8574 mapping */
#define LCD_BACKLIGHT 0x08
#define ENABLE 0x04
#define RS 0x01

static void lcd_write(uint8_t data)
{
	HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, &data, 1, HAL_MAX_DELAY);
}

static void lcd_toggle_enable(uint8_t data)
{
    lcd_write(data | ENABLE);
    HAL_Delay(1);
    lcd_write(data & ~ENABLE);
    HAL_Delay(1);
}

static void lcd_send_nibble(uint8_t nibble,uint8_t rs)
{
	uint8_t data = (nibble & 0xF0) | LCD_BACKLIGHT;

    if(rs)
        data |= RS;

    lcd_write(data);
    lcd_toggle_enable(data);
}

static void lcd_send_cmd(uint8_t cmd)
{
    lcd_send_nibble(cmd & 0xF0,0);
    lcd_send_nibble((cmd<<4)&0xF0,0);
}

static void lcd_send_data(uint8_t data)
{
    lcd_send_nibble(data & 0xF0,1);
    lcd_send_nibble((data<<4)&0xF0,1);
}

/* Keep same function name so your main.c works */
void lcd16x2_init(void)
{
    HAL_Delay(50);

    lcd_send_nibble(0x30,0);
    HAL_Delay(5);

    lcd_send_nibble(0x30,0);
    HAL_Delay(1);

    lcd_send_nibble(0x30,0);
    HAL_Delay(10);

    lcd_send_nibble(0x20,0);

    lcd_send_cmd(0x28);
    lcd_send_cmd(0x0C);
    lcd_send_cmd(0x06);

    lcd_send_cmd(0x01);
    HAL_Delay(5);
}

void lcd16x2_clear(void)
{
    lcd_send_cmd(0x01);
    HAL_Delay(5);
}

void lcd16x2_setCursor(uint8_t row,uint8_t col)
{
    uint8_t addr;

    if(row==0)
        addr = 0x80 + col;
    else
        addr = 0xC0 + col;

    lcd_send_cmd(addr);
}

void lcd16x2_1stLine(void)
{
    lcd16x2_setCursor(0,0);
}

void lcd16x2_2ndLine(void)
{
    lcd16x2_setCursor(1,0);
}

void lcd16x2_printf(const char* str,...)
{
    char buffer[32];

    va_list args;

    va_start(args,str);
    vsprintf(buffer,str,args);
    va_end(args);

    for(uint8_t i=0;i<strlen(buffer)&&i<16;i++)
    {
        lcd_send_data(buffer[i]);
    }
}
