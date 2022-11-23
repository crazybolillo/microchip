#ifndef __LCD_GUARD__
#define __LCD_GUARD__

#define DATA PORTD
#define RS PORTDbits.RD4
#define EN PORTDbits.RD5
#define _XTAL_FREQ 8000000

#include <stdbool.h>
#include "config.h"

void lcd_send(bool rs, char data);
void lcd_init(bool display, bool cursor, bool blink);
void lcd_display(bool display, bool cursor, bool blink);
void lcd_write_string(char *string);
void lcd_write_char(char data);
void lcd_move_cursor(char address);
void lcd_clear_display(void);

#endif