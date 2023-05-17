#include "keys.h"

uint8_t read_click(void) {
    if (MODE_BTN == 0) {
        is_clicked(MODE_BTN, MODE_CLICK);
    }
    else if (UP_BTN == 0) {
        is_pressed(UP_BTN, UP_CLICK);
    }
    else if (DOWN_BTN == 0) {
        is_pressed(DOWN_BTN, DOWN_CLICK);
    }
    else if (POWER_BTN == 0) {
        is_clicked(POWER_BTN, POWER_CLICK);
    }
    else {
        return 0;
    }
}