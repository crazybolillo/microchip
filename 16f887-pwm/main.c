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

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "lcd.h"
#include "keypad.h"

#define TMR1H_VAL 0x9E
#define TMR1L_VAL 0x57
#define TRIGGER PORTCbits.RC3
#define DUTY_START 410 // Fan turns off below 40% duty cycle.
#define DUTY_RAMP (1023 - DUTY_START)

uint16_t time = 0; // Measured in centiseconds (100ms)
uint16_t time_target = 0;
uint16_t duty = DUTY_START;
uint16_t duty_step;

bool rising; // States whether the duty cycle is increasing or decreasing.
uint8_t start_secs;
uint8_t work_secs;
uint8_t stop_secs;

#define STATUS_START 'S'
#define STATUS_WORK 'W'
#define STATUS_END 'E'
unsigned char status = STATUS_START;

unsigned char key;
uint8_t key_num;
uint8_t key_count;

char buffer[4];

#define READ_SECONDS(x) x = 0; \
                        while (key_count != 2) { \
                            key = keypad_read(); \
                            key_num = key_int(key); \
                            if (key_num <= 9) { \
                                if (key_count == 0) { \
                                    x += key_num * 10; \
                                } \
                                else { \
                                    x += key_num; \
                                } \
                                lcd_write_char(key_char(key)); \
                                key_count++; \
                            } \
                        } \
                        key_count = 0; __delay_ms(600);

void set_duty_cycle(uint16_t duty) {
    CCPR1L = (uint8_t) (duty >> 2);
    CCP1CON &= 0xCF;
    CCP1CON |= ((duty & 0x03) << 4);
}

uint8_t duty_to_speed(void) {
    if (duty <= DUTY_START) {
        return 0;
    }
    else {
        return (uint8_t) (0.1631 * (duty - DUTY_START));
    }
}

void main(void) {
    OSCCON = 0x70;
    while (OSCCONbits.HTS == 0);
    
    PORTC = 0x00;
    TRISC = 0x00;
    T2CON = 0x03;
    PR2 = 0xFF;
    CCP1CON = 0x0C;
    CCPR1L = 0x00;
    
    T1CON = 0x30;
    TMR1H = TMR1H_VAL;
    TMR1L = TMR1L_VAL;
    
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.TMR1IE = 1;
    
    lcd_init(true, false, false);
    keypad_init();
    
    while (1) {
        while (status == STATUS_START) {
            lcd_clear_display();
            lcd_write_string("Tiempo arranque:");
            lcd_move_cursor(0x47);
            READ_SECONDS(start_secs);
            
            lcd_clear_display();
            lcd_write_string("Tiempo trabajo:");
            lcd_move_cursor(0x47);
            READ_SECONDS(work_secs);
            
            lcd_clear_display();
            lcd_write_string("Tiempo frenado:");
            lcd_move_cursor(0x47);
            READ_SECONDS(stop_secs);
            
            status = STATUS_WORK;
        }
        while (status == STATUS_WORK) {
            time = 0;
            lcd_clear_display();
            lcd_move_cursor(0x03);
            lcd_write_string("Arrancando");
            lcd_move_cursor(0x46);
            lcd_write_string("000%");
            __delay_ms(100);
            
            // PWM RAMP UP
            rising = true;
            time_target = start_secs * 10; 
            duty_step = (uint16_t) (floor(DUTY_RAMP / time_target));
            TRIGGER = 1;
            set_duty_cycle(DUTY_START);
            T2CONbits.TMR2ON = 1;
            __delay_ms(100);
            TRIGGER = 0;
            T1CONbits.TMR1ON = 1;
            while (time != time_target);
            set_duty_cycle(1023);
            
            // Stay working at 100% duty
            lcd_clear_display();
            lcd_move_cursor(0x03);
            lcd_write_string("Trabajando");
            lcd_move_cursor(0x46);
            lcd_write_string("100%");
            for (uint8_t seconds = 0; seconds != work_secs; seconds++) {
                __delay_ms(1000);
            }
            
            lcd_clear_display();
            lcd_move_cursor(0x04);
            lcd_write_string("Frenando");
            lcd_move_cursor(0x46);
            lcd_write_string("100%");
            rising = false;
            time = 0;
            time_target = stop_secs * 10;
            duty_step = (uint16_t) (floor(DUTY_RAMP / time_target));
            T1CONbits.TMR1ON = 1;
            while (time != time_target);
            set_duty_cycle(0);
            
            status = STATUS_END;
        }
        while (status == STATUS_END) {
            lcd_clear_display();
            lcd_write_string("Final - Apagado");
            lcd_move_cursor(0x40);
            lcd_write_string("* Rst -- # Start");
            while (1) {
                key = key_char(keypad_read());
                if (key == '*') {
                    status = STATUS_WORK;
                    break;
                }
                else if (key == '#') {
                    status = STATUS_START;
                    break;
                }
            }
        }
    }
}

void __interrupt() handle_interrupt() {
    if (PIR1bits.TMR1IF == 1) {
        T1CONbits.TMR1ON = 0;
        time++;
        TMR1H = TMR1H_VAL;
        TMR1L = TMR1L_VAL;
        if (time != time_target) {
            T1CONbits.TMR1ON = 1;
        }
        
        if (rising) {
            duty += duty_step;
        }
        else {
            duty -= duty_step;
        }
        
        set_duty_cycle(duty);
        lcd_move_cursor(0x46);
        sprintf(buffer, "%03d", duty_to_speed());
        lcd_write_string(buffer);
        
        PIR1bits.TMR1IF = 0;
    }
}