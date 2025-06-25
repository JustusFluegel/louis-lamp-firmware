#ifndef __TEST_FIRMWARE_CONFIG_H
#define __TEST_FIRMWARE_CONFIG_H

#include <stdbool.h>
#include <stdint.h>

// Simple python script to calculate the below table according to idk how its called plz look up Stevens' power law
//
// steps = 256 
// min_pwm = 0       # fully on max_pwm = 16010   # fully off
// exponent = 0.4
// table = []
// for x in range(steps):
//     norm = x / (steps - 1)            # normalize 0..1
//     val = norm ** (1 / exponent)      # apply Stevens' power law
//     pwm_val = round(max_pwm - val * (max_pwm - min_pwm))  # invert and scale
//     table.append(pwm_val)
// print(table)

// Controlls the brightness ramp. 0 = dimmest possible on state, 255 = fully on.
// Code will linearly go though this table when changing brightness.
static const uint16_t brightness_steps[256] = {16010, 16010, 16010, 16010, 16010, 16010, 16010, 16010, 16010, 16010, 16010, 16010, 16009, 16009, 16009, 16009, 16008, 16008, 16008, 16007, 16007, 16006, 16005, 16005, 16004, 16003, 16002, 16001, 16000, 15999, 15997, 15996, 15994, 15992, 15991, 15989, 15987, 15984, 15982, 15979, 15977, 15974, 15971, 15968, 15964, 15961, 15957, 15953, 15949, 15944, 15940, 15935, 15930, 15925, 15919, 15914, 15908, 15901, 15895, 15888, 15881, 15874, 15866, 15859, 15850, 15842, 15833, 15824, 15815, 15805, 15795, 15784, 15774, 15762, 15751, 15739, 15727, 15714, 15701, 15688, 15674, 15660, 15645, 15630, 15615, 15599, 15583, 15566, 15548, 15531, 15513, 15494, 15475, 15455, 15435, 15414, 15393, 15371, 15349, 15327, 15303, 15279, 15255, 15230, 15205, 15178, 15152, 15124, 15097, 15068, 15039, 15009, 14979, 14948, 14916, 14884, 14851, 14817, 14783, 14748, 14712, 14676, 14639, 14601, 14562, 14523, 14483, 14442, 14401, 14358, 14315, 14272, 14227, 14181, 14135, 14088, 14040, 13992, 13942, 13892, 13841, 13788, 13736, 13682, 13627, 13571, 13515, 13457, 13399, 13340, 13280, 13218, 13156, 13093, 13029, 12964, 12898, 12831, 12763, 12694, 12624, 12553, 12481, 12408, 12334, 12259, 12182, 12105, 12026, 11947, 11866, 11784, 11701, 11617, 11532, 11446, 11358, 11269, 11180, 11088, 10996, 10903, 10808, 10712, 10615, 10517, 10417, 10316, 10214, 10111, 10006, 9900, 9793, 9684, 9574, 9463, 9350, 9237, 9121, 9005, 8887, 8767, 8646, 8524, 8400, 8275, 8149, 8021, 7892, 7761, 7629, 7495, 7359, 7223, 7084, 6945, 6803, 6660, 6516, 6370, 6223, 6074, 5923, 5771, 5617, 5461, 5304, 5145, 4985, 4823, 4659, 4494, 4327, 4158, 3988, 3816, 3642, 3467, 3289, 3110, 2929, 2747, 2562, 2376, 2188, 1999, 1807, 1614, 1419, 1222, 1023, 822, 619, 415, 208, 0};

// The minimum index from the above table, in the range 0..=255, for which the
// led will turn on from a fully off state
static const uint16_t min_brightness_dim_on = 150;
// The minimum period in milliseconds for the led to be off to be considered
// "fully off" (startup procedure is not done during this duration)
static const uint32_t min_brightness_min_period_ms = 250;
// The delay between steps of automatic rampdown, range 1..uint32_t::MAX_VALUE.
// Smaller = rampdown faster, larger = rampdown slower
static const uint32_t brightness_rampdown_delay_ms = 7;
// The delay between steps of automatic rampup, range 1..uint32_t::MAX_VALUE.
// Smaller = rampup faster, larger = rampup slower
static const uint32_t brightness_rampup_delay_ms = 7;
// The delay between steps of rampdown when user is pressing the touch button,
// range 1..uint32_t::MAX_VALUE. Smaller = rampdown faster, larger = rampdown
// slower
static const uint32_t brightness_touch_rampup_delay_ms = 11;
// The delay between steps of rampup when user is pressing the touch button,
// range 1..uint32_t::MAX_VALUE. Smaller = rampup faster, larger = rampup
// slower
static const uint32_t brightness_touch_rampdown_delay_ms = 11;
static const uint32_t turn_off_brightness_rampdown_delay_ms = 3;
// Duration after which the press is considered a long press and the brightness
// change starts.
static const uint32_t single_touch_duration_ms = 250;
// Initial brightness value on power-on, 0..=255, index to the brightness table.
static const uint16_t turn_on_brightness = 255;
// Flash the lights once with full brightness when powering up the lamp
static const bool led_blink_on_on = false;
// Enables test mode. Test mode will ramp brightness all the time even without
// touch input and will turn on the debug led, when enabled, when the touch
// sensor is pressed.
static const bool test_mode = false;

// Enable or disable the red debug led. Highly recommended to enable for test
// mode.
static const bool debug_led_enabled = test_mode || false;

// WARNING: The following settings are advanced and should probably be left at
// their defaults

// Timer prescaler. Changes PWM frequency. 48 in combination with a
// led_off_value of 16383 is around 60 Hz.
static const uint16_t timer_prescale = 0;
// Timer reset point / max led value (off) for the brightness table. Will
// affect PWM frequency. Highly suggested to leave as-is.
static const uint16_t led_off_value = 16383;
// Iterations of oversampling when meassuring touch inputs. Needs to be
// determined through experimentation. Higher = better accuracy, lower = faster
// touch response.
static const uint16_t touch_oversampling_iterations = 3000;
// Touch hysteresis: how many touch messurements need to be on for the sensor to
// be considered pressed, how many need to be off for the sensor to be
// considered depressed?
//
// Hysteresis, because if for example the value is 3 the
// messurements 111 will turn on the sensor, but 101 and 100 and 001 won't
// change the sensor to off again, only 000 does. Once it is off then, the
// values 101, 100, 011, ... won't change the sensor to on again, until a 111 is
// encountered.
//
// Has a multiplicative effect with the previous value for touch responsiveness.
// Needs to be determined through experimentation.
static const uint8_t touch_hysteresis_window = 3;
// How many messurements to take to determine the idle value of the touch sensor
// when initialized. Deviations from the idle value to the top will be taken as
// touch inputs, so a good idle value is critical.
static const uint16_t touch_turn_on_calibration_count = 25;
// How many iterations to wait after the touch sensor is depressed to start
// re-calibrating the idle value automatically.
static const uint16_t touch_recalibrate_settle_iterations = 1000;
#endif
