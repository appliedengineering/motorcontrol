/**
 * Motor Controller Firmware
 * Copyright (c) 2020 Applied Engineering
 * 
 * Based on software from various contributors.
 * Copyright (c) 2020 Andrew Berkun / Alex Liu / William Zhou
 * 
 */

/**
 * pins.cpp
 * 
 * Configures pin directions
 * 
 */

#include "pins.h"

void configurePins() {
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(INH_1, OUTPUT);
  pinMode(INH_2, OUTPUT);
  #if defined(THROTTLE)
    pinMode(THROTTLE, INPUT);       // Pin A2
  #elif defined(BUTTON)
    pinMode(BUTTON, INPUT_PULLUP);  // Pin 0
  #endif
  pinMode(IS_1, INPUT);
  pinMode(IS_2, INPUT);
  pinMode(VBAT, INPUT);

  digitalWrite(IN_1, LOW);    // Make sure IN_1 starts OFF
  digitalWrite(IN_2, LOW);    // Make sure IN_2 starts OFF
  digitalWrite(INH_1, HIGH);  // Enable OUT_1
  digitalWrite(INH_2, LOW);   // Disable OUT_2
}