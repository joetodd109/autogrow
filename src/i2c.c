/**
 ******************************************************************************
 * @file    i2c.c
 * @author  Joe Todd
 * @version
 * @date    January 2014
 * @brief   Theremin
 *
  ******************************************************************************/


/* Includes -------------------------------------------------------------------*/
#include "i2c.h"
#include "stdbool.h"
#include "stdint.h"
#include "string.h"


/* Prototypes ----------------------------------------------------------------*/
static i2c_status_t i2c_status;

/**
 * Setup I2C for Magnetometer
 */
extern void
i2c_mems_init(void)
{
    uint32_t ccr;
    i2c_status = i2c_idle;

    /*
     * Turn on the clock to the peripheral.
     */
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    /*
     * Hold the peripheral in reset while we ensure the bus is clear.
     */
    I2C1->CR1 |= I2C_CR1_SWRST;

    /* 
     * Configure I2C pins 
     */
    iox_configure_pin(iox_port_b, PIN6, iox_mode_af,
                    iox_type_pp, iox_speed_fast, iox_pupd_down);
    iox_configure_pin(iox_port_b, PIN9, iox_mode_af,
                    iox_type_pp, iox_speed_fast, iox_pupd_down);
    
    iox_alternate_func(iox_port_b, PIN6, AF4);
    iox_alternate_func(iox_port_b, PIN9, AF4); 

    ccr = 8;
    I2C1->TRISE = ((I2CCLK / 1000000u) + 1);

    /*
     * Bring out of reset.
     */
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    /*
     * Enable the peripheral.
     */
    I2C1->CR1 |= (I2C_CR1_PE                /* ACK enable */
        | (I2C_CR1_ACK));

    I2C1->CCR = (0u << I2C_CCR_DUTY_Pos)    /* Duty cycle = 50% */
        | (1u << I2C_CCR_FS_Pos)            /* Fast mode. */
        | ccr;

    I2C1->CR2 = (I2CCLK / 1000000u);

    /*
     * Set interface address
     */
    I2C1->OAR1 = (1u << 14);               /* must be set */
}

/**
 * Setup I2C for CS43L22 initialisation sequence.
 */
extern void
i2c_codec_init(void)
{
    uint32_t ccr;
    i2c_status = i2c_idle;

    /*
     * Turn on the clock to the peripheral.
     */
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    /*
     * Hold the peripheral in reset while we ensure the bus is clear.
     */
    I2C1->CR1 |= I2C_CR1_SWRST;

    /* 
     * Configure I2C pins 
     */
    iox_configure_pin(iox_port_b, PIN6, iox_mode_af,
                    iox_type_od, iox_speed_fast, iox_pupd_none);
    iox_configure_pin(iox_port_b, PIN9, iox_mode_af,
                    iox_type_od, iox_speed_fast, iox_pupd_none);
    
    iox_alternate_func(iox_port_b, PIN6, AF4);
    iox_alternate_func(iox_port_b, PIN9, AF4); 

    ccr = 128;
    I2C1->TRISE = ((I2CCLK / 1000000u) + 1);

    /*
     * Bring out of reset.
     */
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    /*
     * Enable the peripheral.
     */
    I2C1->CR1 |= (I2C_CR1_PE);              /* ACK enable */
       // | (I2C_CR1_ACK));

    I2C1->CCR = (0u << I2C_CCR_DUTY_Pos)    /* Duty cycle = 50% */
        | (1u << I2C_CCR_FS_Pos)            /* Fast mode. */
        | ccr;

    I2C1->CR2 = (I2CCLK / 1000000u);

    /*
     * Set interface address
     */
     I2C1->OAR1 = ((1u << 14)               /* must be set */
        | (I2C_DAC_ADDR << 1u))             /* interface address, must have 0 LSB */
        ;                                   /* for 7 bit addressing */
}

/*
 * Write data to I2C
 */
