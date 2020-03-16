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
 * inputs.h
 * 
 * Defines values for physical input devices
 * 
 */

#include "pins.h"
#include <avdweb_AnalogReadFast.h>
#include <digitalWriteFast.h>

extern int targetDuty;  // (%)

#if defined(THROTTLE)
  extern int inputSamples;
  extern int rawDuty;   // (ADC)
#endif

void readInput();