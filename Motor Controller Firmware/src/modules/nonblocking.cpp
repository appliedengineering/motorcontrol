/**
 * Motor Controller Firmware
 * Copyright (c) 2020 Applied Engineering
 * 
 * Based on software from various contributors.
 * Copyright (c) 2020 Andrew Berkun / Alex Liu / William Zhou
 * 
 */

/**
 * nonblocking.cpp
 * 
 * Implements time multiplexing "multitasking"
 * 
 * Usage:
 *   // Create object with default constructor (executionInterval = 0).
 *   NonBlockingTask taskName;
 * 
 *   // Or create object and simultaneously set executionInterval to 5 ms.
 *   NonBlockingTask taskName(5);
 * 
 *   // Check if doSomething should be run.
 *   if (nonblockingUpdate(taskName)) {
 *     doSomething();
 *   }
 * 
 */

#include "nonblocking.h"

unsigned long currentMillis;  // (ms)

bool nonblockingUpdate(NonBlockingTask& task) {
  currentMillis = millis();
  if (currentMillis - task.previousMillis >= task.executionInterval) {
    task.previousMillis = currentMillis;
    return true;
  } else {
    return false;
  }
}