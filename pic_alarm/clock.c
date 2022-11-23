#include "clock.h"
#include "lcd.h"

void init_timer(void) {
    PIE1bits.TMR1IE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
    TMR1H = CLOCK_HIGH_START;
    TMR1L = CLOCK_LOW_START;
    TMR1IF = 0;
    T1CON = 0xB1;
}

void display_time(char *string, uint8_t hours, uint8_t minutes, 
        uint8_t seconds) {
    lcd_move_cursor(0x08);
    sprintf(string, "%02d:%02d:%02d", hours, minutes, seconds);
    lcd_write_string(string);
}

void adc_to_clock(char *string, uint8_t *clock_value, uint16_t adc_value, 
        uint8_t range) {
    *clock_value = (uint8_t) adc_value / range;
    sprintf(string, "%02d", *clock_value);
}