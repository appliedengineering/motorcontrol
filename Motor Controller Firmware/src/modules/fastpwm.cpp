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

int pwmFreq[] = {1250, 2500, 5000, 10000, 13333}

// Set up nonblocking PWM update task.
NonBlockingTask pwmUpdate(INTERVAL_DOWN);

//===========================================================================
//============================= Update PWM Pin ==============================
//===========================================================================

// Returns integer pwmFreq (do NOT use in pwmResolution calculations!)
float pwmZone(int percent) {
  if (percent <= 1) { return 1250; }
  else if (percent <= 3) { return 2500; }
  else if (percent <= 7) { return 5000; }
  else if (percent <= 13) { return 10000; }
  else if (percent == 14) { return 13333; } // actually 40000/3
  else if (percent == 15) { return 16666; } // actually 50000/3
  else { return 20000; }
}

void setPWM() {
  if (duty == 0) {
    // Turn off PWM.
    TCCR2A &= ~(_BV(COM2A1) | _BV(COM2B1));

    // Reset zero current.
    #if defined(MINIMUM_DUTY_PROTECTION)
      senseZeroCurrent();
    #endif
  } else if (lastDuty != 0) {
    OCR2B = duty - 1;
  } else {
    // Set up 1250Hz Fast PWM with OCR2A as TOP and Prescaler Divide Clock by 64 on Timer2.
    // (http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf, P.130)
    TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
    TCCR2B = _BV(WGM22) | _BV(CS22);
    OCR2A = pwmResolution - 1;
    // duty = (OCR2B + 1) / (OCR2A + 1)
    // OCR2B = ((OCR2A + 1) * duty) - 1
    OCR2B = duty - 1;
  }
}

//===========================================================================
//============================= Update Duty =================================
//===========================================================================

void updateDuty() {
  if (!targetReached) {
    if (duty < targetDuty) {
      duty++;
      pwmUpdate.executionInterval = INTERVAL_UP;
    } else if (duty > targetDuty) {
      duty--;
      pwmUpdate.executionInterval = INTERVAL_DOWN;
    } else {
      targetReached = true;
    }
  }
}