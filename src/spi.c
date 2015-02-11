/**
 ******************************************************************************
 * @file    spi.c
 * @author  Joe Todd
 * @version
 * @date    January 2014
 * @brief   Theremin
 *
  ******************************************************************************/


/* Includes -------------------------------------------------------------------*/
#include "spi.h"


/* 
 * Bit definitions - 
 * configured when i2s disabled 
 */

/* ------- SPI_I2SCFGR ------- */

#define I2S_MODE_EN			(1u << 11)
#define I2S_PERIPH_EN		(1u << 10)
#define I2S_MASTER_TX		(2u << 8)
#define I2S_CKPOL_HIGH		(1u << 3)

/* ------- SPI_I2SPR -------- */

#define I2S_MCLK_EN			(1u << 9)

typedef enum {
    spi_st_busy,
    spi_st_idle
} spi_status_t;

typedef spi_rc_t(*trans_fn) (void);

static void spi_configure_dma1_ch(const uint8_t * src, uint16_t nbytes,
                                  bool transmit, bool incr_mem);
static spi_rc_t spi_do_transactions_int(spi_dma_transaction_t const
                                        *transactions);
static void spi_start_dma(uint8_t const *txdata, 
					uint8_t * rxdata, uint16_t len);
static spi_rc_t spi_trans_normal_fn(void);
static spi_rc_t spi_op_ld_fn(void);
static spi_rc_t spi_op_ldr_fn(void);
static spi_rc_t spi_op_lds_fn(void);
static spi_rc_t spi_op_sto_fn(void);
static spi_rc_t spi_op_stos_fn(void);
static spi_rc_t spi_op_add_fn(void);
static spi_rc_t spi_op_and_fn(void);
static spi_rc_t spi_op_shr_fn(void);
static spi_rc_t spi_op_shl_fn(void);
static spi_rc_t spi_op_b_fn(void);
static spi_rc_t spi_op_bz_fn(void);
static spi_rc_t spi_op_bnz_fn(void);
static spi_rc_t spi_op_j_fn(void);
static spi_rc_t spi_op_jz_fn(void);
static spi_rc_t spi_op_jnz_fn(void);
static spi_rc_t spi_op_unlock_fn(void);

static const trans_fn spi_trans_fns[spi_num_transtypes] = {
    spi_trans_normal_fn,
    spi_op_ld_fn,
    spi_op_ldr_fn,
    spi_op_lds_fn,
    spi_op_sto_fn,
    spi_op_stos_fn,
    spi_op_add_fn,
    spi_op_and_fn,
    spi_op_shr_fn,
    spi_op_shl_fn,
    spi_op_b_fn,
    spi_op_bz_fn,
    spi_op_bnz_fn,
    spi_op_j_fn,
    spi_op_jz_fn,
    spi_op_jnz_fn,
    spi_op_unlock_fn
};

static spi_dma_callback_fn spi_dma_callback;
static uint32_t spi_start_err_cnt;
static void *spi_callback_handle;
static int32_t spi_trans_count;
static spi_status_t spi_status;
static uint32_t dma1_str5_int_cnt;
static uint32_t dma1_str5_tc_int_cnt;
static uint32_t dma1_str5_ht_int_cnt;
static uint32_t dma1_str5_te_int_cnt;
static uint32_t dma1_str5_gi_int_cnt;
static spi_dma_transaction_t const *spi_transactions;
static const uint8_t spi_null_byte = 0;
static uint8_t spi_dummy_byte = 0;
static uint32_t *op_reg;
static spi_dma_transaction_t const *trans;
static spi_dma_transaction_t const *ret_trans;
static int32_t ret_trans_count;
static uint32_t spi_op_regs[spi_num_op_regs];

/**
 * Initialise the I2S.
 */
extern void
spi_i2s_init(void)
{
	uint8_t i2s_div_prescalar = 11;

	/* 
	 * I2S pins 
	 */
	iox_configure_pin(iox_port_c, PIN7, iox_mode_af,
						iox_type_pp, iox_speed_fast, iox_pupd_none);
	iox_configure_pin(iox_port_c, PIN10, iox_mode_af,
						iox_type_pp, iox_speed_fast, iox_pupd_none);
	iox_configure_pin(iox_port_c, PIN12, iox_mode_af,
						iox_type_pp, iox_speed_fast, iox_pupd_none);
	iox_configure_pin(iox_port_a, PIN4, iox_mode_af,
						iox_type_pp, iox_speed_fast, iox_pupd_none);

	/* Prepare output ports for alternate function */
	iox_alternate_func(iox_port_a, PIN4, AF6);
	iox_alternate_func(iox_port_c, PIN7, AF6);
	iox_alternate_func(iox_port_c, PIN10, AF6);
	iox_alternate_func(iox_port_c, PIN12, AF6);

	RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;

	SPI3->I2SCFGR = (I2S_MODE_EN
		|	(I2S_PERIPH_EN)
		|	(I2S_MASTER_TX)
		|	(I2S_CKPOL_HIGH));

	SPI3->I2SPR = (I2S_MCLK_EN
		|	(i2s_div_prescalar));
}

