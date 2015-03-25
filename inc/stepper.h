/**
  ******************************************************************************
  * @file    stepper.h 
  * @author  Joe Todd
  * @version 
  * @date    
  * @brief   Header for stepper.c
  *
  ******************************************************************************
*/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STEPPER_H
#define STEPPER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

extern void stepper_init(void);
extern void stepper_turn_cw(uint16_t turns);
extern void stepper_turn_acw(uint16_t turns);

#endif