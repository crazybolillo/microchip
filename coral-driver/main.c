/*
 * File:   main.c
 * Author: anton
 *
 * Created on December 5, 2021, 11:23 PM
 */

// CONFIG1
#pragma config FEXTOSC = OFF    // External Oscillator mode selection bits (Oscillator not enabled)
#pragma config RSTOSC = HFINT1  // Power-up default value for COSC bits (HFINTOSC (1MHz))
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit (CLKOUT function is disabled; i/o or oscillator function on OSC2)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (FSCM timer disabled)

// CONFIG2
#pragma config MCLRE = OFF      // Master Clear Enable bit (MCLR pin function is port defined function)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config LPBOREN = OFF    // Low-Power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = OFF      // Brown-out reset enable bits (Brown-out reset disabled)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (VBOR) set to 1.9V on LF, and 2.45V on F Devices)
#pragma config ZCD = OFF        // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR.)
#pragma config PPS1WAY = ON     // Peripheral Pin Select one-way control (The PPSLOCK bit can be cleared and set only once in software)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a reset)

// CONFIG3
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF       // WDT operating mode (WDT Disabled, SWDTEN is ignored)
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4
#pragma config BBSIZE = BB512   // Boot Block Size Selection bits (512 words boot block size)
#pragma config BBEN = OFF       // Boot Block Enable bit (Boot Block disabled)
#pragma config SAFEN = OFF      // SAF Enable bit (SAF disabled)
#pragma config WRTAPP = OFF     // Application Block Write Protection bit (Application Block not write protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block not write protected)
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration Register not write protected)
#pragma config WRTSAF = OFF     // Storage Area Flash Write Protection bit (SAF not write protected)
#pragma config LVP = OFF         // Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/Vpp pin function is MCLR.)

// CONFIG5
#pragma config CP = OFF         // UserNVM Program memory code protection bit (UserNVM code protection disabled)

#include <xc.h>
#include <stdbool.h>
#include "pwm.h"
#include "keys.h"

#define _XTAL_FREQ 1000000

#define STATUS_BLINK_SEP 'S'
#define STATUS_BLINK_TOG 'T'
#define STATUS_FIXED 'F'
#define STATUS_OFF 'I'

uint8_t click;
uint8_t status = STATUS_FIXED;
uint8_t previous_status;

uint16_t pwm_duty;
uint16_t pwm_limit = PWM_STEP_SIZE * 5;
bool blink_rising = false;
volatile uint8_t *active_pwm;

#define pwm_inc() pwm_increase(&PWM3DCH);pwm_increase(&PWM4DCH);
#define pwm_dec() pwm_decrease(&PWM3DCH);pwm_decrease(&PWM4DCH);
#define pwm_set(duty) pwm_wrduty(&PWM3DCH, duty); pwm_wrduty(&PWM4DCH, duty);

#define BLINK_OPTIONS_SIZE 4
#define BLINK_OFF_LIM 30
uint8_t blink_off_buffer = 0; // Used to spend some TMR0 cycles with LEDs off
uint8_t blink_setting = 0;
uint8_t blink_options[4] = {1, 5, 10, 15};

void set_prescaler(void);
void blink_together(void);
void blink_separately(void);

