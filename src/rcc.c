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
    RCC->CR |= RCC_CR_HSION;

    RCC->CFGR = RCC_CFGR_SWS_HSI;

    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;
    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);	
}
