#ifndef __CLOCK__
#define __CLOCK__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "lcd.h"

#define CLOCK_HIGH_START 0x0B
#define CLOCK_LOW_START 0xDB 

#define stop_timer() T1CONbits.TMR1ON = 0; TMR1H = CLOCK_HIGH_START; TMR1L = 0x0DB

void init_timer(void);
void disable_timer(void);
void display_time(char *str, uint8_t hours, uint8_t minutes, uint8_t seconds);
void adc_to_clock(char *str, uint8_t *clock_value, uint16_t adc_value, 
        uint8_t max);

#endif