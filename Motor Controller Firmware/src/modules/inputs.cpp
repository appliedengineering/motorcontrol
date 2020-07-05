/**
 * Motor Controller Firmware
 * Copyright (c) 2020 Applied Engineering
 * 
 * Based on software from various contributors.
 * Copyright (c) 2020 Andrew Berkun / Alex Liu / William Zhou
 * 
 */

/**
 * inputs.cpp
 * 
 * Manages physical input devices
 * 
 */

#include "inputs.h"

int targetDuty = 0;     // (%)
int lastTargetDuty = 0; // (%)
bool targetReached = false;

#if defined(THROTTLE)
  int inputSamples;
  int rawDuty = 0;  // (ADC)
#endif

void readInput() {
  #if defined(THROTTLE)
    // Use average of 5 samples to smooth readings.
    for (inputSamples = 0; inputSamples < 5; inputSamples++) {
      rawDuty += analogReadFast(THROTTLE);
    }
    rawDuty /= 5;
    
    // 1 V (205) is 0% and 4 V (820) is 100%.
    if (rawDuty < 205) {
      rawDuty = 205;
    } else if (rawDuty > 820) {
      rawDuty = 820;
    }

    // Integer math for (ADC - 205) * (100/615).
    targetDuty = (rawDuty - 205) * 333 / 2048;
  #elif defined(BUTTON)
    if (digitalReadFast(BUTTON) == HIGH) {
      // Treat unpressed button (HIGH) as 0% throttle.
      targetDuty = 0;
    } else {
      // Treat pressed button (LOW) as 100% throttle.
      targetDuty = 100;
    }
  #elif defined(SELFTEST)
    if (targetReached) {
      if (targetDuty == 0) {
        targetDuty = 100;
      } else if (targetDuty == 100) {
        targetDuty = 0;
      }
    }
  #endif
  if (targetDuty != lastTargetDuty) {
    targetReached = false;
  }
  lastTargetDuty = targetDuty;
}