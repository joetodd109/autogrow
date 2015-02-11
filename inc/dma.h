/**
  ******************************************************************************
  * @file    dma.h 
  * @author  Joe Todd
  * @version 
  * @date    
  * @brief   Header for dma.c
  *
  ******************************************************************************
*/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DMA_H
#define DMA_H

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "iox.h"


/*
 * Bit positions.
 */
#define  DMA_CR_EN_Pos        0       /*!< Position of Stream enable */
#define  DMA_CR_DMEIE_Pos     1       /*!< Position of Direct mode error interrupt enable */
#define  DMA_CR_TEIE_Pos      2       /*!< Position of Transfer error interrupt enable */
#define  DMA_CR_HTIE_Pos      3       /*!< Position of Half Transfer interrupt enable */
#define  DMA_CR_TCIE_Pos      4       /*!< Position of Transfer complete interrupt enable */
#define  DMA_CR_PFCTRL_Pos    5       /*!< Position of Peripheral flow controller, 0 = DMA controller, 1 = periph controller */
#define  DMA_CR_DIR_Pos       6       /*!< Position of Data transfer direction */
#define  DMA_CR_CIRC_Pos      8       /*!< Position of Circular mode */
#define  DMA_CR_PINC_Pos      9       /*!< Position of Peripheral increment mode */
#define  DMA_CR_MINC_Pos      10      /*!< Position of Memory increment mode */
#define  DMA_CR_PSIZE_Pos     11      /*!< Position of PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CR_MSIZE_Pos     13      /*!< Position of MSIZE[1:0] bits (Memory size) */
#define  DMA_CR_PINCOS_Pos    15      /*!< Position of Peripheral increment offset size */
#define  DMA_CR_PL_Pos        16      /*!< Position of PL[1:0] bits(Channel Priority level) */
#define  DMA_CR_DBM_Pos       18      /*!< Position of Double buffer mode */
#define  DMA_CR_CT_Pos        19      /*!< Position of Current target (DBmode) */
#define  DMA_CR_PBURST_Pos    21      /*!< Position of Peripheral burst transfer config */
#define  DMA_CR_MBURST_Pos    23      /*!< Position of Memory burst transfer config */
#define  DMA_CR_CHSEL_Pos     25      /*!< Position of Channel selection */


/**
 * Initialise the DMA system.
 */
extern void dma_init(void);

/**
 * Configure a single DMA channel.
 */
extern void dma_init_dma1_chx(uint32_t str, DMA_Stream_TypeDef const *cfg);


#endif