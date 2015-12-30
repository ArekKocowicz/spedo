#include <xc.h>
#include <stdint.h>


#ifdef MODULE_EUSART

#define EUSART_FOSC (16000000l)
#define EUSART_BAUDRATE (1200)

void eusartInit(void);

#endif


#ifndef MODULE_EUSART

extern void eusartInit(void);

#endif