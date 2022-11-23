#include "keypad.h"

void keypad_init(void) {
    ANSELH = 0x00;
    PORTB = 0xF0;
    TRISB = 0x0F;
    OPTION_REGbits.nRBPU = 0;
    WPUB = 0x0F;
}

static char column_read(void) {
    if (COL4 == 0) {
        DEBOUNCE(COL4, 4);
    }
    else if (COL3 == 0) {
        DEBOUNCE(COL3, 3);
    }
    else if (COL2 == 0) {
        DEBOUNCE(COL2, 2);
    }
    else if (COL1 == 0) {
        DEBOUNCE(COL1, 1);
    }
    
    return 0;
}

char keypad_read(void) {
    char col = 0;
    
    ROW1 = 0;
    col = column_read();
    if (col != 0) {
        return col;
    }
    ROW1 = 1;
    
    ROW2 = 0;
    col = column_read();
    if (col != 0) {
        return 4 + col;
    }
    ROW2 = 1;
    
    ROW3 = 0;
    col = column_read();
    if (col != 0) {
        return 8 + col;
    }
    ROW3 = 1;
    
    ROW4 = 0;
    col = column_read();
    if (col != 0) {
        return 12 + col;
    }
    ROW4 = 1;
    
    return 0;
}

char key_char(char key) {
    switch (key) {
        case 1: return '1';
        case 2: return '2';
        case 3: return '3';
        case 4: return 'A';
        case 5: return '4';
        case 6: return '5';
        case 7: return '6';
        case 8: return 'B';
        case 9: return '7';
        case 10: return '8';
        case 11: return '9';
        case 12: return 'C';
        case 13: return '*';
        case 14: return '0';
        case 15: return '#';
        case 16: return 'D';
        default: return 0;
    }
}

uint8_t key_int(char key) {
    switch(key) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 4: return 'A';
        case 5: return 4;
        case 6: return 5;
        case 7: return 6;
        case 8: return 'B';
        case 9: return 7;
        case 10: return 8;
        case 11: return 9;
        case 12: return 'C';
        case 13: return '*';
        case 14: return 0;
        case 15: return '#';
        case 16: return 'D';
        default: return 50;
    }
}