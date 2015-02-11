#include <stdio.h>
#include <stdlib.h>
#include <pic16lf1902.h>

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

#define SEG_P (1<<0)
#define SEG_A (1<<5)
#define SEG_B (1<<4)
#define SEG_C (1<<3)
#define SEG_D (1<<2)
#define SEG_E (1<<1)
#define SEG_F (1<<6)
#define SEG_G (1<<7)

char digits[]={
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F      , //0
          SEG_B|SEG_C                        , //1
    SEG_A|SEG_B|      SEG_D|SEG_E|      SEG_G, //2
    SEG_A|SEG_B|SEG_C|SEG_D|            SEG_G, //3
          SEG_B|SEG_C|            SEG_F|SEG_G, //4
    SEG_A|      SEG_C|SEG_D|      SEG_F|SEG_G, //5
    SEG_A|      SEG_C|SEG_D|SEG_E|SEG_F|SEG_G, //6
    SEG_A|SEG_B|SEG_C                        , //7
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G, //8
    SEG_A|SEG_B|SEG_C|SEG_D|      SEG_F|SEG_G  //9
};

struct status_t
{
    unsigned tick:1;
    unsigned dot:1;
}status;

int period;


void display(char speed)
{
    //TODO: some nicer way of maping segments would be needed here
    if(speed/10!=0)
        LCDDATA0=digits[speed/10];
    else
        LCDDATA0=0;
    //TODO: brzydko tu jest, popraw ta obsluge kropki

    if(status.dot)
    LCDDATA0|=SEG_P;

    LCDDATA1=digits[speed%10];
    if(LCDDATA1&SEG_C)
        LCDDATA12bits.SEG24COM0=1;
    else
        LCDDATA12bits.SEG24COM0=0;
}

void main(void)

{
    //turn off ADCs, by default all pins as outputs
    ANSELA=0;
    ANSELB=0;
    TRISB=0;
    TRISA=0;

    //LCD configuration
    OSCCONbits.IRCF=0;       //drive micro from LFOSC 32kHz
    INTCONbits.GIE=1;        //turn the interrupts on
    INTCONbits.PEIE=1;       //turn on peripheral interrupts

    LCDCONbits.LCDEN=1;
    LCDCONbits.WERR=0;
    LCDCONbits.CS=2;    //drive LCD from LFINTOSC
    LCDCONbits.LMUX=0;  // static mode

    LCDPSbits.BIASMD=0; //
    LCDPSbits.LCDA=1;
    LCDPSbits.WA=1; //data register write enabled
    LCDPSbits.LP=0; //prescaller by 2

    LCDREFbits.LCDIRE=1; //internal ref enabled
    LCDREFbits.VLCD1PE=0;
    LCDCST=0;
    LCDSE0=0xFF;
    LCDSE1=0xF7; //enable SEG8, 9, 10,   12, 13, 14, 15 enabled
    LCDSE3=0x01; //enable SEG24
    LCDRLbits.LRLAP=2;
    LCDRLbits.LRLBP=2;

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

    display(20);         //show that you got so far
    while(1)
    {
        if(status.tick)
        {
            status.tick=0;
            display(period/4);
            if(status.dot)
                status.dot=0;
            else
                status.dot=1;
        }
    }
}


void interrupt my_interrupt(void)
{
    OSCCONbits.IRCF=12;  //change clock to a faster one

    if(PIR1bits.TMR1GIF)
    {
        PIR1bits.TMR1GIF=0;
        period=TMR1L;
        period|=(TMR1H<<8);
        TMR1H=0;
        TMR1L=0;
    }

    if(INTCONbits.TMR0IF)
    {
        INTCONbits.TMR0IF=0;
        status.tick=1;
    }

    OSCCONbits.IRCF=0; //and back to the slower clock
}