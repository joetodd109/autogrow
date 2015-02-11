/**
  ******************************************************************************
  * @file    adc.h 
  * @author  Joe Todd
  * @version 
  * @date    
  * @brief   Header for adc.c
  *
  ******************************************************************************
*/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ADC_H
#define ADC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "utl.h"

extern void adc_init(void);
extern uint16_t adc_get_measurement(void);

#endif