/**
 * Public function to request that some transactions are performed. This
 * function is a wrapper around an internal function as we make internal
 * transaction requests too and we aren't concerned with locking etc in
 * that case.
 */
extern spi_rc_t
spi_do_transactions(spi_dma_transaction_t const *transactions,
                    int16_t count)
{
    spi_rc_t rc = spi_busy;

    if (spi_status == spi_st_idle) {
        spi_trans_count = count;
        spi_status = spi_st_busy;
        spi_start_err_cnt = dma1_str5_te_int_cnt;
        rc = spi_do_transactions_int(transactions);

        if (rc != spi_ok) {
            spi_status = spi_st_idle;
        }
    }

    return rc;
}

static spi_rc_t
spi_do_transactions_int(spi_dma_transaction_t const *transaction)
{
    spi_rc_t rc = spi_busy;

    trans = transaction;

    while (rc == spi_busy) {
        /* check to see if this is the end of a transaction */
        if (trans == NULL) {
            rc = spi_error;
        }
        else if (spi_trans_count == 0) {
            if (ret_trans_count > 0) {
                spi_trans_count = ret_trans_count;
                trans = ret_trans + 1;
                ret_trans_count = 0;
                ret_trans = NULL;
                /* we'll loop round again and execute the next transaction */
            }
            else {
                rc = spi_finished;
            }
        }
        else {
            spi_transactions = trans + 1;
            spi_trans_count--;
            //op_reg = &spi_op_regs[trans->reg];
            asm volatile ("":::"memory");       /* memory barrier */

            if (trans->type < spi_num_transtypes) {
                rc = spi_trans_fns[trans->type] ();
            }
            else {
                rc = spi_error;
            }
        }
    }

    return rc;
}

/**
 * Function to kick off DMA operations.
 */
static void
spi_start_dma(uint8_t const *txdata, uint8_t * rxdata, uint16_t len)
{
    if (txdata != NULL) {
        spi_configure_dma1_ch(txdata, len, true, true);
    }
    else {
        spi_configure_dma1_ch(&spi_null_byte, len, true, false);
    }

    /*if (rxdata != NULL) {
        spi_configure_dma1_ch(rxdata, len, false, true);
    }
    else {
        spi_configure_dma1_ch(&spi_dummy_byte, len, false, false);
    }*/

    SPI3->CR2 |= (SPI_CR2_TXDMAEN);
    //SPI3->CR1 |= SPI_CR1_SPE;
}

/**
 * Configure the relevant eDMA channel for either transmission (transmit=true)
 * or reception.
 */
static void
spi_configure_dma1_ch(const uint8_t * src, uint16_t nbytes, bool transmit,
                      bool incr_mem)
{
    uint32_t dma_chan;

    dma_chan = transmit ? 3u : 2u;

    DMA_Stream_TypeDef cfg = {
        .CR = (1u << DMA_CR_EN_Pos)  /* Enable. */
            |(1u << DMA_CR_TCIE_Pos)  /* Transfer complete interrupt enabled. */
            |(0u << DMA_CR_HTIE_Pos)  /* No half-transfer interrupt. */
            |(1u << DMA_CR_TEIE_Pos)  /* Transfer error interrupt enabled. */
            |((transmit ? 1u : 0u) << DMA_CR_DIR_Pos) /* Read from memory. */
            |(1u << DMA_CR_CIRC_Pos)  /* Not circular mode. */
            |(0u << DMA_CR_PINC_Pos)  /* Don't increment peripheral address. */
            |((incr_mem ? 1u : 0u) << DMA_CR_MINC_Pos)        /* Do increment memory address. */
            |(0u << DMA_CR_PSIZE_Pos) /* 8-bit peripheral size. */
            |(0u << DMA_CR_MSIZE_Pos) /* 8-bit memory size. */
            |((transmit ? 3u : 1u) << DMA_CR_PL_Pos)  /* high or medium priority. */
            |(1u << DMA_CR_CHSEL_Pos),      /* Channel Selection. */
        .NDTR = nbytes,
        .PAR = (uint32_t) & SPI3->DR,
        .M0AR = (uint32_t) src,
    };

    dma_init_dma1_chx(dma_chan, (DMA_Stream_TypeDef const *) &cfg);
}

/**
 * Transaction handling functions
 */

