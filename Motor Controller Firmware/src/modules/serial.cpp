/**
 * Motor Controller Firmware
 * Copyright (c) 2020 Applied Engineering
 * 
 * Based on software from various contributors.
 * Copyright (c) 2020 Andrew Berkun / Alex Liu / William Zhou
 * 
 */

/**
 * serial.cpp
 * 
 * Enables serial communication features:
 * 
 * - Output telemetry over serial
 * - Enable programming over serial
 * 
 */

#include "serial.h"

telemetryData boatData;

// Send telemetry every half second.
NonBlockingTask telemetryUpdate(500);

void configureSerial() {
  Serial.begin(TELEMETRY);
  boatData.id = 1;
  Serial.println("Motor Controller Firmware");
  Serial.println("Copyright (c) 2020 Applied Engineering");
  Serial.print("Booted version ");
  Serial.println(STRING_VERSION);
}

void sendData() {
  boatData.throttlePercent = targetDuty;
  boatData.targetDutyPercent = duty;
  boatData.sourceVoltage = voltage * (44.8/1024);
  boatData.pwmCurrent = current / 10.24;
  boatData.mddStatus = mddActive;
  boatData.ocpStatus = ocpActive;
  boatData.uvpStatus = uvpActive;
  boatData.ovpStatus = ovpActive;
  boatData.length = sizeof(boatData);
  Serial.write((byte*)&boatData, boatData.length);
}