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

#include "telemetry.h"

telemetryData boatData;

// Send telemetry at 10 Hz.
NonBlockingTask telemetryUpdate(100);

const float conversionFactor = 5.0/1024/1000;

unsigned int dataLength;
SerialTransfer dataTransfer;

void configureSerial() {
  Serial.begin(TELEMETRY);
  dataTransfer.begin(Serial);
  strlcpy(boatData.version, STRING_VERSION, 13);
  boatData.psuMode = POWER_SUPPLY;
  Serial.println("Motor Controller Firmware");
  Serial.println("Copyright (c) 2020 Applied Engineering");
  Serial.print("Booted version ");
  Serial.println(STRING_VERSION);
}

void sendData() {
  boatData.throttlePercent = targetDuty;
  boatData.dutyPercent = duty;
  boatData.pwmFrequency = F_CPU / (cpuPrescaler * pwmResolution);
  boatData.tempC = tempC[0];
  boatData.sourceVoltage = voltage * (44.8/1024);
  // Calculate load current using (I_IS - I_IS(offset)) * dk_ILIS.
  // (https://www.infineon.com/dgdl/Infineon-BTN8982TA-DS-v01_00-EN.pdf?fileId=db3a30433fa9412f013fbe32289b7c17)
  boatData.pwmCurrent = conversionFactor * (iSenseVADC - zeroISenseVADC) * 19500;
  boatData.mddStatus = mddActive;
  boatData.ocpStatus = ocpActive;
  boatData.uvpStatus = uvpActive;
  boatData.ovpStatus = ovpActive;
  dataLength = sizeof(boatData);
  dataTransfer.txObj(boatData, dataLength);
  dataTransfer.sendData(dataLength);
}