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

#include "lcd.h"
#include "keypad.h"
#include <stdint.h>

#define CODE_LEN 4
#define ATTEMPT_LIMIT 3
#define EMPTY_LINE "                "

#define STATUS_INIT 'I'
#define STATUS_OPEN 'O'
#define STATUS_CLOSED 'C'
#define STATUS_LOCKED 'L'

#define BUZZER PORTCbits.RC0
#define BANDWIDTH 25

#define SHOW_CLOSED_LEGEND() lcd_clear_display(); \
            lcd_move_cursor(0x02); \
            lcd_write_string("CAJA CERRADA"); \
            lcd_move_cursor(0x46);

char status = STATUS_INIT;
uint8_t clicks = 0;
uint8_t attempt_count = 0;
char code[CODE_LEN];
char attempt[CODE_LEN];
char key = 0;

char convert_key(char key) {
    switch (key) {
        case 1: return '1';
        case 2: return '2';
        case 3: return '3';
        case 4: return 'A';
        case 5: return '4';
        case 6: return '5';
        case 7: return '6';
        case 8: return 'B';
        case 9: return '7';
        case 10: return '8';
        case 11: return '9';
        case 12: return 'C';
        case 13: return '*';
        case 14: return '0';
        case 15: return '#';
        case 16: return 'D';
        default: return 0;
    }
}

bool verify_code() {
    for (uint8_t index = 0; index != CODE_LEN; index++) {
        if (code[index] != attempt[index]) {
            return false;
        }
    }
   
    return true;
}

void __interrupt() handle_interrupt() {
    if (PIR1bits.TMR2IF == 1) {
        BUZZER = !BUZZER;
        PIR1bits.TMR2IF = 0;
    }
}

void play_sound() {
    PR2 = 15;
    T2CONbits.TMR2ON = 1;
    __delay_ms(100);
    T2CONbits.TMR2ON = 0;
    BUZZER = 0;
}

void main(void) {
    OSCCON = 0x79;
    
    PORTC = 0x00;
    TRISC = 0x00;
    
    /**
     * ------------------------------
     *  Setup to play sound using PWM
     * ------------------------------
     */
    // Postscaler = 16 & Prescaler = 16 - TMR2 off
    T2CON = 0xFB;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    PIE1bits.TMR2IE = 1;
    
    keypad_init();
    lcd_init(true, false, false);
    lcd_move_cursor(0x03);
    lcd_write_string("Bienvenido");
    lcd_move_cursor(0x40);
    lcd_write_string("Establezca clave");
    
    // Initialize before going into main loop.
    while (status == STATUS_INIT) {
        if (clicks == CODE_LEN) {
            __delay_ms(500);
            status = STATUS_OPEN;
            clicks = 0;
            break;
        }
        
        key = convert_key(keypad_read());
        if ((key >= '0') && (key <= '9')) {
            if (clicks == 0) {
                lcd_move_cursor(0x40);
                lcd_write_string(EMPTY_LINE);
                lcd_move_cursor(0x46);
            }
            play_sound();
            code[clicks] = key;
            lcd_write_char(key);
            clicks++;
        }
    }
    
    // Initilization  complete. Main program loop starts.
    while (1) {
        /*
         * -----------------------------
         * CONTROL LOOP WHEN BOX IS OPEN
         * -----------------------------
         */
        while (status == STATUS_OPEN) {
            lcd_clear_display();
            lcd_move_cursor(0x02);
            lcd_write_string("Caja abierta");
            lcd_move_cursor(0x42);
            lcd_write_string("Cerrar con *");
            while (status == STATUS_OPEN) {
                key = keypad_read();
                if (key == 13) {
                    play_sound();
                    status = STATUS_CLOSED;
                    break;
                }
            }
        }
        /*
         * ------------------------------
         * CONTROL LOOP WHEN BOX IS CLOSED
         * ------------------------------- 
         */
        while (status == STATUS_CLOSED) {
            SHOW_CLOSED_LEGEND();
            clicks = 0;
            while (status == STATUS_CLOSED) {
                if (clicks == CODE_LEN) {
                    clicks = 0;
                    __delay_ms(500);
                    if (verify_code()) {
                        status = STATUS_OPEN;
                        attempt_count = 0;
                        break;
                    }
                    else {
                        lcd_clear_display();
                        lcd_move_cursor(0x06);
                        lcd_write_string("ERROR");
                        lcd_move_cursor(0x40);
                        lcd_write_string("Clave incorrecta");
                        __delay_ms(500);
                        SHOW_CLOSED_LEGEND();
                        attempt_count++;
                        
                        if (attempt_count == 3) {
                            status = STATUS_LOCKED;
                            break;
                        }
                    }
                }
                key = convert_key(keypad_read());
                if ((key >= '0') && (key <= '9')) {
                    play_sound();
                    lcd_write_char(key);
                    attempt[clicks] = key;
                    clicks++;
                }
            }
        }
        /*
         * ----------------------------------------------------------
         * CONTROL LOOP WHEN BOX IS CLOSED - NO MORE ATTEMPTS ALLOWED
         * ----------------------------------------------------------
         */
        while (status == STATUS_LOCKED) {
            SHOW_CLOSED_LEGEND();
            lcd_move_cursor(0x40);
            lcd_write_string("Sin mas intentos");
            while (1);
        }
    }
    
    
}
