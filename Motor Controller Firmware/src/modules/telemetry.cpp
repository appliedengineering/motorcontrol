/**
 * Motor Controller Firmware
 * Copyright (c) 2020 Applied Engineering
 * 
 * Based on software from various contributors.
 * Copyright (c) 2020 Andrew Berkun / Alex Liu / William Zhou
 * 
 */

/**
 * telemetry.cpp
 * 
 * Enables serial communication features:
 * 
 * - Output telemetry over serial
 * 
 */

#include "telemetry.h"

StaticJsonDocument<128> boatData;

#if TESTING_MODE!=3
  // Send telemetry at 10 Hz.
  NonBlockingTask telemetryUpdate(50);
#endif

void configureSerial() {
  Serial.begin(TELEMETRY);
  boatData["psuMode"] = POWER_SUPPLY;
}

void sendData() {
  #if TESTING_MODE==3 
    boatData["lastRPMTime"] = lastRPMTime;
    boatData["numInterrupts"] = numInterrupts;
    boatData["rpm"] = rpm;
    boatData["torque"] = torque;
  #elif POWER_SUPPLY==2
    boatData["throttleDuty"] = throttleDuty;
    boatData["mpptDuty"] = mpptDuty;
  #endif
  boatData["throttlePercent"] = targetDuty;
  boatData["dutyPercent"] = duty;
  boatData["pwmFrequency"] = F_CPU / (cpuPrescaler * pwmResolution);
  boatData["tempC"] = tempC[0];
  boatData["sourceVoltage"] = voltage;
  // Calculate load current using (I_IS - I_IS(offset)) * dk_ILIS.
  // (https://www.infineon.com/dgdl/Infineon-BTN8982TA-DS-v01_00-EN.pdf?fileId=db3a30433fa9412f013fbe32289b7c17)
  boatData["pwmCurrent"] = current;
  boatData["powerChange"] = dP;
  boatData["voltageChange"] = dV;
  boatData["mddStatus"] = mddActive;
  boatData["ocpStatus"] = ocpActive;
  boatData["ovpStatus"] = ovpActive;
  serializeMsgPack(boatData, Serial);
  // Send message end sequence.
  Serial.print("EOM\n");
}

// Simple terminal printing
void sendData2() {
  Serial.print("\tdutyPercent: ");
  Serial.print(duty);
  Serial.print("\tthrottleDuty: ");
  Serial.print(throttleDuty);
  Serial.print("\ttargetDuty: ");
  Serial.print(targetDuty);
  Serial.print("\tsourceVoltage: ");
  Serial.print(voltage);
  Serial.print("\tpwmCurrent: ");
  Serial.print(current);
  Serial.println();
}