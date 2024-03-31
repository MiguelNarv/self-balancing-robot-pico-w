# Description
The following is a project based on RP2040 Pico W oriented to a self-balancing robot. At this moment a demo is included with 2 tasks alternating LEDs with different periods to test the integration functionality.

LED_PIN_1 connected to pin 22 toggling at 50ms.   
LED_PIN_2 connected to pin 24 toggling at 100ms.

![pico pinout](<misc/PicoW pinout.png>)

Main branch will contain the project template for now. If you are interested in the robot implementation go to ToDo.

## Features:
Located under Source/lib to achieve portability, main folder contains .cmakes for configuration of each of them.

### Pico C/C++ SDK v1.5.1
Contains MCAL libraries for peripherals, CMSIS, among others. See [repository](https://github.com/raspberrypi/pico-sdk) for more information.

### FreeRTOS v11.0.1
RTOS used for deterministic task operation, critical for target application. See [repository](https://github.com/FreeRTOS/FreeRTOS-Kernel) for more information.

User configuration can be defined under Source/inc/FreeRTOSConfig.h.
Modifications:
- RP2040.h for CMSIS SytemCoreClock definition.
- SEGGER_SYSVIEW_FreeRTOS.h required patching.
- User configurations.


### SEGGER SystemView v3.54
Used to analize events timing as well as other cpu properties. See [page](https://www.segger.com/products/development-tools/systemview/) for more information.

Single shot analysis is being used.

Modifications:
- Patching for M0+ according to user manual.

![SystemView trace](<misc/SystemView trace.png>)
### Notes:
- FreeRtos config is set for 1 core as SystemView doesn't support multi core yet. If you are not interested in SystemView analysis you can set `#define configNUMBER_OF_CORES                   2` and other configurations accordingly.
- For SystemView:
    - The recording buffer can be modified in SEGGER_SYSVIEW_Conf.h, its size has an impact on your system SRAM.
    - If the number of task, interruptions, timers or their occurences increase in your application, the recording buffer will fill faster. This means that tasks with high period of execution won't be recorded unless you decrease the frequency of configTICK_RATE_HZ or increase the buffer size.  
    Error -140 in the application might mean that the buffer overflowed.
    - If tasks are invisible in timeline but visible in trace window go to the last event in timeline and start analysis from there.


## Tools
The project is designed to be used with VScode, the following extensions might be needed:

- C/C++

    Id: ms-vscode.cpptools
    Description: C/C++ IntelliSense, debugging, and code browsing.
    Version: 1.19.9
    Publisher: Microsoft
    VS Marketplace Link: https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools

- CMake

    Name: CMake Tools
    Id: ms-vscode.cmake-tools
    Description: Extended CMake support in Visual Studio Code
    Version: 1.17.17
    Publisher: Microsoft
    VS Marketplace Link: https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools

- Cortex-Debug

    Name: Cortex-Debug
    Id: marus25.cortex-debug
    Description: ARM Cortex-M GDB Debugger support for VSCode
    Version: 1.12.1
    Publisher: marus25
    VS Marketplace Link: https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug

### OPENOCD-xpack v0.12.0
Used to debug and flash the target. Portable version installed under tools folder.  
See [repo](https://github.com/xpack-dev-tools/openocd-xpack) for more information about xpack.

The configuration for integration with VScode is defined under .vscode/launch.json, upload and make tasks are defined under .vscode/tasks.json. You can modify those files accordingly.

Additionally you need to define you path to CMake in tasks.json "command", current one might be different from yours.

Any DAPLink debugger can be used, tested with LuatOS debugger.

![Debug window](<misc/Debug window.png>)

### dump_format.py
Creates the .SVDat required to load for SystemView analysis.
How to use it:


1. With cortex-debug extension create a new watch variable called _SEGGER_RTT.
2. Stop your debug session when you are done with recording, go to _SEGGER_RTT.aUp[1] and copy the address of pBuffer.
By default that index will contain the recording, if not replace it with the one that does.
3. Go to MEMORY tab and create a new view with the pBuffer address.
4. Copy all the memory view.
5. Create a .txt and paste it.
6. Open in a terminal dump_format.py and execute with the following arguments.  
    `python dump_format.py [TXT PATH - String] [MEMORY OFFSET (0 to 15) - Integer] [LENGHT (pBuffer.WrOff) - Integer]`
7. The .SVDat will be created under the same .txt path.
8. Open SystemView and load it.

## Installation
FreeRTOS and PicoSDK submodules need to be cloned as well, to do that open a git console in the project folder and do:

```
git submodule update --init --recursive
```
Alternatively you can checkout other submodule version individually doing:

```
cd Source/lib/pico-sdk
cd checkout [VERSION]
git submodule update --init --recursive
...
cd Source/lib/FreeRTOS-Kernel
cd checkout [VERSION]
git submodule update --init --recursive
```

SystemView can be download from this [page](https://www.segger.com/downloads/systemview/). Any update or patch on it needs to be done manually.