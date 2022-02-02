/****************************************************************************************************************************
  ISR_4_PWMs_Array_Simple.ino
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
// Don't define _PWM_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define _PWM_LOGLEVEL_      4

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

// Use max 4 for slow SAMD21
#define NUMBER_ISR_PWMS         4

#define PIN_D2      2
#define PIN_D7      7
#define PIN_D8      8
#define PIN_D9      9
#define PIN_D10     10
#define PIN_D11     11
#define PIN_D12     12

//////////////////////////////////////////////////////

#define USING_PWM_FREQUENCY     true

//////////////////////////////////////////////////////

// You can assign pins here. Be carefull to select good pin to use or crash, e.g pin 6-11
uint32_t PWM_Pin[] =
{  
   LED_BUILTIN, PIN_D2, PIN_D7, PIN_D8
};

// You can assign any interval for any timer here, in microseconds
uint32_t PWM_Period[] =
{
  1000000,   500000,   250000,   200000
};

// You can assign any interval for any timer here, in Hz
float PWM_Freq[] =
{
  1.0,  2.0,  4.0,  5.0
};

// You can assign any interval for any timer here, in milliseconds
float PWM_DutyCycle[] =
{
   40.00, 45.00, 50.00, 55.00
};


////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  delay(2000);

  Serial.print(F("\nStarting ISR_4_PWMs_Array_Simple on ")); Serial.println(BOARD_NAME);
  Serial.println(SAMD_SLOW_PWM_VERSION);

  // Interval in microsecs
  if (ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_US, TimerHandler))
  {
    startMicros = micros();
    Serial.print(F("Starting ITimer OK, micros() = ")); Serial.println(startMicros);
  }
  else
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));

  // Just to demonstrate, don't use too many ISR Timers if not absolutely necessary
  // You can use up to 16 timer for each ISR_PWM
  for (uint16_t i = 0; i < NUMBER_ISR_PWMS; i++)
  {
    //void setPWM(uint32_t pin, uint32_t frequency, uint32_t dutycycle
    // , timer_callback_p StartCallback = nullptr, timer_callback_p StopCallback = nullptr)

#if USING_PWM_FREQUENCY

    // You can use this with PWM_Freq in Hz
    ISR_PWM.setPWM(PWM_Pin[i], PWM_Freq[i], PWM_DutyCycle[i]);

#else
  #if USING_MICROS_RESOLUTION
    // Or using period in microsecs resolution
    ISR_PWM.setPWM_Period(PWM_Pin[i], PWM_Period[i], PWM_DutyCycle[i]);
  #else
    // Or using period in millisecs resolution
    ISR_PWM.setPWM_Period(PWM_Pin[i], PWM_Period[i] / 1000, PWM_DutyCycle[i]);
  #endif
#endif
  }
}

void loop()
{
}
