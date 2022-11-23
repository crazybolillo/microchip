#ifndef _DS1307_GUARD__
#define _DS1307_GUARD__

#include <stdint.h>
#include <stdbool.h>

#define SLV_ADDR_WR 0xD0
#define SLV_ADDR_RD 0xD1

bool ds1307_rdsec(uint8_t *seconds);
bool ds1307_rdmin(uint8_t *minutes);
bool ds1307_rdhour(uint8_t *hours);

bool ds1307_wrsec(uint8_t seconds);
bool ds1307_wrmin(uint8_t minutes);
bool ds1307_wrhour(uint8_t hours);

#endif