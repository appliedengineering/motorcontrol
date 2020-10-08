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
 * sense.h
 * 
 * Detects electrical operating conditions.
 * 
 */

#include "fastpwm.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RunningAverage.h>

extern int deviceCount;
extern float tempC[];

extern int senseSamples;
extern int avgCount;
extern unsigned int sumCounter;
extern const float conversionFactor;
extern int zeroISenseVADC;  // (ADC)
extern int iSenseVADC;      // (ADC)
extern float current;       // (A)
extern int vSenseADC;       // (ADC)
extern float voltage;       // (V)
extern float lastVoltage;   // (V)
extern float dV;            // (V)
extern float power;         // (W)
extern float lastPower;     // (W) 
extern float dP;            // (W)
extern int it;              // increment or decrement of duty
extern int mpptDuty;        // a %
extern int lastMPPTDuty;        // also a %

extern OneWire oneWire;
extern DallasTemperature tempSensors;

extern NonBlockingTask tempUpdate;
extern NonBlockingTask iSenseUpdate;
extern NonBlockingTask vSenseUpdate;
extern NonBlockingTask pSenseUpdate;
extern NonBlockingTask mpptUpdate;

extern RunningAverage movAvgCurrent;

void configureTempSensors();
void senseTemperatures();

void senseZeroCurrent();
void senseCurrent();
void senseVoltage();
void sensePower();
void trackMPPT();
