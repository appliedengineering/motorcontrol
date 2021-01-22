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
  #include "modules/telemetry.h"
#endif

// isr for tachometer (Interrupt Service Routine)
void isr() {
  detachInterrupt(digitalPinToInterrupt(2));
  dTRPM = micros()-lastRPMTime;
  lastRPMTime = micros();
  attachInterrupt(digitalPinToInterrupt(2), isr, RISING);
}

/******************** BEGIN Setup ****************************/
void setup() {
  configurePins();
  #if defined(TELEMETRY)
    configureSerial();
  #endif

  configureTempSensors();
  // Initialize ADC circuitry and discard first dummy sample
  (void)analogRead(IS_1);
  senseZeroCurrent();

  #if TESTING_MODE==3 
      #if SIMULATION_MODE==1
        pinMode(2, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(2), isr, RISING);
      #endif
  #endif
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

  // Read throttle input without blocking.
  if (nonblockingUpdate(inputUpdate)) {
    readInput();
  }

  // Update duty without blocking.
  if (nonblockingUpdate(pwmUpdate)) {
    updateDuty();
    checkProtections();
  }

  // Read temperatures without blocking.
  if (nonblockingUpdate(tempUpdate)) {
    senseTemperatures();
  }

  // Read current without blocking.
  if (nonblockingUpdate(iSenseUpdate)) {
    senseCurrent();
  }

  // Read voltage without blocking.
  if (nonblockingUpdate(vSenseUpdate)) {
    senseVoltage();
  }

  // Read power without blocking.
  if (nonblockingUpdate(pSenseUpdate)) {
    sensePower();
  }

  // Track MPP without blocking.
  #if defined(MAX_POWER_POINT_TRACKING)
    if (nonblockingUpdate(mpptUpdate)) {
      trackMPP();
    }
  #endif

  // Track rpm without blocking.
  #if TESTING_MODE==3
    #if SIMULATION_MODE==2 
      if (nonblockingUpdate(accelerateRPM)) {
        accelerate();
      }
    #endif
    if (nonblockingUpdate(rpmUpdate)) {
      rpm = 60000000.0/dTRPM;
    }
    if (nonblockingUpdate(torqueUpdate)) {
      trackTorque();
    }
  #endif

  // Send telemetry without blocking.
  #if defined(TELEMETRY)
    if (nonblockingUpdate(telemetryUpdate)) {
      sendData();
    }
  #endif
}
/******************** END Main Loop **************************/