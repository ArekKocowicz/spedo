#define MODULE_TIMERS
#include "timers.h"

void timer0Init(void)
{
    INTCONbits.TMR0IF=0;
    INTCONbits.TMR0IE=1;
    OPTION_REGbits.TMR0CS=0;
    OPTION_REGbits.PSA=0;
    OPTION_REGbits.PS=2;
}

void timer1Init(void)
{
    TMR1H=0;
    TMR1L=0;
    PIR1bits.TMR1GIF=0; //wyczysc flage
    PIE1bits.TMR1GIE=1; //wlacz interrupt od gejta
    T1CONbits.TMR1CS=2;  //use external crystal resonator as source
    T1CONbits.T1OSCEN=1; //enable external 32kHz oscillator

    T1CONbits.T1CKPS=2;  //prescaler by 4 so Timer 1 is with f= 2^15/4=2^13 Hz
    T1CONbits.nT1SYNC=1; //nie synchronizuj
    T1GCONbits.T1GSS=0;  //gejtuj pinem T1G
    T1GCONbits.TMR1GE=1; //Timer1 counting is controlled by the Timer1 gate function
    T1GCONbits.T1GPOL=1;
    T1GCONbits.T1GTM=1;  //Timer1 Gate Toggle mode is enabled
    T1CONbits.TMR1ON=1;
}