#ifndef __ADC_GUARD__
#define __ADC_GUARD__

#include <stdint.h>

extern float adc_resolution;

void adc_set_channel(uint8_t channel);
void adc_read_voltage(uint16_t *adc_read, float *adc_voltage);
void adc_chread_voltage(uint8_t channel, uint16_t *adc_read, float *adc_voltage);

#endif