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
#include "stepper.h"

#define BUFFERSIZE      128u
#define MOIST_LEVEL     3000u

#define SENSOR_IN_PORT  iox_port_c
#define SENSOR_IN_PIN   1u
#define SENSOR_EN_PORT  iox_port_c
#define SENSOR_EN_PIN   2u
#define VALVE_ON_PORT   iox_port_b
#define VALVE_ON_PIN    2u

//#define TESTING         /* testing mode */
//#define VALVE           /* not using valve */
//#define HOLD_TIME       86400u / 2u  /* one day */
#define HOLD_TIME         86400u / 96u /* every 30 mins */

/* Prototypes -----------------------------------------------------------------*/
static uint16_t moisture[BUFFERSIZE] = {0};

/* 
 * Turn on water flow for 'time' in ms.
 */
static void
water_on(uint32_t time, uint16_t turns)
{
    /* timer ticks at 1ms */
    timer_reconfigure(0x001e, 0xFFFF); 
    timer_delay(time); 
    iox_led_on(false, false, false, true);
    stepper_turn_cw(turns);
    timer_delay(time);
    iox_led_on(false, false, false, false);
    stepper_turn_acw(turns);
}

/* Main -----------------------------------------------------------------------*/
int main(void)
{
    clk_init();
    iox_led_init();
    timer_init();
    adc_init();
    stepper_init();

    uint32_t i = 0;

    iox_configure_pin(SENSOR_IN_PORT, SENSOR_IN_PIN, iox_mode_analog, 
            iox_type_pp, iox_speed_low, iox_pupd_none);
    iox_configure_pin(SENSOR_EN_PORT, SENSOR_EN_PIN, iox_mode_out,
            iox_type_pp, iox_speed_low, iox_pupd_down);
    iox_configure_pin(VALVE_ON_PORT, VALVE_ON_PIN, iox_mode_out,
            iox_type_pp, iox_speed_low, iox_pupd_down);
    /* Test clock frequency 
    iox_configure_pin(iox_port_a, 8, iox_mode_af,
            iox_type_pp, iox_speed_high, iox_pupd_none);
    */

    while (1) {
        timer_reconfigure(0x7800, 0xFFFF);
        //iox_set_pin_state(SENSOR_EN_PORT, SENSOR_EN_PIN, true); 
        timer_delay(3);
        //moisture[i] = adc_get_measurement();  
        //iox_set_pin_state(SENSOR_EN_PORT, SENSOR_EN_PIN, false);    
        //if (moisture[i] > MOIST_LEVEL) {
            /* 
             * Soil is too dry!
             */
#ifdef VALVE
            iox_set_pin_state(VALVE_ON_PORT, VALVE_ON_PIN, true);
            timer_delay(5);
            iox_set_pin_state(VALVE_ON_PORT, VALVE_ON_PIN, false);
#else
            /* 
             * Water flow on for 3s 
             */
            water_on(3000, 450);
#endif
        //}
        i = (i + 1) % BUFFERSIZE;

#ifdef TESTING
        /* 
         * Just wait 2secs if testing 
         */
        timer_reconfigure(0x7800, 0xFFFF);
        timer_delay(2);
#else
        /* 
         * Else timer overflows in 24hours
         */
        timer_reconfigure(0xF000, HOLD_TIME);
        /* 
         * Sleep until timer overflows 
         */
        __WFI();
#endif
    }
}