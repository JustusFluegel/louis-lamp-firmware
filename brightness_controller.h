#ifndef _LAMP_BRIGHTNESS_CONTROLLER_H
#define _LAMP_BRIGHTNESS_CONTROLLER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct BrightnessController {
  uint32_t last_on_time;
  uint32_t last_brightness;
  bool is_on;
  volatile uint32_t **control_field;
  int count;
  const uint16_t *brightness_step_mapping;
  uint16_t min_brightness_dim_on;
  uint32_t min_brightness_min_period_ms;
  uint32_t brightness_rampdown_delay_ms;
  uint32_t brightness_rampup_delay_ms;
  uint32_t turn_off_brightness_rampdown_delay_ms;
  uint16_t led_off_value;

} BrightnessController;

BrightnessController brightnessController(
    volatile uint32_t **control_field, int count,
    const uint16_t *brightness_step_mapping, uint16_t min_brightness_dim_on,
    uint32_t min_brightness_min_period_ms,
    uint32_t brightness_rampdown_delay_ms, uint32_t brightness_rampup_delay_ms,
    uint32_t turn_off_brightness_rampdown_delay_ms, uint16_t led_off_value

);

void brightnessController_rampUpDown(BrightnessController *controller,
                                     uint8_t start_brightness,
                                     uint8_t end_brightness, uint32_t speed);

void brightnessController_set(BrightnessController *controller,
                              uint8_t target_brightness);

void brightnessController_on(BrightnessController *controller);

void brightnessController_off(BrightnessController *controller);

void brightnessController_toggle(BrightnessController *controller);

#endif
