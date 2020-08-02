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

#include "inputs.h"
#include "nonblocking.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RunningAverage.h>

extern int deviceCount;
extern float tempC[];

extern int senseSamples;
extern int avgCount;
extern unsigned int sumCounter;
extern int zeroISenseVADC;  // (ADC)
extern int iSenseVADC;      // (ADC)
extern int voltage;         // (ADC)

extern OneWire oneWire;
extern DallasTemperature tempSensors;

extern NonBlockingTask tempUpdate;
extern NonBlockingTask iSenseUpdate;
extern NonBlockingTask vSenseUpdate;

extern RunningAverage movAvgCurrent;

void configureTempSensors();
void senseTemperatures();

void senseZeroCurrent();
void senseCurrent();
void senseVoltage();