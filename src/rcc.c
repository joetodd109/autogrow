/**
 ******************************************************************************
 * @file    rcc.c
 * @author  Joe Todd
 * @version
 * @date    November 2014
 * @brief   Theremin
 *
  ******************************************************************************/


/* Includes -------------------------------------------------------------------*/
#include "rcc.h"

extern void
clk_init(void)
{
    RCC->CFGR = (RCC_CFGR_SWS_HSE
    	| RCC_CFGR_PPRE1_DIV16
    	| RCC_CFGR_HPRE_DIV64
    	| RCC_CFGR_MCO1_1);
    RCC->PLLCFGR = 0x0;
    RCC->CR = RCC_CR_HSEON;
    while ((RCC->CR & RCC_CR_HSERDY) != RCC_CR_HSERDY);	
}
