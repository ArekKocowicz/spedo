#include <xc.h>
#include <stdint.h>


#ifdef MODULE_TIMERS

void timer0Init(void);
void timer1Init(void);

#endif


#ifndef MODULE_TIMERS

extern void timer0Init(void);
extern void timer1Init(void);

#endif