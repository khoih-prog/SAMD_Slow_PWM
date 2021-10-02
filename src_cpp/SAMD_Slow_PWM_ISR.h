/****************************************************************************************************************************
  SAMD_Slow_PWM_ISR.h
  For SAMD21/SAMD51 boards
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/SAMD_Slow_PWM
  Licensed under MIT license

  Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
  unsigned long miliseconds), you just consume only one SAMD timer and avoid conflicting with other cores' tasks.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.

  Version: 1.0.0

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K.Hoang      01/10/2021 Initial coding for SAMD21/SAMD51 boards
*****************************************************************************************************************************/

#pragma once

#ifndef SAMD_SLOW_PWM_ISR_H
#define SAMD_SLOW_PWM_ISR_H

#if !( defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_MKRWIFI1010) \
    || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_SAMD_MKRFox1200) || defined(ARDUINO_SAMD_MKRWAN1300) || defined(ARDUINO_SAMD_MKRWAN1310) \
    || defined(ARDUINO_SAMD_MKRGSM1400) || defined(ARDUINO_SAMD_MKRNB1500) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(__SAMD21G18A__) \
    || defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(__SAMD21E18A__) || defined(__SAMD51__) || defined(__SAMD51J20A__) || defined(__SAMD51J19A__) \
    || defined(__SAMD51G19A__) || defined(__SAMD51P19A__) || defined(__SAMD21G18A__) )
  #error This code is designed to run on SAMD21/SAMD51 platform! Please check your Tools->Board setting.
#endif


// Too many boards sharing the same ARDUINO_SAMD_ZERO but very different, such as SAMD21 SparkFun RedBoard Turbo
// Have to exclude some from the list
#if ( defined(ARDUINO_SAMD_ZERO) && ! ( defined(ADAFRUIT_FEATHER_M0) || defined(ARDUINO_SAMD_FEATHER_M0) || defined(ADAFRUIT_METRO_M0_EXPRESS) || \
      defined(ARDUINO_SAMD_HALLOWING_M0) || defined(ADAFRUIT_BLM_BADGE) ) )
  // Specific for SAMD21 SparkFun RedBoard Turbo
  #if !defined(Serial)
    #define Serial    SerialUSB
  #endif
#endif

#include "Arduino.h"

#ifndef SAMD_SLOW_PWM_VERSION
  #define SAMD_SLOW_PWM_VERSION       "SAMD_Slow_PWM v1.0.0"
#endif

#ifndef _PWM_LOGLEVEL_
  #define _PWM_LOGLEVEL_       1
#endif

#include "PWM_Generic_Debug.h"

#include <stddef.h>

#include <inttypes.h>

#if defined(ARDUINO)
  #if ARDUINO >= 100
    #include <Arduino.h>
  #else
    #include <WProgram.h>
  #endif
#endif

#define SAMD_Slow_PWM_ISR SAMD_Slow_PWM

typedef void (*timer_callback)();
typedef void (*timer_callback_p)(void *);

#if !defined(USING_MICROS_RESOLUTION)
  #warning Not USING_MICROS_RESOLUTION, using millis resolution
  #define USING_MICROS_RESOLUTION       false
#endif

#define INVALID_SAMD_PIN         255


class SAMD_Slow_PWM_ISR 
{

  public:
    // maximum number of PWM channels
#define MAX_NUMBER_CHANNELS        16

    // constructor
    SAMD_Slow_PWM_ISR();

    void init();

    // this function must be called inside loop()
    void run();
    
    //////////////////////////////////////////////////////////////////
    // PWM
    void setPWM(uint32_t pin, uint32_t frequency, uint32_t dutycycle, timer_callback StartCallback = nullptr,
                timer_callback StopCallback = nullptr)
    {
      uint32_t period = 0;
      
      if ( ( frequency != 0 ) && ( frequency <= 1000 ) )
      {
#if USING_MICROS_RESOLUTION
      // period in us
      period = 1000000 / frequency;
#else    
      // period in ms
      period = 1000 / frequency;
#endif
      }
      else
      {       
        PWM_LOGERROR("Error: Invalid frequency, max is 500Hz");
      }
      
      setupPWMChannel(pin, period, dutycycle, (void *) StartCallback, (void *) StopCallback);     
    }

#if USING_MICROS_RESOLUTION
    //period in us
    void setPWM_Period(uint32_t pin, uint32_t period, uint32_t dutycycle, timer_callback StartCallback = nullptr,
                       timer_callback StopCallback = nullptr)
#else    
    // PWM
    //period in ms
    void setPWM_Period(uint32_t pin, uint32_t period, uint32_t dutycycle, timer_callback StartCallback = nullptr,
                       timer_callback StopCallback = nullptr)
#endif    
    {     
      setupPWMChannel(pin, period, dutycycle, (void *) StartCallback, (void *) StopCallback);       
    }    
    
    //////////////////////////////////////////////////////////////////

    // destroy the specified PWM channel
    void deleteChannel(unsigned channelNum);

    // restart the specified PWM channel
    void restartChannel(unsigned channelNum);

    // returns true if the specified PWM channel is enabled
    bool isEnabled(unsigned channelNum);

    // enables the specified PWM channel
    void enable(unsigned channelNum);

    // disables the specified PWM channel
    void disable(unsigned channelNum);

    // enables all PWM channels
    void enableAll();

    // disables all PWM channels
    void disableAll();

    // enables the specified PWM channel if it's currently disabled, and vice-versa
    void toggle(unsigned channelNum);

    // returns the number of used PWM channels
    unsigned getnumChannels();

    // returns the number of available PWM channels
    unsigned getNumAvailablePWMChannels() 
    {
      return MAX_NUMBER_CHANNELS - numChannels;
    };

  private:

    // low level function to initialize and enable a new PWM channel
    // returns the PWM channel number (channelNum) on success or
    // -1 on failure (f == NULL) or no free PWM channels 
    int setupPWMChannel(uint32_t pin, uint32_t period, uint32_t dutycycle, void* cbStartFunc = nullptr, void* cbStopFunc = nullptr);

    // find the first available slot
    int findFirstFreeSlot();

    typedef struct 
    {
      ///////////////////////////////////
      
      
      ///////////////////////////////////
      
      uint64_t      prevTime;           // value returned by the micros() or millis() function in the previous run() call
      uint32_t      period;             // period value, in us / ms
      uint32_t      onTime;             // onTime value, ( period * dutyCycle / 100 ) us  / ms
      
      void*         callbackStart;      // pointer to the callback function when PWM pulse starts (HIGH)
      void*         callbackStop;       // pointer to the callback function when PWM pulse stops (LOW)
      
      ////////////////////////////////////////////////////////////
      
      uint32_t      pin;                // PWM pin
      bool          pinHigh;            // true if PWM pin is HIGH
      ////////////////////////////////////////////////////////////
      
      bool          enabled;            // true if enabled
    } PWM_t;

    volatile PWM_t PWM[MAX_NUMBER_CHANNELS];

    // actual number of PWM channels in use (-1 means uninitialized)
    volatile int numChannels;
};


#endif    // SAMD_SLOW_PWM_ISR_H


