#ifndef __LED__
#define __LED__

#include <stdint.h>
#include <stdbool.h>
#include "piconf.h"

#define DATA PORTB
#define RS PORTDbits.RD1
#define EN PORTDbits.RD0

#define _XTAL_FREQ 8000000
#define BLINK_DELAY 500

void send(bool rs, char data);
void lcd_display(bool display, bool cursor, bool blink);
void lcd_init(bool display, bool cursor, bool blink);
void lcd_clear(void);
void lcd_home(void);
void lcd_move_cursor(char address);
void lcd_write_string(char *string);
void lcd_write_char(char value);
void lcd_blink_string(char *string, char address, uint8_t size);

#endif
