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
 * pins.h
 * 
 * Includes pin definitions
 * 
 * Pin numbering schemes:
 * 
 * - Infineon Motor Control Shield with BTN8982TA for Arduino
 *   BTN8982TA pins (e.g. IN_1) and Shield pins (e.g. OUT_1)
 * 
 * - ATmega328P Arduino Pin Mapping
 *   Digital pins (e.g. 5) and Analog pins (e.g. A5)
 * 
 */

#include "../Configuration.h"
#include <Arduino.h>

#define IN_1 3
#define IN_2 11
#define INH_1 12
#define INH_2 13

#if INPUT_DEVICE == 1
  #define THROTTLE A2
#elif INPUT_DEVICE == 2
  #define BUTTON 0
#elif INPUT_DEVICE == 3
  #define SELFTEST
#endif

#define IS_1 A0
#define IS_2 A1
#define VBAT A3

#define ONE_WIRE_BUS 2

void configurePins();