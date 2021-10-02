/****************************************************************************************************************************
  SAMD_Slow_PWM.h
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

#ifndef SAMD_SLOW_PWM_H
#define SAMD_SLOW_PWM_H

#if !( defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_MKRWIFI1010) \
    || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_SAMD_MKRFox1200) || defined(ARDUINO_SAMD_MKRWAN1300) || defined(ARDUINO_SAMD_MKRWAN1310) \
    || defined(ARDUINO_SAMD_MKRGSM1400) || defined(ARDUINO_SAMD_MKRNB1500) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(__SAMD21G18A__) \
    || defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(__SAMD21E18A__) || defined(__SAMD51__) || defined(__SAMD51J20A__) || defined(__SAMD51J19A__) \
    || defined(__SAMD51G19A__) || defined(__SAMD51P19A__) || defined(__SAMD21G18A__) )
  #error This code is designed to run on SAMD21/SAMD51 platform! Please check your Tools->Board setting.
#endif

#if ( defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_MKRWIFI1010) \
    || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_SAMD_MKRFox1200) || defined(ARDUINO_SAMD_MKRWAN1300) || defined(ARDUINO_SAMD_MKRWAN1310) \
    || defined(ARDUINO_SAMD_MKRGSM1400) || defined(ARDUINO_SAMD_MKRNB1500) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(__SAMD21G18A__) \
    || defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(__SAMD21E18A__) || defined(__SAMD21G18A__) )
     
  #define TIMER_INTERRUPT_USING_SAMD21      true
  
  #if defined(ARDUINO_QWIIC_MICRO)
    #define BOARD_NAME    "Sparkfun SAMD21_QWIIC_MICRO"
    #warning BOARD_NAME == Sparkfun SAMD21_QWIIC_MICRO
  #endif
  
  #if !defined(BOARD_NAME)
    #define BOARD_NAME    "Unknown SAMD21"
  #endif
  
  #warning Using SAMD21 Hardware Timer
#elif ( defined(__SAMD51__) || defined(__SAMD51J20A__) || defined(__SAMD51J19A__) || defined(__SAMD51G19A__) || defined(__SAMD51P19A__) )
  #define TIMER_INTERRUPT_USING_SAMD51      true
  
  #if defined(ARDUINO_SAMD51_THING_PLUS)
    #define BOARD_NAME    "Sparkfun SAMD51_THING_PLUS"
    #warning BOARD_NAME == Sparkfun SAMD51_THING_PLUS
  #elif defined(ARDUINO_SAMD51_MICROMOD)
    #define BOARD_NAME    "Sparkfun SAMD51_MICROMOD"
    #warning BOARD_NAME == Sparkfun SAMD51_MICROMOD
  #endif
  
  #if !defined(BOARD_NAME)
    #define BOARD_NAME    "Unknown SAMD51"
  #endif
  
  #warning Using SAMD51 Hardware Timer
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
  #define SAMD_SLOW_PWM_VERSION       "SAMD_Slow_PWM v1.0.0"
#endif

///////////////////////////////////////////


#include "PWM_Generic_Debug.h"

class SAMDTimerInterrupt;

typedef SAMDTimerInterrupt SAMDTimer;

typedef void (*timerCallback)  ();


#define TIMER_HZ      48000000L

////////////////////////////////////////////////////

#if (TIMER_INTERRUPT_USING_SAMD51)

typedef enum
{
  TIMER_TC3 = 0,
  MAX_TIMER
} SAMDTimerNumber;


timerCallback TC3_callback;

#define SAMD_TC3        ((TcCount16*) _SAMDTimer)

static inline void TC3_wait_for_sync() 
{
  while (TC3->COUNT16.SYNCBUSY.reg != 0);
}

void TC3_Handler() 
{
  // If this interrupt is due to the compare register matching the timer count
  if (TC3->COUNT16.INTFLAG.bit.MC0 == 1) 
  {
    TC3->COUNT16.INTFLAG.bit.MC0 = 1;
    (*TC3_callback)();
  }
}


class SAMDTimerInterrupt
{
  private:
    SAMDTimerNumber _timerNumber;
    
    // point to timer struct, (TcCount16*) TC3 for SAMD51
    void*           _SAMDTimer = NULL;
    
    timerCallback  _callback;        // pointer to the callback function
    float           _frequency;       // Timer frequency
    
    unsigned long   _period;
    int             _prescaler;
    int             _compareValue;

  public:

    SAMDTimerInterrupt(SAMDTimerNumber timerNumber)
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

    // frequency (in hertz) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be addes in the future by adding similar functions here or to SAMD-hal-timer.c
    bool setFrequency(float frequency, timerCallback callback)
    {
      _period = (unsigned long) (1000000.0f / frequency);
      
      if (_timerNumber == TIMER_TC3)
      {    
        PWM_LOGWARN3(F("SAMDTimerInterrupt: F_CPU (MHz) ="), F_CPU/1000000, F(", TIMER_HZ ="), TIMER_HZ/1000000);
        PWM_LOGWARN3(F("TC_Timer::startTimer _Timer = 0x"), String((uint32_t) _SAMDTimer, HEX), F(", TC3 = 0x"), String((uint32_t) TC3, HEX));

        // Enable the TC bus clock, use clock generator 0
        GCLK->PCHCTRL[TC3_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK1_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);
        
        while (GCLK->SYNCBUSY.reg > 0);

        TC3->COUNT16.CTRLA.bit.ENABLE = 0;
        
        // Use match mode so that the timer counter resets when the count matches the
        // compare register
        TC3->COUNT16.WAVE.bit.WAVEGEN = TC_WAVE_WAVEGEN_MFRQ;
        TC3_wait_for_sync();
        
         // Enable the compare interrupt
        TC3->COUNT16.INTENSET.reg = 0;
        TC3->COUNT16.INTENSET.bit.MC0 = 1;

        // Enable IRQ
        NVIC_EnableIRQ(TC3_IRQn);

        //func1 = f;
        _callback     = callback;
	      TC3_callback  = callback;

        //setPeriod(period);
        setPeriod_TIMER_TC3(_period);
        
        return true;
	    }
	    else
	      return false;
    }

    // interval (in microseconds) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be addes in the future by adding similar functions here or to SAMD-hal-timer.c
    bool setInterval(unsigned long interval, timerCallback callback)
    {
      return setFrequency((float) (1000000.0f / interval), callback);
    }

    bool attachInterrupt(float frequency, timerCallback callback)
    {
      return setFrequency(frequency, callback);
    }

    // interval (in microseconds) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be addes in the future by adding similar functions here or to SAMD-hal-timer.c
    bool attachInterruptInterval(unsigned long interval, timerCallback callback)
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
    
    void setPeriod_TIMER_TC3(unsigned long period)
    {
      uint32_t TC_CTRLA_PRESCALER_DIVN;

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

      _compareValue = (int)(TIMER_HZ / (_prescaler/((float)period / 1000000))) - 1;

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

timerCallback TC3_callback;
timerCallback TCC_callback;


#define SAMD_TC3        ((TcCount16*) _SAMDTimer)
#define SAMD_TCC        ((Tcc*) _SAMDTimer)


////////////////////////////////////////////////////////


void TC3_Handler()
{
  // get timer struct
	TcCount16* TC = (TcCount16*) TC3;
	
  // If the compare register matching the timer count, trigger this interrupt
  if (TC->INTFLAG.bit.MC0 == 1) 
  {
    TC->INTFLAG.bit.MC0 = 1;
		(*TC3_callback)();
  }
}

void TCC0_Handler()
{
  // get timer struct
  Tcc* TC = (Tcc*) TCC0;
  
  // If the compare register matching the timer count, trigger this interrupt
  if (TC->INTFLAG.bit.MC0 == 1) 
  {  
    // A compare to cc0 caused the interrupt
	  TC->INTFLAG.bit.MC0 = 1;    // writing a one clears the flag ovf flag
  }

  if (TC->INTFLAG.bit.OVF == 1) 
  {
	  (*TCC_callback)();
	  
	  TC->INTFLAG.bit.OVF = 1;
  }
}
  
class SAMDTimerInterrupt
{
  private:
    SAMDTimerNumber _timerNumber;
    
    // point to timer struct, (TcCount16*) TC3 or (Tcc*) TCC0 for SAMD21
    void*           _SAMDTimer = NULL;
    
    timerCallback  _callback;        // pointer to the callback function
    float           _frequency;       // Timer frequency
    //uint32_t        _timerCount;      // count to activate timer
    
    unsigned long   _period;
    int             _prescaler;
    int             _compareValue;

  public:

    SAMDTimerInterrupt(SAMDTimerNumber timerNumber)
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

    // frequency (in hertz) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be addes in the future by adding similar functions here or to SAMD-hal-timer.c
    bool setFrequency(float frequency, timerCallback callback)
    {
      _period = (unsigned long) (1000000.0f / frequency);
      
      PWM_LOGDEBUG3(F("_period ="), _period, F(", frequency ="), frequency);
      
      if (_timerNumber == TIMER_TC3)
      {    
        REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID (GCM_TCC2_TC3));
        
        while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
        
        PWM_LOGWARN3(F("SAMDTimerInterrupt: F_CPU (MHz) ="), F_CPU/1000000, F(", TIMER_HZ ="), TIMER_HZ/1000000);
        PWM_LOGWARN3(F("TC3_Timer::startTimer _Timer = 0x"), String((uint32_t) _SAMDTimer, HEX), F(", TC3 = 0x"), String((uint32_t) TC3, HEX));
       
        SAMD_TC3->CTRLA.reg &= ~TC_CTRLA_ENABLE;

        // Use the 16-bit timer
        SAMD_TC3->CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
        
        while (SAMD_TC3->STATUS.bit.SYNCBUSY == 1);

        // Use match mode so that the timer counter resets when the count matches the compare register
        SAMD_TC3->CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
        
        while (SAMD_TC3->STATUS.bit.SYNCBUSY == 1);
    
	      setPeriod_TIMER_TC3(_period);

        // Enable the compare interrupt
        SAMD_TC3->INTENSET.reg = 0;
        SAMD_TC3->INTENSET.bit.MC0 = 1;

        NVIC_EnableIRQ(TC3_IRQn);

        SAMD_TC3->CTRLA.reg |= TC_CTRLA_ENABLE;
        
        while (SAMD_TC3->STATUS.bit.SYNCBUSY == 1);

        _callback     = callback;
	      TC3_callback  = callback;
	    }
	    else if (_timerNumber == TIMER_TCC)
      {
        REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TCC0_TCC1));
	    
	      while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
	      
	      PWM_LOGWARN3(F("SAMDTimerInterrupt: F_CPU (MHz) ="), F_CPU/1000000, F(", TIMER_HZ ="), TIMER_HZ/1000000);
        PWM_LOGWARN3(F("TCC_Timer::startTimer _Timer = 0x"), String((uint32_t) _SAMDTimer, HEX), F(", TCC0 = 0x"), String((uint32_t) TCC0, HEX));
       
        SAMD_TCC->CTRLA.reg &= ~TCC_CTRLA_ENABLE;   // Disable TC
        
        while (SAMD_TCC->SYNCBUSY.bit.ENABLE == 1); // wait for sync 
	            
	      setPeriod_TIMER_TCC(_period);

        // Use match mode so that the timer counter resets when the count matches the compare register
        SAMD_TCC->WAVE.reg |= TCC_WAVE_WAVEGEN_NFRQ;   // Set wave form configuration 
        
        while (SAMD_TCC->SYNCBUSY.bit.WAVE == 1); // wait for sync 

        // Enable the compare interrupt
        SAMD_TCC->INTENSET.reg = 0;
	      SAMD_TCC->INTENSET.bit.OVF = 1;
        SAMD_TCC->INTENSET.bit.MC0 = 1;

        NVIC_EnableIRQ(TCC0_IRQn);

        SAMD_TCC->CTRLA.reg |= TCC_CTRLA_ENABLE;
        
        while (SAMD_TCC->SYNCBUSY.bit.ENABLE == 1); // wait for sync 

	      _callback     = callback;
	      TCC_callback  = callback;
      }
    
      return true;
    }

    // interval (in microseconds) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be addes in the future by adding similar functions here or to SAMD-hal-timer.c
    bool setInterval(unsigned long interval, timerCallback callback)
    {
      return setFrequency((float) (1000000.0f / interval), callback);
    }

    bool attachInterrupt(float frequency, timerCallback callback)
    {
      return setFrequency(frequency, callback);
    }

    // interval (in microseconds) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be addes in the future by adding similar functions here or to SAMD-hal-timer.c
    bool attachInterruptInterval(unsigned long interval, timerCallback callback)
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
    
    void setPeriod_TIMER_TC3(unsigned long period)
    {
      TcCount16* _Timer = (TcCount16*) _SAMDTimer;
      
      //uint32_t TC_CTRLA_PRESCALER_DIVN;

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

	    _compareValue = (int)(TIMER_HZ / (_prescaler / ((float)period / 1000000))) - 1;

      
      // Make sure the count is in a proportional position to where it was
      // to prevent any jitter or disconnect when changing the compare value.
      _Timer->COUNT.reg = map(_Timer->COUNT.reg, 0, _Timer->CC[0].reg, 0, _compareValue);
      _Timer->CC[0].reg = _compareValue;
      
      while (_Timer->STATUS.bit.SYNCBUSY == 1);
      
      PWM_LOGDEBUG3(F("SAMD21 TC3 period ="), period, F(", _prescaler ="), _prescaler);
      PWM_LOGDEBUG1(F("_compareValue ="), _compareValue);
    }
    
    void setPeriod_TIMER_TCC(unsigned long period)
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
	    
	    _compareValue = (int)(TIMER_HZ / (_prescaler / ((float)period / 1000000))) - 1;

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

#endif

#include "SAMD_Slow_PWM_ISR.h"

#endif    // SAMD_SLOW_PWM_H

