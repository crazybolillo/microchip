#include <pic18f4550.h>

#include "adc.h"
#include "config.h"

float adc_resolution = 0.0048828125;

void adc_set_channel(uint8_t channel) {
    ADCON0 &= 0x03;
    ADCON0 |= ((channel << 2) & 0x3C);
}

void adc_read(uint16_t *read) {
    ADCON0bits.GO_nDONE = 1;
    while (ADCON0bits.GO_nDONE == 1);
    NOP();
    
    *read = 0;
    *read |= ADRESH;
    *read = *read << 2;
    *read |= (ADRESL >> 6);
}

void adc_read_voltage(uint16_t *read, float *voltage) {
    adc_read(read);
    *voltage = (*read) * adc_resolution;
}

void adc_chread_voltage(uint8_t channel, uint16_t *read, float *voltage) {
    adc_set_channel(channel);
    adc_read_voltage(read, voltage);
}