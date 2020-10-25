/**
 * Motor Controller Firmware
 * Copyright (c) 2020 Applied Engineering
 *
 * Based on software from various contributors.
 * Copyright (c) 2020 Andrew Berkun / Alex Liu / William Zhou
 *
 */

/**
 * sense.cpp
 *
 * Detects electrical operating conditions.
 *
 */

#include "sense.h"
#include "../Configuration.h"

int deviceCount;
// Set temperature sensor resolution {9, 10, 11, 12}.
int tempResolution = 12;
// Temperature conversion takes up to 750 milliseconds.
int conversionTime = 750 / (1 << (12 - tempResolution));
float tempC[2];

int senseSamples;
int avgCount = 16;
unsigned int sumCounter;
const float conversionFactor = 5.0 / 1024 / 1000;
int zeroISenseVADC;    // (ADC)
int iSenseVADC;        // (ADC)
float current;         // (A)
int vSenseADC;         // (ADC)
float voltage;         // (V)
float lastVoltage = 0; // (V)
float dV = 0;          // (V)
float power;           // (W)
float lastPower = 0;   // (W)
float dP = 0;          // (W)
int it = 1;
int mpptDuty = 20;
int lastMPPTduty = 20;
int dD;
bool powerSupply = true;

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass oneWire reference to DallasTemperature library
DallasTemperature tempSensors(&oneWire);

// Sense temperatures after conversions complete.
NonBlockingTask tempUpdate(conversionTime);
// Sense current every 10 milliseconds.
NonBlockingTask iSenseUpdate(10);
// Sense voltage every 10 milliseconds.
NonBlockingTask vSenseUpdate(10);
// Sense power every 10 milliseconds.
NonBlockingTask pSenseUpdate(10);
// Track MPP every 50 milliseconds.
NonBlockingTask mpptUpdate(50);

// Moving average uses last avgCount samples.
RunningAverage movAvgCurrent(avgCount);

void configureTempSensors() {
  // Start up the DallasTemperature library.
  tempSensors.begin();
  // Locate devices on the bus.
  deviceCount = tempSensors.getDeviceCount();
  // Set resolution on all sensors.
  tempSensors.setResolution(tempResolution);
  // Do not block while sensors do conversion.
  tempSensors.setWaitForConversion(false);
  // Send command to all sensors to begin temperature conversion.
  tempSensors.requestTemperatures();
}

void senseTemperatures() {
  // Get temperature from each sensor.
  for (senseSamples = 0; senseSamples < deviceCount; senseSamples++) {
    tempC[senseSamples] = tempSensors.getTempCByIndex(senseSamples);
  }
  // Send command to all sensors to begin next temperature conversion.
  tempSensors.requestTemperatures();
}

// Get offset current, I_IS(offset).
void senseZeroCurrent() {
  for (senseSamples = 0; senseSamples < 5; senseSamples++) {

    zeroISenseVADC += analogRead(IS_1);
  }
  zeroISenseVADC /= 5;
}

// Get sense current, I_IS.
void senseCurrent() {
  movAvgCurrent.addValue(analogReadFast(IS_1));
  sumCounter++;
  if (sumCounter != 65535) {
    iSenseVADC = movAvgCurrent.getFastAverage();
  } else {
    // Update MA internal sum to prevent accumulating errors.
    iSenseVADC = movAvgCurrent.getAverage();
  }
  if (powerSupply) {
    current = conversionFactor * (iSenseVADC - zeroISenseVADC) * 19500;
  } else {
    current = (duty / 100.0) * 7.77;
  }
}

void senseVoltage() {
  lastVoltage = voltage;
  for (senseSamples = 0; senseSamples < 5; senseSamples++) {
    vSenseADC += analogReadFast(VBAT);
  }
  vSenseADC /= 5;
  if (powerSupply) {
    voltage = vSenseADC * (44.8 / 1024);
  } else {
    voltage = 13.0 * log(10 - current);
  }
}

void sensePower() {
  power = voltage * current * duty;
}

void trackMPP() {
  dD = mpptDuty - lastMPPTduty;
  dP = power - lastPower;
  // finding MPP
  if (dP > 0) {
    mpptDuty += it; // keep changing duty in the same direction we were before
  } else {
    it *= -1; // power is decreasing
    mpptDuty += it;
  }
  // MPP and throttle decision
  if (targetDuty>=border) {
    if (throttleDuty<border) { // ramp up
      // throttle, make mpptDuty start from throttle to prevent throttle--> mppt jump
      mpptDuty = throttleDuty;
    } else { // mppt
      duty = mpptDuty;
    }
  } else {
    if (throttleDuty<border) { // ramp down
      // throttle
    } else { // get mppt to make duty reach border
      if (abs(border-duty)>=2) { // when duty too far from border, push it towards the border 
        if (duty<border) {
          duty++;
        } else if (duty>border) {
          duty--;
        }
      }
    }
  }
  // Bound duty
  if (mpptDuty > 100) {
    mpptDuty = 100;
  } else if (mpptDuty < 0) {
    mpptDuty = 0;
  }
  lastPower = power;
  lastMPPTduty = mpptDuty;
}