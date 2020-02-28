/**
 * 2019-2020 Applied Engineering
 * Motor Controller Firmware
 * by Andrew Berkun, Alex Liu, and William Zhou
 * 
 * Version 2020.02.28-1
 */

#include <Arduino.h>

/******************** BEGIN Configuration ********************/
#define MINIMUM_DUTY_DETECTION false
#define TELEMETRY true
#define OVERCURRENT_PROTECTION false
#define SOLAR_MPPT false
/******************** END Configuration **********************/

/******************** BEGIN Pin Definitions ******************/
#define dir1 3
#define dir2 11
#define enable1 12
#define enable2 13
#define button 0
#define isense1 A0
#define isense2 A1
#define vsense A3
/******************** END Pin Definitions ********************/

/******************** BEGIN Global Variables *****************/
#define pwmFreq 20000 // (Hz)
#define pwmResolution ((F_CPU / 8) / pwmFreq)

// The motor will ramp up as quickly as possible.
#define rampFastTime 6 * 1000   // (ms)
// The motor will take longer to ramp down at lower duty cycles.
#define rampSlowTime 10 * 1000 // (ms)

unsigned long previousMillis = 0;                       // (ms)
unsigned long currentMillis;                            // (ms)
const long intervalFast = rampFastTime / pwmResolution; // (ms)
const long intervalSlow = rampSlowTime / pwmResolution; // (ms)
int interval = intervalFast;                            // (ms)

int current;        // (?)
#if MINIMUM_DUTY_DETECTION
  int zeroCurrent;  // (?)
#endif
int duty = 0;       // (%)
int lastDuty = 0;   // (%)

bool buttonPressed;
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
  pinMode(dir1, OUTPUT);         // IN_1
  pinMode(dir2, OUTPUT);         // IN_2
  pinMode(enable1, OUTPUT);      // INH_1
  pinMode(enable2, OUTPUT);      // INH_2
  pinMode(button, INPUT_PULLUP); // Pin 0
  pinMode(isense1, INPUT);       // IS_1
  pinMode(isense2, INPUT);       // IS_2
  pinMode(vsense, INPUT);        // Vbat

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
    digitalWrite(dir1, HIGH);
    zeroCurrent = analogRead(isense1);
    digitalWrite(dir1, LOW);
  #endif
}
/******************** END Setup ******************************/

/******************** BEGIN PWM Function *********************/
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
/******************** END PWM Function ***********************/

/******************** BEGIN Main Loop ************************/
void loop()
{
  // Duty must be between 0 and 100 (%).
  if (duty < 0)
  {
    duty = 0;
  }
  else if (duty > 100)
  {
    duty = 100;
  }

  // Check how duty should be updated.
  if (duty < lastDuty && duty < 25)
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
  else if (duty != lastDuty)
  {
    setPWM();
    lastDuty = duty;
  }

  if (digitalRead(button) == HIGH)
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

    if (buttonPressed && duty < 100)
    {
      duty++;
    }
    else if (!buttonPressed && duty > 0)
    {
      duty--;
    }

    current = analogRead(isense1);
    #if MINIMUM_DUTY_DETECTION
      current -= zeroCurrent;
    #endif
    #if TELEMETRY
      Serial.println(current);
    #endif

    #if OVERCURRENT_PROTECTION
      if (current > 200)
      {
        duty -= 2;
      }
    #endif

    #if SOLAR_MPPT
      // Keep input voltage above 30 volts (30/11 * 1024/5 = 559).
      if (analogRead(vsense) < 559)
      {
        duty -= 2;
      }
    #endif
  }
}
/******************** END Main Loop **************************/