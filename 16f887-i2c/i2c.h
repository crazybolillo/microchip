#ifndef __I2C_GUARD__
#define __I2C_GUARD__

#include <stdbool.h>

#define i2c_hold() while (PIR1bits.SSPIF == 0); PIR1bits.SSPIF == 0;

void i2c_init(void);
void i2c_start(void);
void i2c_rep_start(void);
void i2c_stop(void);
void i2c_ack(void);
bool i2c_wait_ack(void);
bool i2c_write(unsigned char val);
unsigned char i2c_read(void);

#endif

