/**
  ******************************************************************************
  * @file    uart.h 
  * @author  Joe Todd
  * @version 
  * @date    
  * @brief   Header for uart.c
  *
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef UART_H
#define UART_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "iox.h"
#include "utl.h"

/* Definitions ---------------------------------------------------------------*/
#define PCLK2		16000000
#define RX_BUFFER_SIZE 4096

#define UART_DMA	DMA2_Stream2

typedef void (*uart_recv_callback_fn) (uint8_t bytes);

extern void uart_init(uint32_t baudrate);
extern void uart_send_data(unsigned char *buf, uint32_t len);
extern uint8_t uart_get_byte(uint32_t i);
extern void data_recv_callback(uint8_t bytes);

#endif