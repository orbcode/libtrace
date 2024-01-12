#pragma once
#include <stdint.h>
#include <stdbool.h>

#if !defined(CoreDebug)
#    error \
        "CoreDebug not defined. Include dwt.h AFTER core_cmX.h (typically after including device-specific header)"
#endif

#if !defined(DWT)
#    error \
        "DWT not defined. Include dwt.h AFTER core_cmX.h (typically after including device-specific header)"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @defgroup dwt Data Watchpoint & Trace Unit
     * @ingroup trace
     *
     * @brief Use DWT to perform PC sampling and trace memory accesses
     *
     * Data Watchpoint & Trace unit provides built-in capabilites for:
     * * PC sampling with configurable interval
     * * Exception entry/exit trace
     * * Events counters
     * * Watchpoints allowing to output trace packet when memory region is accessed
     * * Cycle counter
     *
     * DWT outputs data to ITM which forwards it to formatter. When configuring ITM option ITMOptions#ForwardDWT must be enabled.
     *
     * \image html trace/dwt_clocks.diagramsnet.svg "Relationships between DWT clocks and packets"
     *
     * Reference: ARMv7-M Architecture Reference Manual, chapter C1.8 The Data Watchpoint and Trace unit
     *
     * @{
     */


    /**
     * @brief Interval of ITM synchronization packet
     *
     * See ARMv7-M Architecture Reference Manual, section C1.8.3 "CYCCNT cycle counter and related timers" for details
     */
    typedef enum
    {
        /**
         * @brief Disabled
         */
        DWTSyncTapDisabled = 0,
        /**
         * @brief Output synchronization when CYCCNT[24] bit changes
         *
         * \f$PacketRate = \frac{ProcessorClock}{16 \cdot 10^6{}}\f$
         */
        DWTSyncTap24 = 1,
        /**
         * @brief Output synchronization when CYCCNT[26] bit changes
         *
         * \f$PacketRate = \frac{ProcessorClock}{64 \cdot 10^6{}}\f$
         */
        DWTSyncTap26 = 2,

        /**
         * @brief Output synchronization when CYCCNT[28] bit changes
         *
         * \f$PacketRate = \frac{ProcessorClock}{16 \cdot 256^6{}}\f$
         */
        DWTSyncTap28 = 3,
    } DWTSyncTap;

    /**
     * @brief Interval of PC sampling and event counter packets
     *
     * See ARMv7-M Architecture Reference Manual, section C1.8.3 "CYCCNT cycle counter and related timers" for details
     */
    typedef enum
    {
        /**
         * @brief Output packet when bit CYCCNT[6] changes
         *
         * \f$POSTCNTClockRate = \frac{ProcessorClock}{64}\f$
         */
        DWTCycleTap6 = 0,
        /**
         * @brief Output packet when bit CYCCNT[10] changes
         *
         * \f$POSTCNTClockRate = \frac{ProcessorClock}{1024}\f$
         */
        DWTCycleTap10 = 1,
    } DWTCycleTap;

    /**
     * @brief DWT configuration
     *
     * Check with vendor documetation which capabilities are available.
     *
     * See ARMv7-M Architecture Reference Manual, section C1.8.7 "Control register, DWT_CTRL" for details
     */
    typedef struct
    {
        /**
         * @brief Enable folded instruction event counter
         */
        bool FoldedInstructionCounterEvent;
        /**
         * @brief Enable load/store event counter
         */
        bool LSUCounterEvent;
        /**
         * @brief Enable sleep event counter
         */
        bool SleepCounterEvent;
        /**
         * @brief Enable exception overhead counter event
         */
        bool ExceptionOverheadCounterEvent;
        /**
         * @brief Enable Cycles-per-Instruction counter event
         */
        bool CPICounterEvent;
        /**
         * @brief Enable exception entry/exit trace
         *
         * Generates packet for each exception entry and exit
         */
        bool ExceptionTrace;
        /**
         * @brief Enable PC sampling
         *
         * Periodically generates packet with current program counter. Interval defined by DWTOptions#CycleTap
         */
        bool PCSampling;
        /**
         * @brief Specifies interval for ITM synchronization and timestamp packets
         */
        DWTSyncTap SyncTap;
        /**
         * @brief Specifies divider for counter used in PC sampling and event counters
         */
        DWTCycleTap CycleTap;
        /**
         * @brief Defines prescaler for PC sampling
         *
         * Range: 1 - 16
         *
         * \f$Frequency = \frac{POSTCNTClockRate}{SamplingPrescaler}\f$
         */
        uint8_t SamplingPrescaler;
    } DWTOptions;

    /**
     * @brief Configures DWT component
     *
     * Note that there is no validation if specified configuration is valid (e.g. sampling prescaler values) and there are
     * no checks to verify MCU capabilities (enabling exception tracing when it is not implemented).
     *
     * @param options DWT configuration
     */
    static inline void DWTSetup(const DWTOptions* options);

    /**
     * @brief Configures DWT comparator
     *
     * Comparator might issue a trace packet when specified memory range is accesses for read or write.
     *
     * Number of comparators is vendor-specific, check with documention how many are available. This function does not
     * check if specified comparator is available.
     *
     * Comparator can monitor single byte in memory or range by ignoring specified number of least significant bits, e.g. to monitor single
     * uint32_t variable setup @p address parameter to variable address and @p ignoreBits parameter to 2
     *
     * @param comparator Comparator to use
     * @param address Watchpoint base address
     * @param ignoreBits Number of bits to ignore
     * @param emitRange If true, DWT will emit packet with actual data access address. if false - packet with program counter that resulted in memory access.
     * @param function Comparator function, see ARMv7-M Architecture Reference Manual, table C1-14 for list of available functions
     */
    static inline void
        DWTEnableComparator(uint8_t comparator, uintptr_t address, uint8_t ignoreBits, bool emitRange, uint8_t function);

    /**
     * @brief Disables DWT comparator
     *
     * Number of comparators is vendor-specific, check with documention how many are available. This function does not
     * check if specified comparator is available.
     *
     * @param comparator Comparator to disable
     */
    static inline void DWTDisableComparator(uint8_t comparator);

    /** @} */

    void DWTSetup(const DWTOptions* options)
    {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Enable ITM and DWT
        DWT->LAR = 0xC5ACCE55;                          // Unlock DWT access via magic number

        uint32_t ctrl = 0;
        ctrl |= (options->FoldedInstructionCounterEvent ? 1 : 0) << DWT_CTRL_FOLDEVTENA_Pos;
        ctrl |= (options->LSUCounterEvent ? 1 : 0) << DWT_CTRL_LSUEVTENA_Pos;
        ctrl |= (options->SleepCounterEvent ? 1 : 0) << DWT_CTRL_SLEEPEVTENA_Pos;
        ctrl |= (options->ExceptionOverheadCounterEvent ? 1 : 0) << DWT_CTRL_EXCEVTENA_Pos;
        ctrl |= (options->CPICounterEvent ? 1 : 0) << DWT_CTRL_CPIEVTENA_Pos;
        ctrl |= (options->ExceptionTrace ? 1 : 0) << DWT_CTRL_EXCTRCENA_Pos;
        ctrl |= (options->PCSampling ? 1 : 0) << DWT_CTRL_PCSAMPLENA_Pos;
        ctrl |= (((int)options->SyncTap) & 3) << DWT_CTRL_SYNCTAP_Pos;
        ctrl |= (((int)options->CycleTap) & 3) << DWT_CTRL_CYCTAP_Pos;
        ctrl |= (options->SamplingPrescaler - 1) << DWT_CTRL_POSTPRESET_Pos;
        ctrl |= DWT_CTRL_CYCCNTENA_Msk;

        DWT->CTRL = ctrl;
    }

    void DWTEnableComparator(uint8_t comparator, uintptr_t address, uint8_t ignoreBits, bool emitRange, uint8_t function)
    {
        uint32_t funcRaw = ((function << DWT_FUNCTION_FUNCTION_Pos) & DWT_FUNCTION_FUNCTION_Msk) |
            ((emitRange ? 1 : 0) << DWT_FUNCTION_EMITRANGE_Pos);

        switch(comparator)
        {
            case 0:
                DWT->COMP0 = address;
                DWT->MASK0 = ignoreBits;
                DWT->FUNCTION0 = funcRaw;
                break;
            case 1:
                DWT->COMP1 = address;
                DWT->MASK1 = ignoreBits;
                DWT->FUNCTION1 = funcRaw;
                break;
            case 2:
                DWT->COMP2 = address;
                DWT->MASK2 = ignoreBits;
                DWT->FUNCTION2 = funcRaw;
                break;
            case 3:
                DWT->COMP3 = address;
                DWT->MASK3 = ignoreBits;
                DWT->FUNCTION3 = funcRaw;
                break;
        }
    }

    void DWTDisableComparator(uint8_t comparator)
    {
        switch(comparator)
        {
            case 0:
                DWT->FUNCTION0 = 0;
                break;
            case 1:
                DWT->FUNCTION1 = 0;
                break;
            case 2:
                DWT->FUNCTION2 = 0;
                break;
            case 3:
                DWT->FUNCTION3 = 0;
                break;
        }
    }

#ifdef __cplusplus
}
#endif
