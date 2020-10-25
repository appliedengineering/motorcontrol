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
 * telemetry.h
 * 
 * Enables serial communication features:
 * 
 * - Output telemetry over serial
 * 
 */

#include <ArduinoJson.h>
#include "inputs.h"
#include "nonblocking.h"
#include "sense.h"
#include "fastpwm.h"
#include "protections.h"

extern NonBlockingTask telemetryUpdate;

void configureSerial();
void sendData();