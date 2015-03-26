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

/* 
 * SYSCLK = 250kHz
 * TIM2CLK = 15.625kHz
 */
extern void 
timer_init(void) 
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
timer_delay(uint16_t time) 
{
    TIM2->CNT = 0;
    timer = TIM2->CNT;
    
    while (timer < time) {
        timer = TIM2->CNT;
    }
}

extern void
timer_reconfigure(uint16_t prescalar, uint16_t reload)
{
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM2->PSC = prescalar; 
    TIM2->ARR = reload;
    TIM2->CNT = 0;
    TIM2->EGR = TIM_EGR_UG;     /* update registers now */

    TIM2->CR1 |= TIM_CR1_CEN;       /* counter enabled */
}

void TIM2_IRQHandler(void) 
{
    TIM2->SR = 0x0;
}
