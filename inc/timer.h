#ifndef TIMER_H
#define TIMER_H

#include "stm32f4xx.h"
#include "utl.h"

extern void timer2_init(void);
extern void timer2_delay(uint16_t time);
extern void timer2_reconfigure(uint16_t prescalar, uint16_t reload);

extern void timer4_init(void);
extern void timer4_off(void);
extern void timer4_start(void);
extern uint32_t timer4_get(void);
extern void timer4_delay(uint32_t time);

#endif