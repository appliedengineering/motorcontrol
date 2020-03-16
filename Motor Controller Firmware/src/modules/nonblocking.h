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
 * nonblocking.h
 * 
 * Implements time multiplexing "multitasking"
 * 
 * -- Overflow occurs after about 50 days, but this does not affect proper operation
 *    (https://arduino.stackexchange.com/questions/12587/how-can-i-handle-the-millis-rollover/12588#12588). --
 * 
 */

#include <Arduino.h>

extern unsigned long currentMillis;   // (ms)

class NonBlockingTask {
  public:
    unsigned long previousMillis = 0; // (ms)
    int executionInterval;            // (ms)

    NonBlockingTask() {
      executionInterval = 0;
    }

    NonBlockingTask(int interval) {
      executionInterval = interval;
    }
};

bool nonblockingUpdate(NonBlockingTask& task);