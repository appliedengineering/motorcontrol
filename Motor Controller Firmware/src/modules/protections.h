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
 * protections.h
 * 
 * Checks limits to protect the controller.
 * 
 */

#include "fastpwm.h"

extern bool mddActive, ocpActive, ovpActive;

void checkProtections();