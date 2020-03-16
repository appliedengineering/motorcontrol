/**
 * Motor Controller Firmware
 * Copyright (c) 2020 Applied Engineering
 * 
 * Based on software from various contributors.
 * Copyright (c) 2020 Andrew Berkun / Alex Liu / William Zhou
 * 
 */

/**
 * sense.cpp
 * 
 * Detects electrical operating conditions.
 * 
 */

#include "../Configuration.h"
#include "sense.h"

int senseSamples;

// Modified Moving Average
int movAvgCurrent, movAvgCurrentSum;
int avgCount = 100;
int current;  // (ADC)
int voltage;  // (ADC)

#if defined(MINIMUM_DUTY_DETECTION)
  int zeroCurrent;  // (ADC)
#endif

// Sense current every millisecond.
NonBlockingTask iSenseUpdate(1);
// Sense voltage every 10 milliseconds.
NonBlockingTask vSenseUpdate(10);

void preloadISenseMMA() {
  for (senseSamples = 0; senseSamples < avgCount; senseSamples++) {
    movAvgCurrentSum += analogReadFast(IS_1);
  }
  movAvgCurrent = movAvgCurrentSum / avgCount;
}

void senseCurrent() {
  for (senseSamples = 0; senseSamples < avgCount; senseSamples++) {
    movAvgCurrentSum -= movAvgCurrent;
    movAvgCurrentSum += analogReadFast(IS_1);
    movAvgCurrent = movAvgCurrentSum / avgCount;
  }
  #if defined(MINIMUM_DUTY_DETECTION)
    current = movAvgCurrent - zeroCurrent;
  #else
    current = movAvgCurrent;
  #endif
}

void senseVoltage() {
  for (senseSamples = 0; senseSamples < 5; senseSamples++) {
    voltage += analogReadFast(VBAT);
  }
  voltage /= 5;
}

#if defined(MINIMUM_DUTY_DETECTION)
  void senseZeroCurrent() {
    for (senseSamples = 0; senseSamples < 5; senseSamples++) {
      zeroCurrent += analogReadFast(IS_1);
    }
    zeroCurrent /= 5;
  }
#endif