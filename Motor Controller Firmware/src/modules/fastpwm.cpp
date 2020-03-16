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

// Set up nonblocking PWM update task.
NonBlockingTask pwmUpdate(INTERVAL_SLOW);

//===========================================================================
//============================= Update PWM Pin ==============================
//===========================================================================

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
    // Set up 20kHz Fast PWM with OCR2A as TOP and Prescaler Divide Clock by 8 on Timer2.
    // (http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf, P.130)
    TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM20) | _BV(WGM21);
    TCCR2B = _BV(WGM22) | _BV(CS21);
    OCR2A = PWM_RESOLUTION - 1;
    // duty = (OCR2B + 1) / (OCR2A + 1)
    // OCR2B = ((OCR2A + 1) * duty) - 1
    OCR2B = duty - 1;
  }
}

//===========================================================================
//============================= Update Duty =================================
//===========================================================================

void updateDuty() {
  if (duty < targetDuty) {
    duty++;
    pwmUpdate.executionInterval = INTERVAL_FAST;
  } else if (duty > targetDuty) {
    duty--;
    pwmUpdate.executionInterval = INTERVAL_SLOW;
  }
}