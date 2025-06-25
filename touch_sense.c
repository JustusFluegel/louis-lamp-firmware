#include "touch_sense.h"

#include "ch32fun.h"
#include "ch32v003_touch.h"

bool touchSensorInitialized = false;

TouchSensor touchSensor(GPIO_TypeDef *io, int portpin, int adcno,
                        uint16_t iterations, uint16_t idle_val_init_count,
                        uint8_t window_size, uint16_t settle_iterations) {
  TouchSensor sensor = {.io = io,
                        .portpin = portpin,
                        .adcno = adcno,
                        .iterations = iterations,
                        .idle_val_init_count = idle_val_init_count,
                        .last_triggered_states = 0,
                        .current_state_change_systick = SysTick->CNT,
                        .window_size = window_size,
                        .idle_val = 0,
                        .current_state = false,
                        .time_since_trigger = 0,
                        .settle_iterations = settle_iterations};

  return sensor;
}

void initTouchSensor(TouchSensor *sensor) {
  if (!touchSensorInitialized) {
    touchSensorInitialized = true;
    InitTouchADC();
  }

  sensor->idle_val = 0;
  for (int i = 0; i < sensor->idle_val_init_count; i++) {
    sensor->idle_val += ReadTouchPin(sensor->io, sensor->portpin, sensor->adcno,
                                     sensor->iterations);
  }

  sensor->idle_val /= sensor->idle_val_init_count;
  sensor->current_state = false;
  sensor->last_triggered_states = 0;
}

TouchSensorReadResult readTouchSensor(TouchSensor *sensor) {
  uint32_t oversampled_val = ReadTouchPin(sensor->io, sensor->portpin,
                                          sensor->adcno, sensor->iterations);

  sensor->last_triggered_states <<= 1;

  uint32_t trigger_val = ((sensor->idle_val) * 201 / 200);
  bool is_triggered = oversampled_val > trigger_val;

  sensor->last_triggered_states |= is_triggered;

  bool current_state = sensor->current_state;
  uint32_t current_state_systick = sensor->current_state_change_systick;

  uint32_t mask = (sensor->window_size >= 32)
                      ? 0xFFFFFFFF
                      : ((1U << sensor->window_size) - 1);

  uint32_t last_state_duration = SysTick->CNT - current_state_systick;

  bool timeout_triggered = false;
  if (current_state) {
    bool timeouted = (last_state_duration / DELAY_MS_TIME) >= 1000 * 30;
    if (timeouted) {
      timeout_triggered = true;
      sensor->last_triggered_states = 0;
      sensor->time_since_trigger = sensor->settle_iterations;
    }
    if ((sensor->last_triggered_states & mask) == 0x00) {
      current_state = false;
      current_state_systick = SysTick->CNT;
    }
  } else {
    if ((sensor->last_triggered_states & mask) == mask) {
      current_state = true;
      current_state_systick = SysTick->CNT;
    }
  }

  if (current_state) {
    sensor->time_since_trigger = 0;
  }

  if (!current_state) {
    if (sensor->time_since_trigger < 65535) {
      sensor->time_since_trigger++;
    }

    if (sensor->time_since_trigger >= sensor->settle_iterations) {
      if (timeout_triggered) {
        sensor->idle_val = 0;
        for (int i = 0; i < sensor->idle_val_init_count; i++) {
          sensor->idle_val += ReadTouchPin(sensor->io, sensor->portpin,
                                           sensor->adcno, sensor->iterations);
        }
        sensor->idle_val /= sensor->idle_val_init_count;
      } else {
        sensor->idle_val =
            (sensor->idle_val * (sensor->idle_val_init_count - 1) +
             oversampled_val) /
            sensor->idle_val_init_count;
      }
    }
  }

  TouchSensorReadState state = TouchSensorReadStateUnchanged;

  if (current_state && !sensor->current_state) {
    state = TouchSensorReadStateRisingEdge;
  } else if (!current_state && sensor->current_state) {
    state = TouchSensorReadStateFallingEdge;
  }

  sensor->current_state = current_state;
  sensor->current_state_change_systick = current_state_systick;

  TouchSensorReadResult result = {.state = state,
                                  .last_state_duration = last_state_duration,
                                  .pressed = current_state

  };

  return result;
}
