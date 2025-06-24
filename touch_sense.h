#ifndef _LAMP_TOUCH_SENSE_H
#define _LAMP_TOUCH_SENSE_H

#include "ch32fun.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct TouchSensor {
  GPIO_TypeDef *io;
  int portpin;
  int adcno;
  uint16_t iterations;
  uint16_t idle_val_init_count;
  uint32_t idle_val;
  uint32_t last_triggered_states;
  uint32_t current_state_change_systick;
  uint8_t window_size;
  bool current_state;
  uint16_t time_since_trigger;
  uint16_t settle_iterations;
} TouchSensor;

extern bool touchSensorInitialized;

TouchSensor touchSensor(GPIO_TypeDef *io, int portpin, int adcno,
                        uint16_t iterations, uint16_t idle_val_init_count,
                        uint8_t window_size, uint16_t settle_iterations);

void initTouchSensor(TouchSensor *sensor);

typedef enum TouchSensorReadState {
  TouchSensorReadStateUnchanged = 0,
  TouchSensorReadStateFallingEdge = 1,
  TouchSensorReadStateRisingEdge = 2
} TouchSensorReadState;

typedef struct TouchSensorReadResult {
  TouchSensorReadState state;
  uint32_t last_state_duration;
  bool pressed;
} TouchSensorReadResult;

TouchSensorReadResult readTouchSensor(TouchSensor *sensor);
#endif
