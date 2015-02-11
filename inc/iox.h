/**
  ******************************************************************************
  * @file    iox.h 
  * @author  Joe Todd
  * @version 
  * @date    
  * @brief   Header for iox.c
  *
  ******************************************************************************
*/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IOX_H
#define IOX_H

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"
#include "stm32f4xx.h"


/* Global Defines ----------------------------------------------------------- */

/* 
 * Pin definitions 
 */
#define PIN0    0
#define PIN1    1
#define PIN2    2
#define PIN3    3
#define PIN4    4
#define PIN5    5
#define PIN6    6
#define PIN7    7
#define PIN8    8
#define PIN9    9
#define PIN10   10
#define PIN11   11
#define PIN12   12
#define PIN13   13
#define PIN14   14
#define PIN15   15

/* Alternate function defintions */
#define AF0     0
#define AF1     1
#define AF2     2
#define AF3     3
#define AF4     4
#define AF5     5
#define AF6     6
#define AF7     7
#define AF8     8
#define AF9     9
#define AF10    10
#define AF11    11
#define AF12    12
#define AF13    13
#define AF14    14
#define AF15    15


/**
 * Macro to get the pin state.
 *
 * The compiler does a good job on this, so it is worth using a macro.
 */
#define iox_get_pin_state(port, pin) \
    ((iox_gpios[port]->IDR & (1u << (pin))) ? true : false)

/**
 * Macro to set the pin state.
 *
 * The compiler does a good job on this, so it is worth using a macro.
 * @notes:  upgrade to atomic write.
 */
#define iox_set_pin_state(port, pin, state) \
    ((state) ? (iox_gpios[port]->ODR |= (1u << (pin))) \
        : (iox_gpios[port]->ODR &= ~(1u << (pin))))

typedef enum {
    iox_port_a,
    iox_port_b,
    iox_port_c,
    iox_port_d,
    iox_port_e,
} iox_port_t;

typedef struct iox_port_pin_t iox_port_pin_t;

struct iox_port_pin_t {
    iox_port_t port;
    uint8_t pin;
};

typedef enum {
    iox_mode_in,
    iox_mode_out,
    iox_mode_af,
    iox_mode_analog,
} iox_mode_t;

typedef enum {
    iox_type_pp,
    iox_type_od,
} iox_type_t;

typedef enum {
    iox_speed_low,
    iox_speed_med,
    iox_speed_fast,
    iox_speed_high,
} iox_speed_t;

typedef enum {
    iox_pupd_none,
    iox_pupd_up,
    iox_pupd_down,
} iox_pupd_t;


/*
 * Used by the macros above.  
 */
extern GPIO_TypeDef *const iox_gpios[];

/**
 * Configure a port pin.
 */
extern void iox_configure_pin(iox_port_t port, uint32_t pin,
                            iox_mode_t mode, iox_type_t type, 
                            iox_speed_t speed, iox_pupd_t pupd);

/*
 * Configure a pin for an alternate function
 */
extern void iox_alternate_func(iox_port_t port, uint32_t pin,
                                uint32_t af);

/**
 * Change an output pin state.
 */
extern void (iox_set_pin_state) (iox_port_t port, uint32_t pin,
                                 bool state);

/**
 * Read the state of an input pin.
 */
extern bool(iox_get_pin_state) (iox_port_t port, uint32_t pin);

/* 
 * Sets up GPIO's for LED's
 */
extern void iox_led_init(void);

/**
 * Turn on an LED
 */
extern void iox_led_on(bool green, bool amber, bool red, bool blue);

/**
 * Turn off all LEDs
 */
extern void iox_leds_off(void);


#endif