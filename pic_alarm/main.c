#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>


#include "piconf.h"
#include "lcd.h"
#include "adc.h"
#include "clock.h"

#pragma config WDTE = OFF
#pragma config LVP = 0
#pragma config FCMEN = 0
#pragma config IESO = 0
#pragma config FOSC = 101

#define CLOCK_BTN PORTCbits.RC3
#define ALARM_BTN PORTCbits.RC2
#define STOP_BTN PORTCbits.RC1
#define ALARM_SET PORTCbits.RC0
#define ALARM_ON PORTDbits.RD2

#define is_pushed(x) __delay_ms(32); if (x != 1) { continue; } while (x == 1)
    

uint8_t clock_ranges[] = {43, 17, 17};

uint8_t clock_vals[] = {0, 0, 0};
char clock_lcd_addr[] = {0x08, 0x0B, 0x0E};
char time_str[12];

uint8_t alarm_vals[] = {0, 0, 0};
char alarm_lcd_addr[] = {0x48, 0x4B, 0x4E};

uint8_t timer_counter = 0;

uint16_t adc_value;
char str_adc[5];

void read_display_adc(char addr, uint8_t range, uint8_t *val) {
    static uint8_t blink_counter = 0;
    
    lcd_move_cursor(addr); 
    read_adc(&adc_value); 
    adc_to_clock(str_adc, val, adc_value, range); 
    lcd_write_string(str_adc);
    blink_counter++;
    if (blink_counter == 30) {
        blink_counter = 0;
        lcd_move_cursor(addr);
        lcd_write_string("  ");
        __delay_ms(180);
    }
}

void add_second() {
    if (clock_vals[2] == 59) {
        clock_vals[2] = 0;
        if (clock_vals[1] == 59) {
            clock_vals[1] = 0;
            if (clock_vals[0] == 23) {
                clock_vals[0] = 0;
            }
            else {
                clock_vals[0]++;
            }
        }
        else {
            clock_vals[1]++;
        }
    }
    else {
        clock_vals[2]++;
    }
    if (ALARM_SET == 1) {
        for (uint8_t x = 0; x != 3; x++) {
            if (alarm_vals[x] != clock_vals[x]) {
                return;
            }
            else {
                continue;
            }
        }
        ALARM_ON = 1;
    }
}

void __interrupt() timer_interrupt(void) {
    if (PIE1bits.TMR1IE && TMR1IF) {
        stop_timer();
        T1CONbits.TMR1ON = 1;
        timer_counter++;
        if (timer_counter == 4) {
            add_second();
            display_time(time_str, clock_vals[0], clock_vals[1], clock_vals[2]);
            timer_counter = 0;
        }
        TMR1IF = 0;
    }
}

void main(void) {
    OSCCON = 0x79;
    init_timer();
    init_adc();
    
    PORTC = 0x00;
    TRISC = 0x0E;
    ALARM_ON = 0;
    TRISDbits.TRISD2 = 0;
    
    __delay_ms(10);
    lcd_init(true, false, false);
    lcd_write_string("Hora:   00:00:00");
    lcd_move_cursor(0x40);
    lcd_write_string("Alarma: OFF");
    
    uint8_t counter;
    while (1) {
        if (ALARM_BTN == 1) {
            is_pushed(ALARM_BTN);
            ALARM_SET = 0;
            counter = 0;
            lcd_move_cursor(alarm_lcd_addr[counter]);
            lcd_write_string("00:00:00");
            while (1) {
                read_display_adc(alarm_lcd_addr[counter], clock_ranges[counter],
                        &alarm_vals[counter]);
                if (ALARM_BTN == 1) {
                    is_pushed(ALARM_BTN);
                    lcd_move_cursor(alarm_lcd_addr[counter]);
                    lcd_write_string(str_adc);
                    counter++;
                    if (counter == 3) {
                        ALARM_SET = 1;
                        break;
                    }
                }
            }
        }
        else if (CLOCK_BTN == 1) {
            is_pushed(CLOCK_BTN);
            stop_timer();
            counter = 0;
            lcd_move_cursor(clock_lcd_addr[0]);
            lcd_write_string("00:00:00");
            while (1) {
                 read_display_adc(clock_lcd_addr[counter], clock_ranges[counter],
                        &clock_vals[counter]);
                if (CLOCK_BTN == 1) {
                    is_pushed(CLOCK_BTN);
                    lcd_move_cursor(clock_lcd_addr[counter]);
                    lcd_write_string(str_adc);
                    counter++;
                    if (counter == 3) {
                        T1CONbits.TMR1ON = 1;
                        break;
                    }
                }
            }
        }
        else if (STOP_BTN == 1) {
            is_pushed(STOP_BTN);
            if (ALARM_ON == 1) {
                ALARM_ON = 0;
            }
            else {
                ALARM_SET = 0;
                lcd_move_cursor(alarm_lcd_addr[0]);
                lcd_write_string("OFF     ");
            }
        }
    }
}
