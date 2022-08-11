/** @file */

#pragma once
#include <stdbool.h>
#include <stdint.h>

#if !defined(TPI)
#    error \
        "TPI not defined. Include tpiu.h AFTER core_cmX.h (typically after including device-specific header)"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @defgroup tpiu Trace Port Interface Unit
     * @ingroup trace
     *
     * @brief Configure how trace data is transmitted from MCU
     *
     * Trace Port Interface Unit (TPIU) is responsible for outputing data received from ITM and ETM using one of configured trace protocols (@ref TpiuProtocol).
     *
     * Reference: ARMv7-M Architecture Reference Manual, chapter C1.10
     *
     * @{
     */

    /**
     * @brief Available trace protocols
     */
    typedef enum
    {
        /**
         * @brief Parallel trace port 1-4 bits
         * Use tool like Orbtrace to receive data. Good for transmitting lots of information (e.g. ETM trace)
         */
        TpiuProtocolParallel = 0,

        /**
         * @brief Manchester encoded data
         * Use tool like Orbtrace to receive data. Single pin, lower bandwitdh, speed automatically detected.
         */
        TpiuProtocolSwoManchester = 1,

        /**
         * @brief UART (NRZ) encoded data
         * Use any UART adapter to receive data. Single pin, easy to access, low bandwidth. Receiver baudrate must match.
         */
        TpiuProtocolSwoUart = 2,
    } TpiuProtocol;

    /**
     * @brief TPIU configuration options
     */
    typedef struct
    {
        /**
         * @brief Selected protocol
         *
         * Not every protocol might be available on each MCU, refer to vendor documentation for details.
         * For parallel trace, specify trace port width in @ref TracePortWidth field.
         * For SWO protocol, specify baudrate in @ref SwoPrescaler field.
         */
        TpiuProtocol Protocol;

        /**
         * @brief Include TPIU framining in output data.
         *
         * TPIU can output data from ITM/ETM without any additional formating (`false`) or wrap it in additional
         * layer (`true`). That additional layer allows to identify trace source that produced it each packet as
         * it include **TraceBusID** value (which is set when configuring ITM or ETM).
         *
         * Remember that receiving tools probably will need to know if formatting is enabled and if it is
         * enabled - TraceBusID of each source
         */
        bool FormattingEnabled;

        /**
         * @brief Prescaler for SWO output data.
         *
         * When one of SWO protocols is selected, this field will determine output speed. For Manchester encoding
         * receiver should automatically detect data rate, for UART receiver needs to know exact baudrate.
         * Baudrate is calculated as: \f$Baudrate = \frac{TraceClock}{SwoPrescaler}\f$
         */
        int SwoPrescaler;

        /**
         * @brief Number of bits in parallel trace port
         *
         * When using parallel trace protocol it is possible to define number of data lines used to outputing data.
         */
        uint8_t TracePortWidth;
    } TpiuOptions;

    /**
     * @brief Configure TPIU component.
     *
     * After calling this function TPIU will be configured as specified in options.
     *
     * Note that there is no validation if specified configuration is valid (e.g. specified protocol
     * is not valid) and there are no checks to verify MCU capabilities (e.g. specifing more trace bits line than supported)
     *
     * @param options TPIU configuration
     */
    static inline void TpiuSetup(const TpiuOptions* options);

    /** @} */

    void TpiuSetup(const TpiuOptions* options)
    {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Enable ITM and DWT

        TPI->ACPR = options->SwoPrescaler - 1;
        TPI->SPPR = (int)options->Protocol;
        TPI->CSPSR = 1 << (options->TracePortWidth - 1);

        if(options->FormattingEnabled)
        {
            TPI->FFCR |= TPI_FFCR_EnFCont_Msk;
        }
        else
        {
            TPI->FFCR &= ~TPI_FFCR_EnFCont_Msk;
        }
    }

#ifdef __cplusplus
}
#endif