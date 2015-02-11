#ifndef TIMER_H
#define TIMER_H

#include "stm32f4xx.h"
#include "utl.h"

extern void timer_init(void);
extern void timer_delay(uint32_t time);
extern uint32_t timer_get(void);
extern void set_mems_read(void);

#endif