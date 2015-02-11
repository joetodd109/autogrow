/*******************************************************************************
 * @file    main.c
 * @author  Joe Todd
 * @version
 * @date    January 2015
 * @brief   Autogrow
 *
 ******************************************************************************/


/* Includes -------------------------------------------------------------------*/
#include "rcc.h"
#include "timer.h"
#include "iox.h"
#include "adc.h"

#define BUFFERSIZE 		128u
#define MOIST_LEVEL		3000u

#define SENSOR_IN_PORT	iox_port_c
#define SENSOR_IN_PIN	1u
#define SENSOR_EN_PORT	iox_port_c
#define SENSOR_EN_PIN	2u
#define VALVE_ON_PORT	iox_port_b
#define VALVE_ON_PIN	2u

#define TESTING			/* testing mode */

#ifndef TESTING
#define HOLD_TIME		86400u	/* one day */
#else
#define HOLD_TIME		10u		/* 10s for testing */
#endif

/* Prototypes -----------------------------------------------------------------*/
static uint16_t moisture[BUFFERSIZE] = {0};

/* Main -----------------------------------------------------------------------*/
int main(void)
{
	uint32_t i = 0;
    iox_led_init();
    timer_init();
    adc_init();

#ifdef TESTING
    bool blue = true;
    iox_led_on(false, false, false, blue);
#endif
    iox_configure_pin(SENSOR_IN_PORT, SENSOR_IN_PIN, iox_mode_analog, 
    		iox_type_pp, iox_speed_low, iox_pupd_none);
    iox_configure_pin(SENSOR_EN_PORT, SENSOR_EN_PIN, iox_mode_out,
    		iox_type_pp, iox_speed_low, iox_pupd_down);
    iox_configure_pin(VALVE_ON_PORT, VALVE_ON_PIN, iox_mode_out,
    		iox_type_pp, iox_speed_low, iox_pupd_down);

    while (1) {
    	iox_set_pin_state(SENSOR_EN_PORT, SENSOR_EN_PIN, true);	
    	timer_delay(2);
    	moisture[i] = adc_get_measurement();  
    	iox_set_pin_state(SENSOR_EN_PORT, SENSOR_EN_PIN, false);	
    	if (moisture[i] > MOIST_LEVEL) {
    		/* soil is too dry */
    		iox_set_pin_state(VALVE_ON_PORT, VALVE_ON_PIN, true);
    		timer_delay(5);
    		iox_set_pin_state(VALVE_ON_PORT, VALVE_ON_PIN, false);
    	}
    	i = (i + 1) % BUFFERSIZE;

#ifdef TESTING
    	blue = !blue;
    	iox_led_on(false, false, false, blue);
#endif
    	timer_delay(HOLD_TIME); 
    }

}