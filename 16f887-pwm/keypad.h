#ifndef __KEYPAD_GUARD__
#define __KEYPAD_GUARD__

#include <stdint.h>
#include "config.h"

#define COL4 PORTBbits.RB0
#define COL3 PORTBbits.RB1
#define COL2 PORTBbits.RB2
#define COL1 PORTBbits.RB3

#define ROW4 PORTBbits.RB4
#define ROW3 PORTBbits.RB5
#define ROW2 PORTBbits.RB6
#define ROW1 PORTBbits.RB7

#define DEBOUNCE(x, code) __delay_ms(20); if (x == 0) {while (x == 0); return code;} 

void keypad_init(void);
char keypad_read(void);
char key_char(char key);
uint8_t key_int(char key);

#endif