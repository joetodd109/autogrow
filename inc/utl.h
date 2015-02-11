/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef UTL_H
#define UTL_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/**
 * Enable an interrupt.
 */
extern void utl_enable_irq(IRQn_Type irq);

/**
 * Disable an interrupt.
 */
extern void utl_disable_irq(IRQn_Type irq);

#endif /* __UTL_H */