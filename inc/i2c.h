/**
  ******************************************************************************
  * @file    i2c.h 
  * @author  Joe Todd
  * @version 
  * @date    
  * @brief   Header for i2c.c
  *
  ******************************************************************************
*/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef I2C_H
#define I2C_H

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"
#include "stm32f4xx.h"
#include "iox.h"
#include "mems.h"
#include "codec.h"

#define I2C_DAC_ADDR        0x25

#define I2C_WRITE           0
#define I2C_READ            1

#define I2CCLK              16000000u

/* 
 * Bit definitions
 */

#define I2C_CCR_DUTY_Pos    14
#define I2C_CCR_FS_Pos      15

#define I2C_ACK_EN          (1u << 10)

/** 
  * SR2 register flags  
  */

#define I2C_FLAG_DUALF                  ((uint32_t)0x00800000)
#define I2C_FLAG_SMBHOST                ((uint32_t)0x00400000)
#define I2C_FLAG_SMBDEFAULT             ((uint32_t)0x00200000)
#define I2C_FLAG_GENCALL                ((uint32_t)0x00100000)
#define I2C_FLAG_TRA                    ((uint32_t)0x00040000)
#define I2C_FLAG_BUSY                   ((uint32_t)0x00020000)
#define I2C_FLAG_MSL                    ((uint32_t)0x00010000)

/** 
  * SR1 register flags  
  */

#define I2C_FLAG_SMBALERT               ((uint32_t)0x10008000)
#define I2C_FLAG_TIMEOUT                ((uint32_t)0x10004000)
#define I2C_FLAG_PECERR                 ((uint32_t)0x10001000)
#define I2C_FLAG_OVR                    ((uint32_t)0x10000800)
#define I2C_FLAG_AF                     ((uint32_t)0x10000400)
#define I2C_FLAG_ARLO                   ((uint32_t)0x10000200)
#define I2C_FLAG_BERR                   ((uint32_t)0x10000100)
#define I2C_FLAG_TXE                    ((uint32_t)0x10000080)
#define I2C_FLAG_RXNE                   ((uint32_t)0x10000040)
#define I2C_FLAG_STOPF                  ((uint32_t)0x10000010)
#define I2C_FLAG_ADD10                  ((uint32_t)0x10000008)
#define I2C_FLAG_BTF                    ((uint32_t)0x10000004)
#define I2C_FLAG_ADDR                   ((uint32_t)0x10000002)
#define I2C_FLAG_SB                     ((uint32_t)0x10000001)


#define  I2C_EVENT_MASTER_MODE_SELECT                   ((uint32_t)0x00030001)  /* BUSY, MSL and SB flag */
#define  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED     ((uint32_t)0x00070082)  /* BUSY, MSL, ADDR, TXE and TRA flags */
#define  I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED        ((uint32_t)0x00030002)  /* BUSY, MSL and ADDR flags */
#define  I2C_EVENT_MASTER_MODE_ADDRESS10                ((uint32_t)0x00030008)  /* BUSY, MSL and ADD10 flags */
#define  I2C_EVENT_MASTER_BYTE_RECEIVED                 ((uint32_t)0x00030040)  /* BUSY, MSL and RXNE flags */

#define I2C_EVENT_MASTER_BYTE_TRANSMITTING              ((uint32_t)0x00070080) /* TRA, BUSY, MSL, TXE flags */
#define  I2C_EVENT_MASTER_BYTE_TRANSMITTED              ((uint32_t)0x00070084)  /* TRA, BUSY, MSL, TXE and BTF flags */


typedef enum {
    i2c_idle,
    i2c_writing,
    i2c_reading,
} i2c_status_t;


/**
 * Callback function type - the argument passed will indicate
 * success (true) or failure (false) of the operation.
 */
//typedef void (*i2c_dma_callback_fn) (bool);

/**
 * Setup I2C for Magnetometer
 */
extern void i2c_mems_init(void);

/**
 * Setup I2C for CS43L22 initialisation sequence.
 */
extern void i2c_codec_init(void);

/**
 * Write data to I2C
 */
extern bool i2c_write(uint8_t address, uint8_t txaddr, uint8_t const *txdata, uint8_t num_bytes);

/**
 * Read data from I2C
 */
extern bool i2c_read(uint8_t address, uint8_t txaddr, uint8_t *rxdata, uint8_t num_bytes);

/*
 * Returns the status registers
 */
extern bool i2c_check_status(uint32_t status);


#endif