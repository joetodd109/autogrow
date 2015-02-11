/**
  ******************************************************************************
  * @file    spi.h 
  * @author  Joe Todd
  * @version 
  * @date    
  * @brief   Header for spi.c
  *
  ******************************************************************************
*/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SPI_H
#define SPI_H

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "iox.h"
#include "dma.h"

/*
 * Special values to use with jump operation
 */
#define SPI_OP_JP_EXIT_OK  (0xFFFF) /** Exit with OK status   */
#define SPI_OP_JP_EXIT_ERR (0xFFFE) /** Exit signalling error */

/**
 * Return codes
 */
typedef enum {
    spi_ok,
    spi_busy,
    spi_finished,
    spi_error
} spi_rc_t;

typedef enum {
    spi_op_reg_a,
    spi_op_reg_b,

    spi_num_op_regs,

    spi_op_reg_invalid
} spi_op_reg_t;

/**
 * Transaction Types
 */
typedef enum {
    /**
     * Transfer bytes over SPI interface
     * txdata and rxdata point to byte arrays, if valid data is transmitted to
     * and/or from these arrays. The array can be the same for transmit and
     * receive for full duplex operation.
     * len = length of transfer in bytes
     * reg = if not spi_op_reg_invalid, the given register contains an offset
     * that will be applied to txdata and rxdata pointers.
     */
    spi_trans_normal,

    /**
     * as spi_trans_normal above, but toggle NSS line before transaction
     * If len = 0, then will do toggle and return.
     */
    spi_trans_tog_nss,

    /**
     * Load (32-bit) word to register or variable.
     * txdata points to uint32_t source
     *   if NULL - SPI clocks in 32-bit word from SPI
     * rxdata points to uint32_t dest
     *   if NULL - data is stored in reg
     * reg = register
     */
    spi_op_ld,

   /**
     * Load (32-bit) word to register from register
     * txdata is src register value
     * reg = dst register
     */
    spi_op_ldr,

    /**
     * Load short (16-bit) word to register.
     * txdata points to uint32_t source
     * if NULL - SPI clocks in 2 bytes from SPI
     * rxdata points to uint32_t dest
     *   if NULL - data is stored in reg
     * reg = register
     */
    spi_op_lds,

    /**
     * Store word (32-bit) from register.
     * txdata points to uint32_t dest
     * if NULL - SPI clocks out 4 bytes from reg
     * reg = register
     */
    spi_op_sto,

    /**
     * Store short (16-bit) word from register.
     * txdata points to uint32_t dest
     * if NULL - SPI clocks out 2 bytes from reg
     * reg = register
     */
    spi_op_stos,

    /**
     * Add len to internal register.
     * reg = register
     */
    spi_op_add,

    /**
     * Apply bitwise AND operation to register
     * txdata points to uint32_t mask
     * reg = register
     */
    spi_op_and,

    /**
     * Shift register right
     * len = number of places to shift
     * reg = register
     */
    spi_op_shr,

    /**
     * Shift register left
     * len = number of places to shift
     * reg = register
     */
    spi_op_shl,

    /**
     * Branch (to another transaction) unconditional.
     * txdata = pointer to spi_transtype_t.
     * len = number of transactions in target structure
     */
    spi_op_b,

    /**
     * Branch (to another transaction) conditional.
     * txdata = pointer to spi_transtype_t.
     * for lt, gt & eq: rxdata points to int32_t to compare to register
     * len = number of transactions in target structure
     * reg = register 
     */
    spi_op_bz,                  /* branch if reg is zero */
    spi_op_bnz,                 /* branch if reg is non-zero */

    /**
     * Jump (relative) unconditional.
     * len = number of transactions to jump (positive or negative) or
     *       SPI_OP_JP_EXIT_OK or SPI_OP_JP_EXIT_ERR.
     */
    spi_op_j,

    /**
     * Jump (relative) conditional.
     * for lt, gt & eq rxdata points to int32_t to compare to register
     * txdata = pointer to mask applied to reg (if valid)
     * len = number of transactions to jump (positive or negative) or
     *       SPI_OP_JP_EXIT_OK or SPI_OP_JP_EXIT_ERR.
     * reg = register
     */
    spi_op_jz,                  /* jump if reg is zero */
    spi_op_jnz,                 /* jump if reg is non-zero */

    /**
     * Performs unlock computation
     * This is a bit PLD-specific, but adding all instructions necessary
     * to do this by hand and then constructing the appropriate sequence
     * would take far more space.
     * reg = register.
     * Converts 16-bit value in reg, storing the result back to reg.
     */
    spi_op_unlock,

    spi_num_transtypes
} spi_transtype_t;

/**
 * Callback function type - the argument passed will indicate
 * success (true) or failure (false) of the operation.
 */
typedef void (*spi_dma_callback_fn) (bool, void *);

/**
 * This structure defines one SPI operation.
 *
 * Note that genuine full duplex operation is possible, even if
 * txdata == rxdata, as the data transmitted will be overwritten by the
 * received data (i.e., meaning that one buffer can be used).
 */
struct spi_dma_transaction {
    /**
     * Data to transmit.
     * If NULL, then the transmit line will be held low during the transaction
     * (i.e., as if transmitting zeros).
     */
    void const *txdata;

    /**
     * Received data is stored here. If NULL, then the reception DMA
     * will not be enabled (e.g., for transmit-only operations).
     */
    void *rxdata;

    /*
     * Length, in bytes, of the transaction.
     */
    int32_t len;

    /**
     * This flag is used to indicate that this is a new SPI operation i.e., one
     * that requires a toggle on the SPI_NSS line to indicate to the slave that a
     * new operation is in progress.
     */
    spi_transtype_t type;

    /**
     * The register to use with ops
     */
    spi_op_reg_t reg;
};

typedef struct spi_dma_transaction spi_dma_transaction_t;


/**
 * Initialise the I2S.
 */
extern void spi_i2s_init(void);

/**
 * This function performs one or more SPI transactions via DMA. 
 */
extern spi_rc_t spi_do_transactions(spi_dma_transaction_t const
                                    *transactions, int16_t count);


#endif
