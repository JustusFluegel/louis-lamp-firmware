#include "brightness_controller.h"
#include "ch32fun.h"
#include "config.h"

BrightnessController brightnessController(
    volatile uint32_t **control_field, int count,
    const uint16_t *brightness_step_mapping, uint16_t min_brightness_dim_on,
    uint32_t min_brightness_min_period_ms,
    uint32_t brightness_rampdown_delay_ms, uint32_t brightness_rampup_delay_ms,
    uint32_t turn_off_brightness_rampdown_delay_ms, uint16_t led_off_value) {
  BrightnessController controller = {
      .control_field = control_field,
      .last_on_time = 0,
      .last_brightness = 255,
      .is_on = false,
      .count = count,
      .brightness_step_mapping = brightness_step_mapping,
      .min_brightness_dim_on = min_brightness_dim_on,
      .min_brightness_min_period_ms = min_brightness_min_period_ms,
      .brightness_rampdown_delay_ms = brightness_rampdown_delay_ms,
      .brightness_rampup_delay_ms = brightness_rampup_delay_ms,
      .turn_off_brightness_rampdown_delay_ms =
          turn_off_brightness_rampdown_delay_ms,
      .led_off_value = led_off_value,
  };

  return controller;
}

void brightnessController_rampUpDown(BrightnessController *controller,
                                     uint8_t start_brightness,
                                     uint8_t end_brightness, uint32_t speed) {
  if (start_brightness == end_brightness) {
    for (int i = 0; i < controller->count; i++) {
      *controller->control_field[i] =
          controller->brightness_step_mapping[start_brightness];
    }

  } else if (start_brightness < end_brightness) {
    uint32_t brightness = start_brightness;
    uint32_t last_duration = SysTick->CNT;
    for (int i = 0; i < controller->count; i++) {
      *controller->control_field[i] =
          controller->brightness_step_mapping[brightness];
    }
    while (brightness < end_brightness) {
      uint32_t systick = SysTick->CNT;
      if (((systick - last_duration) / DELAY_MS_TIME) >=
          (speed == 0 ? controller->brightness_rampup_delay_ms : speed)) {
        last_duration = systick;
        brightness++;
        brightness &= 255;
        for (int i = 0; i < controller->count; i++) {
          *controller->control_field[i] =
              controller->brightness_step_mapping[brightness];
        }
      }
      Delay_Ms(1);
    }

  } else {
    uint32_t brightness = start_brightness;
    uint32_t last_duration = SysTick->CNT;
    for (int i = 0; i < controller->count; i++) {
      *controller->control_field[i] =
          controller->brightness_step_mapping[brightness];
    }
    while (brightness > end_brightness) {
      uint32_t systick = SysTick->CNT;
      if (((systick - last_duration) / DELAY_MS_TIME) >=
          (speed == 0 ? controller->brightness_rampdown_delay_ms : speed)) {
        last_duration = systick;
        brightness--;
        brightness &= 255;
        for (int i = 0; i < controller->count; i++) {
          *controller->control_field[i] =
              controller->brightness_step_mapping[brightness];
        }
      }
      Delay_Ms(1);
    }
  }
}

void brightnessController_set(BrightnessController *controller,
                              uint8_t target_brightness) {
  if (target_brightness < controller->min_brightness_dim_on &&
      (SysTick->CNT - controller->last_on_time) / DELAY_MS_TIME >
          controller->min_brightness_min_period_ms &&
      !controller->is_on) {
    uint32_t brightness = controller->min_brightness_dim_on;
    for (int i = 0; i < controller->count; i++) {
      *controller->control_field[i] =
          controller->brightness_step_mapping[brightness];
    }
    Delay_Ms(50);
    brightnessController_rampUpDown(controller, brightness, target_brightness,
                                    0);
  } else {
    for (int i = 0; i < controller->count; i++) {
      *controller->control_field[i] =
          controller->brightness_step_mapping[target_brightness];
    }
  }
  controller->last_brightness = target_brightness;
  controller->is_on = true;
  controller->last_on_time = SysTick->CNT;
}

void brightnessController_on(BrightnessController *controller) {
  brightnessController_rampUpDown(controller, controller->min_brightness_dim_on,
                                  controller->last_brightness, 2);
  controller->is_on = true;
  controller->last_on_time = SysTick->CNT;
}

void brightnessController_off(BrightnessController *controller) {
  brightnessController_rampUpDown(
      controller, controller->last_brightness, 10,
      controller->turn_off_brightness_rampdown_delay_ms);

  while (led_off_value - *controller->control_field[0] > 10) {
    for (int i = 0; i < controller->count; i++) {
      *controller->control_field[i] +=
          (led_off_value - *controller->control_field[i]) / 2;
    }
    Delay_Ms(controller->turn_off_brightness_rampdown_delay_ms);
  }

  controller->is_on = false;
  controller->last_on_time = SysTick->CNT;
}

void brightnessController_toggle(BrightnessController *controller) {
  if (controller->is_on) {
    brightnessController_off(controller);
  } else {
    brightnessController_on(controller);
  }
}
