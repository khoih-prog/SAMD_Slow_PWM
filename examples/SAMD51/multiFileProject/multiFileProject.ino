/****************************************************************************************************************************
  multiFileProject.ino
  
  For SAMD21/SAMD51 boards
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/SAMD_Slow_PWM
  Licensed under MIT license
*****************************************************************************************************************************/

// To demo how to include files in multi-file Projects

#if !( defined(__SAMD51__) || defined(__SAMD51J20A__) || defined(__SAMD51J19A__) || defined(__SAMD51G19A__) || defined(__SAMD51P19A__) )
  #error This code is designed to run on SAMD51 platform! Please check your Tools->Board setting.
#endif

#define SAMD_SLOW_PWM_VERSION_MIN_TARGET      "SAMD_Slow_PWM v1.2.3"
#define SAMD_SLOW_PWM_VERSION_MIN             1002003

#include "multiFileProject.h"

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "SAMD_Slow_PWM.h"

void setup() 
{
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("\nStart multiFileProject");
  Serial.println(SAMD_SLOW_PWM_VERSION);

#if defined(SAMD_SLOW_PWM_VERSION_MIN)
  if (SAMD_SLOW_PWM_VERSION_INT < SAMD_SLOW_PWM_VERSION_MIN)
  {
    Serial.print("Warning. Must use this example on Version equal or later than : ");
    Serial.println(SAMD_SLOW_PWM_VERSION_MIN_TARGET);
  }
#endif
}

void loop() 
{
  // put your main code here, to run repeatedly:
}
