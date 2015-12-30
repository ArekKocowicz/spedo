#define MODULE_EUSART
#include "eusart.h"

void eusartInit(void)
{
    SPBRGH=(((EUSART_FOSC/EUSART_BAUDRATE)/64-1)>>8)&0xFF;
    SPBRGL=((EUSART_FOSC/EUSART_BAUDRATE)/64-1)&0xFF;
    TRISCbits.TRISC6=1;
    TRISCbits.TRISC7=1;
    RCSTAbits.SPEN=1;
    TXSTAbits.SYNC=0;
    RCSTAbits.CREN=1;
    PIR1bits.RCIF=0;
    PIE1bits.RCIE=1;
    BRG16=0;
    BRGH=0;
}