
#include "brightness_controller.h"
#include "ch32fun.h"
#include "ch32v003hw.h"
#include "config.h"
#include "touch_sense.h"

// #include <inttypes.h>
#include <stdbool.h>

void write_led(bool on) {
  if (on && debug_led_enabled) {
    GPIOC->BSHR = (1 << 3);
  } else {
    GPIOC->BSHR = (1 << (16 + 3));
  }
}

void setup_hw() {
  RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1 |
                    RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1 |
                    RCC_APB2Periph_AFIO;

  RCC->APB1PCENR |= RCC_APB1Periph_TIM2;

  AFIO->PCFR1 |= AFIO_PCFR1_TIM1_REMAP_PARTIALREMAP1;

  GPIOC->CFGLR &= ~(0xf << (4 * 3));
  GPIOC->CFGLR |= (GPIO_Speed_50MHz | GPIO_CNF_OUT_PP) << (4 * 3);

  // LED1 is TIM2 Channel 3 PC0
  GPIOC->CFGLR &= ~(0xf << (4 * 0));
  GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF) << (4 * 0);

  // LED2 is TIM1 Channel 2 PC7
  GPIOC->CFGLR &= ~(0xf << (4 * 7));
  GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF) << (4 * 7);

  // Reset timer 1
  RCC->APB2PRSTR |= RCC_APB2Periph_TIM1;
  RCC->APB2PRSTR &= ~RCC_APB2Periph_TIM1;

  // Reset timer 2
  RCC->APB1PRSTR |= RCC_APB1Periph_TIM2;
  RCC->APB1PRSTR &= ~RCC_APB1Periph_TIM2;

  TIM1->PSC = timer_prescale;
  TIM2->PSC = timer_prescale;

  TIM1->ATRLR = led_off_value;
  TIM2->ATRLR = led_off_value;

  TIM1->SWEVGR |= TIM_UG;
  TIM2->SWEVGR |= TIM_UG;

  // Enable Timer1 Channel2 Output
  TIM1->CCER |= TIM_CC2E | TIM_CC2P;
  TIM1->CCER |= TIM_CC2E;
  TIM1->CHCTLR1 |= TIM_OC2M_2 | TIM_OC2M_1;

  // Enable Timer2 Channel3 Output
  TIM2->CCER |= TIM_CC3E | TIM_CC3P;
  // TIM2->CCER |= TIM_CC3E;
  TIM2->CHCTLR2 |= TIM_OC3M_2 | TIM_OC3M_1;

  TIM1->CH2CVR = led_off_value;
  TIM2->CH3CVR = led_off_value;

  TIM1->BDTR |= TIM_MOE;
  TIM2->BDTR |= TIM_MOE;

  TIM1->CTLR1 |= TIM_CEN;
  TIM2->CTLR1 |= TIM_CEN;
}

int main() {
  SystemInit();
  setup_hw();

  TouchSensor sensor =
      touchSensor(GPIOA, 2, 0, touch_oversampling_iterations,
                  touch_turn_on_calibration_count, touch_hysteresis_window,
                  touch_recalibrate_settle_iterations);

  volatile uint32_t *timers[2] = {&TIM2->CH3CVR, &TIM1->CH2CVR};

  BrightnessController controller = brightnessController(
      timers, 2, brightness_steps, min_brightness_dim_on,
      min_brightness_min_period_ms, brightness_rampdown_delay_ms,
      brightness_rampup_delay_ms, turn_off_brightness_rampdown_delay_ms,
      led_off_value);

  initTouchSensor(&sensor);

  if (led_blink_on_on) {
    write_led(true);
    brightnessController_on(&controller);
    Delay_Ms(1000);
    write_led(false);
    brightnessController_off(&controller);
    Delay_Ms(1000);
  }

  uint8_t brightness = turn_on_brightness;
  brightnessController_set(&controller, brightness);

  uint32_t last_duration = 0;
  bool brightness_ramp_started = false;

  bool brightness_ramp_direction = brightness != 255;
  for (;;) {
    TouchSensorReadResult result = readTouchSensor(&sensor);

    if (result.state == TouchSensorReadStateFallingEdge) {
      if (!brightness_ramp_started) {
        brightnessController_toggle(&controller);
      }
      brightness_ramp_started = false;
    }

    uint32_t systick = SysTick->CNT;
    if (((controller.is_on &&
          (brightness_ramp_started ||
           result.last_state_duration >=
               (DELAY_MS_TIME * single_touch_duration_ms)) &&
          result.pressed) ||
         test_mode) &&
        ((systick - last_duration) / DELAY_MS_TIME) >=
            (((brightness_ramp_direction
                   ? brightness_touch_rampup_delay_ms
                   : brightness_touch_rampdown_delay_ms)) /
             ((int)test_mode + 1))) {
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
      brightnessController_set(&controller, brightness);
    }
    write_led(result.pressed);
  }
}
