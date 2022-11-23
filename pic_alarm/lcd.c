#include "lcd.h"

void send(bool rs, char data) {
    RS = rs;
    DATA = data;
    EN = 1;
    __delay_ms(2);
    EN = 0;
}

void lcd_init(bool display, bool cursor, bool blink) {
    PORTB = 0x00;
    TRISB = 0x00;
    PORTD &= 0xFC;
    TRISD &= 0xFC;
    
    send(0, 0x38);
    lcd_display(display, cursor, blink);
    send(0, 0x14);
    send(0, 0x01);
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
    send(0, config);
}

void lcd_write_string(char *string) {
    while(*string != '\0') {
        send(true, *string);
        string++;
    }
}

void lcd_move_cursor(char address) {
    send(0, (0x80 | address));
}