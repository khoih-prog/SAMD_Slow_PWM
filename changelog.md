# SAMD_Slow_PWM Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/SAMD_Slow_PWM.svg?)](https://www.ardu-badge.com/SAMD_Slow_PWM)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/SAMD_Slow_PWM.svg)](https://github.com/khoih-prog/SAMD_Slow_PWM/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/SAMD_Slow_PWM/blob/main/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/SAMD_Slow_PWM.svg)](http://github.com/khoih-prog/SAMD_Slow_PWM/issues)



<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Donate to my libraries using BuyMeACoffee" style="height: 50px !important;width: 181px !important;" ></a>
<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://img.shields.io/badge/buy%20me%20a%20coffee-donate-orange.svg?logo=buy-me-a-coffee&logoColor=FFDD00" style="height: 20px !important;width: 200px !important;" ></a>

---
---

## Table of Contents

* [Changelog](#changelog)
  * [Releases v1.2.3](#releases-v123)
  * [Releases v1.2.2](#releases-v122)
  * [Releases v1.2.1](#releases-v121)
  * [Releases v1.2.0](#releases-v120)
  * [Releases v1.1.0](#releases-v110)
  * [Initial Releases v1.0.0](#Initial-Releases-v100)

---
---

## Changelog

### Releases v1.2.3

1. Fix `DutyCycle` bug. Check [float precisison of DutyCycle only sometimes working #3](https://github.com/khoih-prog/SAMD_Slow_PWM/issues/3)
2. Fix `New Period` display bug. Check [random dropouts #4](https://github.com/khoih-prog/SAMD_Slow_PWM/issues/4)
3. Update examples


### Releases v1.2.2

1. Use correct `PWM_Generic_Debug.h` file. Check [missing header #2](https://github.com/khoih-prog/SAMD_Slow_PWM/issues/2)
2. Display informational warning only when `_PWM_LOGLEVEL_` > 3
3. Update `Packages' Patches`


### Releases v1.2.1

1. Use `float` for `DutyCycle` and `Freq`, `uint32_t` for `period`. 
2. Optimize code by not calculation in ISR

### Releases v1.2.0

1. Fix `multiple-definitions` linker error. Drop `src_cpp` and `src_h` directories
2. DutyCycle to be optionally updated at the end current PWM period instead of immediately. Check [DutyCycle to be updated at the end current PWM period #2](https://github.com/khoih-prog/ESP8266_PWM/issues/2)
3. Add examples [SAMD21 multiFileProject](examples/SAMD21/multiFileProject) and [SAMD51 multiFileProject](examples/SAMD51/multiFileProject)to demo for multiple-file project
4. Improve accuracy by using `double`, instead of `uint32_t` for `dutycycle`, `period`. Check [Change Duty Cycle #1](https://github.com/khoih-prog/ESP8266_PWM/issues/1#issuecomment-1024969658)
5. Optimize library code by using `reference-passing` instead of `value-passing`
6. Add support to many more boards, such as `SAMD21E1xA`, `SAMD21G1xA` and`SAMD21J1xA`
7. Update examples accordingly
8. Update `Packages' Patches`

### Releases v1.1.0

1. Add functions to modify PWM settings on-the-fly
2. Add example to demo how to modify PWM settings on-the-fly

### Initial Releases v1.0.0

1. Initial coding to support **SAMD21/SAMD51 boards** such as NANO_33_IOT, ITSYBITSY_M4, SEEED_XIAO_M0, SparkFun_SAMD51_Thing_Plus, etc. using Arduino, Adafruit or Sparkfun core
2. The hybrid ISR-based PWM channels can generate from very low (much less than 1Hz) to highest PWM frequencies up to 1000Hz with acceptable accuracy.

---
---

## Copyright

Copyright 2021- Khoi Hoang


