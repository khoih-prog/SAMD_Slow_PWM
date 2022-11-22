/****************************************************************************************************************************
  SAMD_Slow_PWM_Impl.h
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

#ifndef SAMD_SLOW_PWM_IMPL_H
#define SAMD_SLOW_PWM_IMPL_H

////////////////////////////////////////////////////

#if (TIMER_INTERRUPT_USING_SAMD51)

timerCallback TC3_callback;

//#define SAMD_TC3        ((TcCount16*) _SAMDTimer)

void TC3_Handler()
{
  // If this interrupt is due to the compare register matching the timer count
  if (TC3->COUNT16.INTFLAG.bit.MC0 == 1)
  {
    TC3->COUNT16.INTFLAG.bit.MC0 = 1;
    (*TC3_callback)();
  }
}

// frequency (in hertz) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
// No params and duration now. To be addes in the future by adding similar functions here or to SAMD-hal-timer.c
bool SAMDTimerInterrupt::setFrequency(const float& frequency, timerCallback callback)
{
  _period =  (1000000.0f / frequency);

  if (_timerNumber == TIMER_TC3)
  {
    PWM_LOGWARN3(F("SAMDTimerInterrupt: F_CPU (MHz) ="), F_CPU / 1000000, F(", TIMER_HZ ="), TIMER_HZ / 1000000);
    PWM_LOGWARN3(F("TC_Timer::startTimer _Timer = 0x"), String((uint32_t) _SAMDTimer, HEX), F(", TC3 = 0x"),
                 String((uint32_t) TC3, HEX));

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


////////////////////////////////////////////////////////

#elif (TIMER_INTERRUPT_USING_SAMD21)

timerCallback TC3_callback;
timerCallback TCC_callback;

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

// frequency (in hertz) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
// No params and duration now. To be addes in the future by adding similar functions here or to SAMD-hal-timer.c
bool SAMDTimerInterrupt::setFrequency(const float& frequency, timerCallback callback)
{
  _period =  (1000000.0f / frequency);

  PWM_LOGDEBUG3(F("_period ="), _period, F(", frequency ="), frequency);

  if (_timerNumber == TIMER_TC3)
  {
    REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID (GCM_TCC2_TC3));

    while ( GCLK->STATUS.bit.SYNCBUSY == 1 );

    PWM_LOGWARN3(F("SAMDTimerInterrupt: F_CPU (MHz) ="), F_CPU / 1000000, F(", TIMER_HZ ="), TIMER_HZ / 1000000);
    PWM_LOGWARN3(F("TC3_Timer::startTimer _Timer = 0x"), String((uint32_t) _SAMDTimer, HEX), F(", TC3 = 0x"),
                 String((uint32_t) TC3, HEX));

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

    PWM_LOGWARN3(F("SAMDTimerInterrupt: F_CPU (MHz) ="), F_CPU / 1000000, F(", TIMER_HZ ="), TIMER_HZ / 1000000);
    PWM_LOGWARN3(F("TCC_Timer::startTimer _Timer = 0x"), String((uint32_t) _SAMDTimer, HEX), F(", TCC0 = 0x"),
                 String((uint32_t) TCC0, HEX));

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


#endif    // #if (TIMER_INTERRUPT_USING_SAMD51)

#endif    // SAMD_SLOW_PWM_IMPL_H

