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

#ifndef SAMD_SLOW_PWM_ISR_H
#define SAMD_SLOW_PWM_ISR_H


#include "SAMD_Slow_PWM_ISR.hpp"
#include "SAMD_Slow_PWM_ISR_Impl.h"

#endif    // SAMD_SLOW_PWM_ISR_H


