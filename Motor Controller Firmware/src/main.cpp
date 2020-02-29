/**
 * 2019-2020 Applied Engineering
 * Motor Controller Firmware
 * by Andrew Berkun, Alex Liu, and William Zhou
 * 
 * Version 2020.02.28-3
 */

#include <Arduino.h>
#include <avdweb_AnalogReadFast.h>
#include <digitalWriteFast.h>

/******************** BEGIN Configuration ********************/
#define MINIMUM_DUTY_DETECTION false
#define OVERCURRENT_PROTECTION false
#define PARALLEL_OUTPUTS false
#define TELEMETRY true
#define USE_THROTTLE true

// Enable these features when running on solar.
#define OVERVOLTAGE_PREVENTION false
#define UNDERVOLTAGE_PREVENTION false
/******************** END Configuration **********************/

/******************** BEGIN Pin Definitions ******************/
#define dir1 3
#define dir2 11
#define enable1 12
#define enable2 13

#if USE_THROTTLE
  #define throttle A2
#else
  #define button 0
#endif

#define isense1 A0
#define isense2 A1
#define vsense A3
/******************** END Pin Definitions ********************/

/******************** BEGIN Global Variables *****************/
#if PARALLEL_OUTPUTS
  unsigned int duty = 10;
  int timeOn, timeOff, current, zeroCurrent, buttonPressed;
  int rampSpeed = 10; // 4 * seconds to go from off to full or vice versa
  int period = 100;
#else
  #define pwmFreq 20000 // (Hz)
  #define pwmResolution ((F_CPU / 8) / pwmFreq)
  
  // The motor will ramp up as quickly as possible.
  #define rampFastTime 6 * 1000   // (ms)
  // The motor will take longer to ramp down at lower duty cycles.
  #define rampSlowTime 10 * 1000  // (ms)

  unsigned long previousMillis = 0;                       // (ms)
  unsigned long currentMillis;                            // (ms)
  const long intervalFast = rampFastTime / pwmResolution; // (ms)
  const long intervalSlow = rampSlowTime / pwmResolution; // (ms)
  unsigned long interval = intervalFast;                  // (ms)

  int current;        // (?)
  #if MINIMUM_DUTY_DETECTION
    int zeroCurrent;  // (?)
  #endif
  int duty = 0;       // (%)
  int lastDuty = 0;   // (%)

  #if USE_THROTTLE
    int rawDuty = 0;  // (ADC)
    int maxDuty = 0;  // (%)
  #endif

  bool buttonPressed;
#endif
/******************** END Global Variables *******************/

/******************** BEGIN Setup ****************************/
void setup()
{
  #if TELEMETRY
    Serial.begin(9600);
  #endif
  /********** Configure Pins **********/
  // (0.67 volts is 10 amps, a good limit per driver)
  // (revised, analog 0, 1 volt delta is 20 amps delta with a 1k resistor, current ratio of 20,000)
  pinMode(dir1, OUTPUT);            // IN_1
  pinMode(dir2, OUTPUT);            // IN_2
  pinMode(enable1, OUTPUT);         // INH_1
  pinMode(enable2, OUTPUT);         // INH_2
  #if USE_THROTTLE
    pinMode(throttle, INPUT);       // A2
  #else
    pinMode(button, INPUT_PULLUP);  // Pin 0
  #endif
  pinMode(isense1, INPUT);          // IS_1
  pinMode(isense2, INPUT);          // IS_2
  pinMode(vsense, INPUT);           // Vbat

  /********** Set Pins ****************/
  digitalWrite(dir1, LOW);     // Make sure IN_1 starts OFF
  digitalWrite(dir2, LOW);     // Make sure IN_2 starts OFF
  digitalWrite(enable1, HIGH); // Enable Output 1
  digitalWrite(enable2, LOW);  // Disable Output 2

  #if MINIMUM_DUTY_DETECTION
    /********** Set Zero Current ********/
    // Initialize ADC circuitry and discard first dummy sample
    (void)analogRead(isense1);
    // Can only measure current when HIGH, so we must pulse IN_1
    digitalWriteFast(dir1, HIGH);
    zeroCurrent = analogReadFast(isense1);
    digitalWriteFast(dir1, LOW);
  #endif
}
/******************** END Setup ******************************/

