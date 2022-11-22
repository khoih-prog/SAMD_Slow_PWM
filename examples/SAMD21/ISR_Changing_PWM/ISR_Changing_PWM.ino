/****************************************************************************************************************************
  ISR_Changing_PWM.ino
  For SAMD21/SAMD51 boards
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/SAMD_Slow_PWM
  Licensed under MIT license

  Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
  unsigned long miliseconds), you just consume only one SAMD timer and avoid conflicting with other cores' tasks.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.
*****************************************************************************************************************************/

#if !( defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_MKRWIFI1010) \
    || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_SAMD_MKRFox1200) || defined(ARDUINO_SAMD_MKRWAN1300) || defined(ARDUINO_SAMD_MKRWAN1310) \
    || defined(ARDUINO_SAMD_MKRGSM1400) || defined(ARDUINO_SAMD_MKRNB1500) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(__SAMD21G18A__) \
    || defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(__SAMD21G18A__) )
#error This code is designed to run on SAMD21 platform! Please check your Tools->Board setting.
#endif

// These define's must be placed at the beginning before #include "SAMD_Slow_PWM.h"
// _PWM_LOGLEVEL_ from 0 to 4
#define _PWM_LOGLEVEL_      3

#define USING_MICROS_RESOLUTION       true    //false

// Default is true, uncomment to false
//#define CHANGING_PWM_END_OF_CYCLE     false

#define MAX_SAMD_PWM_FREQ            1000

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "SAMD_Slow_PWM.h"

#define LED_OFF             LOW
#define LED_ON              HIGH

#ifndef LED_BUILTIN
  #define LED_BUILTIN       13
#endif

#ifndef LED_BLUE
  #define LED_BLUE          2
#endif

#ifndef LED_RED
  #define LED_RED           3
#endif

// Use 50uS for slow SAMD21
#define HW_TIMER_INTERVAL_US      50L

uint64_t startMicros = 0;

// Init SAMD timer TIMER_TC3
SAMDTimer ITimer(TIMER_TC3);

// Init SAMD timer TIMER_TCC
//SAMDTimer ITimer(TIMER_TCC);


// Init SAMD_Slow_PWM
SAMD_Slow_PWM ISR_PWM;

//////////////////////////////////////////////////////

void TimerHandler()
{
  ISR_PWM.run();
}

//////////////////////////////////////////////////////

#define USING_PWM_FREQUENCY     true

//////////////////////////////////////////////////////

// You can assign pins here. Be carefull to select good pin to use or crash
uint32_t PWM_Pin    = LED_BUILTIN;

// You can assign any interval for any timer here, in Hz
float PWM_Freq1   = 1.0f;
// You can assign any interval for any timer here, in Hz
float PWM_Freq2   = 2.0f;

// You can assign any interval for any timer here, in microseconds
uint32_t PWM_Period1 = 1000000 / PWM_Freq1;
// You can assign any interval for any timer here, in microseconds
uint32_t PWM_Period2 = 1000000 / PWM_Freq2;

// You can assign any duty_cycle for any PWM here, from 0-100
float PWM_DutyCycle1  = 50.0;
// You can assign any duty_cycle for any PWM here, from 0-100
float PWM_DutyCycle2  = 90.0;

// Channel number used to identify associated channel
int8_t channelNum;

////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);

  while (!Serial);

  delay(2000);

  Serial.print(F("\nStarting ISR_Changing_PWM on "));
  Serial.println(BOARD_NAME);
  Serial.println(SAMD_SLOW_PWM_VERSION);

  // Interval in microsecs
  if (ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_US, TimerHandler))
  {
    startMicros = micros();
    Serial.print(F("Starting ITimer OK, micros() = "));
    Serial.println(startMicros);
  }
  else
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));
}

void loop()
{
  Serial.print(F("Using PWM Freq = "));
  Serial.print(PWM_Freq1);
  Serial.print(F(", PWM DutyCycle = "));
  Serial.println(PWM_DutyCycle1);

#if USING_PWM_FREQUENCY

  // You can use this with PWM_Freq in Hz
  channelNum = ISR_PWM.setPWM(PWM_Pin, PWM_Freq1, PWM_DutyCycle1);

#else
#if USING_MICROS_RESOLUTION
  // Or using period in microsecs resolution
  channelNum = ISR_PWM.setPWM_Period(PWM_Pin, PWM_Period1, PWM_DutyCycle1);
#else
  // Or using period in millisecs resolution
  channelNum = ISR_PWM.setPWM_Period(PWM_Pin, PWM_Period1 / 1000, PWM_DutyCycle1);
#endif
#endif

  delay(10000);

  ISR_PWM.deleteChannel((uint8_t) channelNum);

  Serial.print(F("Using PWM Freq = "));
  Serial.print(PWM_Freq2);
  Serial.print(F(", PWM DutyCycle = "));
  Serial.println(PWM_DutyCycle2);

#if USING_PWM_FREQUENCY

  // You can use this with PWM_Freq in Hz
  channelNum = ISR_PWM.setPWM(PWM_Pin, PWM_Freq2, PWM_DutyCycle2);

#else
#if USING_MICROS_RESOLUTION
  // Or using period in microsecs resolution
  channelNum = ISR_PWM.setPWM_Period(PWM_Pin, PWM_Period2, PWM_DutyCycle2);
#else
  // Or using period in millisecs resolution
  channelNum = ISR_PWM.setPWM_Period(PWM_Pin, PWM_Period2 / 1000, PWM_DutyCycle2);
#endif
#endif

  delay(10000);

  ISR_PWM.deleteChannel((uint8_t) channelNum);
}