static spi_rc_t
spi_trans_normal_fn(void)
{
    uint32_t offset;
    uint8_t const *txdata;
    uint8_t *rxdata;

    offset = (trans->reg != spi_op_reg_invalid) ? *op_reg : 0;
    txdata = (trans->txdata != NULL) ? (trans->txdata + offset) : NULL;
    rxdata = (trans->rxdata != NULL) ? (trans->rxdata + offset) : NULL;

    spi_start_dma(txdata, rxdata, trans->len);
    return spi_ok;
}

static spi_rc_t
spi_op_ld_fn(void)
{
    if (trans->txdata != NULL) {
        *op_reg = *(uint32_t *) trans->txdata;
        trans++;
        return spi_busy;
    }
    else {
        spi_start_dma(NULL, (trans->rxdata != NULL) ? trans->rxdata : (uint8_t *) op_reg, 4);
        return spi_ok;
    }
}

static spi_rc_t
spi_op_ldr_fn(void)
{
    spi_op_reg_t src_reg;

    src_reg = (spi_op_reg_t) trans->txdata;

    if (src_reg < spi_num_op_regs) {
        *op_reg = spi_op_regs[src_reg];
        trans++;
        return spi_busy;
    }

    return spi_error;
}

static spi_rc_t
spi_op_lds_fn(void)
{
    if (trans->txdata != NULL) {
        *op_reg = *(uint32_t *) trans->txdata & UINT16_MAX;
        trans++;
        return spi_busy;
    }
    else {
        spi_start_dma(NULL, (trans->rxdata != NULL) ? trans->rxdata : (uint8_t *) op_reg, 2);
        return spi_ok;
    }
}

static spi_rc_t
spi_op_sto_fn(void)
{
    if (trans->txdata != NULL) {
        *(uint32_t *) trans->txdata = *op_reg;
        trans++;
        return spi_busy;
    }
    else {
        spi_start_dma((uint8_t *) op_reg, NULL, 4);
        return spi_ok;
    }
}

static spi_rc_t
spi_op_stos_fn(void)
{
    if (trans->txdata != NULL) {
        *(uint32_t *) trans->txdata = *op_reg & UINT16_MAX;
        trans++;
        return spi_busy;
    }
    else {
        spi_start_dma((uint8_t *) op_reg, NULL, 2);
        return spi_ok;
    }
}

static spi_rc_t
spi_op_add_fn(void)
{
    *op_reg = (uint32_t) (*(int32_t *) op_reg + trans->len);
    trans++;
    return spi_busy;
}

static spi_rc_t
spi_op_and_fn(void)
{
    *op_reg = *op_reg & *(uint32_t *) trans->txdata;
    trans++;
    return spi_busy;
}

static spi_rc_t
spi_op_shr_fn(void)
{
    *op_reg >>= trans->len;
    trans++;
    return spi_busy;
}

static spi_rc_t
spi_op_shl_fn(void)
{
    *op_reg <<= trans->len;
    trans++;
    return spi_busy;
}

static spi_rc_t
spi_op_unlock_fn(void)
{
    *op_reg = ((*op_reg+1) & 0xff)<<8 | ((*op_reg>>8) & 0xff);
    trans++;
    return spi_busy;
}

static spi_rc_t
spi_op_b_fn(void)
{
    /* TODO - check for branch in progress */
    ret_trans_count = spi_trans_count;
    ret_trans = trans;
    spi_trans_count = trans->len;
    trans = (spi_dma_transaction_t const *) trans->txdata;
    return spi_busy;
}

static spi_rc_t
spi_op_bz_fn(void)
{
    if (*op_reg == 0) {
        return spi_op_b_fn();
    }

    trans++;
    return spi_busy;
}

static spi_rc_t
spi_op_bnz_fn(void)
{
    if (*op_reg > 0) {
        return spi_op_b_fn();
    }

    trans++;
    return spi_busy;
}

static spi_rc_t
spi_op_j_fn(void)
{
    spi_rc_t rc;

    switch (trans->len) {
    case SPI_OP_JP_EXIT_OK:
        rc = spi_finished;
        break;

    case SPI_OP_JP_EXIT_ERR:
        rc = spi_error;
        break;

    default:
        spi_trans_count -= (trans->len - 1);
        trans += trans->len;
        rc = spi_busy;
    }

    return rc;
}

static spi_rc_t
spi_op_jz_fn(void)
{
    uint32_t mask =
        (trans->txdata == NULL) ? UINT32_MAX : *(uint32_t *) trans->txdata;

    if ((*op_reg & mask) == 0) {
        return spi_op_j_fn();
    }

    trans++;
    return spi_busy;
}

static spi_rc_t
spi_op_jnz_fn(void)
{
    uint32_t mask =
        (trans->txdata == NULL) ? UINT32_MAX : *(uint32_t *) trans->txdata;

    if ((*op_reg & mask) > 0) {
        return spi_op_j_fn();
    }

    trans++;
    return spi_busy;
}

