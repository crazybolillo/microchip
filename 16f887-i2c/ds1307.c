#include "ds1307.h"
#include "i2c.h"

bool ds1307_rdsec(uint8_t *seconds) {
    i2c_start();
    if (i2c_write(SLV_ADDR_WR) == true) {
        return false;
    }
    if (i2c_write(0x00) == true) {
        return false;
    }
    if (i2c_write(SLV_ADDR_RD) == true) {
        return false;
    }
    uint8_t i2c_secs = i2c_read();
    i2c_ack();
    i2c_stop();
    
    uint8_t dec_secs = ((i2c_secs >> 4) & 0x07) * 10;
    uint8_t secs = (i2c_secs & 0x0F);
    *seconds = dec_secs + secs;
    
    return true;
}