/** @file */

#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#if !defined(ITM)
#    error \
        "ITM not defined. Include itm.h AFTER core_cmX.h (typically after including device-specific header)"
#endif

#if !defined(CoreDebug)
#    error \
        "CoreDebug not defined. Include itm.h AFTER core_cmX.h (typically after including device-specific header)"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @defgroup itm Instrumentation Trace Macrocell
     * @ingroup trace
     *
     * @brief Configure ITM for passthrough of DWT messages and user-defined output through stimulus ports.
     *
     * Instrumentation Trace Macrocell provides following capabilites:
     * * Output of user-defined data in packets of 1,2 or 4 bytes through up to 255 separate **stimulus ports**
     * * Passthrough of DWT data (See @ref dwt)
     * * Periodic timestamp generation (Needs additional configuration in @ref dwt)
     *
     * Reference: ARMv7-M Architecture Reference Manual, chapter C1.7 The Instrumentation Trace Macrocell
     * 
     * @{
     */

/**
 * @brief Enable all ITM stimuls ports
 *
 * Set as value of ITMOptions#EnabledStimulusPorts to enable all available stimulus ports.
 */
#define ITM_ENABLE_STIMULUS_PORTS_ALL 0xFFFFFFFF

    /**
     * @brief Global timestamp frequency
     *
     * See ITMOptions#GlobalTimestampFrequency for more details
     */
    typedef enum
    {
        ITMGlobalTimestampFrequencyDisabled = 0,
        ITMGlobalTimestampFrequencyPosition7 = 1,
        ITMGlobalTimestampFrequencyPosition13 = 2,
        ITMGlobalTimestampFrequencyIfOutputFIFOEmpty = 3
    } ITMGlobalTimestampFrequency;

    /**
     * @brief Local timestamp frequency
     *
     * See ITMOptions#LocalTimestampPrescaler for more details
     */
    typedef enum
    {
        ITMLocalTimestampPrescalerNoPrescaling = 0,
        ITMLocalTimestampPrescalerDivideBy4 = 1,
        ITMLocalTimestampPrescalerDivideBy10 = 2,
        ITMLocalTimestampPrescalerDivideBy64 = 3,
    } ITMLocalTimestampPrescaler;

    /**
     * @brief ITM configuration options
     *
     * Check with vendor documetation which capabilities are available.
     */
    typedef struct
    {
        /**
         * @brief Trace bus ID
         *
         * This value is available in TPIU packets when formatting (TpiuOptions#FormattingEnabled) is enabled. It used to differentiate different trace data sources (e.g. ITM and ETM.)
         */
        int TraceBusID;

        /**
         * @brief Configure global timestamp frequency
         *
         * Global timestamps are absolute values.
         *
         * See ARMv7-M Architecture Reference Manual, section C1.7.1 ITM operation subsection Timestamp support for details.
         */
        ITMGlobalTimestampFrequency GlobalTimestampFrequency;

        /**
         * @brief Configure local timestamp frequency
         *
         * Each local timestamp packet provides time delta relative to previous local timestamp packet.
         *
         * See ARMv7-M Architecture Reference Manual, section C1.7.1 ITM operation subsection Timestamp support for details.
         */
        ITMLocalTimestampPrescaler LocalTimestampPrescaler;

        /**
         * @brief Enable local timestamps
         *
         * When enabled, local timestamp packets will be generated according to @ref LocalTimestampPrescaler setting.
         */
        bool EnableLocalTimestamp;

        /**
         * @brief Forward DWT packets
         *
         * This is necessary to output e.g. PC sampling or data watchpoint packets. If the only required ITM capability is stimululs port output, there is no need to enable this option
         */
        bool ForwardDWT;

        /**
         * @brief Enable ITM synchronization packet
         *
         * Enable periodic synchronization packet which can be used by receiver to synchronize to incoming bit stream.
         *
         * See ARMv7-M Architecture Reference Manual, section C1.7.1 ITM operation subsection Synchronization support for details.
         */
        bool EnableSyncPacket;

        /**
         * @brief Enable stimulus port
         *
         * Each bit corresponds to single stimulus port. Bit set to 1 means that port is enabled, set to 0 - disabled. It is valid
         * to set 1 for ports not available in specific MCU, however these ports will still be treated as disabled.
         *
         * Set to @ref ITM_ENABLE_STIMULUS_PORTS_ALL to enable all available ports.
         */
        uint32_t EnabledStimulusPorts;
    } ITMOptions;

    /**
     * @brief Configures ITM as requested.
     *
     * Note that there is no validation if specified configuration is valid (e.g. timestamp prescaler values) and there are no
     * checks to verify MCU capabilities (enabling timestamp when it is not implemented).
     *
     * @param options ITM configuration
     */
    static inline void ITMSetup(const ITMOptions* options);

    /**
     * @brief Checks if stimulus port is enabled
     *
     * @param port Port to check
     * @return true Port is enabled
     * @return false Port is disabled (or not available)
     */
    static inline bool ITMIsPortEnabled(uint8_t port);

    /**
     * @brief Writes 8-bit value to stimulus port
     *
     * @param port Port
     * @param c Value to be written
     */
    static inline void ITMWrite8(uint8_t port, uint8_t c);

    /**
     * @brief Writes 16-bit value to stimulus port
     *
     * @param port Port
     * @param value Value to be written
     */
    static inline void ITMWrite16(uint8_t port, uint16_t value);

    /**
     * @brief Writes 32-bit value to stimulus port
     *
     * @param port Port
     * @param value Value to be written
     */
    static inline void ITMWrite32(uint8_t port, uint32_t value);

    /**
     * @brief Writes buffer to stimulus port
     *
     * Writes buffer to stimulus port using largest packet size possible - starts with 32-bit writes, followed by 16-bit writes and 8-bit write.
     *
     * Data from buffer are memcpy'ied which is subject to compiler optimization and might result in unaligned memory access if compiler 
     * decides it is valid (e.g. on ARMv7-M platform).
     *
     * @param port Port
     * @param buffer Buffer to be written (must not be NULL)
     * @param size Size of buffer to be written
     */
    static inline void ITMWriteBuffer(uint8_t port, const void* buffer, size_t size);

    /** @} */

    void ITMSetup(const ITMOptions* options)
    {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Enable ITM and DWT
        ITM->LAR = 0xC5ACCE55;                          // unlock ITM access (magic number)

        uint32_t tcr = 0;

        tcr |= options->TraceBusID << ITM_TCR_TraceBusID_Pos;
        tcr |= ((int)options->GlobalTimestampFrequency) << ITM_TCR_GTSFREQ_Pos;
        tcr |= ((int)options->LocalTimestampPrescaler) << ITM_TCR_TSPrescale_Pos;
        tcr |= (options->ForwardDWT ? 1 : 0) << ITM_TCR_DWTENA_Pos;
        tcr |= (options->EnableSyncPacket ? 1 : 0) << ITM_TCR_SYNCENA_Pos;
        tcr |= (options->EnableLocalTimestamp ? 1 : 0) << ITM_TCR_TSENA_Pos;
        tcr |= ITM_TCR_ITMENA_Msk;

        ITM->TCR = tcr;

        ITM->TER = 0xFFFFFFFF; // Enable all stimulus ports
    }

    bool ITMIsPortEnabled(uint8_t port)
    {
        return ((ITM->TCR & ITM_TCR_ITMENA_Msk) != 0UL) && /* ITM enabled */
            ((ITM->TER & (1 << port)) != 0UL);             /* ITM Port enabled */
    }

    void ITMWrite8(uint8_t port, uint8_t value)
    {
        if(!ITMIsPortEnabled(port))
        {
            return;
        }

        while(ITM->PORT[port].u32 == 0UL)
        {
            __NOP();
        }
        ITM->PORT[port].u8 = value;
    }

    void ITMWrite16(uint8_t port, uint16_t value)
    {
        if(!ITMIsPortEnabled(port))
        {
            return;
        }

        while(ITM->PORT[port].u32 == 0UL)
        {
            __NOP();
        }
        ITM->PORT[port].u16 = value;
    }

    void ITMWrite32(uint8_t port, uint32_t value)
    {
        if(!ITMIsPortEnabled(port))
        {
            return;
        }

        while(ITM->PORT[port].u32 == 0UL)
        {
            __NOP();
        }
        ITM->PORT[port].u32 = value;
    }

    void ITMWriteBuffer(uint8_t port, const void* buffer, size_t size)
    {
        if(!ITMIsPortEnabled(port))
        {
            return;
        }

        const uint8_t* buf8 = (const uint8_t*)buffer;
        while(size >= 4)
        {
            uint32_t v;
            memcpy(&v, buf8, sizeof(v));
            while(ITM->PORT[port].u32 == 0UL)
            {
                __NOP();
            }
            ITM->PORT[port].u32 = v;

            buf8 += sizeof(v);
            size -= sizeof(v);
        }

        while(size >= 2)
        {
            uint16_t v;
            memcpy(&v, buf8, sizeof(v));
            memcpy(&v, buf8, sizeof(v));
            while(ITM->PORT[port].u32 == 0UL)
            {
                __NOP();
            }
            ITM->PORT[port].u16 = v;

            buf8 += sizeof(v);
            size -= sizeof(v);
        }

        while(size > 0)
        {
            while(ITM->PORT[port].u32 == 0UL)
            {
                __NOP();
            }
            ITM->PORT[port].u8 = *(buf8++);

            size--;
        }
    }

#ifdef __cplusplus
}
#endif