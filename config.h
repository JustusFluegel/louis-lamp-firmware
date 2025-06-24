#ifndef __TEST_FIRMWARE_CONFIG_H
#define __TEST_FIRMWARE_CONFIG_H

#include <stdbool.h>
#include <stdint.h>
static const uint16_t brightness_steps[256] = {
    16010, 16010, 16010, 16010, 16010, 16009, 16009, 16008, 16007, 16006, 16005,
    16004, 16002, 16001, 15999, 15997, 15994, 15992, 15989, 15986, 15982, 15979,
    15975, 15971, 15966, 15962, 15957, 15952, 15946, 15940, 15934, 15928, 15921,
    15914, 15906, 15898, 15890, 15882, 15873, 15864, 15854, 15844, 15834, 15823,
    15812, 15801, 15789, 15777, 15764, 15751, 15737, 15724, 15709, 15695, 15680,
    15664, 15648, 15632, 15615, 15598, 15580, 15562, 15543, 15524, 15505, 15485,
    15464, 15443, 15422, 15400, 15378, 15355, 15332, 15308, 15284, 15259, 15234,
    15208, 15182, 15155, 15127, 15100, 15071, 15042, 15013, 14983, 14952, 14921,
    14890, 14858, 14825, 14792, 14758, 14724, 14689, 14654, 14618, 14581, 14544,
    14506, 14468, 14429, 14390, 14350, 14309, 14268, 14226, 14184, 14141, 14097,
    14053, 14009, 13963, 13917, 13871, 13823, 13775, 13727, 13678, 13628, 13578,
    13527, 13475, 13423, 13370, 13317, 13262, 13207, 13152, 13096, 13039, 12982,
    12923, 12865, 12805, 12745, 12684, 12623, 12561, 12498, 12434, 12370, 12305,
    12240, 12173, 12106, 12039, 11970, 11901, 11832, 11761, 11690, 11618, 11546,
    11472, 11398, 11323, 11248, 11172, 11095, 11017, 10939, 10860, 10780, 10699,
    10618, 10536, 10453, 10370, 10285, 10200, 10114, 10028, 9940,  9852,  9764,
    9674,  9584,  9492,  9400,  9308,  9214,  9120,  9025,  8929,  8833,  8735,
    8637,  8538,  8438,  8338,  8236,  8134,  8031,  7928,  7823,  7718,  7611,
    7504,  7397,  7288,  7179,  7068,  6957,  6845,  6733,  6619,  6505,  6389,
    6273,  6157,  6039,  5920,  5801,  5681,  5560,  5438,  5315,  5191,  5067,
    4941,  4815,  4688,  4560,  4431,  4302,  4171,  4040,  3907,  3774,  3640,
    3505,  3370,  3233,  3095,  2957,  2818,  2677,  2536,  2394,  2252,  2108,
    1963,  1818,  1671,  1524,  1375,  1226,  1076,  925,   773,   620,   467,
    312,   156,   0};

static const uint16_t min_brightness_dim_on = 150;
static const uint32_t min_brightness_min_period = 250;
static const uint32_t brightness_rampdown_delay_ms = 7;
static const uint32_t brightness_rampup_delay_ms = 7;
static const uint32_t brightness_touch_rampup_delay_ms = 15;
static const uint32_t brightness_touch_rampdown_delay_ms = 15;
static const uint32_t single_touch_duration_ms = 250;
static const uint16_t turn_on_brightness = 255;
static const bool led_blink_on_on = false;
static const bool debug_led_enabled = false;

// WARNING: The following settings are advanced and should probably be left at
// their defaults
static const uint16_t timer_prescale = 48;
static const uint16_t led_off_value = 16383;
static const uint16_t touch_oversampling_iterations = 3000;
static const uint8_t touch_hysteresis_window = 3;
static const uint16_t touch_turn_on_calibration_count = 25;
static const uint16_t touch_recalibrate_settle_iterations = 1000;
#endif
