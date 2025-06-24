
#include "ch32fun.h"
#include "ch32v003hw.h"
#include "config.h"
#include "touch_sense.h"

#include <inttypes.h>
#include <stdbool.h>

void write_led(bool on) {
  if (on && debug_led_enabled) {
    GPIOC->BSHR = (1 << 3);
    // GPIOD->BSHR = (1 << 2);
    // GPIOD->BSHR = (1 << 3);
  } else {
    GPIOC->BSHR = (1 << (16 + 3));
    // GPIOD->BSHR = (1 << (16 + 2));
    // GPIOD->BSHR = (1 << (16 + 3));
  }
}

typedef struct BrightnessController {
  uint32_t last_on_time;
  uint32_t last_brightness;
  bool is_on;
  volatile uint32_t *control_field;
} BrightnessController;

BrightnessController brightnessController(volatile uint32_t *control_field) {
  BrightnessController controller = {.control_field = control_field,
                                     .last_on_time = 0,
                                     .last_brightness = 255,
                                     .is_on = false};

  return controller;
}

void ramp_up_down_brightness(BrightnessController *controller,
                             uint8_t start_brightness, uint8_t end_brightness,
                             uint32_t speed) {
  if (start_brightness == end_brightness) {
    TIM1->CH1CVR = brightness_steps[start_brightness];

  } else if (start_brightness < end_brightness) {
    uint32_t brightness = start_brightness;
    uint32_t last_duration = SysTick->CNT;
    *controller->control_field = brightness_steps[brightness];
    while (brightness < end_brightness) {
      uint32_t systick = SysTick->CNT;
      if (((systick - last_duration) / DELAY_MS_TIME) >=
          (speed == 0 ? brightness_rampup_delay_ms : speed)) {
        last_duration = systick;
        brightness++;
        brightness &= 255;
        *controller->control_field = brightness_steps[brightness];
      }
      Delay_Ms(1);
    }

  } else {
    uint32_t brightness = start_brightness;
    uint32_t last_duration = SysTick->CNT;
    *controller->control_field = brightness_steps[brightness];
    while (brightness > end_brightness) {
      uint32_t systick = SysTick->CNT;
      if (((systick - last_duration) / DELAY_MS_TIME) >=
          (speed == 0 ? brightness_rampdown_delay_ms : speed)) {
        last_duration = systick;
        brightness--;
        brightness &= 255;
        *controller->control_field = brightness_steps[brightness];
      }
      Delay_Ms(1);
    }
  }
}

void set_brightness(BrightnessController *controller,
                    uint8_t target_brightness) {
  if (target_brightness < min_brightness_dim_on &&
      (SysTick->CNT - controller->last_on_time) / DELAY_MS_TIME >
          min_brightness_min_period &&
      !controller->is_on) {
    uint32_t brightness = min_brightness_dim_on;
    *controller->control_field = brightness_steps[brightness];
    Delay_Ms(50);
    ramp_up_down_brightness(controller, brightness, target_brightness, 0);
  } else {
    *controller->control_field = brightness_steps[target_brightness];
  }
  controller->last_brightness = target_brightness;
  controller->is_on = true;
  controller->last_on_time = SysTick->CNT;
}

void turn_on(BrightnessController *controller) {
  ramp_up_down_brightness(controller, min_brightness_dim_on,
                          controller->last_brightness, 0);
  controller->is_on = true;
  controller->last_on_time = SysTick->CNT;
}

void turn_off(BrightnessController *controller) {
  ramp_up_down_brightness(controller, controller->last_brightness, 0, 0);
  *controller->control_field = led_off_value;

  controller->is_on = false;
  controller->last_on_time = SysTick->CNT;
}

void toggle(BrightnessController *controller) {
  if (controller->is_on) {
    turn_off(controller);
  } else {
    turn_on(controller);
  }
}

int main() {
  SystemInit();

  RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1 |
                    RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD |
                    RCC_APB2Periph_TIM1;

  GPIOC->CFGLR &= ~(0xf << (4 * 3));
  GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP) << (4 * 3);
  // LED1 is TIM1 Channel 1
  GPIOD->CFGLR &= ~(0xf << (4 * 2));
  GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF) << (4 * 2);
  GPIOD->CFGLR &= ~(0xf << (4 * 3));
  GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP) << (4 * 3);

  // Reset timer 1
  RCC->APB2PRSTR |= RCC_APB2Periph_TIM1;
  RCC->APB2PRSTR &= ~RCC_APB2Periph_TIM1;

  TIM1->PSC = timer_prescale;
  TIM1->ATRLR = led_off_value;

  TIM1->SWEVGR |= TIM_UG;

  // Enable Timer1 Channel1 Output
  TIM1->CCER |= TIM_CC1E | TIM_CC1P;
  TIM1->CHCTLR1 |= TIM_OC1M_2 | TIM_OC1M_1;

  TIM1->CH1CVR = led_off_value;

  TIM1->BDTR |= TIM_MOE;
  TIM1->CTLR1 |= TIM_CEN;

  TouchSensor sensor =
      touchSensor(GPIOA, 2, 0, touch_oversampling_iterations,
                  touch_turn_on_calibration_count, touch_hysteresis_window,
                  touch_recalibrate_settle_iterations);
  initTouchSensor(&sensor);

  BrightnessController controller = brightnessController(&TIM1->CH1CVR);

  if (led_blink_on_on) {
    turn_on(&controller);
    write_led(true);
    Delay_Ms(1000);
    write_led(false);
    turn_off(&controller);
    Delay_Ms(1000);
  }

  uint8_t brightness = turn_on_brightness;
  set_brightness(&controller, brightness);

  uint32_t last_duration = 0;
  bool brightness_ramp_started = false;

  bool brightness_ramp_direction = brightness != 255;
  for (;;) {
    TouchSensorReadResult result = readTouchSensor(&sensor);

    if (result.state == TouchSensorReadStateFallingEdge) {
      if (!brightness_ramp_started) {
        toggle(&controller);
      }
      brightness_ramp_started = false;
    }
    // printf("%d \n", result.last_state_duration / DELAY_MS_TIME);

    uint32_t systick = SysTick->CNT;
    if (controller.is_on &&
        (brightness_ramp_started ||
         result.last_state_duration >=
             (DELAY_MS_TIME * single_touch_duration_ms)) &&
        result.pressed &&
        ((systick - last_duration) / DELAY_MS_TIME) >=
            (brightness_ramp_direction ? brightness_touch_rampup_delay_ms
                                       : brightness_touch_rampdown_delay_ms)) {
      brightness_ramp_started = true;
      last_duration = systick;

      if (brightness_ramp_direction) {
        brightness++;
        if (brightness == 255) {
          brightness_ramp_direction = false;
        }
      } else {
        brightness--;
        if (brightness == 0) {
          brightness_ramp_direction = true;
        }
      }
      set_brightness(&controller, brightness);
    }
    write_led(controller.is_on);
  }
}
