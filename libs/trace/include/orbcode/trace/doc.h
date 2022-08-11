/**
 * @defgroup trace ARM Cortex-M tracing functions
 *
 * This library provides set of low-level functions for setting up tracing components of ARM Cortex-M microcontrollers directly with code.
 *
 * Header files included in this library depend on CMSIS `core_cmXX.h` header files providing necessary type definitions and macros. They are however dependant on vendor specific information and cannot be used standalone (e.g. `IRQn_Type` is required to be defined before including `core_cmXX.h`). Therefore CMSIS headers are **not included** in this library and it is up to user to ensure proper include order:
 *
 * @code{.cpp}
 * #include <my_mcu_cm3.h>         // Typically includes also core_cm3.h
 * #include "orbcode/trace/tpiu.h" // Now we can include Orbcode functions
 * @endcode
 *
 * Before trace components can be used, they need to be configured which invovles:
 * 1. Setting up MCU specific options (trace clock, GPIO alternate functions) - refer to vendor's documentation for details.
 * 2. Setting up TPIU responsible for pushing ITM and ETM data into physical layer (SWO or parallel trace) (See @ref tpiu).
 * 3. Configure ITM module (if needed) for outputing user-defined data by stimulus port or pass-through of DWT output (See @ref itm).
 * 4. Configure DWT module (if needed) for timestamping, PC sampling, watchpoints and various counters (See @ref dwt).
 * 5. Configure ETM module (if needed) for instruction-level tracing (not covered by this library yet).
 *
 * Once trace components are configured they can be used during normal program operations (e.g. outputing data via ITM stimulus ports) or will automatically output data (like DWT PC sampling, watchpoints or ETM instruction trace).
 *
 * Refer to individual modules for details how to configure and use each component.
 */