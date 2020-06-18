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
 * serial.h
 * 
 * Enables serial communication features:
 * 
 * - Output telemetry over serial
 * - Enable programming over serial
 * 
 */

#include "inputs.h"
#include "nonblocking.h"
#include "sense.h"
#include "fastpwm.h"
#include "protections.h"
#include <SerialTransfer.h>

extern unsigned int dataLength;

struct telemetryData {
  // Firmware info
  char version[12];
  int psuMode;
  // Input readings
  int throttlePercent;
  int dutyPercent;
  // Sense readings
  float sourceVoltage;
  float pwmCurrent;
  // Protections status
  bool mddStatus;
  bool ocpStatus;
  bool uvpStatus;
  bool ovpStatus;
};

extern telemetryData boatData;
extern NonBlockingTask telemetryUpdate;
extern SerialTransfer dataTransfer;

void configureSerial();
void sendData();