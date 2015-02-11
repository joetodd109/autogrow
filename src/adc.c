/**
 ******************************************************************************
 * @file    adc.c
 * @author  Joe Todd
 * @version
 * @date    January 2015
 * @brief   Autogrow
 *
 ******************************************************************************/
#include "adc.h"

#define ADC_CHAN	11u
#define ADC_SAMPLE_144_CYCLES	6u

static uint16_t adc_reading;
static uint32_t adc_conv_cnt;

static void adc_configure_sample_time(uint32_t ch, uint32_t smp);

/*
 * Initialise the ADC.
 */
extern void
adc_init(void)
{
	adc_conv_cnt = 0;
    adc_reading = 0;

    /*
     * Enable clock for ADC1.
     */
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    /*
     * Turn on the ADC.
     */
    ADC1->CR2 = ADC_CR2_ADON;

    /*
     * And now configure.
     */
    ADC1->CR1 = 0;
    ADC1->CR2 |= ADC_CR2_EOCS;		/* Enable end of conversion flag */

    ADC1->SMPR1 = 0;
    ADC1->SMPR2 = 0;

    /*
     * All sample times are: 144 cycles.
     */
    adc_configure_sample_time(ADC_CHAN, ADC_SAMPLE_144_CYCLES);

    ADC1->SQR1 = 0;
    ADC1->SQR2 = 0;
    ADC1->SQR3 = ADC_CHAN << 0u; /* perform 1st conversion on channel 11 */
}

/*
 * Return a single conversion.
 */
extern uint16_t
adc_get_measurement(void)
{
	ADC1->CR2 |= ADC_CR2_SWSTART;
	while ((ADC1->SR & ADC_SR_EOC) == 0);
	return ADC1->DR;
}

/*
 * Configure the sample time for a channel.
 */
static void
adc_configure_sample_time(uint32_t ch, uint32_t smp)
{
    uint32_t shift;

    if (ch > 9) {
        shift = (ch - 10) * 3;
        ADC1->SMPR1 = (ADC1->SMPR1 & ~(7 << shift)) | (smp << shift);
    }
    else {
        shift = ch * 3;
        ADC1->SMPR2 = (ADC1->SMPR2 & ~(7 << shift)) | (smp << shift);
    }
}
