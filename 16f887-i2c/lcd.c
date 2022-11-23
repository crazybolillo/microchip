#include "lcd.h"

void lcd_send(bool rs, char data) {
    RS = rs;
    DATA = ((DATA & 0xF0) | (data >> 4));
    EN = 1;
    __delay_ms(2);
    EN = 0;
    DATA = ((DATA & 0xF0) | (data & 0x0F));
    EN = 1;
    __delay_ms(2);
    EN = 0;
}

void lcd_init(bool display, bool cursor, bool blink) {
    __delay_ms(12);
    PORTD = 0x00;
    TRISD &= 0xC0;
    
    lcd_send(0, 0x02);
    lcd_send(0, 0x28);
    lcd_display(display, cursor, blink);
    lcd_send(0, 0x14);
    lcd_send(0, 0x01);
}

void lcd_display(bool display, bool cursor, bool blink) {
    char config = 0x08;
    if (display) {
        config |= 0x04;
    }
    if (cursor) {
        config |= 0x02;
    } 
    if (blink) {
        config |= 0x01;
    }
    lcd_send(0, config);
}

void lcd_write_string(char *string) {
    while(*string != '\0') {
        lcd_send(true, *string);
        string++;
    }
}

void lcd_write_char(char data) {
    lcd_send(true, data);
}

void lcd_move_cursor(char address) {
    lcd_send(0, (0x80 | address));
}

void lcd_clear_display(void) {
    lcd_send(false, 0x01);
}