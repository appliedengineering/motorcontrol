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
 * fastpwm.h
 * 
 * Configures PWM generation parameters
 * 
 */

#include "inputs.h"
#include "nonblocking.h"
#include "sense.h"
#include "../Configuration.h"

extern int border;            // value of throttleDuty at which to switch to mppt
extern int duty;              // (%)
extern int lastDuty;          // (%)
extern int throttleDuty;      // (%)
extern int lastThrottleDuty;  // (%)

struct pwmConfig {
  int percentMax;
  int prescale;
  int resolution;
  float multiplier;
};

extern pwmConfig pwmZones[];
extern int zoneIndex;
extern int cpuPrescaler;
extern int pwmResolution;
extern float dutyMultiplier;
extern int dutyZone;
extern int lastDutyZone;

// Set up nonblocking PWM update task.
#define INTERVAL_UP (RAMP_TIME_UP / pwmResolution)      // (ms)
#define INTERVAL_DOWN (RAMP_TIME_DOWN / pwmResolution)  // (ms)
extern NonBlockingTask pwmUpdate;

int pwmCheckZone();
void pwmSelectZone();
void setPWM();
void updateDuty();
