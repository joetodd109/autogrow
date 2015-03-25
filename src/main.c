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

#define TESTING         /* testing mode */
#define HOLD_TIME       86400u / 2u  /* one day */

/* Prototypes -----------------------------------------------------------------*/
#ifndef TESTING
static uint16_t moisture[BUFFERSIZE] = {0};
#endif

/* Main -----------------------------------------------------------------------*/
int main(void)
{
    clk_init();
    iox_led_init();
    timer2_init();
    timer4_init();
    adc_init();
    stepper_init();
#ifndef TESTING
    uint32_t i = 0;
    bool led = false;
#endif

#ifdef TESTING
    iox_led_on(false, false, false, true);
#endif
    iox_configure_pin(SENSOR_IN_PORT, SENSOR_IN_PIN, iox_mode_analog, 
            iox_type_pp, iox_speed_low, iox_pupd_none);
    iox_configure_pin(SENSOR_EN_PORT, SENSOR_EN_PIN, iox_mode_out,
            iox_type_pp, iox_speed_low, iox_pupd_down);
    iox_configure_pin(VALVE_ON_PORT, VALVE_ON_PIN, iox_mode_out,
            iox_type_pp, iox_speed_low, iox_pupd_down);
    iox_configure_pin(iox_port_a, 8, iox_mode_af,
            iox_type_pp, iox_speed_high, iox_pupd_none);

    while (1) {

#ifndef TESTING
        timer_reconfigure(0x7800, 0xFFFF);
        iox_set_pin_state(SENSOR_EN_PORT, SENSOR_EN_PIN, true); 
        timer2_delay(2);
        moisture[i] = adc_get_measurement();  
        iox_set_pin_state(SENSOR_EN_PORT, SENSOR_EN_PIN, false);    
        if (moisture[i] > MOIST_LEVEL) {
            /* soil is too dry */
            iox_set_pin_state(VALVE_ON_PORT, VALVE_ON_PIN, true);
            timer2_delay(5);
            iox_set_pin_state(VALVE_ON_PORT, VALVE_ON_PIN, false);
        }
        i = (i + 1) % BUFFERSIZE;
        /* timer overflows in 24hours */
        timer2_reconfigure(0xF000, HOLD_TIME);
        /* sleep until timer overflows */
        __WFI();
#endif

#ifdef TESTING
        timer2_delay(2);     /* wait 2s */
        iox_led_on(false, false, false, false);
        stepper_turn_cw(125);  /* test stepper motor, turn for 2s */
        timer2_delay(2);     /* wait 2s */
        iox_led_on(false, false, false, true);
#endif
    }
}