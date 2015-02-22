#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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

typedef enum
{
    SEG_A=(1<<0),
    SEG_B=(1<<1),
    SEG_C=(1<<2),
    SEG_D=(1<<3),
    SEG_E=(1<<4),
    SEG_F=(1<<5),
    SEG_G=(1<<6),
    SEG_P=(1<<7)
}segment_names;

/*
 * value = segmentsToLCDDataRegisters[p][s]
 * p - position on the display (first or second character)
 * s - segment for which DATA register addres is searched
 */
const uint8_t segmentsToLCDDataRegisters[2][8]=
{   {0*8+5,  //LCDDATA0, bit 5, first digit, SEG_A
     0*8+4,  //LCDDATA0, bit 4, first digit, SEG_B
     0*8+3,  //LCDDATA0, bit 3, first digit, SEG_C
     0*8+2,  //LCDDATA0, bit 2, first digit, SEG_D
     0*8+1,  //LCDDATA0, bit 1, first digit, SEG_E
     0*8+6,  //LCDDATA0, bit 6, first digit, SEG_F
     0*8+7,  //LCDDATA0, bit 7, first digit, SEG_G
     0*8+0}, //LCDDATA0, bit 0, first digit, SEG_DP
    {1*8+5,  //LCDDATA1, bit 5, second digit, SEG_A
     1*8+4,  //LCDDATA1, bit 4, second digit, SEG_B
     12*8+0, //LCDDATA12, bit 0, second digit, SEG_C
     1*8+2,  //LCDDATA1, bit 2, second digit, SEG_D
     1*8+1,  //LCDDATA1, bit 1, second digit, SEG_E
     1*8+6,  //LCDDATA1, bit 6, second digit, SEG_F
     1*8+7,  //LCDDATA1, bit 7, second digit, SEG_G
     1*8+0}  //LCDDATA1, bit 0, second digit, SEG_DP
};

//following table shows which segments have to be activated to
//display different characters like numbers and some letters
char charactersToSegments[]={
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
    unsigned dot:1;
}status;

uint16_t timer1CountedTicks=UINT16_MAX;
uint8_t speed;

void lcdDisplay(uint8_t lcdCharacter, uint8_t lcdPosition)
{   
    uint8_t requiredSegments;
    uint8_t segmentsIndex;
    uint8_t segmentAddress;

        requiredSegments=charactersToSegments[lcdCharacter];    //check which segments to activate
        for(segmentsIndex=0;segmentsIndex<8;segmentsIndex++)                     //go through all possible segments
        {
            segmentAddress=segmentsToLCDDataRegisters[lcdPosition][segmentsIndex];         //look up for address of that segment
            if(requiredSegments&(1<<segmentsIndex))                              //if this segment should be activated
                *((&LCDDATA0)+segmentAddress/8)|=(1<<(segmentAddress%8));        //decode address and write one to that register
            else
                *((&LCDDATA0)+segmentAddress/8)&=~(1<<(segmentAddress%8));       //decode address and write one to that register
        }
}

/*
void display(uint8_t number, display_mode_e mode)
{
    if(mode==DISPLAY_MODE_NUMBER)
    {
        if(number/10!=0)
            LCDDATA0=charactersToSegments[number/10];
        else
            LCDDATA0=0;
        LCDDATA1=charactersToSegments[number%10];
        if(LCDDATA1&SEG_C)
            LCDDATA12bits.SEG24COM0=1;
            
        else
            LCDDATA12bits.SEG24COM0=0;
    }
    else
    {
        LCDDATA0=SEG_G;
        LCDDATA1=SEG_G;
        LCDDATA12=0;
    }

    if(status.dot)
       LCDDATA0|=SEG_P;
}
*/

void main(void)

{
    //turn off ADCs, by default all pins as outputs
    ANSELA=0;
    ANSELB=0;
    TRISB=0;
    TRISA=0;

    //LCD configuration
    //display(0,DISPLAY_MODE_BLANK);
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

    while(1)
    {
        if(status.tick)
        {
            status.tick=0;
            /*speed=timer1CountedTicks/4;
            //speed=WHEEL_CIRCUMFERENCE_CM*36/(MAGNET_POLES_NUMBER*timer1CountedTicks/4);
            
            if(speed>99)  //speed out of limit
                display(0,DISPLAY_MODE_BLANK);
            else           //speed within limit
                display(speed,DISPLAY_MODE_NUMBER);
            
            if(status.dot)
                status.dot=0;
            else
                status.dot=1;
            */
            OPTION_REGbits.TMR0CS=1; //disable timer 0
            OSCCONbits.IRCF=7;       //change clock to a faster one
            lcdDisplay(speed,0);
            lcdDisplay(speed,1);
            
            speed++;
            if(speed>9)
                speed=0;
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