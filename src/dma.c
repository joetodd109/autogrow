#include "dma.h"


static DMA_Stream_TypeDef *const 
 dma1_streams[] = {
    DMA1_Stream1,
    DMA1_Stream2,
    DMA1_Stream3,
    DMA1_Stream4,
    DMA1_Stream5,
    DMA1_Stream6,
    DMA1_Stream7,
};

extern void
dma_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
}

extern void
dma_init_dma1_chx(uint32_t str, DMA_Stream_TypeDef const *cfg)
{
    DMA_Stream_TypeDef *rstr;

    rstr = dma1_streams[str - 1];

    /**
     * The channel must be disabled before we can write the CNDTR register
     */
    rstr->CR = 0;
    rstr->M0AR = cfg->M0AR;
    rstr->PAR = cfg->PAR;
    rstr->NDTR = cfg->NDTR;
    rstr->CR = cfg->CR;
}