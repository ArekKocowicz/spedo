#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pic16lf1902.h>
#include "lcd.h"

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF    // Clock Out Enable (CLKOUT function is enabled on the CLKOUT pin)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = HI        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), high trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown-out Reset (Low-Power BOR is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

#define WHEEL_CIRCUMFERENCE_CM (200)
#define MAGNET_POLES_NUMBER (13)

typedef enum
{
    DISPLAY_MODE_BLANK,
    DISPLAY_MODE_NUMBER
}display_mode_e;

struct status_t
{
    unsigned tick:1;
}status;

uint16_t timer1CountedTicks=UINT16_MAX;
uint8_t speed;
//lcd_character_t testCharacter;
lcd_display_t display;

void main(void)
{
    //turn off ADCs, by default all pins as outputs
    ANSELA=0;
    ANSELB=0;
    TRISB=0;
    TRISA=0;

    lcdInit();
    //Timer0 configuration
    INTCONbits.TMR0IF=0;
    INTCONbits.TMR0IE=1;
    OPTION_REGbits.TMR0CS=0;
    OPTION_REGbits.PSA=0;
    OPTION_REGbits.PS=3;

    //Timer1 configuration
    TMR1H=0;
    TMR1L=0;
    PIR1bits.TMR1GIF=0; //wyczysc flage
    PIE1bits.TMR1GIE=1; //wlacz interrupt od gejta
    T1CONbits.TMR1CS=2;  //use external crystal resonator as source
    T1CONbits.T1OSCEN=1; //enable external 32kHz oscillator

    T1CONbits.T1CKPS=3;  //prescaler by 8 so Timer 1 is counting miliseconds
    T1CONbits.nT1SYNC=1; //nie synchronizuj
    T1GCONbits.T1GSS=0;  //gejtuj pinem T1G
    T1GCONbits.TMR1GE=1; //Timer1 counting is controlled by the Timer1 gate function
    T1GCONbits.T1GPOL=1;
    T1GCONbits.T1GTM=1;  //Timer1 Gate Toggle mode is enabled
    T1CONbits.TMR1ON=1;

    while(1)
    {
        if(status.tick)
        {
            status.tick=0;

            OPTION_REGbits.TMR0CS=1; //disable timer 0
            OSCCONbits.IRCF=7;       //change clock to a faster one            

            /*
            display.number=speed;
            display.dot1=~display.dot1;
            lcdDisplayAll(display);
            
            speed++;
            if(speed>99)
                speed=0;
            */

            display.number=timer1CountedTicks/4;
            display.dot1=~display.dot1;
            lcdDisplayAll(display);

            OSCCONbits.IRCF=0;       //and back to the slower clock
            OPTION_REGbits.TMR0CS=0; //enable timer 0
        }
    }
}


void interrupt my_interrupt(void)
{
    if(PIR1bits.TMR1GIF)
    {
        PIR1bits.TMR1GIF=0;
        timer1CountedTicks=TMR1L;
        timer1CountedTicks|=(TMR1H<<8);
        TMR1H=0;
        TMR1L=0;
    }

    if(INTCONbits.TMR0IF)
    {
        INTCONbits.TMR0IF=0;
        status.tick=1;
    }
}