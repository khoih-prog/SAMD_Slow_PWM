/****************************************************************************************************************************
  SAMD_Slow_PWM_ISR.hpp
  For SAMD21/SAMD51 boards
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/SAMD_Slow_PWM
  Licensed under MIT license

  Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
  unsigned long miliseconds), you just consume only one SAMD timer and avoid conflicting with other cores' tasks.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.

  Version: 1.2.3

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K.Hoang      01/10/2021 Initial coding for SAMD21/SAMD51 boards
  1.1.0   K Hoang      10/11/2021 Add functions to modify PWM settings on-the-fly
  1.2.0   K Hoang      31/01/2022 Fix multiple-definitions linker error. Improve accuracy. Change DutyCycle update
  1.2.1   K Hoang      01/02/2022 Use float for DutyCycle and Freq, uint32_t for period
  1.2.2   K Hoang      02/03/2022 Use correct PWM_Generic_Debug.h file. Display informational warning when debug level > 3
  1.2.3   K Hoang      03/03/2022 Fix `DutyCycle` and `New Period` display bugs
*****************************************************************************************************************************/

#pragma once

#ifndef SAMD_SLOW_PWM_ISR_HPP
#define SAMD_SLOW_PWM_ISR_HPP

#if !( defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_MKRWIFI1010) \
      || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_SAMD_MKRFox1200) || defined(ARDUINO_SAMD_MKRWAN1300) || defined(ARDUINO_SAMD_MKRWAN1310) \
      || defined(ARDUINO_SAMD_MKRGSM1400) || defined(ARDUINO_SAMD_MKRNB1500) || defined(ARDUINO_SAMD_MKRVIDOR4000) \
      || defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(__SAMD51__) || defined(__SAMD51J20A__) \
      || defined(__SAMD51J19A__) || defined(__SAMD51G19A__) || defined(__SAMD51P19A__)  \
      || defined(__SAMD21E15A__) || defined(__SAMD21E16A__) || defined(__SAMD21E17A__) || defined(__SAMD21E18A__) \
      || defined(__SAMD21G15A__) || defined(__SAMD21G16A__) || defined(__SAMD21G17A__) || defined(__SAMD21G18A__) \
      || defined(__SAMD21J15A__) || defined(__SAMD21J16A__) || defined(__SAMD21J17A__) || defined(__SAMD21J18A__) )
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
  #define SAMD_SLOW_PWM_VERSION             "SAMD_Slow_PWM v1.2.1"
  
  #define SAMD_SLOW_PWM_VERSION_MAJOR       1
  #define SAMD_SLOW_PWM_VERSION_MINOR       2
  #define SAMD_SLOW_PWM_VERSION_PATCH       1

  #define SAMD_SLOW_PWM_VERSION_INT         1002001
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

  #if (_PWM_LOGLEVEL_ > 3)
    #warning Not USING_MICROS_RESOLUTION, using millis resolution
  #endif
    
  #define USING_MICROS_RESOLUTION       false
#endif

#if !defined(CHANGING_PWM_END_OF_CYCLE)
  #if (_PWM_LOGLEVEL_ > 3)
    #warning Using default CHANGING_PWM_END_OF_CYCLE == true
  #endif
  
  #define CHANGING_PWM_END_OF_CYCLE     true
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
    // Return the channelNum if OK, -1 if error
    int setPWM(const uint32_t& pin, const float& frequency, const float& dutycycle, timer_callback StartCallback = nullptr, 
                timer_callback StopCallback = nullptr)
    {
      uint32_t period = 0;
      
      if ( ( frequency > 0.0 ) && ( frequency <= 1000.0 ) )
      {
#if USING_MICROS_RESOLUTION
      // period in us
      period = (uint32_t) (1000000.0f / frequency);
#else    
      // period in ms
      period = (uint32_t) (1000.0f / frequency);
#endif
      }
      else
      {       
        PWM_LOGERROR("Error: Invalid frequency, max is 1000Hz");
        
        return -1;
      }
      
      return setupPWMChannel(pin, period, dutycycle, (void *) StartCallback, (void *) StopCallback);   
    }

    // period in us
    // Return the channelNum if OK, -1 if error
    int setPWM_Period(const uint32_t& pin, const uint32_t& period, const float& dutycycle, 
                      timer_callback StartCallback = nullptr, timer_callback StopCallback = nullptr)  
    {           
      return setupPWMChannel(pin, period, dutycycle, (void *) StartCallback, (void *) StopCallback);      
    } 
    
    //////////////////////////////////////////////////////////////////
    
    // low level function to modify a PWM channel
    // returns the true on success or false on failure
    bool modifyPWMChannel(const uint8_t& channelNum, const uint32_t& pin, const float& frequency, const float& dutycycle)
    {
      uint32_t period = 0;
      
      if ( ( frequency > 0.0 ) && ( frequency <= 1000.0 ) )
      {
#if USING_MICROS_RESOLUTION
      // period in us
      period = (uint32_t) (1000000.0f / frequency);
#else    
      // period in ms
      period = (uint32_t) (1000.0f / frequency);
#endif
      }
      else
      {       
        PWM_LOGERROR("Error: Invalid frequency, max is 1000Hz");
        return false;
      }
           
      return modifyPWMChannel_Period(channelNum, pin, period, dutycycle);
    }
    
    //////////////////////////////////////////////////////////////////
    
    //period in us
    bool modifyPWMChannel_Period(const uint8_t& channelNum, const uint32_t& pin, const uint32_t& period, const float& dutycycle);
    
    //////////////////////////////////////////////////////////////////

    // destroy the specified PWM channel
    void deleteChannel(const uint8_t& channelNum);

    // restart the specified PWM channel
    void restartChannel(const uint8_t& channelNum);

    // returns true if the specified PWM channel is enabled
    bool isEnabled(const uint8_t& channelNum);

    // enables the specified PWM channel
    void enable(const uint8_t& channelNum);

    // disables the specified PWM channel
    void disable(const uint8_t& channelNum);

    // enables all PWM channels
    void enableAll();

    // disables all PWM channels
    void disableAll();

    // enables the specified PWM channel if it's currently disabled, and vice-versa
    void toggle(const uint8_t& channelNum);

    // returns the number of used PWM channels
    int8_t getnumChannels();

    // returns the number of available PWM channels
    uint8_t getNumAvailablePWMChannels() 
    {
      if (numChannels <= 0)
        return MAX_NUMBER_CHANNELS;
      else 
        return MAX_NUMBER_CHANNELS - numChannels;
    };

  private:

    // low level function to initialize and enable a new PWM channel
    // returns the PWM channel number (channelNum) on success or
    // -1 on failure (f == NULL) or no free PWM channels 
    int setupPWMChannel(const uint32_t& pin, const uint32_t& period, const float& dutycycle, void* cbStartFunc = nullptr, void* cbStopFunc = nullptr);

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
      
      // New from v1.2.0     
      uint32_t      newPeriod;          // period value, in us / ms
      uint32_t      newOnTime;          // onTime value, ( period * dutyCycle / 100 ) us  / ms
      float         newDutyCycle;       // from 0.00 to 100.00, float precision
    } PWM_t;

    volatile PWM_t PWM[MAX_NUMBER_CHANNELS];

    // actual number of PWM channels in use (-1 means uninitialized)
    volatile int8_t numChannels;
};


#endif    // SAMD_SLOW_PWM_ISR_HPP


