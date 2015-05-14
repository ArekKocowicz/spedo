#define MODULE_LCD
#include "lcd.h"


void lcdDisplayAll(lcd_display_t display)
{
    lcd_character_t character0, character1;

    if(display.blank)
    { 
        character0.character=10;
        character0.character=10;
    }
    else
    {
        character0.character=display.number/10;
        character1.character=display.number%10;
        if(character0.character==0)
            character0.character=11;

    }
    character0.dot=display.dot1;
    character1.dot=display.dot2;
    lcdDisplayCharacter(character0, 0);
    lcdDisplayCharacter(character1, 1);
}

void lcdDisplayCharacter(lcd_character_t character, uint8_t lcdPosition)
{
    uint8_t requiredSegments;
    uint8_t segmentsIndex;
    uint8_t segmentAddress;

    requiredSegments=charactersToSegments[character.character];               //check which segments to activate
    for(segmentsIndex=0;segmentsIndex<7;segmentsIndex++)                      //go through all possible segments (only seven, dot is treated separately)
    {
        segmentAddress=segmentsToLCDDataRegisters[lcdPosition][segmentsIndex]; //look up for address of that segment
        if(requiredSegments&(1<<segmentsIndex))                                //if this segment should be activated
            *((&LCDDATA0)+segmentAddress/8)|=(1<<(segmentAddress%8));          //decode address and write one to that register
        else
            *((&LCDDATA0)+segmentAddress/8)&=~(1<<(segmentAddress%8));         //decode address and write one to that register
    }
    segmentAddress=segmentsToLCDDataRegisters[lcdPosition][7];                 //this seven here hurts my eyes
    if(character.dot)
        *((&LCDDATA0)+segmentAddress/8)|=(1<<(segmentAddress%8));
    else
        *((&LCDDATA0)+segmentAddress/8)&=~(1<<(segmentAddress%8));
}


void lcdInit(void)
{
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
    LCDSE3=0xFF; //enable SEG24
    LCDRLbits.LRLAP=2;
    LCDRLbits.LRLBP=2;
}