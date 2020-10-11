/**
 * Motor Controller Firmware
 * Copyright (c) 2020 Applied Engineering
 * 
 * Based on software from various contributors.
 * Copyright (c) 2020 Andrew Berkun / Alex Liu / William Zhou
 * 
 */

/**
 * fastpwm.cpp
 * 
 * Controls PWM signal output
 * 
 */

#include "fastpwm.h"

int duty = 0;     // (%)
int lastDuty = 0; // (%)
int throttleDuty = 0; // %
int lastThrottleDuty = 0; // %

// Configure PWM Zones.
// pwmConfig pwmZones = {percentMax, prescale, resolution, multiplier};
pwmConfig pwmZones[7] = {{1, 64, 200, 2},   // (0-1), (99-100)  %
                         {3, 32, 200, 2},   // (2-3), (97-98)   %
                         {7, 32, 100, 1},   // (4-7), (93-96)   %
                         {13, 8, 200, 2},   // (8-13), (87-92)  %
                         {14, 8, 150, 1.5}, // (14), (86)       %
                         {15, 8, 120, 1.2}, // (15), (85)       %
                         {50, 8, 100, 1}};  // (16-50), (50-84) %
int zoneIndex = 0;
int cpuPrescaler = 64;
int pwmResolution = 200;
float dutyMultiplier = 2;
int dutyZone;
int lastDutyZone;

// Set up nonblocking PWM update task.
NonBlockingTask pwmUpdate(INTERVAL_DOWN);

//===========================================================================
//============================= Update PWM Pin ==============================
//===========================================================================

// Returns index of match in pwmZones.
int pwmCheckZone(int dutyCyclePercent) {
  if (dutyCyclePercent > 50) {
    dutyCyclePercent = 100 - dutyCyclePercent;
  }
  for (zoneIndex = 0; zoneIndex < 6; zoneIndex++) {
    if (dutyCyclePercent <= pwmZones[zoneIndex].percentMax) {
      return zoneIndex;
    }
  }
  return 6;
}

// 1 : Saves prescaler info for serial monitoring.
// 2 : Directly sets prescaler.
// 3 : Loads other settings into variables used by setPWM().
void pwmSelectZone(int detectedConfig) {
  if (pwmZones[detectedConfig].prescale == 64) {
    cpuPrescaler = 64;
    TCCR2B = _BV(WGM22) | _BV(CS22);
  } else if (pwmZones[detectedConfig].prescale == 32) {
    cpuPrescaler = 32;
    TCCR2B = _BV(WGM22) | _BV(CS21) | _BV(CS20);
  } else {
    cpuPrescaler = 8;
    TCCR2B = _BV(WGM22) | _BV(CS21);
  }
  pwmResolution = pwmZones[detectedConfig].resolution;
  dutyMultiplier = pwmZones[detectedConfig].multiplier;
}

void setPWM() {
  if (duty == 0) {
    // Turn off PWM.
    TCCR2A &= ~(_BV(COM2A1) | _BV(COM2B1));

    // Reset zero current.
    #if defined(MINIMUM_DUTY_PROTECTION)
      senseZeroCurrent();
    #endif
  } else {
    dutyZone = pwmCheckZone(duty);
    lastDutyZone = pwmCheckZone(lastDuty);
    if (lastDuty == 0) {
      // Set up first PWM Zone: 1250Hz Fast PWM with OCR2A as TOP and Prescaler Divide Clock by 64 on Timer2.
      // (http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf, P.130)
      TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
    }
    if (dutyZone != lastDutyZone) {
      pwmSelectZone(dutyZone);
      OCR2A = pwmResolution - 1;
    }
    // duty = (OCR2B + 1) / (OCR2A + 1)
    // OCR2B = ((OCR2A + 1) * duty) - 1
    OCR2B = (int)(duty * dutyMultiplier) - 1;
  }
}

//===========================================================================
//============================= Update Duty =================================
//===========================================================================

void updateDuty() {
  if (!targetReached) {
    if (duty < targetDuty) {
      throttleDuty++;
      pwmUpdate.executionInterval = INTERVAL_UP;
    } else if (duty > targetDuty) {
      throttleDuty--;
      pwmUpdate.executionInterval = INTERVAL_DOWN;
    } else {
      targetReached = true;
    }
  }
  if ((throttleDuty<60 && POWER_SUPPLY==2) || (POWER_SUPPLY==1)) {
    duty = throttleDuty;
  } 
}
