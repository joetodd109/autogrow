/*******************************************************************************
 * @file    utl.c
 * @author  Joe Todd
 * @version
 * @date    January 2014
 * @brief   Theremin
 *
  ******************************************************************************/


/* Includes -------------------------------------------------------------------*/
#include "utl.h"

/**
 * Enable an interrupt.
 */
extern void
utl_enable_irq(IRQn_Type irq)
{
	uint32_t i;

	i = irq / 32;
	NVIC->ISER[i] = 1 << (irq - (i * 32));
}

/**
 * Disable an interrupt.
 */
extern void
utl_disable_irq(IRQn_Type irq)
{
	uint32_t i;

	i = irq / 32;
	NVIC->ICER[i] = 1 << (irq - (i * 32));
}