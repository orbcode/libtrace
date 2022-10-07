# Orbcode Trace functions library

## Overview
This library provides C/C++ functions for configuring ARM Cortex-M trace components.

## Contents
Following features are supported:
* Trace Port Interface Unit
    * Configuring trace protocol
* Instrumentation Trace Macrocell
    * Configuring ITM
    * Outputing data over stimulus ports
* Data Watchpoint & Trace Unit
    * Configuring DWT including PC sampling, timestamp generations and counters
    * Setting up watchpoints

All functions are available as header-only library depending only on CMSIS `core_cmXX.h` header provided by MCU vendor. Once library is available (see Installation section below) it can be used in application code as follow:

```cpp
#include "my_mcu_device.h" // Vendor-provided header that includes core_cmXX.h
#include "orbcode/trace/tpiu.h"
#include "orbcode/trace/itm.h"
#include "orbcode/trace/dwt.h"

static void ConfigureTraceComponents()
{
    TpiuOptions tpiu;
    // TODO: setup TPIU using by setting fields in tpiu variable
    ITMOptions itm;
    // TODO: setup ITM using by setting fields in itm variable
    DWTOptions dwt;
    // TODO: setup DWT using by setting fields in dwt variable

    // Apply configuration as defined in structs
    TpiuSetup(&tpiu);
    ITMSetup(&itm);
    DWTSetup(&dwt);
}

static void UseITM()
{
    // Output data on ITM stimulus port
    ITMWrite8(4, 'H');
    ITMWrite8(4, 'E');
    ITMWrite8(4, 'L');
    ITMWrite8(4, 'L');
    ITMWrite8(4, 'O');
}

int main()
{
    ConfigureVendorSpecificOptionsRequiredForTracing(); // i.e. clock, GPIOs
    ConfigureTraceComponents();

    UseITM();
}
```

Refer to documentation of each module for details how each component is configured and what functionalities are available.

**Warning:** Currently `libtrace` is optimistic when it comes to MCU capabilities. For devices with limited trace features it is possible for `libtrace` to generate invalid configuration. If that happens, please let us know by submitting issue and we will try to adapt library.

## Installation
As library is header only it is straightforward to use with any build system.

### CMake + FetchContent
For projects using CMake it is recommended to use `FetchContent` to download `libtrace` library and include it in build system automatically.

Fetch library:

```cmake
include(FetchContent)

FetchContent_Declare(
    libtrace
    GIT_REPOSITORY https://github.com/orbcode/libtrace.git
    GIT_TAG v0.1.0
)
FetchContent_MakeAvailable(libtrace)
```

Include `Orbcode::Trace` as dependency:

```cmake
target_link_libraries(MyApplication PUBLIC Orbcode::Trace)
```

### Other build systems
Other build system must download `libtrace` manually (git submodule, wget, etc) and add folder `libs/trace/include` to include directories in compiler command line:

```
CFLAGS += -Idownloaded_libs/libtrace/libs/trace/include
```

## Building
Although for end-user this library can be treated as header-only with no build steps required, for development of library itself it is useful to be able to generate proper build system.

Simple command line for using Ninja generator:
```
cmake -G Ninja -S<libtrace source dir> -B<build directory> --toolchain <toolchain file for arm-none-eabi> -DORBCODE_LIBTRACE_BUILD_TEST=ON -DORBCODE_LIBTRACE_DOCS=ON
ninja -C <build directory> # Compile test libraries
ninja -C <build directory> docs # Generate Doxygen documentation in <build directory>/docs/html
```

Following options are availble:
* `ORBCODE_LIBTRACE_BUILD_TEST` (default: `OFF`) - compile simple test files to make sure that header files are correct (useful for detecting syntax errors). When this option is enable toolchain capable for compiling for ARM Cortex-M is required (e.g. arm-none-eabi-gcc with `-mmcu=cortex-m3`)
* `ORBCODE_LIBTRACE_DOCS` (default: `OFF`) - build Doxygen documentation (requires Doxygen)