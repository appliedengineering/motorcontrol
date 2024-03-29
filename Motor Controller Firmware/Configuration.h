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
 * Configuration.h
 * 
 * Provides an easy way to switch between
 * control modes and to change settings:
 * 
 * - Type of power supply
 * - Input and output options
 * - Circuit protection features
 * 
 */

// @section info

// Version info of this build reported to host (must be YYYY.MM.DD-X format).
#define STRING_VERSION "2020.10.24-5"

// @section machine

// Enable telemetry at the specified serial communication speed.
#define TELEMETRY 500000
#if defined(TELEMETRY)
  /**
   * Available telemetry options:
   * 
   *   1 : Uses the original boat telemetry; requires an Atomic Pi to run
   *   2 : Just prints desired values to the PlatformIO Serial Monitor (for when mode 1 doesn't work)
   * 
   */
  #define TELEMETRY_MODE 2
#endif

//===========================================================================
//============================ Testing Options ==============================
//===========================================================================
/**
 * Tells us what kind of testing we are doing
 * 
 *   1 : no testing
 *   2 : mppt simulation (watches how mppt behaves for simulated values of current and voltage)
 *   3 : tachometer testing
 *  
 */
#define TESTING_MODE 1
#if TESTING_MODE==3 
  #define SIMULATION_MODE 1 // 2 simulates acceleration, 1 for reading actual signals to pin 2
#endif

//===========================================================================
//============================= Power Options ===============================
//===========================================================================

/**
 * Available types of power supplies:
 * 
 *   1 : batteries
 *   2 : solar panels
 * 
 * -- Power supply voltage must NOT exceed 40V! --
 * 
 */
#define POWER_SUPPLY 1

// Enable these features when running on solar.
/**
 * Available solar modes:
 *    1 : MPPT -- extremely slow when there are bad current readings (as there are now)
 *    2 : VPPT -- Attempts to approximate MPPT by chasing a user-defined V_mpp (voltage of max power point)
*/
#if POWER_SUPPLY == 2
  #define OVERVOLTAGE_PREVENTION
  #define SOLAR_MODE 2
  #if SOLAR_MODE == 1 
    #define MAX_POWER_POINT_TRACKING;
  #elif SOLAR_MODE ==2
    #define V_MPP 24;
  #endif
#endif

//===========================================================================
//============================= Input Options ===============================
//===========================================================================

/**
 * Available types of input devices:
 * 
 *   1 : 1 - 4V throttle
 *   2 : momentary push-button switch
 *   3 : self-test (loops target duty between 100% and 0% for ramp times)
 *   4 : constant rpm, with controllable ramp up
 *   5 : constant rpm, no controllable ramp up
 * 
 */
#define INPUT_DEVICE 1
#if INPUT_DEVICE >= 4
  #define CONSTANT_DUTY 60
#endif

//===========================================================================
//============================= Output Options ==============================
//===========================================================================

/**
 * Available motor control output modes:
 * 
 *   1 : hardware single driver (fast)
 * 
 * -- It is possible to parallel drivers by hard-wiring a
 *    bridge between IN_1 and IN_2 on the motor control shield. --
 * 
 */
#define OUTPUT_MODE 1

// The motor will ramp up as quickly as possible.
#define RAMP_TIME_UP 6 * 1000     // (ms)

// The motor will take longer to ramp down.
#define RAMP_TIME_DOWN 10 * 1000  // (ms)

//===========================================================================
//============================= Protections =================================
//===========================================================================

// Prevent negative current.
#define MINIMUM_DUTY_DETECTION

// Limit the current through the board to 30A.
#define OVERCURRENT_PROTECTION
