/**
 * Motor Controller Firmware
 * Copyright (c) 2020 Applied Engineering
 * 
 * Based on software from various contributors.
 * Copyright (c) 2020 Andrew Berkun / Alex Liu / William Zhou
 * 
 */

#include "../Configuration.h"
#include "modules/pins.h"
#include "modules/inputs.h"
#include "modules/sense.h"
#include "modules/nonblocking.h"
#include "modules/fastpwm.h"
#include "modules/protections.h"
#if defined(TELEMETRY)
  #include "modules/serial.h"
#endif

/******************** BEGIN Setup ****************************/
void setup() {
  configurePins();
  #if defined(TELEMETRY)
    configureSerial();
  #endif

  // Initialize ADC circuitry and discard first dummy sample
  (void)analogRead(IS_1);
  #if defined(MINIMUM_DUTY_DETECTION)
    senseZeroCurrent();
  #endif
  preloadISenseMMA();
}
/******************** END Setup ******************************/

/******************** BEGIN Main Loop ************************/
void loop() {
  if (duty < 0) {
    duty = 0;
  } else if (duty > 100) {
    duty = 100;
  }
  
  // Set PWM output and store value before duty is updated.
  if (duty != lastDuty) {
    setPWM();
    lastDuty = duty;
  }

  readInput();

  // Update duty without blocking.
  if (nonblockingUpdate(pwmUpdate)) {
    updateDuty();
    checkProtections();
  }

  // Read current without blocking.
  if (nonblockingUpdate(iSenseUpdate)) {
    senseCurrent();
  }

  // Read voltage without blocking.
  if (nonblockingUpdate(vSenseUpdate)) {
    senseVoltage();
  }

  // Send telemetry without blocking.
  #if defined(TELEMETRY)
    if (nonblockingUpdate(telemetryUpdate)) {
      sendData();
    }
  #endif
}
/******************** END Main Loop **************************/