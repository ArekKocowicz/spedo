#include <xc.h>
#include <stdint.h>


#ifndef LCD_DATATYPES_GUARD
#define LCD_DATATYPES_GUARD

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
}lcd_segment_names_t;


typedef struct
{
    unsigned character:7;
    unsigned dot:1;
}lcd_character_t;

typedef struct
{
    uint8_t number;
    unsigned dot1:1;
    unsigned dot2:1;
    unsigned blank:1;
    unsigned hex:1;
}lcd_display_t;

#endif


#ifdef MODULE_LCD

//#define BREADBOARD_CIRCUIT
//#define PCB_REV_1
#define PCB_REV_2

#ifdef BREADBOARD_CIRCUIT
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
#endif

#ifdef PCB_REV_1
/*
 * value = segmentsToLCDDataRegisters[p][s]
 * p - position on the display (first or second character)
 * s - segment for which DATA register addres is searched
 */
const uint8_t segmentsToLCDDataRegisters[2][8]=
{   {12*8+1,  //LCDDATA0, bit 5, first digit, SEG_A
     12*8+0,  //LCDDATA0, bit 4, first digit, SEG_B
     1*8+7,  //LCDDATA0, bit 3, first digit, SEG_C
     0*8+5,  //LCDDATA0, bit 2, first digit, SEG_D
     0*8+7,  //LCDDATA0, bit 1, first digit, SEG_E
     0*8+6,  //LCDDATA0, bit 6, first digit, SEG_F
     0*8+3,  //LCDDATA0, bit 7, first digit, SEG_G
     1*8+4}, //LCDDATA0, bit 0, first digit, SEG_DP
    {1*8+1,  //LCDDATA1, bit 5, second digit, SEG_A
     1*8+2,  //LCDDATA1, bit 4, second digit, SEG_B
     1*8+5, //LCDDATA12, bit 0, second digit, SEG_C
     1*8+6,  //LCDDATA1, bit 2, second digit, SEG_D
     12*8+2,  //LCDDATA1, bit 1, second digit, SEG_E
     1*8+0,  //LCDDATA1, bit 6, second digit, SEG_F
     0*8+0,  //LCDDATA1, bit 7, second digit, SEG_G
     0*8+4}  //LCDDATA1, bit 0, second digit, SEG_DP
};
#endif

#ifdef PCB_REV_2
/*
 * value = segmentsToLCDDataRegisters[p][s]
 * p - position on the display (first or second character)
 * s - segment for which DATA register addres is searched
 */
const uint8_t segmentsToLCDDataRegisters[2][8]=
{   {0*8+2,  //LCDDATA0, bit 5, first digit, SEG_A
     12*8+1,  //LCDDATA0, bit 4, first digit, SEG_B
     0*8+4,  //LCDDATA0, bit 3, first digit, SEG_C
     0*8+5,  //LCDDATA0, bit 2, first digit, SEG_D
     0*8+7,  //LCDDATA0, bit 1, first digit, SEG_E
     0*8+1,  //LCDDATA0, bit 6, first digit, SEG_F
     0*8+3,  //LCDDATA0, bit 7, first digit, SEG_G
     1*8+4}, //LCDDATA0, bit 0, first digit, SEG_DP
    {1*8+1,  //LCDDATA1, bit 5, second digit, SEG_A
     1*8+2,  //LCDDATA1, bit 4, second digit, SEG_B
     1*8+5, //LCDDATA12, bit 0, second digit, SEG_C
     1*8+6,  //LCDDATA1, bit 2, second digit, SEG_D
     12*8+2,  //LCDDATA1, bit 1, second digit, SEG_E
     0*8+0,  //LCDDATA1, bit 6, second digit, SEG_F
     12*8+0,  //LCDDATA1, bit 7, second digit, SEG_G
     0*8+5}  //LCDDATA1, bit 0, second digit, SEG_DP
};
#endif

//following table shows which segments have to be activated to
//display different characters like numbers and some letters
uint8_t charactersToSegments[]={
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F      , //0
          SEG_B|SEG_C                        , //1
    SEG_A|SEG_B|      SEG_D|SEG_E|      SEG_G, //2
    SEG_A|SEG_B|SEG_C|SEG_D|            SEG_G, //3
          SEG_B|SEG_C|            SEG_F|SEG_G, //4
    SEG_A|      SEG_C|SEG_D|      SEG_F|SEG_G, //5
    SEG_A|      SEG_C|SEG_D|SEG_E|SEG_F|SEG_G, //6
    SEG_A|SEG_B|SEG_C                        , //7
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G, //8
    SEG_A|SEG_B|SEG_C|SEG_D|      SEG_F|SEG_G, //9
    SEG_A|SEG_B|SEG_C|      SEG_E|SEG_F|SEG_G, //A
                SEG_C|SEG_D|SEG_E|SEG_F|SEG_G, //B
    SEG_A|            SEG_D|SEG_E|SEG_F      , //C
          SEG_B|SEG_C|SEG_D|SEG_E|      SEG_G, //D
    SEG_A|            SEG_D|SEG_E|SEG_F|SEG_G, //E
    SEG_A|                  SEG_E|SEG_F|SEG_G, //F
                                        SEG_G, //-
    0                                          //nothing
};



void lcdInit(void);
void lcdDisplayAll(lcd_display_t display);
void lcdDisplayCharacter(lcd_character_t character, uint8_t lcdPosition);

#endif
#ifndef MODULE_LCD

extern void lcdInit(void);
extern void lcdDisplayAll(lcd_display_t display);
//extern void lcdDisplayCharacter(lcd_character_t character, uint8_t lcdPosition);

#endif
