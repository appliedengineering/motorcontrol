/**
 * 2019-2020 Applied Engineering
 * Motor Controller Firmware
 * by Andrew Berkun, Alex Liu, and William Zhou
 * 
 * Version 2020.02.26-2
 */

#include <Arduino.h>

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

// The motor will take 5 seconds to ramp up or ramp down.
#define rampTime 5 * 1000 // (ms)

unsigned long previousMillis = 0;               // (ms)
unsigned long currentMillis;                    // (ms)
const long interval = rampTime / pwmResolution; // (ms)

int current;     // (?)
int duty;        // (%)

bool buttonPressed = false;
bool lastButtonState = false;
bool doneRamping = false;
/******************** END Global Variables *******************/

/******************** BEGIN Setup ****************************/
void setup()
{
  Serial.begin(9600);
  /********** Configure Pins **********/
  // (0.67 volts is 10 amps, a good limit per driver)
  // (revised, analog 0, 1 volt delta is 20 amps delta with a 1k resistor, current ratio of 20,000)
  pinMode(dir1, OUTPUT);          // IN_1
  // pinMode(dir2, OUTPUT);       // IN_2 (hardwired from Pin 3)
  pinMode(enable1, OUTPUT);       // INH_1
  pinMode(enable2, OUTPUT);       // INH_2
  pinMode(button, INPUT_PULLUP);  // Pin 0
  pinMode(isense1, INPUT);        // IS_1
  pinMode(isense2, INPUT);        // IS_2
  pinMode(vsense, INPUT);         // Vbat

  /********** Set Pins ****************/
  digitalWrite(dir1, LOW);      // Make sure IN_1 starts OFF
  // digitalWrite(dir2, LOW);   // Make sure IN_2 starts OFF (hardwired to mirror IN_1)
  digitalWrite(enable1, HIGH);  // Enable Output 1
  digitalWrite(enable2, LOW);   // Disable Output 2
}
/******************** END Setup ******************************/

/******************** BEGIN PWM Function *********************/
void setPWM()
{
  if (duty == 0)
  {
    TCCR2B = _BV(WGM22);
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
  if (digitalRead(button) == HIGH)
  {
    buttonPressed = false;
  }
  else
  {
    buttonPressed = true;
  }

  if (buttonPressed != lastButtonState)
  {
    doneRamping = false;
  }

  lastButtonState = buttonPressed;

  // Check whether current and duty should be updated.
  if (!doneRamping)
  {
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
      else
      {
        doneRamping = true;
      }
      setPWM();

      current = analogRead(isense1);
      Serial.println(current);
    }
  }

  /********** ?????????? **************/
  if (current > 200)
  {
    duty -= 10;
  }

  /********** Solar MPPT **************/
  // Keep input voltage above 30 volts (30/11 * 1024/5 = 559).
  if (analogRead(vsense) < 559)
  {
    duty -= 10;
  }
}
/******************** END Main Loop **************************/