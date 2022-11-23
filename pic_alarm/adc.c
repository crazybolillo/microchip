#include "adc.h"

void init_adc(void) {
    PORTE = 0x00;
    TRISE = 0x04;
    ANSEL = 0x80;
    ADCON0 = 0x9C;
    ADCON1 = 0x00;
    ADCON0 |= 0x01;
}

void read_adc(uint16_t *value) {
    *value = 0;
    ADCON0bits.GO_nDONE = 1;
    while(ADCON0bits.GO_nDONE) {}
    NOP();
    
    *value |= ADRESH;
    *value = *value << 2;
    *value |= (ADRESL >> 6);
}
