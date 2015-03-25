/**
 ******************************************************************************
 * @file    timer.c
 * @author  Joe Todd
 * @version
 * @date    January 2014
 * @brief   Autogrow
 *
  ******************************************************************************/

#include "timer.h"

static uint32_t timer;
static uint32_t ms_count;

/* 
 * SYSCLK = 250kHz
 * TIM2CLK = 15.625kHz
 */
extern void 
timer2_init(void) 
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; 
    RCC->APB1LPENR |= RCC_APB1LPENR_TIM2LPEN;   /* enable tim2 in sleep mode */
    
    TIM2->PSC = 0x7800;             /* 61440 prescalar not 62500? */
    TIM2->DIER |= TIM_DIER_UIE;     /* enable update interrupt */
    TIM2->ARR = 0xFFFF;             /* count to 65535 */
    TIM2->CR1 |= TIM_CR1_ARPE;      /* autoreload on */
    TIM2->EGR = TIM_EGR_UG;         /* trigger update event */

    utl_enable_irq(TIM2_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;       /* counter enabled */
}

extern void 
timer4_init(void) 
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    TIM4->PSC = 0x000F;             /* 16 prescalar */
    TIM4->ARR = 0x03E8;             /* count to 1000 */
    TIM4->DIER |= TIM_DIER_UIE;     /* enable update interrupt */
    TIM4->CR1 |= TIM_CR1_ARPE       /* autoreload on */
        | (TIM_CR1_CEN);            /* counter enabled */
    TIM4->EGR = 1;                  /* trigger update event */

    utl_enable_irq(TIM4_IRQn);
}

extern void
timer4_off(void)
{
    TIM4->CR1 &= ~TIM_CR1_CEN;
}

extern void 
timer2_delay(uint16_t time) {
    uint16_t end;

    end = time + TIM2->CNT;
    while (timer < end) {
        timer = TIM2->CNT;
    }
}

extern void
timer2_reconfigure(uint16_t prescalar, uint16_t reload)
{
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM2->PSC = prescalar; 
    TIM2->ARR = reload;
    TIM2->EGR = TIM_EGR_UG;     /* update registers now */

    TIM2->CR1 |= TIM_CR1_CEN;       /* counter enabled */
}

extern void 
timer4_start(void)
{
    ms_count = 0;   /* ms_count overflows every 72mins so reset */
}

extern uint32_t 
timer4_get(void)
{
    return (ms_count * 1000UL) + TIM2->CNT;
}

extern void 
timer4_delay(uint32_t time) {
    uint32_t start;
    uint32_t timer;
    uint32_t end;

    timer4_start();
    start = timer4_get();
    end = start + time;
    timer = start;

    while (timer < end) {
        timer = timer4_get();
    }
}

void TIM4_IRQHandler(void) 
{
    uint32_t sr;
    sr = TIM4->SR;

    if (sr & TIM_SR_UIF) {
        ms_count++;
    }
    TIM4->SR &= ~TIM_SR_UIF;
}

void TIM2_IRQHandler(void) 
{
    TIM2->SR = 0x0;
}