void main(void) {
    /*
     * Pins RA5:2 used as input pins with internal pull-ups enabled
     * Pins RA1:0 used to drive PWM. Set as output
     */
    LATA = 0x00;
    ANSELA = 0x00;
    TRISA = 0xFC;
    WPUA = TRISA;
    RA5PPS = 0x05;
    RA4PPS = 0x04;
    RA3PPS = 0x03;
    RA2PPS = 0x02;
    
    init_pwm();
    pwm_wrduty(&PWM3DCH, pwm_limit);
    pwm_wrduty(&PWM4DCH, pwm_limit);
    pwm_on();
    
    /*
     * Timer 0 Setup
     * 1:1 postcaler, 8 bit mode with 1:32768 prescaler and FOSC/4 as source
     */
    T0CON0 = 0x00;
    T0CON1 = 0x48;
    
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE0bits.TMR0IE = 1;
    
    
    while (1) {
        click = read_click();
        if (click == POWER_CLICK) {
            if (status != STATUS_OFF) {
                pwm_set(0);
                previous_status = status;
                status =  STATUS_OFF;
            }
            else {
                status = previous_status;
                if (status == STATUS_FIXED) {
                    pwm_set(pwm_limit);
                }
            }
        }
        else if (status == STATUS_FIXED) {
            switch (click) {
                case UP_CLICK:
                    pwm_inc();
                    break;
                    
                case DOWN_CLICK:
                    pwm_dec();
                    break;
                   
                case MODE_CLICK: 
                    set_prescaler();
                    status = STATUS_BLINK_TOG;
                    blink_setting = 0;
                    TMR0H = blink_options[blink_setting];
                    PIE0bits.TMR0IE = 1;
                    T0CON0bits.T0EN = 1;
                    continue;
            } 
            pwm_limit = pwm_rdduty(&PWM3DCH);
        }
        else if ((status == STATUS_BLINK_TOG) || (status == STATUS_BLINK_SEP)) {
            switch (click) {
                case UP_CLICK:
                    blink_setting++;
                    if (blink_setting == BLINK_OPTIONS_SIZE) {
                        blink_setting = BLINK_OPTIONS_SIZE - 1;
                    }
                    break;
                    
                case DOWN_CLICK:
                    if (blink_setting != 0) {
                        blink_setting--;
                    }
                    break;
                    
                case MODE_CLICK:
                    if (status == STATUS_BLINK_TOG) {
                        status = STATUS_BLINK_SEP;
                        pwm_wrduty(&PWM4DCH, 0);
                        active_pwm = &PWM3DCH;
                        continue;
                    }
                    else if (status == STATUS_BLINK_SEP) {
                        status = STATUS_FIXED;
                        pwm_set(pwm_limit);
                        PIE0bits.TMR0IE = 0;
                        T0CON0bits.T0EN = 0;
                        continue;
                    }
            }
            TMR0H = blink_options[blink_setting];
        }  
    }
}

void __interrupt() handle_interrupt(void) {
    if (PIR0bits.TMR0IF == 1) {
        T0CON0bits.T0EN = 0;
        
        if (status == STATUS_BLINK_TOG) {
            blink_together();
        }
        else if (status == STATUS_BLINK_SEP) {
            blink_separately();
        }
        
        PIR0bits.TMR0IF = 0;
        T0CON0bits.T0EN = 1;
    }
}

void set_prescaler(void) {
    if (pwm_limit <= 128) {
        T0CON1 = (0x0A | (T0CON1 & 0xF0));
    }
    if (pwm_limit <= 256) {
        T0CON1 = (0x08 | (T0CON1 & 0xF0));
    }
    else if (pwm_limit <= 384) {
        T0CON1 = (0x07 | (T0CON1 & 0xF0));
    }
    else if (pwm_limit <= 512) {
        T0CON1 = (0x06 | (T0CON1 & 0xF0));
    }
    else if (pwm_limit <= 640) {
        T0CON1 = (0x05 | (T0CON1 & 0xF0));
    }
    else if (pwm_limit <= 768) {
        T0CON1 = (0x04 | (T0CON1 & 0xF0));
    }
    else if (pwm_limit <= 896) {
        T0CON1 = (0x03 | (T0CON1 & 0xF0));
    }
    else if (pwm_limit <= 1023) {
        T0CON1 = (0x02 | (T0CON1 & 0xF0));
    }
}

void blink_together(void) {
    pwm_duty = pwm_rdduty(&PWM3DCH);
    if (blink_rising == true) {
        if (pwm_duty < pwm_limit) {
            pwm_duty++;
            pwm_set(pwm_duty);
        }
        else {
            blink_rising = false;
        }
    }
    else {
        if (pwm_duty != 0) {
            pwm_duty--;
            pwm_set(pwm_duty);
        }
        else {
            blink_off_buffer++;
            if (blink_off_buffer == BLINK_OFF_LIM) {
                blink_rising = true;
                blink_off_buffer = 0;
            }
            
        }
    }
}

void blink_separately(void) {
    pwm_duty = pwm_rdduty(active_pwm);
    if (blink_rising == true) {
        if (pwm_duty < pwm_limit) {
            pwm_duty++;
            pwm_wrduty(active_pwm, pwm_duty);
        }
        else {
            blink_rising = false;
        }
    }
    else {
        if (pwm_duty != 0) {
            pwm_duty--;
            pwm_wrduty(active_pwm, pwm_duty);
        }
        else {
            blink_rising = true;
            if (active_pwm == &PWM3DCH) {
                active_pwm = &PWM4DCH;
            }
            else {
                active_pwm = &PWM3DCH;
            }
        }
    }
    
}