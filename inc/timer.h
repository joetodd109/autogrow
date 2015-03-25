#ifndef TIMER_H
#define TIMER_H

#include "stm32f4xx.h"
#include "utl.h"

extern void timer_init(void);
extern void timer_delay(uint16_t time);
extern void timer_reconfigure(uint16_t prescalar, uint16_t reload);

#endif