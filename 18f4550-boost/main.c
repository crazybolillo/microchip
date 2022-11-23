#include <stdio.h>
#include <stdint.h>
#include "config.h"
#include "lcd.h"
#include "adc.h"

#define ADC_TARGET_CHANNEL 0
#define ADC_VOUT_CHANNEL 2
#define ADC_FACTOR 4.34782608

uint16_t adc_read_value;
float target_voltage;
float vout_voltage;
uint16_t duty_cycle = 0;

char buffer[8];

void set_duty_cycle(uint16_t duty) {
    CCPR1L = (uint8_t) (duty >> 2);
    CCP1CON &= 0xCF;
    CCP1CON |= ((duty & 0x03) << 4);
}

void main(void) {
    OSCCON = 0x73; // Using internal oscillator as system clock @8M Hz
    
    TRISA = 0x0F;
    
    PORTC = 0x00;
    TRISC = 0x00;
    T2CON = 0x00;
    PR2 = 0x64;
    CCP1CON = 0x0C;
    CCPR1L = 0x00;
    set_duty_cycle(duty_cycle);
    
    ADCON0bits.ADON = 1;
    ADCON1 = 0x0B; // AN0:4 as analog inputs
    ADCON2 = 0x36; // Left justified. 16 TAD. FOSC/64
    
    lcd_init(true, false, false);
    lcd_clear_display();
    lcd_write_string("Vout:");
    lcd_move_cursor(0x40);
    lcd_write_string("Target:");
    
    T2CONbits.TMR2ON = 1;
    while (1) {
        for (uint8_t count = 0; count != 255; count++) {
            adc_chread_voltage(ADC_VOUT_CHANNEL, &adc_read_value, &vout_voltage);
            vout_voltage *= ADC_FACTOR;
            adc_chread_voltage(ADC_TARGET_CHANNEL, &adc_read_value, &target_voltage);
            target_voltage *= 3;
            if (target_voltage < 5) {
                target_voltage = 5;
            }        
            if (vout_voltage > target_voltage) {
                if (duty_cycle > 0) {
                    duty_cycle--;
                }
            }
            else if (vout_voltage < target_voltage) {
                if (duty_cycle < 512) {
                    duty_cycle++;
                }
            }       
            set_duty_cycle(duty_cycle);
        }
        sprintf(buffer, "%.2f  %03d", vout_voltage, duty_cycle);
        lcd_move_cursor(0x06);
        lcd_write_string(buffer);
        sprintf(buffer, "%.2f   ", target_voltage);
        lcd_move_cursor(0x48);
        lcd_write_string(buffer);
    }
}
