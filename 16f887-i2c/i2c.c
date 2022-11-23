#include <pic16f887.h>

#include "i2c.h"
#include "config.h"

void i2c_init(void) {
    TRISC |= 0b00011000; //RC4 & RC3 (SDA & SCL) as inputs
    SSPSTAT = 0x80; // Disable slew rate control
    SSPCON = 0x28; // Enable MSSP as I2C Master
    SSPADD = ((_XTAL_FREQ / (4 * 100000)) - 1);
}

void i2c_start(void) {
    SSPCON2bits.SEN = 1;
    i2c_hold();
}

void i2c_rep_start(void) {
    SSPCON2bits.RSEN = 1;
    i2c_hold();
}

void i2c_stop(void) {
    SSPCON2bits.PEN = 1;
    i2c_hold();
}

void i2c_ack(void) {
    SSPCON2bits.ACKDT = 0;
    SSPCON2bits.ACKEN = 1;
    i2c_hold();
}

bool i2c_wait_ack(void) {
    i2c_hold();
    return SSPCON2bits.ACKSTAT;
}

bool i2c_write(unsigned char val) {
    SSPBUF = val;
    return i2c_wait_ack();
}

unsigned char i2c_read(void) {
    SSPCON2bits.RCEN = 1;
    i2c_hold();
    return SSPBUF;
}