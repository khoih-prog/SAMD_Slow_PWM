/****************************************************************************************************************************
  SAMD_Slow_PWM.hpp
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

#ifndef SAMD_TIMERINTERRUPT_HPP
#define SAMD_TIMERINTERRUPT_HPP

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

#if ( defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) \
   || defined(ARDUINO_SAMD_MKRFox1200) || defined(ARDUINO_SAMD_MKRWAN1300) || defined(ARDUINO_SAMD_MKRWAN1310) || defined(ARDUINO_SAMD_MKRGSM1400) \
   || defined(ARDUINO_SAMD_MKRNB1500) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) \
   || defined(__SAMD21E15A__) || defined(__SAMD21E16A__) || defined(__SAMD21E17A__) || defined(__SAMD21E18A__) \
   || defined(__SAMD21G15A__) || defined(__SAMD21G16A__) || defined(__SAMD21G17A__) || defined(__SAMD21G18A__) \
   || defined(__SAMD21J15A__) || defined(__SAMD21J16A__) || defined(__SAMD21J17A__) || defined(__SAMD21J18A__) || defined(__SAMD21__) )
     
  #define TIMER_INTERRUPT_USING_SAMD21      true
   
  #if !defined(BOARD_NAME)
  
    #if defined(ARDUINO_QWIIC_MICRO)
      #define BOARD_NAME    "Sparkfun SAMD21_QWIIC_MICRO"
      
      #if (_PWM_LOGLEVEL_ > 3)
        #warning BOARD_NAME == Sparkfun SAMD21_QWIIC_MICRO
      #endif
      
    #elif defined(__SAMD21E15A__)
      #define BOARD_NAME    "__SAMD21E15A__"
    #elif defined(__SAMD21E16A__)
      #define BOARD_NAME    "__SAMD21E16A__"
    #elif defined(__SAMD21E17A__)
      #define BOARD_NAME    "__SAMD21E17A__"
    #elif defined(__SAMD21E18A__)
      #define BOARD_NAME    "__SAMD21E18A__"
    #elif defined(__SAMD21G15A__)
      #define BOARD_NAME    "__SAMD21G15A__"
    #elif defined(__SAMD21G16A__)
      #define BOARD_NAME    "__SAMD21G16A__"
    #elif defined(__SAMD21G17A__)
      #define BOARD_NAME    "__SAMD21G17A__"
    #elif defined(__SAMD21G18A__)
      #define BOARD_NAME    "__SAMD21G18A__"
    #elif defined(__SAMD21J15A__)
      #define BOARD_NAME    "__SAMD21J15A__"
    #elif defined(__SAMD21J16A__)
      #define BOARD_NAME    "__SAMD21J16A__"
    #elif defined(__SAMD21J17A__)
      #define BOARD_NAME    "__SAMD21J17A__"
    #elif defined(__SAMD21J18A__)
      #define BOARD_NAME    "__SAMD21J18A__" 
    #else
      #define BOARD_NAME    "Unknown SAMD21"   
    #endif
    
  #endif
  
  #if (_PWM_LOGLEVEL_ > 3)
    #warning Using SAMD21 Hardware Timer
  #endif

#elif ( defined(__SAMD51__) || defined(__SAMD51J20A__) || defined(__SAMD51J19A__) || defined(__SAMD51G19A__) || defined(__SAMD51P19A__) )
  #define TIMER_INTERRUPT_USING_SAMD51      true
   
  #if !defined(BOARD_NAME)
  
    #if defined(ARDUINO_SAMD51_THING_PLUS)
      #define BOARD_NAME    "Sparkfun SAMD51_THING_PLUS"
      
      #if (_PWM_LOGLEVEL_ > 3)
        #warning BOARD_NAME == Sparkfun SAMD51_THING_PLUS
      #endif
      
    #elif defined(ARDUINO_SAMD51_MICROMOD)
      #define BOARD_NAME    "Sparkfun SAMD51_MICROMOD"
      
      #if (_PWM_LOGLEVEL_ > 3)
        #warning BOARD_NAME == Sparkfun SAMD51_MICROMOD
      #endif
      
    #elif defined(__SAMD51J20A__)
      #define BOARD_NAME    "__SAMD51J20A__"
    #elif defined(__SAMD51J19A__)
      #define BOARD_NAME    "__SAMD51J19A__"
    #elif defined(__SAMD51G19A__)
      #define BOARD_NAME    "__SAMD51G19A__"
    #elif defined(__SAMD51P19A__)
      #define BOARD_NAME    "__SAMD51P19A__"      
    #else
      #define BOARD_NAME    "Unknown SAMD51"    
    #endif
    
  #endif
  
  #if (_PWM_LOGLEVEL_ > 3)
    #warning Using SAMD51 Hardware Timer
  #endif
  
#else
  #error Unknown board  
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
  #define SAMD_SLOW_PWM_VERSION             "SAMD_Slow_PWM v1.2.3"
  
  #define SAMD_SLOW_PWM_VERSION_MAJOR       1
  #define SAMD_SLOW_PWM_VERSION_MINOR       2
  #define SAMD_SLOW_PWM_VERSION_PATCH       3

  #define SAMD_SLOW_PWM_VERSION_INT         1002003
#endif

#include "PWM_Generic_Debug.h"

#define TIMER_HZ      48000000L

////////////////////////////////////////////////////

#if (TIMER_INTERRUPT_USING_SAMD51)

typedef enum
{
  TIMER_TC3 = 0,
  MAX_TIMER
} SAMDTimerNumber;

class SAMDTimerInterrupt;

typedef SAMDTimerInterrupt SAMDTimer;

typedef void (*timerCallback)  ();

#define SAMD_TC3        ((TcCount16*) _SAMDTimer)

static inline void TC3_wait_for_sync() 
{
  while (TC3->COUNT16.SYNCBUSY.reg != 0);
}

class SAMDTimerInterrupt
{
  private:
    SAMDTimerNumber _timerNumber;
    
    // point to timer struct, (TcCount16*) TC3 for SAMD51
    void*           _SAMDTimer = NULL;
    
    timerCallback   _callback;        // pointer to the callback function
    float           _frequency;       // Timer frequency
    
    float           _period;
    int             _prescaler;
    int             _compareValue;

  public:

    SAMDTimerInterrupt(const SAMDTimerNumber& timerNumber)
    {
      _timerNumber = timerNumber;
           
      if (_timerNumber == TIMER_TC3)
      {
        _SAMDTimer = (TcCount16*) TC3;    
      }
      
      _callback = NULL;      
    };
    
    ~SAMDTimerInterrupt()
    {
    }

    bool setFrequency(const float& frequency, timerCallback callback);

    // interval (in microseconds) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be addes in the future by adding similar functions here or to SAMD-hal-timer.c
    bool setInterval(const unsigned long& interval, timerCallback callback)
    {
      return setFrequency((float) (1000000.0f / interval), callback);
    }

    bool attachInterrupt(const float& frequency, timerCallback callback)
    {
      return setFrequency(frequency, callback);
    }

    // interval (in microseconds) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be addes in the future by adding similar functions here or to SAMD-hal-timer.c
    bool attachInterruptInterval(const unsigned long& interval, timerCallback callback)
    {
      return setFrequency( (float) ( 1000000.0f / interval), callback);
    }

    void detachInterrupt()
    {
      // Disable Interrupt
      if (_timerNumber == TIMER_TC3)
      {
        NVIC_DisableIRQ(TC3_IRQn); 
      }
    }

    void disableTimer()
    {
      // Disable Timer
      if (_timerNumber == TIMER_TC3)
      {        
        // Disable TC3
        TC3->COUNT16.CTRLA.bit.ENABLE = 0;
      }
    }

    // Duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    void reattachInterrupt()
    {
      // Disable Interrupt
      if (_timerNumber == TIMER_TC3)
      {
        NVIC_EnableIRQ(TC3_IRQn); 
      }
    }

    // Duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    void enableTimer()
    {     
      // Enable Timer
      if (_timerNumber == TIMER_TC3)
      {       
        // Enable TC3
        SAMD_TC3->CTRLA.reg |= TC_CTRLA_ENABLE;
      }
    }

    // Just stop clock source, clear the count
    void stopTimer()
    {
      // TODO, clear the count
      disableTimer();
    }

    // Just reconnect clock source, start current count from 0
    void restartTimer()
    {
      // TODO, clear the count
      enableTimer();
    }
    
    private:
    
    void setPeriod_TIMER_TC3(const float& period)
    {
      uint32_t TC_CTRLA_PRESCALER_DIVN = 1;

      TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
      TC3_wait_for_sync();
      TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_PRESCALER_DIV1024;
      TC3_wait_for_sync();
      TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_PRESCALER_DIV256;
      TC3_wait_for_sync();
      TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_PRESCALER_DIV64;
      TC3_wait_for_sync();
      TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_PRESCALER_DIV16;
      TC3_wait_for_sync();
      TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_PRESCALER_DIV4;
      TC3_wait_for_sync();
      TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_PRESCALER_DIV2;
      TC3_wait_for_sync();
      TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_PRESCALER_DIV1;
      TC3_wait_for_sync();

      if (period > 300000) 
      {
        TC_CTRLA_PRESCALER_DIVN = TC_CTRLA_PRESCALER_DIV1024;
        _prescaler = 1024;
      } 
      else if (80000 < period && period <= 300000) 
      {
        TC_CTRLA_PRESCALER_DIVN = TC_CTRLA_PRESCALER_DIV256;
        _prescaler = 256;
      } 
      else if (20000 < period && period <= 80000) 
      {
        TC_CTRLA_PRESCALER_DIVN = TC_CTRLA_PRESCALER_DIV64;
        _prescaler = 64;
      } 
      else if (10000 < period && period <= 20000) 
      {
        TC_CTRLA_PRESCALER_DIVN = TC_CTRLA_PRESCALER_DIV16;
        _prescaler = 16;
      } 
      else if (5000 < period && period <= 10000) 
      {
        TC_CTRLA_PRESCALER_DIVN = TC_CTRLA_PRESCALER_DIV8;
        _prescaler = 8;
      } 
      else if (2500 < period && period <= 5000) 
      {
        TC_CTRLA_PRESCALER_DIVN = TC_CTRLA_PRESCALER_DIV4;
        _prescaler = 4;
      } 
      else if (1000 < period && period <= 2500) {
        TC_CTRLA_PRESCALER_DIVN = TC_CTRLA_PRESCALER_DIV2;
        _prescaler = 2;
      } 
      else if (period <= 1000) 
      {
        TC_CTRLA_PRESCALER_DIVN = TC_CTRLA_PRESCALER_DIV1;
        _prescaler = 1;
      }
      
      TC3->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIVN;
      TC3_wait_for_sync();

      _compareValue = (int)(TIMER_HZ / (_prescaler/(period / 1000000.0))) - 1;

      // Make sure the count is in a proportional position to where it was
      // to prevent any jitter or disconnect when changing the compare value.
      TC3->COUNT16.COUNT.reg = map(TC3->COUNT16.COUNT.reg, 0,
                                   TC3->COUNT16.CC[0].reg, 0, _compareValue);
      TC3->COUNT16.CC[0].reg = _compareValue;
      TC3_wait_for_sync();

      TC3->COUNT16.CTRLA.bit.ENABLE = 1;
      TC3_wait_for_sync();
      
      PWM_LOGDEBUG3(F("SAMD51 TC3 period ="), period, F(", _prescaler ="), _prescaler);
      PWM_LOGDEBUG1(F("_compareValue ="), _compareValue);
    }
}; // class SAMDTimerInterrupt


////////////////////////////////////////////////////////

#elif (TIMER_INTERRUPT_USING_SAMD21)

typedef enum
{
  TIMER_TC3 = 0,
  TIMER_TCC = 1,
  MAX_TIMER
} SAMDTimerNumber;

class SAMDTimerInterrupt;

typedef SAMDTimerInterrupt SAMDTimer;

typedef void (*timerCallback)  ();

#define SAMD_TC3        ((TcCount16*) _SAMDTimer)
#define SAMD_TCC        ((Tcc*) _SAMDTimer)

class SAMDTimerInterrupt
{
  private:
    SAMDTimerNumber _timerNumber;
    
    // point to timer struct, (TcCount16*) TC3 or (Tcc*) TCC0 for SAMD21
    void*           _SAMDTimer = NULL;
    
    timerCallback   _callback;        // pointer to the callback function
    float           _frequency;       // Timer frequency
    //uint32_t        _timerCount;      // count to activate timer
    
    float           _period;
    int             _prescaler;
    int             _compareValue;

  public:

    SAMDTimerInterrupt(const SAMDTimerNumber& timerNumber)
    {
      _timerNumber = timerNumber;
           
      if (_timerNumber == TIMER_TC3)
      {
        _SAMDTimer = (TcCount16*) TC3;    
      }
      else if (_timerNumber == TIMER_TCC)
      {
        _SAMDTimer = (Tcc*) TCC0;        
      }
      
      _callback = NULL;      
    };
    
    ~SAMDTimerInterrupt()
    {
    }
   
    bool setFrequency(const float& frequency, timerCallback callback);

    // interval (in microseconds) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be addes in the future by adding similar functions here or to SAMD-hal-timer.c
    bool setInterval(const unsigned long& interval, timerCallback callback)
    {
      return setFrequency((float) (1000000.0f / interval), callback);
    }

    bool attachInterrupt(const float& frequency, timerCallback callback)
    {
      return setFrequency(frequency, callback);
    }

    // interval (in microseconds) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be addes in the future by adding similar functions here or to SAMD-hal-timer.c
    bool attachInterruptInterval(const unsigned long& interval, timerCallback callback)
    {
      return setFrequency( (float) ( 1000000.0f / interval), callback);
    }

    void detachInterrupt()
    {
      // Disable Interrupt
      if (_timerNumber == TIMER_TC3)
      {
        NVIC_DisableIRQ(TC3_IRQn); 
      }
      else if (_timerNumber == TIMER_TCC)
      {
        NVIC_DisableIRQ(TCC0_IRQn);     
      }
    }

    void disableTimer()
    {
      // Disable Timer
      if (_timerNumber == TIMER_TC3)
      {        
        // Disable TC3
        SAMD_TC3->CTRLA.reg &= ~TC_CTRLA_ENABLE;
      }
      else if (_timerNumber == TIMER_TCC)
      {       
        // Disable TCC
        SAMD_TCC->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
      }
    }

    // Duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    void reattachInterrupt()
    {
      // Disable Interrupt
      if (_timerNumber == TIMER_TC3)
      {
        NVIC_EnableIRQ(TC3_IRQn); 
      }
      else if (_timerNumber == TIMER_TCC)
      {
        NVIC_EnableIRQ(TCC0_IRQn);     
      }
    }

    // Duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    void enableTimer()
    {     
      // Enable Timer
      if (_timerNumber == TIMER_TC3)
      {       
        // Enable TC3
        SAMD_TC3->CTRLA.reg |= TC_CTRLA_ENABLE;
      }
      else if (_timerNumber == TIMER_TCC)
      {        
        // Enable TCC
        SAMD_TCC->CTRLA.reg |= TCC_CTRLA_ENABLE;
      }
    }

    // Just stop clock source, clear the count
    void stopTimer()
    {
      // TODO, clear the count
      disableTimer();
    }

    // Just reconnect clock source, start current count from 0
    void restartTimer()
    {
      // TODO, clear the count
      enableTimer();
    }
    
    private:
    
    void setPeriod_TIMER_TC3(const float& period)
    {
      TcCount16* _Timer = (TcCount16*) _SAMDTimer;
      
      _Timer->CTRLA.reg &= ~TC_CTRLA_ENABLE;
      while (_Timer->STATUS.bit.SYNCBUSY == 1);
      _Timer->CTRLA.reg &= ~TC_CTRLA_PRESCALER_DIV1024;
      while (_Timer->STATUS.bit.SYNCBUSY == 1);
      _Timer->CTRLA.reg &= ~TC_CTRLA_PRESCALER_DIV256;
      while (_Timer->STATUS.bit.SYNCBUSY == 1);
      _Timer->CTRLA.reg &= ~TC_CTRLA_PRESCALER_DIV64;
      while (_Timer->STATUS.bit.SYNCBUSY == 1);
      _Timer->CTRLA.reg &= ~TC_CTRLA_PRESCALER_DIV16;
      while (_Timer->STATUS.bit.SYNCBUSY == 1);
      _Timer->CTRLA.reg &= ~TC_CTRLA_PRESCALER_DIV4;
      while (_Timer->STATUS.bit.SYNCBUSY == 1);
      _Timer->CTRLA.reg &= ~TC_CTRLA_PRESCALER_DIV2;
      while (_Timer->STATUS.bit.SYNCBUSY == 1);
      _Timer->CTRLA.reg &= ~TC_CTRLA_PRESCALER_DIV1;
      while (_Timer->STATUS.bit.SYNCBUSY == 1);
      
	    if (period > 300000) 
	    {
		    // Set prescaler to 1024
		    _Timer->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1024;
		    _prescaler = 1024;
	    } 
	    else if (80000 < period && period <= 300000) 
	    {
		    // Set prescaler to 256
		    _Timer->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV256;
		    _prescaler = 256;
	    } 
	    else if (20000 < period && period <= 80000) 
	    {
		    // Set prescaler to 64
		    _Timer->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV64;
		    _prescaler = 64;
	    } 
	    else if (10000 < period && period <= 20000) 
	    {
		    // Set prescaler to 16
		    _Timer->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV16;
		    _prescaler = 16;
	    } 
	    else if (5000 < period && period <= 10000) 
	    {
		    // Set prescaler to 8
		    _Timer->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV8;
		    _prescaler = 8;
	    } 
	    else if (2500 < period && period <= 5000) 
	    {
		    // Set prescaler to 4
		    _Timer->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV4;
		    _prescaler = 4;
	    } 
	    else if (1000 < period && period <= 2500) 
	    {
		    // Set prescaler to 2
		    _Timer->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV2;
		    _prescaler = 2;
	    } 
	    else if (period <= 1000) 
	    {
		    // Set prescaler to 1
		    _Timer->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1;
		    _prescaler = 1;
	    }
	    
	    while (_Timer->STATUS.bit.SYNCBUSY == 1);

	    _compareValue = (int)(TIMER_HZ / (_prescaler / (period / 1000000.0))) - 1;

      
      // Make sure the count is in a proportional position to where it was
      // to prevent any jitter or disconnect when changing the compare value.
      _Timer->COUNT.reg = map(_Timer->COUNT.reg, 0, _Timer->CC[0].reg, 0, _compareValue);
      _Timer->CC[0].reg = _compareValue;
      
      while (_Timer->STATUS.bit.SYNCBUSY == 1);
      
      PWM_LOGDEBUG3(F("SAMD21 TC3 period ="), period, F(", _prescaler ="), _prescaler);
      PWM_LOGDEBUG1(F("_compareValue ="), _compareValue);
    }
    
    void setPeriod_TIMER_TCC(const float& period)
    {
      Tcc* _Timer = (Tcc*) _SAMDTimer;
      
      _Timer->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
      while (_Timer->SYNCBUSY.bit.ENABLE == 1);
      _Timer->CTRLA.reg &= ~TCC_CTRLA_PRESCALER_DIV1024;
      while (_Timer->SYNCBUSY.bit.ENABLE == 1);
      _Timer->CTRLA.reg &= ~TCC_CTRLA_PRESCALER_DIV256;
      while (_Timer->SYNCBUSY.bit.ENABLE == 1);
      _Timer->CTRLA.reg &= ~TCC_CTRLA_PRESCALER_DIV64;
      while (_Timer->SYNCBUSY.bit.ENABLE == 1);
      _Timer->CTRLA.reg &= ~TCC_CTRLA_PRESCALER_DIV16;
      while (_Timer->SYNCBUSY.bit.ENABLE == 1);
      _Timer->CTRLA.reg &= ~TCC_CTRLA_PRESCALER_DIV4;
      while (_Timer->SYNCBUSY.bit.ENABLE == 1);
      _Timer->CTRLA.reg &= ~TCC_CTRLA_PRESCALER_DIV2;
      while (_Timer->SYNCBUSY.bit.ENABLE == 1);
      _Timer->CTRLA.reg &= ~TCC_CTRLA_PRESCALER_DIV1;
      while (_Timer->SYNCBUSY.bit.ENABLE == 1);
      
	    if (period > 300000) 
	    {
		    // Set prescaler to 1024
		    _Timer->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV1024;
		    _prescaler = 1024;
	    } 
	    else if (80000 < period && period <= 300000) 
	    {
		    // Set prescaler to 256
		    _Timer->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV256;
		    _prescaler = 256;
	    } 
	    else if (20000 < period && period <= 80000) 
	    {
		    // Set prescaler to 64
		    _Timer->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV64;
		    _prescaler = 64;
	    } 
	    else if (10000 < period && period <= 20000) 
	    {
		    // Set prescaler to 16
		    _Timer->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV16;
		    _prescaler = 16;
	    } 
	    else if (5000 < period && period <= 10000) 
	    {
		    // Set prescaler to 8
		    _Timer->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV8;
		    _prescaler = 8;
	    } 
	    else if (2500 < period && period <= 5000) 
	    {
		    // Set prescaler to 4
		    _Timer->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV4;
		    _prescaler = 4;
	    } 
	    else if (1000 < period && period <= 2500) 
	    {
		    // Set prescaler to 2
		    _Timer->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV2;
		    _prescaler = 2;
	    } 
	    else if (period <= 1000) 
	    {
		    // Set prescaler to 1
		    _Timer->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV1;
		    _prescaler = 1;
	    }
	    
	    _compareValue = (int)(TIMER_HZ / (_prescaler / (period / 1000000))) - 1;

	    _Timer->PER.reg = _compareValue; 
	    
      while (_Timer->SYNCBUSY.bit.PER == 1);

      // Make sure the count is in a proportional position to where it was
      // to prevent any jitter or disconnect when changing the compare value.
      //_Timer->COUNT.reg = map(_Timer->COUNT.reg, 0, _Timer->CC[0].reg, 0, _compareValue);

      _Timer->CC[0].reg = 0xFFF;
	    //_Timer->CC[0].reg = _compareValue;
	    
      while (_Timer->SYNCBUSY.bit.CC0 == 1);
      
      PWM_LOGDEBUG3(F("SAMD21 TCC period ="), period, F(", _prescaler ="), _prescaler);
      PWM_LOGDEBUG1(F("_compareValue ="), _compareValue);
    } 
}; // class SAMDTimerInterrupt

#endif    // #if (TIMER_INTERRUPT_USING_SAMD51)

#endif    // SAMD_SLOW_PWM_HPP

