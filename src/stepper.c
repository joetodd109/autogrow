/**
 ******************************************************************************
 * @file    stepper.c
 * @author  Joe Todd
 * @version
 * @date    March 2015
 * @brief   Autogrow
 *			Drive BYJ48 Stepper Motor to control water flow.
 *
  ******************************************************************************/
#include "stepper.h"
#include "iox.h"
#include "timer.h"

#define ORANGE_PORT		iox_port_b
#define ORANGE_PIN		3u
#define YELLOW_PORT		iox_port_b
#define YELLOW_PIN		4u
#define PINK_PORT		iox_port_b
#define PINK_PIN		5u
#define BLUE_PORT		iox_port_b
#define BLUE_PIN		6u

#define NUM_STATES		8u

typedef struct {
	bool org;
	bool yel;
	bool pnk;
	bool blu;
} stepper_st_t;

typedef enum {
	dir_cw,
	dir_acw,
} stepper_dir_t;

/* 
 * 1/2 phase CW states.
 */
static stepper_st_t stepper_st[NUM_STATES] = {
/*   org,    yel,    pnk,    blu    */
	{true,  false,  false,  false},	 /* 1 */
	{true,  true,   false,  false},	 /* 2 */
	{false, true,   false,  false},	 /* 3 */
	{false, true,   true,   false},	 /* 4 */
	{false, false,  true,   false},	 /* 5 */
	{false, false,  true,   true },	 /* 6 */
	{false, false,  false,  true },	 /* 7 */
	{true,  false,  false,  true },	 /* 8 */
};

static void stepper_reset(void);
static void stepper_do_turns(stepper_dir_t dir);

extern void
stepper_init(void)
{	
	iox_configure_pin(ORANGE_PORT, ORANGE_PIN, iox_mode_out,
						iox_type_pp, iox_speed_fast, iox_pupd_down);
	iox_configure_pin(YELLOW_PORT, YELLOW_PIN, iox_mode_out,
						iox_type_pp, iox_speed_fast, iox_pupd_down);
	iox_configure_pin(PINK_PORT, PINK_PIN, iox_mode_out,
						iox_type_pp, iox_speed_fast, iox_pupd_down);
	iox_configure_pin(BLUE_PORT, BLUE_PIN, iox_mode_out,
						iox_type_pp, iox_speed_fast, iox_pupd_down);

	stepper_reset();
}

static void
stepper_reset(void)
{
	iox_set_pin_state(ORANGE_PORT, ORANGE_PIN, false);
	iox_set_pin_state(YELLOW_PORT, YELLOW_PIN, false);
	iox_set_pin_state(PINK_PORT, PINK_PIN, false);
	iox_set_pin_state(BLUE_PORT, BLUE_PIN, false);
}

extern void
stepper_turn_cw(uint8_t turns)
{
	uint8_t i;

	stepper_reset();

	for (i = 0; i < turns; i++) {
		stepper_do_turns(dir_cw);
	}
}

extern void
stepper_turn_acw(uint8_t turns)
{
	uint8_t i;

	stepper_reset();

	for (i = 0; i < turns; i++) {
		stepper_do_turns(dir_acw);
	}
}

static void
stepper_do_turns(stepper_dir_t dir) 
{
	uint8_t i, j, s;

	/* 1/2 phase control */
	for (j = 0; j < 2; j++) {
		/* initialise state machine */
		s = ((dir == dir_cw) ? 0 : NUM_STATES - 1);

		for (i = 0; i < NUM_STATES; i++) {
			iox_set_pin_state(ORANGE_PORT, ORANGE_PIN, stepper_st[s].org);
			iox_set_pin_state(YELLOW_PORT, YELLOW_PIN, stepper_st[s].yel);
			iox_set_pin_state(PINK_PORT, PINK_PIN, stepper_st[s].pnk);
			iox_set_pin_state(BLUE_PORT, BLUE_PIN, stepper_st[s].blu);

			timer4_delay(1000u);
			(dir == dir_cw) ? s++ : s--;
		}
	}

	stepper_reset();
}