#if PARALLEL_OUTPUTS
  /******************** BEGIN Main Loop ************************/
  void loop()
  {
    if (duty < 50) duty = 50;     // If duty cycle < 1%, turn it off.
    if (duty > 4900) duty = 4900; // Don't go above 99%.
    // timeOn = (duty / 50) - 1;  // (µS) Assume min. on time is 1 µS, which is delayMicroseconds(0).
    // timeOff = 100 - d2 - 1;    // (µS) Assume min. off time is 1 µS.
    
    timeOff = 500 - (duty / 10);  // Range from 500 µS down to almost zero.  Means we start at 2 kHz and frequency goes up as we ramp the motor up.
    timeOn = 50 - timeOff;        // Constant 20 kHz period plus switching time if timeOff < 100 (if duty > 4500).
    if (timeOn < 1) timeOn = 1;   // Min. on time unless duty > 4500
    // So frequency only hits 20 kHz at the end of the scale and we get to 50%, our expected operating point at 4750.
    
    if (duty > 51)
    {
      for (int i = 0; i < rampSpeed; i++) // Loop number of times = spin up or down time in seconds * 4.
      {
        // Briefly only 1 driver is working, but we can be sure dir1 and
        // dir2 are never driving in opposite directions at the same time

        // digitalWrite(enable2, LOW);    // INH_2 OFF, IN_2 HIGH (but INH_2 OFF), IN_1 HIGH, then INH_2 ON again
        // digitalWrite(dir2, HIGH);
        PORTB = 0b011000;                 // Simultaneously set INH_2 (13) OFF, INH_1 (12) ON, and IN_2 (11) HIGH
        // digitalWrite(dir1, HIGH);
        PORTD ^= 0b00001000;              // Toggle IN_1 (3) HIGH
        // digitalWrite(enable2, HIGH);
        PORTB ^= 0b100000;                // Toggle INH_2 (13) ON again
        delayMicroseconds(timeOn);
        // digitalWrite(enable2, LOW);    // Begin reversing the above
        PORTB ^= 0b100000;                // Toggle INH_2 (13) OFF
        // digitalWrite(dir1, LOW);
        PORTD ^= 0b00001000;              // Toggle IN_1 (3) LOW
        // digitalWrite(dir2, LOW);
        // digitalWrite(enable2, HIGH);   // Trimmed to spend the least amount of time high possible
        PORTB = 0b110000;                 // Simultaneously set INH_2 (13) ON, INH_1 (12) ON, and IN_2 (11) LOW
        delayMicroseconds(timeOff);
      }
    }
    else
    {
      delayMicroseconds(rampSpeed * 50);  // When in off state, we don't monkey with the IO.
    }
    
    // Everything below only takes place every millisecond after 20 pulses with the drivers LOW
    // ADC reads 1023 for a 5 V input so raw = volts * 1024/5
    // Current inputs are about .07 V per A according to datasheet, but we should test this.
    
    delayMicroseconds(20);  // To do a current reading we need both outputs HIGH
    PORTB = 0b011000;       // Simultaneously set INH_2 (13) OFF, INH_1 (12) ON, and IN_2 (11) HIGH
    PORTD ^= 0b00001000;    // Toggle IN_1 (3) HIGH
    PORTB ^= 0b100000;      // Toggle INH_2 (13) ON again
    current = analogReadFast(0);
    PORTB ^= 0b100000;      // Toggle INH_2 (13) OFF
    PORTD ^= 0b00001000;    // Toggle IN_1 (3) LOW
    PORTB = 0b110000;       // Simultaneously set INH_2 (13) ON, INH_1 (12) ON, and IN_2 (11) LOW
    delayMicroseconds(10);
    current -= zeroCurrent;
    #if TELEMETRY
      Serial.println(current);
    #endif
    
    if (digitalRead(0) == HIGH) buttonPressed = 0; else buttonPressed = 1;
    if (buttonPressed == 1) duty++; else duty--;
    
    #if OVERCURRENT_PROTECTION
      // 1 V delta is 20 A per driver -> 40 A (1 V * (1024 ADC values / 5 V) ≈ 200).
      // I suggest 30 A per driver (300).
      // Battery current = this current * duty.
      if (current > 300) duty = duty - 10;
    #endif
    
    #if MINIMUM_DUTY_DETECTION
      // Don't reduce duty cycle unless current is positive.
      if (current < 0) duty = duty + 1;
    #endif

    #if UNDERVOLTAGE_PREVENTION
      if (analogRead(vsense) < 559) duty = duty - 10; // Keep input voltage above 30 volts (30/11 * 1024/5 = 559) to get more power out on solar.
    #endif
    #if OVERVOLTAGE_PREVENTION
      if (analogRead(vsense) > 744) duty = duty + 3;  // Keep input voltage below 40 volts (40/11 * 1024/5 = 745) to protect against regen on solar.
    #endif
  }
  /******************** END Main Loop **************************/
