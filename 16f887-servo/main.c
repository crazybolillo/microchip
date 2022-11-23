#pragma config DEBUG = 1
#pragma config LVP = 0
#pragma config FCMEN = 0
#pragma config IESO = 0
#pragma config BOREN = 00
#pragma config CPD = 0
#pragma config CP = 0
#pragma config MCLRE = 0
#pragma config PWRTE = 1
#pragma config WDTE = 0
#pragma config FOSC = 101

#include <stdio.h>
#include <stdint.h>
#include "config.h"
#include "lcd.h"
#include "keypad.h"

#define SERVO PORTCbits.RC2
#define SERVO_ZERO 500
#define SERVO_PULSE 20000
#define ORDER_LEN 5

unsigned char key;
uint8_t key_count = 0;

uint8_t order_count = 0;
uint8_t orders[ORDER_LEN][2]; // First element = degrees. Second = time on pos.

uint8_t servo_count = 0;
uint16_t servo_loop = 0;

char buffer[16];

void read_instruction() {
    lcd_clear_display();
    lcd_write_string("Posicion  :");
    lcd_move_cursor(0x09);
    lcd_write_char(49 + order_count);
    lcd_move_cursor(0x40);
    lcd_write_string("Segundos: ");
    lcd_move_cursor(0x0C);
    lcd_display(true, true, true);
    
    key_count = 0;
    orders[order_count][0] = 0;
    while (key_count != 3) {
        key = key_int(keypad_read());
        if (key <= 9) {
            lcd_write_char(48 + key);
            switch (key_count) {
                case 0:
                     orders[order_count][0] += (100 * key);
                    break;
                case 1:
                    orders[order_count][0] += (10 * key);
                    break;
                case 2:
                    orders[order_count][0] += key;
            }
            key_count++;
        }
    }
    lcd_move_cursor(0x4A);
    key = 100;
    while (key > 9) {
        key = key_int(keypad_read());
        if (key < 9) {
            lcd_write_char(48 + key);
            orders[order_count][1] = key;
        }
    }
    __delay_ms(100);
}

void servo_zero() {
    for (uint8_t count = 0; count != 50; count++) {
        SERVO = 1;
        __delay_us(SERVO_ZERO);
        SERVO = 0;
        __delay_us(SERVO_PULSE - SERVO_ZERO);  
    }
    
}

void servo_move() {
    servo_loop = (uint16_t) (orders[order_count][1] / 0.02);
    for (uint16_t x = 0; x != servo_loop; x++) {
        servo_count = 0;
        SERVO = 1;
        __delay_us(500);
        while (servo_count != orders[order_count][0]) {
            __delay_us(1);
            servo_count++;
        }
        SERVO = 0;
        __delay_ms(18);
    }
}

void  display_order() {
    lcd_clear_display();
    lcd_write_string("   Trabajando   ");
    lcd_move_cursor(0x40);
    sprintf(buffer, "   %03d - %d seg  ", 
            orders[order_count][0], orders[order_count][1]);
    lcd_write_string(buffer);
}

void main() {
    OSCCON = 0x71;
    while (OSCCONbits.HTS == 0);
    
    PORTC = 0x00;
    TRISC = 0x00;
    
    lcd_init(true, false, false);
    keypad_init();
    lcd_clear_display();
    
    while (1) {
        order_count = 0;
        while (order_count != ORDER_LEN) {
            read_instruction();
            order_count++;
        }
        lcd_display(true, false, false);
        lcd_clear_display();
        
        
        order_count = 0;
        while (order_count != ORDER_LEN) {
            display_order();
            servo_move();
            order_count++;
        }
        servo_zero();
        
        lcd_clear_display();
        lcd_write_string("Inicio: A");
        lcd_move_cursor(0x40);
        lcd_write_string("Ultima orden: D");
        key = 0;
        while (key != 'A') {
            key = key_char(keypad_read());
            if(key == 'D') {
                order_count = 4;
                display_order();
                servo_move();
                servo_zero();
                
                lcd_clear_display();
                lcd_write_string("Reinicio: A");
                lcd_move_cursor(0x40);
                lcd_write_string("Ultima orden: D");
            }
        }
    }
}
