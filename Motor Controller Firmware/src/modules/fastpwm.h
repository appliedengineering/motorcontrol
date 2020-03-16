/**
 * Motor Controller Firmware
 * Copyright (c) 2020 Applied Engineering
 * 
 * Based on software from various contributors.
 * Copyright (c) 2020 Andrew Berkun / Alex Liu / William Zhou
 * 
 */
#pragma once

/**
 * fastpwm.h
 * 
 * Configures PWM generation parameters
 * 
 */

#include "inputs.h"
#include "nonblocking.h"
#include "sense.h"

extern int duty;      // (%)
extern int lastDuty;  // (%)

#define PWM_FREQ 20000            // (Hz)
#define PWM_RESOLUTION ((F_CPU / 8) / PWM_FREQ)

// Set up nonblocking PWM update task.
#define INTERVAL_FAST (RAMP_TIME_FAST / PWM_RESOLUTION) // (ms)
#define INTERVAL_SLOW (RAMP_TIME_SLOW / PWM_RESOLUTION) // (ms)
extern NonBlockingTask pwmUpdate;

void setPWM();
void updateDuty();