extern bool
i2c_write(uint8_t address, uint8_t txaddr, uint8_t const *txdata, uint8_t num_bytes) 
{
    uint8_t byte = 0;

    if (txdata != NULL) {
        if (i2c_status == i2c_idle) {
            /*
             * Send the address of the peripheral we
             * want to write to. 
             */
            while ((I2C1->SR2 & I2C_SR2_BUSY) == 1) {
                /* wait until bus isn't busy */
            }
            i2c_status = i2c_writing;

            /* 
             * Generate start condition 
             */
            I2C1->CR1 |= I2C_CR1_START;
            while ((I2C1->SR1 & I2C_SR1_SB) == 0);
            /* 
             * Slave address transmission, LSB must be reset to 
             * enter tx mode in 7-bit addressing mode.
             */
            I2C1->DR = (address << 1u) | I2C_WRITE;

            while ((I2C1->SR1 & I2C_SR1_ADDR) == 0);
            /* Clear ADDR by reading status registers */
            (void) I2C1->SR2;

            /* Send destination address */
            I2C1->DR = txaddr;
            while ((I2C1->SR1 & I2C_SR1_TXE) == 0);

            if (byte < num_bytes) {
                /* Send data */
                I2C1->DR = txdata[byte++];
                while ((I2C1->SR1 & I2C_SR1_TXE) == 0);  
            }
            while ((I2C1->SR1 & I2C_SR1_BTF) == 0);
            /*
             * Generate stop condition
             */
            I2C1->CR1 |= I2C_CR1_STOP;
            i2c_status = i2c_idle;

            return true;
        }
    }
    return false;
}

extern bool
i2c_read(uint8_t address, uint8_t txaddr, uint8_t *rxdata, uint8_t num_bytes) 
{
    uint32_t timeout = 40960;

    if (rxdata != NULL) {
        if (i2c_status == i2c_idle) {
            /*
             * Send the register address we want to read from
             * to the peripheral.
             */
            while ((I2C1->SR2 & I2C_SR2_BUSY) == 1);
            i2c_status = i2c_reading;

            I2C1->CR1 |= I2C_CR1_START;
            while ((I2C1->SR1 & I2C_SR1_SB) == 0);
            /* 
             * Slave address transmission, LSB must be reset to 
             * enter tx mode in 7-bit addressing mode.
             */
            I2C1->DR = (address << 1u) | I2C_WRITE;

            while ((I2C1->SR1 & I2C_SR1_ADDR) == 0);
            /* Clear ADDR by reading status registers */
            (void) I2C1->SR2;

            /* Send data */
            I2C1->DR = txaddr;
            while ((I2C1->SR1 & I2C_SR1_TXE) == 0) {
                if (timeout-- == 0) {
                    return ERROR;
                }
            }

            timeout = 40960;
            I2C1->CR1 |= I2C_CR1_START;
            while ((I2C1->SR1 & I2C_SR1_SB) == 0);
            
            /* 
             * Slave address transmission, LSB must be set to 
             * enter rx mode in 7-bit addressing mode.
             */
            I2C1->DR = (address << 1u) | I2C_READ;
            while ((I2C1->SR1 & I2C_SR1_ADDR) == 0);

            while (num_bytes) {
                if (num_bytes > 3) {
                    /* Clear ADDR by reading status registers */
                    (void) I2C1->SR2;
                    while ((I2C1->SR1 & I2C_SR1_BTF) == 0) {
                        /* wait until byte transfer finished */
                    }
                    *rxdata++ = I2C1->DR;
                    num_bytes--;
                }
                if (num_bytes == 3) {
                    while ((I2C1->SR1 & I2C_SR1_BTF) == 0) {
                        /* wait until byte transfer finished */
                    }
                    /* Clear ACK */
                    I2C1->CR1 &= ~(I2C_ACK_EN);
                    *rxdata++ = I2C1->DR;
                    while ((I2C1->SR1 & I2C_SR1_RXNE) == 0);
                    *rxdata++ = I2C1->DR;
                    /* generate stop condition */
                    I2C1->CR1 |= I2C_CR1_STOP;
                    while ((I2C1->SR1 & I2C_SR1_RXNE) == 0);
                    *rxdata++ = I2C1->DR;
                    num_bytes = 0;
                }
                if (num_bytes == 1) {
                    /* Clear ACK */
                    I2C1->CR1 &= ~(I2C_ACK_EN);
                    /* Clear ADDR by reading status registers */
                    (void) I2C1->SR2;
                    /* Generate Stop condition */
                    I2C1->CR1 |= I2C_CR1_STOP;
                    while ((I2C1->SR1 & I2C_SR1_RXNE) == 0);
                    *rxdata++ = I2C1->DR;
                    num_bytes = 0;
                }
            }
            while ((I2C1->CR1 & I2C_CR1_STOP) != 0) {
            }
            /* Enable ACK */
            I2C1->CR1 |= I2C_ACK_EN;
            i2c_status = i2c_idle;

            return true;
        }
    }
    return false;
}

extern bool
i2c_check_status(uint32_t status)
{
    uint32_t sr1;
    uint32_t sr2;
    uint32_t sr;

    sr1 = I2C1->SR1;
    sr2 = I2C1->SR2;

    sr = sr1 | (sr2 <<= 16);

    if ((sr & status) == status) {
        return true;
    }
    return false;
}