#ifdef COMPILATION
    #include <xc.h>
#else
    #include <pic16f877.h>
#endif
#include <stdint.h>

#pragma config DEBUG = 1
#pragma config LVP = 0
#pragma config FCMEN = 0
#pragma config IESO = 0
#pragma config BOREN = 00
#pragma config CPD = 0
#pragma config CP = 0
#pragma config MCLRE = 0
#pragma config PWRTE = 1
#pragma config WDTE = 0
#pragma config FOSC = 101

#define OSCCON OSCCON
extern volatile unsigned char OSCCON __at(0x08F);
#define ANSEL ANSEL
extern volatile unsigned char ANSEL __at(0x188);
#define ANSELH ANSELH
extern volatile unsigned char ANSELH __at(0x189);
#define _XTAL_FREQ 8000000
#define CUENTA_2 (PORTA & 0x02)
#define CUENTA_1 (PORTA & 0x01)
#define CUENTA_2_ACT (CUENTA_2 == 2) && (CUENTA_1 == 0)
#define CUENTA_1_ACT (CUENTA_2 == 0) && (CUENTA_1 == 1)
#define PLAY_SOUND PORTA |= 0x10
#define DIGIT_1 PORTC
#define DIGIT_2 PORTD


uint8_t cuenta_uno_disp[] = {0x04, 0xBE, 0x48, 0x18, 0xB2, 0x11, 0x01, 0xBC, 0x00, 0x30};
uint8_t cuenta_dos_disp[] = {0x40, 0x7B, 0x8A, 0x1A, 0x39, 0x1C, 0x04, 0x7A, 0x00, 0x30};

uint8_t cuenta_uno = 0;
uint8_t cuenta_dos = 0;
uint8_t *current_display = &cuenta_uno;


void display_led(uint8_t *digit) 
{
    DIGIT_1 = cuenta_uno_disp[*digit / 10];
    DIGIT_2 = cuenta_dos_disp[*digit % 10];
}


void __interrupt() pulse_interrupt(void) 
{
    if ((INTCON & 0x02) == 2) {
        if (CUENTA_2_ACT) {
            cuenta_dos++;
            if (cuenta_dos == 55) {
                PLAY_SOUND;   
            }
        }
        else if (CUENTA_1_ACT) {
            cuenta_uno++;
            if (cuenta_uno == 30) {
                PLAY_SOUND;
            }
        }
        INTCON &= 0xFD;
    }
}

void main () 
{
    INTCON = 0xD0;
    OSCCON = 0x75;
    ANSEL = 0x00;
    ANSELH = 0x00;
    PORTA = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    TRISA = 0x0F;
    TRISB = 0xFF;
    TRISC = 0x00;
    TRISD = 0x00;

    while (1)
    {
        if (CUENTA_2_ACT) {
            current_display = &cuenta_dos;
        }
        else if (CUENTA_1_ACT) {
            current_display = &cuenta_uno;
        }
        display_led(current_display);
    }
    
}
