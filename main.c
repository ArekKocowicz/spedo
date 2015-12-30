#include <xc.h>
#include <stdint.h>
#include "lcd.h"
#include "eusart.h"
#include "flash.h"
#include "timers.h"

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is enabled on the CLKOUT pin)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), high trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown-out Reset (Low-Power BOR is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

//#define WHEEL_CIRCUMFERENCE_CM (220)
#define MAGNET_POLES_NUMBER (13)

#define WHEEL_CIRCUMFERENCE_FLASH_ADDRESS (0x1FF0)

struct status_t{
    unsigned bTick:1;
    unsigned bNewRawPeriod:1;
    unsigned bCharReceived:1;
}status;

uint16_t u16RawPeriod=UINT16_MAX;
uint16_t speed;
lcd_display_t display;
uint8_t receivedCharacter;
uint8_t wheel_circumference_cm;
uint8_t ticksSinceLastSlope=0;    //long time without slopes means that wheel size programming mode should be entered
uint8_t parameterSettingModeInitialized=0;
uint16_t temp;

void main(void)
{
    ANSELA=0;
    ANSELB=0;
    TRISB=0;
    TRISA=0;
    
    TRISCbits.TRISC4=1;      // T1G as input
    OSCCONbits.IRCF=0;       //change clock to low f, 
    INTCONbits.GIE=1;        //turn the interrupts on
    INTCONbits.PEIE=1;       //turn on peripheral interrupts
    
    lcdInit();
    timer0Init();
    timer1Init();
    
    status.bTick=0;
    status.bNewRawPeriod=0;
    status.bCharReceived=0;
    
    wheel_circumference_cm=my_flash_read(WHEEL_CIRCUMFERENCE_FLASH_ADDRESS);  //read wheel size stored in flash
    
    /*
    my_flash_erase(0x1FF0);
    unsigned short testTable[]={'L','A','M','S','D','O','R','F'};
    my_flash_write(0x1FF0, testTable, sizeof(testTable)/sizeof(testTable[0]));
    */
    
    /*while(1){
        if(status.bCharReceived){
            status.bCharReceived=0;
            display.number=receivedCharacter;
            display.hex=1;
            display.dot1=~display.dot1;
            lcdDisplayAll(display);
                       
        }
    }*/
    
    /*
    //uint8_t test=0;
    while(1){
        if(status.bTick){
            status.bTick=0;
            display.number=test;
            display.dot1=~display.dot1;
            lcdDisplayAll(display);
            test++;
            if(test>99)
                test=0;
        }
    }
    */
    
    
    
    while(1)
    {
        if(ticksSinceLastSlope<=10) 
        {
            //this is normal mode
            if(status.bTick){
                status.bTick=0;
                OPTION_REGbits.TMR0CS=1; //disable timer 0
                OSCCONbits.IRCF=7;       //change clock to a faster one
                if(status.bNewRawPeriod){//a slope has been captured
                   status.bNewRawPeriod=0;
                   ticksSinceLastSlope=0;
                   u16RawPeriod*=MAGNET_POLES_NUMBER;
                   //speed=295*WHEEL_CIRCUMFERENCE_CM/u16RawPeriod;
                   speed=295*wheel_circumference_cm/u16RawPeriod;
                   if(speed>99)
                     speed=99;
                   display.number=speed;                               
                   display.dot2=~display.dot2;
                }
                else{                   //no slope captured
                    ticksSinceLastSlope++;
                    display.number=0;    //no slope means either no signal or extremely low speed so display 0
                    display.dot1=~display.dot1;
                }            
                display.hex=0;
                lcdDisplayAll(display);
                OSCCONbits.IRCF=0;       //and back to the slower clock
                OPTION_REGbits.TMR0CS=0; //enable timer 0
            }
        }
        else
        {
            if(parameterSettingModeInitialized==0){
                parameterSettingModeInitialized=1;
                OSCCONbits.IRCF=15;
                eusartInit();
                display.number=wheel_circumference_cm;
                display.hex=1;
                lcdDisplayAll(display);
            }
            if(status.bCharReceived){
                status.bCharReceived=0;
                my_flash_erase(WHEEL_CIRCUMFERENCE_FLASH_ADDRESS);
                temp=receivedCharacter;
                my_flash_write(WHEEL_CIRCUMFERENCE_FLASH_ADDRESS,&temp,1);
                wheel_circumference_cm=my_flash_read(WHEEL_CIRCUMFERENCE_FLASH_ADDRESS);
                display.number=wheel_circumference_cm;
                display.hex=1;
                lcdDisplayAll(display);
            }
            
            
        }       
    }
}


void interrupt my_interrupt(void){
    if(PIR1bits.TMR1GIF){
        PIR1bits.TMR1GIF=0;
        u16RawPeriod=TMR1L;
        u16RawPeriod|=(TMR1H<<8);
        TMR1H=0;
        TMR1L=0;
        status.bNewRawPeriod=1;
    }
    if(INTCONbits.TMR0IF){
        INTCONbits.TMR0IF=0;
        status.bTick=1;
    }
    if(PIR1bits.RCIF){
        receivedCharacter=RCREG;
        status.bCharReceived=1;
    }
}