# SAMD_Slow_PWM Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/SAMD_Slow_PWM.svg?)](https://www.ardu-badge.com/SAMD_Slow_PWM)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/SAMD_Slow_PWM.svg)](https://github.com/khoih-prog/SAMD_Slow_PWM/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/SAMD_Slow_PWM/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/SAMD_Slow_PWM.svg)](http://github.com/khoih-prog/SAMD_Slow_PWM/issues)

<a href="https://www.buymeacoffee.com/khoihprog6" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Buy Me A Coffee" style="height: 60px !important;width: 217px !important;" ></a>


---
---

## Table of Contents

* [Important Change from v1.2.0](#Important-Change-from-v120)
* [Why do we need this SAMD_Slow_PWM library](#why-do-we-need-this-SAMD_Slow_PWM-library)
  * [Features](#features)
  * [Why using ISR-based PWM is better](#why-using-isr-based-pwm-is-better)
  * [Currently supported Boards](#currently-supported-boards)
  * [Important Notes about ISR](#important-notes-about-isr)
* [Changelog](changelog.md)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
  * [Use Arduino Library Manager](#use-arduino-library-manager)
  * [Manual Install](#manual-install)
  * [VS Code & PlatformIO](#vs-code--platformio)
* [Packages' Patches](#packages-patches)
  * [1. For Arduino SAMD boards](#1-for-arduino-samd-boards)
      * [For core version v1.8.10+](#for-core-version-v1810)
      * [For core version v1.8.9-](#for-core-version-v189-)
  * [2. For Adafruit SAMD boards](#2-for-adafruit-samd-boards)
  * [3. For Seeeduino SAMD boards](#3-for-seeeduino-samd-boards)
  * [4. For SparkFun SAMD boards](#4-for-SparkFun-samd-boards)
* [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error) 
* [Usage](#usage)
  * [1. Init Hardware Timer](#1-init-hardware-timer)
  * [2. Set PWM Frequency, dutycycle, attach irqCallbackStartFunc and irqCallbackStopFunc functions](#2-Set-PWM-Frequency-dutycycle-attach-irqCallbackStartFunc-and-irqCallbackStopFunc-functions)
* [Examples](#examples)
  * [For SAMD21](#For-SAMD21)
    * [ 1. ISR_4_PWMs_Array](examples/SAMD21/ISR_4_PWMs_Array)
    * [ 2. ISR_4_PWMs_Array_Complex](examples/SAMD21/ISR_4_PWMs_Array_Complex)
    * [ 3. ISR_4_PWMs_Array_Simple](examples/SAMD21/ISR_4_PWMs_Array_Simple)
    * [ 4. ISR_Changing_PWM](examples/SAMD21/ISR_Changing_PWM)
    * [ 5. ISR_Modify_PWM](examples/SAMD21/ISR_Modify_PWM)
    * [ 6. multiFileProject](examples/SAMD21/multiFileProject) **New**
  * [For SAMD51](#For-SAMD51)
    * [ 1. ISR_16_PWMs_Array](examples/SAMD51/ISR_16_PWMs_Array)
    * [ 2. ISR_16_PWMs_Array_Complex](examples/SAMD51/ISR_16_PWMs_Array_Complex)
    * [ 3. ISR_16_PWMs_Array_Simple](examples/SAMD51/ISR_16_PWMs_Array_Simple)
    * [ 4. ISR_Changing_PWM](examples/SAMD51/ISR_Changing_PWM)
    * [ 5. ISR_Modify_PWM](examples/SAMD51/ISR_Modify_PWM)
    * [ 6. multiFileProject](examples/SAMD51/multiFileProject) **New**
* [Example SAMD51 ISR_16_PWMs_Array_Complex](#Example-SAMD51-ISR_16_PWMs_Array_Complex)
* [Debug Terminal Output Samples](#debug-terminal-output-samples)
  * [1. ISR_4_PWMs_Array_Complex on SAMD_NANO_33_IOT](#1-ISR_4_PWMs_Array_Complex-on-SAMD_NANO_33_IOT)
  * [2. ISR_16_PWMs_Array_Complex on ITSYBITSY_M4](#2-ISR_16_PWMs_Array_Complex-on-ITSYBITSY_M4)
  * [3. ISR_4_PWMs_Array_Complex on SEEED_XIAO_M0](#3-ISR_4_PWMs_Array_Complex-on-SEEED_XIAO_M0)
  * [4. ISR_Modify_PWM on SAMD_NANO_33_IOT](#4-ISR_Modify_PWM-on-SAMD_NANO_33_IOT)
  * [5. ISR_Changing_PWM on SAMD_NANO_33_IOT](#5-ISR_Changing_PWM-on-SAMD_NANO_33_IOT)
  * [6. ISR_Modify_PWM on ITSYBITSY_M4](#6-ISR_Modify_PWM-on-ITSYBITSY_M4)
  * [7. ISR_Changing_PWM on ITSYBITSY_M4](#7-ISR_Changing_PWM-on-ITSYBITSY_M4)
* [Debug](#debug)
* [Troubleshooting](#troubleshooting)
* [Issues](#issues)
* [TO DO](#to-do)
* [DONE](#done)
* [Contributions and Thanks](#contributions-and-thanks)
* [Contributing](#contributing)
* [License](#license)
* [Copyright](#copyright)

---
---

### Important Change from v1.2.0

Please have a look at [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error)

As more complex calculation and check inside ISR are introduced from v1.2.0, there are consequences as follows

- For SAMD21 (slower) => using min 50uS and max 4 PWM channels

```
// Use 50uS for slow SAMD21
#define HW_TIMER_INTERVAL_US      50L
```

- For SAMD51 (faster) => using min 20uS and max 16 PWM channels

```
// Use 30uS for faster SAMD51
#define HW_TIMER_INTERVAL_US      20L
```

You certainly can modify to use better values according to your board and use-case, just remember to test and reverse to conservative values if crash happens.


---
---

### Why do we need this [SAMD_Slow_PWM library](https://github.com/khoih-prog/SAMD_Slow_PWM)

### Features

This library enables you to use Hardware Timers on **SAMD21/SAMD51 boards** such as NANO_33_IOT, ITSYBITSY_M4, SEEED_XIAO_M0, SparkFun_SAMD51_Thing_Plus, etc., to create and output PWM to pins. Because this library doesn't use the powerful hardware-controlled PWM with limitations, the maximum PWM frequency is currently limited at **1000Hz**, which is suitable for many real-life applications.

---

This library enables you to use Interrupt from Hardware Timers on SAMD21/SAMD51 boards to create and output PWM to pins. It now supports 16 ISR-based synchronized PWM channels, while consuming only 1 Hardware Timer. PWM interval can be very long (uint32_t millisecs). The most important feature is they're ISR-based PWM channels. Therefore, their executions are not blocked by bad-behaving functions or tasks. This important feature is absolutely necessary for mission-critical tasks. These hardware PWM channels, using interrupt, still work even if other functions are blocking. Moreover, they are much more precise (certainly depending on clock frequency accuracy) than other software PWM using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

As **Hardware Timers are rare, and very precious assets** of any board, this library now enables you to use up to **16 ISR-based synchronized PWM channels, while consuming only 1 Hardware Timer**. Timers' interval is very long (**ulong millisecs**).

Now with these new **16 ISR-based PWM-channels**, the maximum interval is **practically unlimited** (limited only by unsigned long miliseconds) while **the accuracy is nearly perfect** compared to software timers. 

The most important feature is they're ISR-based PWM channels. Therefore, their executions are **not blocked by bad-behaving functions / tasks**. This important feature is absolutely necessary for mission-critical tasks. 

The [**SAMD51 PWMs_Array_Complex**](examples/SAMD51/PWMs_Array_Complex) example will demonstrate the nearly perfect accuracy, compared to software PWM, by printing the actual period / duty-cycle in `microsecs` of each of PWM-channels.

Being ISR-based PWM, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet or Blynk services. You can also have many `(up to 16)` timers to use.

This non-being-blocked important feature is absolutely necessary for mission-critical tasks.

You'll see `software-based` SimpleTimer is blocked while system is connecting to WiFi / Internet / Blynk, as well as by blocking task 
in loop(), using delay() function as an example. The elapsed time then is very unaccurate

---

#### Why using ISR-based PWM is better

Imagine you have a system with a **mission-critical** function, measuring water level and control the sump pump or doing something much more important. You normally use a software timer to poll, or even place the function in loop(). But what if another function is **blocking** the loop() or setup().

So your function **might not be executed, and the result would be disastrous.**

You'd prefer to have your function called, no matter what happening with other functions (busy loop, bug, etc.).

The correct choice is to use a Hardware Timer with **Interrupt** to call your function.

These hardware timers, using interrupt, still work even if other functions are blocking. Moreover, they are much more **precise** (certainly depending on clock frequency accuracy) than other software timers using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

Functions using normal software timers, relying on loop() and calling millis(), won't work if the loop() or setup() is blocked by certain operation. For example, certain function is blocking while it's connecting to WiFi or some services.

The catch is **your function is now part of an ISR (Interrupt Service Routine), and must be lean / mean, and follow certain rules.** More to read on:

[**HOWTO Attach Interrupt**](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/)

---

### Currently supported Boards

  - **Arduino SAMD21 (ZERO, MKR, NANO_33_IOT, etc.)**.
  - **Adafruit SAM21 (Itsy-Bitsy M0, Metro M0, Feather M0, Gemma M0, etc.)**.
  - **Adafruit SAM51 (Itsy-Bitsy M4, Metro M4, Grand Central M4, Feather M4 Express, etc.)**.
  - **Seeeduino SAMD21/SAMD51 boards (SEEED_WIO_TERMINAL, SEEED_FEMTO_M0, SEEED_XIAO_M0, Wio_Lite_MG126, WIO_GPS_BOARD, SEEEDUINO_ZERO, SEEEDUINO_LORAWAN, SEEED_GROVE_UI_WIRELESS, etc.)** 
  - **Sparkfun SAMD21 boards** such as **SparkFun_RedBoard_Turbo, SparkFun_Qwiic_Micro, etc.**
  - **Sparkfun SAMD51 boards** such as **SparkFun_SAMD51_Thing_Plus, SparkFun_SAMD51_MicroMod, etc.**

---

#### Important Notes about ISR

1. Inside the attached function, **delay() won’t work and the value returned by millis() will not increment.** Serial data received while in the function may be lost. You should declare as **volatile any variables that you modify within the attached function.**

2. Typically global variables are used to pass data between an ISR and the main program. To make sure variables shared between an ISR and the main program are updated correctly, declare them as volatile.

---
---

## Prerequisites

 1. [`Arduino IDE 1.8.19+` for Arduino](https://github.com/arduino/Arduino). [![GitHub release](https://img.shields.io/github/release/arduino/Arduino.svg)](https://github.com/arduino/Arduino/releases/latest)
 2. [`Arduino SAMD core 1.8.13+`](https://github.com/arduino/ArduinoCore-samd) for SAMD ARM Cortex-M0+ boards. [![GitHub release](https://img.shields.io/github/release/arduino/ArduinoCore-samd.svg)](https://github.com/arduino/ArduinoCore-samd/releases/latest)
 3. [`Adafruit SAMD core 1.7.9+`](https://github.com/adafruit/ArduinoCore-samd) for SAMD ARM Cortex-M0+ and M4 boards (Nano 33 IoT, etc.). [![GitHub release](https://img.shields.io/github/release/adafruit/ArduinoCore-samd.svg)](https://github.com/adafruit/ArduinoCore-samd/releases/latest)
 4. [`Seeeduino SAMD core 1.8.2+`](https://github.com/Seeed-Studio/ArduinoCore-samd) for SAMD21/SAMD51 boards (XIAO M0, Wio Terminal, etc.). [![Latest release](https://img.shields.io/github/release/Seeed-Studio/ArduinoCore-samd.svg)](https://github.com/Seeed-Studio/ArduinoCore-samd/releases/latest/)
 5. [`Sparkfun SAMD core 1.8.3+`](https://github.com/sparkfun/Arduino_Boards) for SAMD21/SAMD51 boards (SparkFun_RedBoard_Turbo, SparkFun_SAMD51_Thing_Plus, etc.).

 6. To use with certain example
   - [`SimpleTimer library`](https://github.com/jfturcot/SimpleTimer) to use with some examples.
   
   
---
---

## Installation

### Use Arduino Library Manager

The best and easiest way is to use `Arduino Library Manager`. Search for [**SAMD_Slow_PWM**](https://github.com/khoih-prog/SAMD_Slow_PWM), then select / install the latest version.
You can also use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/SAMD_Slow_PWM.svg?)](https://www.ardu-badge.com/SAMD_Slow_PWM) for more detailed instructions.

### Manual Install

Another way to install is to:

1. Navigate to [**SAMD_Slow_PWM**](https://github.com/khoih-prog/SAMD_Slow_PWM) page.
2. Download the latest release `SAMD_Slow_PWM-master.zip`.
3. Extract the zip file to `SAMD_Slow_PWM-master` directory 
4. Copy whole `SAMD_Slow_PWM-master` folder to Arduino libraries' directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install [**SAMD_Slow_PWM** library](https://registry.platformio.org/libraries/khoih-prog/SAMD_Slow_PWM) by using [Library Manager](https://registry.platformio.org/libraries/khoih-prog/SAMD_Slow_PWM/installation). Search for **SAMD_Slow_PWM** in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)

---
---

### Packages' Patches

#### 1. For Arduino SAMD boards
 
 ***To be able to compile without error and automatically detect and display BOARD_NAME on Arduino SAMD (Nano-33-IoT, etc) boards***, you have to copy the whole [Arduino SAMD Packages_Patches](Packages_Patches/arduino/hardware/samd/1.8.13) directory into Arduino SAMD directory (~/.arduino15/packages/arduino/hardware/samd/1.8.13).
 
#### For core version v1.8.10+

Supposing the Arduino SAMD version is 1.8.13. Now only one file must be copied into the directory:

- `~/.arduino15/packages/arduino/hardware/samd/1.8.13/platform.txt`

Whenever a new version is installed, remember to copy this files into the new version directory. For example, new version is x.yy.zz

This file must be copied into the directory:

- `~/.arduino15/packages/arduino/hardware/samd/x.yy.zz/platform.txt`
 
#### For core version v1.8.9-

Supposing the Arduino SAMD version is 1.8.9. These files must be copied into the directory:

- `~/.arduino15/packages/arduino/hardware/samd/1.8.9/platform.txt`
- ***`~/.arduino15/packages/arduino/hardware/samd/1.8.9/cores/arduino/Arduino.h`***

Whenever a new version is installed, remember to copy these files into the new version directory. For example, new version is x.yy.z

These files must be copied into the directory:

- `~/.arduino15/packages/arduino/hardware/samd/x.yy.z/platform.txt`
- ***`~/.arduino15/packages/arduino/hardware/samd/x.yy.z/cores/arduino/Arduino.h`***
 
 This is mandatory to fix the ***notorious Arduino SAMD compiler error***. See [Improve Arduino compatibility with the STL (min and max macro)](https://github.com/arduino/ArduinoCore-samd/pull/399)
 
```
 ...\arm-none-eabi\include\c++\7.2.1\bits\stl_algobase.h:243:56: error: macro "min" passed 3 arguments, but takes just 2
     min(const _Tp& __a, const _Tp& __b, _Compare __comp)
```

Whenever the above-mentioned compiler error issue is fixed with the new Arduino SAMD release, you don't need to copy the `Arduino.h` file anymore.

#### 2. For Adafruit SAMD boards
 
 ***To be able to compile without error and automatically detect and display BOARD_NAME on Adafruit SAMD (Itsy-Bitsy M4, etc) boards***, you have to copy the files in [Adafruit SAMD Packages_Patches](Packages_Patches/adafruit/hardware/samd/1.7.9) into Adafruit samd directory (~/.arduino15/packages/adafruit/hardware/samd/1.7.9). 

Supposing the Adafruit SAMD core version is 1.7.9. This file must be copied into the directory:

- `~/.arduino15/packages/adafruit/hardware/samd/1.7.9/platform.txt`
- `~/.arduino15/packages/adafruit/hardware/samd/1.7.9/cores/arduino/Print.h`
- `~/.arduino15/packages/adafruit/hardware/samd/1.7.9/cores/arduino/Print.cpp`

Whenever a new version is installed, remember to copy this file into the new version directory. For example, new version is x.yy.zz
This file must be copied into the directory:

- `~/.arduino15/packages/adafruit/hardware/samd/x.yy.zz/platform.txt`
- `~/.arduino15/packages/adafruit/hardware/samd/x.yy.zz/cores/arduino/Print.h`
- `~/.arduino15/packages/adafruit/hardware/samd/x.yy.zz/cores/arduino/Print.cpp`

#### 3. For Seeeduino SAMD boards
 
 ***To be able to compile without error and automatically detect and display BOARD_NAME on Seeeduino SAMD (XIAO M0, Wio Terminal, etc) boards***, you have to copy the files in [Seeeduino SAMD Packages_Patches](Packages_Patches/Seeeduino/hardware/samd/1.8.2) into Seeeduino samd directory (~/.arduino15/packages/Seeeduino/hardware/samd/1.8.2). 

Supposing the Seeeduino SAMD core version is 1.8.2. This file must be copied into the directory:

- `~/.arduino15/packages/Seeeduino/hardware/samd/1.8.2/platform.txt`
- `~/.arduino15/packages/Seeeduino/hardware/samd/1.8.2/cores/arduino/Arduino.h`
- `~/.arduino15/packages/Seeeduino/hardware/samd/1.8.2/cores/arduino/Print.h`
- `~/.arduino15/packages/Seeeduino/hardware/samd/1.8.2/cores/arduino/Print.cpp`

Whenever a new version is installed, remember to copy this file into the new version directory. For example, new version is x.yy.zz
This file must be copied into the directory:

- `~/.arduino15/packages/Seeeduino/hardware/samd/x.yy.zz/platform.txt`
- `~/.arduino15/packages/Seeeduino/hardware/samd/x.yy.zz/cores/arduino/Arduino.h`
- `~/.arduino15/packages/Seeeduino/hardware/samd/x.yy.zz/cores/arduino/Print.h`
- `~/.arduino15/packages/Seeeduino/hardware/samd/x.yy.zz/cores/arduino/Print.cpp`

#### 4. For SparkFun SAMD boards
 
 ***To be able to compile without error and automatically detect and display BOARD_NAME on SparkFun SAMD (XIAO SparkFun_RedBoard_Turbo, SparkFun_SAMD51_Thing_Plus, etc) boards***, you have to copy the file [SparkFun SAMD Packages_Patches](Packages_Patches/SparkFun/hardware/samd/1.8.3) into SparkFun samd directory (~/.arduino15/packages/SparkFun/hardware/samd/1.8.3). 

Supposing the SparkFun SAMD core version is 1.8.3. This file must be copied into the directory:

- `~/.arduino15/packages/SparkFun/hardware/samd/1.8.3/cores/arduino/Print.h`
- `~/.arduino15/packages/SparkFun/hardware/samd/1.8.3/cores/arduino/Print.cpp`
- `~/.arduino15/packages/SparkFun/hardware/samd/1.8.3/cores/arduino51/Print.h`
- `~/.arduino15/packages/SparkFun/hardware/samd/1.8.3/cores/arduino51/Print.cpp`

Whenever a new version is installed, remember to copy this file into the new version directory. For example, new version is x.yy.zz
This file must be copied into the directory:

- `~/.arduino15/packages/SparkFun/hardware/samd/x.yy.zz/cores/arduino/Print.h`
- `~/.arduino15/packages/SparkFun/hardware/samd/x.yy.zz/cores/arduino/Print.cpp`
- `~/.arduino15/packages/SparkFun/hardware/samd/x.yy.zz/cores/arduino51/Print.h`
- `~/.arduino15/packages/SparkFun/hardware/samd/x.yy.zz/cores/arduino51/Print.cpp`

---
---

### HOWTO Fix `Multiple Definitions` Linker Error

The current library implementation, using `xyz-Impl.h` instead of standard `xyz.cpp`, possibly creates certain `Multiple Definitions` Linker error in certain use cases.

You can include this `.hpp` file

```
// Can be included as many times as necessary, without `Multiple Definitions` Linker Error
#include "SAMD_Slow_PWM.hpp"     //https://github.com/khoih-prog/SAMD_Slow_PWM
```

in many files. But be sure to use the following `.h` file **in just 1 `.h`, `.cpp` or `.ino` file**, which must **not be included in any other file**, to avoid `Multiple Definitions` Linker Error

```
// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "SAMD_Slow_PWM.h"           //https://github.com/khoih-prog/SAMD_Slow_PWM
```

Check the new [**SAMD21 multiFileProject** example](examples/SAMD21/multiFileProject) or [**SAMD51 multiFileProject** example](examples/SAMD51/multiFileProject) for a `HOWTO` demo.

Have a look at the discussion in [Different behaviour using the src_cpp or src_h lib #80](https://github.com/khoih-prog/ESPAsync_WiFiManager/discussions/80)


---
---


## Usage

Before using any Timer for a PWM channel, you have to make sure the Timer has not been used by any other purpose.

#### 1. Init Hardware Timer

```
// Depending on the board, you can select SAMD21 Hardware Timer from TC3-TCC
// SAMD21 Hardware Timer from TC3 or TCC
// SAMD51 Hardware Timer only TC3
SAMDTimer ITimer0(TIMER_TC3);

// You can only select SAMD51 Hardware Timer TC3
// Init SAMD timer TIMER_TC3
//SAMDTimer ITimer(TIMER_TC3);

// Init SAMD_Slow_PWM
SAMD_Slow_PWM ISR_PWM;
```

#### 2. Set PWM Frequency, dutycycle, attach irqCallbackStartFunc and irqCallbackStopFunc functions

```
void irqCallbackStartFunc()
{

}

void irqCallbackStopFunc()
{

}

void setup()
{
  ....
  
  // You can use this with PWM_Freq in Hz
    ISR_PWM.setPWM(PWM_Pin, PWM_Freq, PWM_DutyCycle, irqCallbackStartFunc, irqCallbackStopFunc);
                   
  ....                 
}  
```

---
---

### Examples: 

#### For SAMD21

 1. [ISR_4_PWMs_Array](examples/SAMD21/ISR_4_PWMs_Array)
 2. [ISR_4_PWMs_Array_Complex](examples/SAMD21/ISR_4_PWMs_Array_Complex)
 3. [ISR_4_PWMs_Array_Simple](examples/SAMD21/ISR_4_PWMs_Array_Simple)
 4. [ISR_Changing_PWM](examples/SAMD21/ISR_Changing_PWM)
 5. [ISR_Modify_PWM](examples/SAMD21/ISR_Modify_PWM)
 6. [multiFileProject](examples/SAMD21/multiFileProject). **New**
 
#### For SAMD51

 1. [ISR_16_PWMs_Array](examples/SAMD51/ISR_16_PWMs_Array)
 2. [ISR_16_PWMs_Array_Complex](examples/SAMD51/ISR_16_PWMs_Array_Complex)
 3. [ISR_16_PWMs_Array_Simple](examples/SAMD51/ISR_16_PWMs_Array_Simple)
 4. [ISR_Changing_PWM](examples/SAMD51/ISR_Changing_PWM)
 5. [ISR_Modify_PWM](examples/SAMD51/ISR_Modify_PWM)
 6. [multiFileProject](examples/SAMD51/multiFileProject). **New**

 
---
---

### Example SAMD51 [ISR_16_PWMs_Array_Complex](examples/SAMD51/ISR_16_PWMs_Array_Complex)

https://github.com/khoih-prog/SAMD_Slow_PWM/blob/da5c3ce61ffb704f206e72b65e8fd071093617ff/examples/SAMD51/ISR_16_PWMs_Array_Complex/ISR_16_PWMs_Array_Complex.ino#L16-L583

---

### Debug Terminal Output Samples

### 1. ISR_4_PWMs_Array_Complex on SAMD_NANO_33_IOT

The following is the sample terminal output when running example [ISR_16_PWMs_Array_Complex](examples/SAMD21/ISR_4_PWMs_Array_Complex) on SAMD_NANO_33_IOT to demonstrate how to use multiple PWM channels with complex callback functions, the accuracy of ISR Hardware PWM-channels, **especially when system is very busy**.  The ISR PWM-channels is **running exactly according to corresponding programmed periods and duty-cycles**


```
Starting ISR_4_PWMs_Array_Complex on SAMD_NANO_33_IOT
SAMD_Slow_PWM v1.2.2
Starting ITimer OK, micros() = 2750362
Channel : 0	    Period : 1000000		OnTime : 50000	Start_Time : 2750883
Channel : 1	    Period : 500000		OnTime : 50000	Start_Time : 2751343
Channel : 2	    Period : 250000		OnTime : 50000	Start_Time : 2751845
Channel : 3	    Period : 200000		OnTime : 60000	Start_Time : 2752329
SimpleTimer (ms): 2000, us : 12752801, Dus : 10002150
PWM Channel : 0, programmed Period (us): 1000000.00, actual : 1000000, programmed DutyCycle : 5.00, actual : 5.00
PWM Channel : 1, programmed Period (us): 500000.00, actual : 499999, programmed DutyCycle : 10.00, actual : 10.00
PWM Channel : 2, programmed Period (us): 250000.00, actual : 249999, programmed DutyCycle : 20.00, actual : 20.00
PWM Channel : 3, programmed Period (us): 200000.00, actual : 199999, programmed DutyCycle : 30.00, actual : 30.00
SimpleTimer (ms): 2000, us : 22757321, Dus : 10004520
PWM Channel : 0, programmed Period (us): 1000000.00, actual : 1000000, programmed DutyCycle : 5.00, actual : 5.00
PWM Channel : 1, programmed Period (us): 500000.00, actual : 499999, programmed DutyCycle : 10.00, actual : 10.00
PWM Channel : 2, programmed Period (us): 250000.00, actual : 250000, programmed DutyCycle : 20.00, actual : 20.00
PWM Channel : 3, programmed Period (us): 200000.00, actual : 199999, programmed DutyCycle : 30.00, actual : 30.00
SimpleTimer (ms): 2000, us : 32761791, Dus : 10004470
PWM Channel : 0, programmed Period (us): 1000000.00, actual : 1000000, programmed DutyCycle : 5.00, actual : 5.00
PWM Channel : 1, programmed Period (us): 500000.00, actual : 499999, programmed DutyCycle : 10.00, actual : 10.00
PWM Channel : 2, programmed Period (us): 250000.00, actual : 249999, programmed DutyCycle : 20.00, actual : 20.00
PWM Channel : 3, programmed Period (us): 200000.00, actual : 199999, programmed DutyCycle : 30.00, actual : 30.00
SimpleTimer (ms): 2000, us : 42766231, Dus : 10004440
PWM Channel : 0, programmed Period (us): 1000000.00, actual : 1000000, programmed DutyCycle : 5.00, actual : 5.00
PWM Channel : 1, programmed Period (us): 500000.00, actual : 499999, programmed DutyCycle : 10.00, actual : 10.00
PWM Channel : 2, programmed Period (us): 250000.00, actual : 249999, programmed DutyCycle : 20.00, actual : 20.00
PWM Channel : 3, programmed Period (us): 200000.00, actual : 199999, programmed DutyCycle : 30.00, actual : 30.00
```

---

### 2. ISR_16_PWMs_Array_Complex on ITSYBITSY_M4

The following is the sample terminal output when running example [ISR_16_PWMs_Array_Complex](examples/SAMD51/ISR_16_PWMs_Array_Complex) on ITSYBITSY_M4 to demonstrate how to use multiple PWM channels with complex callback functions, the accuracy of ISR Hardware PWM-channels, **especially when system is very busy**.  The ISR PWM-channels is **running exactly according to corresponding programmed periods and duty-cycles**


```
Starting ISR_16_PWMs_Array_Complex on ITSYBITSY_M4
SAMD_Slow_PWM v1.2.2
Starting ITimer OK, micros() = 3830244
Channel : 0	    Period : 1000000		OnTime : 50000	Start_Time : 3830538
Channel : 1	    Period : 500000		OnTime : 50000	Start_Time : 3830903
Channel : 2	    Period : 333333		OnTime : 66666	Start_Time : 3831277
Channel : 3	    Period : 250000		OnTime : 75000	Start_Time : 3831708
Channel : 4	    Period : 200000		OnTime : 80000	Start_Time : 3832077
Channel : 5	    Period : 166666		OnTime : 74999	Start_Time : 3832451
Channel : 6	    Period : 142857		OnTime : 71428	Start_Time : 3832874
Channel : 7	    Period : 125000		OnTime : 68750	Start_Time : 3833251
Channel : 8	    Period : 111111		OnTime : 66666	Start_Time : 3833671
Channel : 9	    Period : 100000		OnTime : 65000	Start_Time : 3834049
Channel : 10	    Period : 66666		OnTime : 46666	Start_Time : 3834419
Channel : 11	    Period : 50000		OnTime : 37500	Start_Time : 3834849
Channel : 12	    Period : 40000		OnTime : 32000	Start_Time : 3835219
Channel : 13	    Period : 33333		OnTime : 28333	Start_Time : 3835639
Channel : 14	    Period : 25000		OnTime : 22500	Start_Time : 3836016
Channel : 15	    Period : 20000		OnTime : 19000	Start_Time : 3836393
SimpleTimer (ms): 2000, us : 13836817, Dus : 10006340
PWM Channel : 0, programmed Period (us): 1000000.00, actual : 1000000, programmed DutyCycle : 5.00, actual : 5.00
PWM Channel : 1, programmed Period (us): 500000.00, actual : 500000, programmed DutyCycle : 10.00, actual : 10.00
PWM Channel : 2, programmed Period (us): 333333.34, actual : 333340, programmed DutyCycle : 20.00, actual : 20.00
PWM Channel : 3, programmed Period (us): 250000.00, actual : 250000, programmed DutyCycle : 30.00, actual : 30.00
PWM Channel : 4, programmed Period (us): 200000.00, actual : 200000, programmed DutyCycle : 40.00, actual : 40.00
PWM Channel : 5, programmed Period (us): 166666.67, actual : 166679, programmed DutyCycle : 45.00, actual : 44.98
PWM Channel : 6, programmed Period (us): 142857.14, actual : 142860, programmed DutyCycle : 50.00, actual : 49.99
PWM Channel : 7, programmed Period (us): 125000.00, actual : 125000, programmed DutyCycle : 55.00, actual : 54.99
PWM Channel : 8, programmed Period (us): 111111.11, actual : 111120, programmed DutyCycle : 60.00, actual : 59.99
PWM Channel : 9, programmed Period (us): 100000.00, actual : 99999, programmed DutyCycle : 65.00, actual : 65.00
PWM Channel : 10, programmed Period (us): 66666.66, actual : 66679, programmed DutyCycle : 70.00, actual : 69.98
PWM Channel : 11, programmed Period (us): 50000.00, actual : 50000, programmed DutyCycle : 75.00, actual : 75.00
PWM Channel : 12, programmed Period (us): 40000.00, actual : 40000, programmed DutyCycle : 80.00, actual : 80.00
PWM Channel : 13, programmed Period (us): 33333.33, actual : 33339, programmed DutyCycle : 85.00, actual : 84.95
PWM Channel : 14, programmed Period (us): 25000.00, actual : 24999, programmed DutyCycle : 90.00, actual : 90.00
PWM Channel : 15, programmed Period (us): 20000.00, actual : 19999, programmed DutyCycle : 95.00, actual : 95.00
SimpleTimer (ms): 2000, us : 23848190, Dus : 10011373
PWM Channel : 0, programmed Period (us): 1000000.00, actual : 1000000, programmed DutyCycle : 5.00, actual : 5.00
PWM Channel : 1, programmed Period (us): 500000.00, actual : 500000, programmed DutyCycle : 10.00, actual : 10.00
PWM Channel : 2, programmed Period (us): 333333.34, actual : 333340, programmed DutyCycle : 20.00, actual : 20.00
PWM Channel : 3, programmed Period (us): 250000.00, actual : 250000, programmed DutyCycle : 30.00, actual : 30.00
PWM Channel : 4, programmed Period (us): 200000.00, actual : 200000, programmed DutyCycle : 40.00, actual : 40.00
PWM Channel : 5, programmed Period (us): 166666.67, actual : 166679, programmed DutyCycle : 45.00, actual : 44.98
PWM Channel : 6, programmed Period (us): 142857.14, actual : 142860, programmed DutyCycle : 50.00, actual : 49.99
PWM Channel : 7, programmed Period (us): 125000.00, actual : 125000, programmed DutyCycle : 55.00, actual : 54.99
PWM Channel : 8, programmed Period (us): 111111.11, actual : 111120, programmed DutyCycle : 60.00, actual : 59.99
PWM Channel : 9, programmed Period (us): 100000.00, actual : 99999, programmed DutyCycle : 65.00, actual : 65.00
PWM Channel : 10, programmed Period (us): 66666.66, actual : 66679, programmed DutyCycle : 70.00, actual : 69.98
PWM Channel : 11, programmed Period (us): 50000.00, actual : 50000, programmed DutyCycle : 75.00, actual : 75.00
PWM Channel : 12, programmed Period (us): 40000.00, actual : 40000, programmed DutyCycle : 80.00, actual : 80.00
PWM Channel : 13, programmed Period (us): 33333.33, actual : 33339, programmed DutyCycle : 85.00, actual : 84.94
PWM Channel : 14, programmed Period (us): 25000.00, actual : 24999, programmed DutyCycle : 90.00, actual : 90.01
PWM Channel : 15, programmed Period (us): 20000.00, actual : 20000, programmed DutyCycle : 95.00, actual : 95.00
```

---

### 3. ISR_4_PWMs_Array_Complex on SEEED_XIAO_M0

The following is the sample terminal output when running example [ISR_4_PWMs_Array_Complex](examples/SAMD21/ISR_16_PWMs_Array_Complex) on SEEED_XIAO_M0 to demonstrate how to use multiple PWM channels with complex callback functions, the accuracy of ISR Hardware PWM-channels, **especially when system is very busy**.  The ISR PWM-channels is **running exactly according to corresponding programmed periods and duty-cycles**


```
Starting ISR_16_PWMs_Array_Complex on SEEED_XIAO_M0
SAMD_Slow_PWM v1.2.2
Starting ITimer OK, micros() = 2681651
Channel : 0	Period : 1000000		OnTime : 50000	Start_Time : 2682003
Channel : 1	Period : 500000		OnTime : 50000	Start_Time : 2682003
Channel : 2	Period : 66666		OnTime : 13333	Start_Time : 2682003
Channel : 3	Period : 50000		OnTime : 15000	Start_Time : 2682003
SimpleTimer (ms): 2000, us : 22699166, Dus : 10011622
PWM Channel : 0, programmed Period (us): 1000000, actual : 1000000, programmed DutyCycle : 5, actual : 5.00
PWM Channel : 1, programmed Period (us): 500000, actual : 500009, programmed DutyCycle : 10, actual : 10.00
PWM Channel : 2, programmed Period (us): 66666, actual : 66679, programmed DutyCycle : 20, actual : 19.98
PWM Channel : 3, programmed Period (us): 50000, actual : 50005, programmed DutyCycle : 30, actual : 29.99
SimpleTimer (ms): 2000, us : 32713108, Dus : 10013942
PWM Channel : 0, programmed Period (us): 1000000, actual : 1000000, programmed DutyCycle : 5, actual : 5.00
PWM Channel : 1, programmed Period (us): 500000, actual : 499999, programmed DutyCycle : 10, actual : 10.00
PWM Channel : 2, programmed Period (us): 66666, actual : 66679, programmed DutyCycle : 20, actual : 19.98
PWM Channel : 3, programmed Period (us): 50000, actual : 49999, programmed DutyCycle : 30, actual : 30.00
```

---

### 4. ISR_Modify_PWM on SAMD_NANO_33_IOT

The following is the sample terminal output when running example [ISR_Modify_PWM](examples/SAMD21/ISR_Modify_PWM) on **SAMD_NANO_33_IOT** to demonstrate how to modify PWM settings on-the-fly without deleting the PWM channel

```
Starting ISR_Modify_PWM on SAMD_NANO_33_IOT
SAMD_Slow_PWM v1.2.2
Starting ITimer OK, micros() = 3070515
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 3071411
Channel : 0	New Period : 500000		OnTime : 450000	Start_Time : 13071416
Channel : 0	New Period : 1000000		OnTime : 500000	Start_Time : 23071416
Channel : 0	New Period : 500000		OnTime : 450000	Start_Time : 32571416
Channel : 0	New Period : 1000000		OnTime : 500000	Start_Time : 43071416
```

---

### 5. ISR_Changing_PWM on SAMD_NANO_33_IOT

The following is the sample terminal output when running example [ISR_Changing_PWM](examples/SAMD21/ISR_Changing_PWM) on **SAMD_NANO_33_IOT** to demonstrate how to modify PWM settings on-the-fly by deleting the PWM channel and reinit the PWM channel

```
Starting ISR_Changing_PWM on SAMD_NANO_33_IOT
SAMD_Slow_PWM v1.2.2
Starting ITimer OK, micros() = 2820370
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 2821335
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 12822309
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 22823296
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 32824232
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 42825239
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 52826230
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 62827189
```

---

### 6. ISR_Modify_PWM on ITSYBITSY_M4

The following is the sample terminal output when running example [ISR_Modify_PWM](examples/SAMD51/ISR_Modify_PWM) on **ITSYBITSY_M4** to demonstrate how to modify PWM settings on-the-fly without deleting the PWM channel

```
Starting ISR_Modify_PWM on ITSYBITSY_M4
SAMD_Slow_PWM v1.2.2
Starting ITimer OK, micros() = 3320212
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 3320837
Channel : 0	New Period : 500000		OnTime : 450000	Start_Time : 13320852
Channel : 0	New Period : 1000000		OnTime : 500000	Start_Time : 23320852
Channel : 0	New Period : 500000		OnTime : 450000	Start_Time : 32820852
Channel : 0	New Period : 1000000		OnTime : 500000	Start_Time : 43320852
Channel : 0	New Period : 500000		OnTime : 450000	Start_Time : 52820852
Channel : 0	New Period : 1000000		OnTime : 500000	Start_Time : 63320852
Channel : 0	New Period : 500000		OnTime : 450000	Start_Time : 72820852
Channel : 0	New Period : 1000000		OnTime : 500000	Start_Time : 83320852
Channel : 0	New Period : 500000		OnTime : 450000	Start_Time : 92820852
Channel : 0	New Period : 1000000		OnTime : 500000	Start_Time : 103320852
Channel : 0	New Period : 500000		OnTime : 450000	Start_Time : 112820852
Channel : 0	New Period : 1000000		OnTime : 500000	Start_Time : 123320852
Channel : 0	New Period : 500000		OnTime : 450000	Start_Time : 132820852
Channel : 0	New Period : 1000000		OnTime : 500000	Start_Time : 143320852
Channel : 0	New Period : 500000		OnTime : 450000	Start_Time : 152820852
Channel : 0	New Period : 1000000		OnTime : 500000	Start_Time : 163320852
Channel : 0	New Period : 500000		OnTime : 450000	Start_Time : 172820852
```

---

### 7. ISR_Changing_PWM on ITSYBITSY_M4

The following is the sample terminal output when running example [ISR_Changing_PWM](examples/SAMD51/ISR_Changing_PWM) on **ITSYBITSY_M4** to demonstrate how to modify PWM settings on-the-fly by deleting the PWM channel and reinit the PWM channel

```
Starting ISR_Changing_PWM on ITSYBITSY_M4
SAMD_Slow_PWM v1.2.2
Starting ITimer OK, micros() = 3660252
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 3660870
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 13661504
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 23662239
```

---
---

### Debug

Debug is enabled by default on Serial.

You can also change the debugging level `_PWM_LOGLEVEL_` from 0 to 4

```cpp
// Don't define _PWM_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define _PWM_LOGLEVEL_     0
```

---

### Troubleshooting

If you get compilation errors, more often than not, you may need to install a newer version of the core for Arduino boards.

Sometimes, the library will only work if you update the board core to the latest version because I am using newly added functions.


---
---

### Issues

Submit issues to: [SAMD_Slow_PWM issues](https://github.com/khoih-prog/SAMD_Slow_PWM/issues)

---
---

## TO DO

1. Search for bug and improvement.
2. Similar features for remaining Arduino boards

---

## DONE

 1. Basic hardware multi-channel PWM for **SAMD21/SAMD51 boards** such as NANO_33_IOT, ITSYBITSY_M4, SEEED_XIAO_M0, SparkFun_SAMD51_Thing_Plus, etc. using Arduino, Adafruit or Sparkfun core
 2. Add Table of Contents
 3. Add functions to modify PWM settings on-the-fly
 4. Fix `multiple-definitions` linker error. Drop `src_cpp` and `src_h` directories
 5. DutyCycle to be optionally updated at the end current PWM period instead of immediately.
 6. Add examples [SAMD21 multiFileProject](examples/SAMD21/multiFileProject) and [SAMD51 multiFileProject](examples/SAMD51/multiFileProject)to demo for multiple-file project
 7. Improve accuracy by using `float`, instead of `uint32_t` for `dutycycle`
 8. Optimize library code by using `reference-passing` instead of `value-passing`
 9. Add support to many more boards, such as `SAMD21E1xA`, `SAMD21G1xA` and`SAMD21J1xA`
10. Display informational warning only when `_PWM_LOGLEVEL_` > 3

---
---

### Contributions and Thanks

Many thanks for everyone for bug reporting, new feature suggesting, testing and contributing to the development of this library.

1. [toniMephis](https://github.com/toniMephis) to report issue [missing header #2](https://github.com/khoih-prog/SAMD_Slow_PWM/issues/2) leading to version v1.2.2.

<table>
  <tr>
    <td align="center"><a href="https://github.com/toniMephis"><img src="https://github.com/toniMephis.png" width="100px;" alt="toniMephis"/><br /><sub><b>toniMephis</b></sub></a><br /></td>
  </tr> 
</table>

---

## Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

### License

- The library is licensed under [MIT](https://github.com/khoih-prog/SAMD_Slow_PWM/blob/master/LICENSE)

---

## Copyright

Copyright 2021- Khoi Hoang