#else
  /******************** BEGIN Fast PWM Function ****************/
  void setPWM()
  {
    if (duty == 0)
    {
      // Turn off PWM
      TCCR2B = _BV(WGM22);
    }
    else if (lastDuty != 0)
    {
      OCR2B = duty - 1;
    }
    else
    {
      // Set up 20kHz Fast PWM with OCR2A as TOP and Prescaler Divide Clock by 8 on Timer 2
      // (http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf, P.130)
      TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM20) | _BV(WGM21);
      TCCR2B = _BV(WGM22) | _BV(CS21);
      OCR2A = pwmResolution - 1;
      // duty = (OCR2B + 1) / (OCR2A + 1)
      // OCR2B = ((OCR2A + 1) * duty) - 1
      OCR2B = duty - 1;
    }
  }
  /******************** END Fast PWM Function ******************/

  /******************** BEGIN Main Loop ************************/
  void loop()
  {
    if (duty < 0)
    {
      duty = 0;
    }
    else if (duty > 100)
    {
      duty = 100;
    }

    // Check how duty should be updated.
    if (duty < lastDuty)
    {
      interval = intervalSlow;
    }
    else
    {
      interval = intervalFast;
    }

    // Sanity checks before setting PWM output.
    if (duty > lastDuty + 2)
    {
      duty = lastDuty + 2;
    }
    else if (duty < lastDuty - 2)
    {
      duty = lastDuty - 2;
    }
    
    // Set PWM output and store value before duty is updated.
    if (duty != lastDuty)
    {
      setPWM();
      lastDuty = duty;
    }

    #if USE_THROTTLE
    // 1 V (205) is 0% and 4 V (820) is 100%
    rawDuty = analogReadFast(throttle);
    maxDuty = (rawDuty - 205) * (100/615);

      if (rawDuty < 205)
    #else
    maxDuty = 100;

      if (digitalReadFast(button) == HIGH)
    #endif
      {
        buttonPressed = false;
      }
      else
      {
        buttonPressed = true;
      }

    // Check whether current and duty should be updated.
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;

      if (buttonPressed && duty < maxDuty)
      {
        duty++;
      }
      else if (!buttonPressed && duty > maxDuty)
      {
        duty--;
      }

      current = analogRead(isense1);
      #if MINIMUM_DUTY_DETECTION
        current -= zeroCurrent;
        if (current < 0)
        {
          duty++;
        }
      #endif
      #if TELEMETRY
        Serial.println(current);
      #endif

      #if OVERCURRENT_PROTECTION
        if (current > 300)
        {
          duty -= 2;
        }
      #endif

      #if UNDERVOLTAGE_PREVENTION
        // Keep input voltage above 30 volts (30/11 * 1024/5 = 559).
        // Gets more power out on solar.
        if (analogRead(vsense) < 559)
        {
          duty -= 2;
        }
      #endif
      #if OVERVOLTAGE_PREVENTION
        // Keep input voltage below 40 volts (40/11 * 1024/5 = 745).
        // Protects against regen on solar.
        if (analogRead(vsense) > 744)
        {
          duty += 2;
        }
      #endif
    }
  }
  /******************** END Main Loop **************************/
#endif