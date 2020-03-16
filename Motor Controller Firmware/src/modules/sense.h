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
 * sense.h
 * 
 * Detects electrical operating conditions.
 * 
 */

#include "inputs.h"
#include "nonblocking.h"

extern int senseSamples;

// Modified Moving Average
extern int movAvgCurrent, movAvgCurrentSum;
extern int avgCount;
extern int current;       // (ADC)
extern int voltage;       // (ADC)

#if defined(MINIMUM_DUTY_DETECTION)
  extern int zeroCurrent; // (ADC)
#endif

extern NonBlockingTask iSenseUpdate;
extern NonBlockingTask vSenseUpdate;

void preloadISenseMMA();
void senseCurrent();
void senseVoltage();

#if defined(MINIMUM_DUTY_DETECTION)
  void senseZeroCurrent();
#endif