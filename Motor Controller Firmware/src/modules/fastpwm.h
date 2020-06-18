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

extern int duty;        // (%)
extern int lastDuty;    // (%)

extern int pwmFreq;     // (Hz)
extern int pwmResolution;

// Set up nonblocking PWM update task.
#define INTERVAL_UP (RAMP_TIME_UP / pwmResolution)      // (ms)
#define INTERVAL_DOWN (RAMP_TIME_DOWN / pwmResolution)  // (ms)
extern NonBlockingTask pwmUpdate;

void setPWM();
void updateDuty();