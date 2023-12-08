/**************************************************************************/
/*
        Distributed with a free-will license.
        Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
        MCP9803
        This code is designed to work with the MCP9803_I2CADC I2C Mini Module available from ControlEverything.com.
        https://www.controleverything.com/content/Temperature?sku=MCP9803_I2CS#tabs-0-product_tabset-2
*/
/**************************************************************************/

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

/**************************************************************************
    I2C ADDRESS/BITS
**************************************************************************/
/*
                                                                    Address
                                                                    Code    A2  A1  A0
*/
    #define MCP9803_DEFAULT_ADDRESS             (0x48)          // 1001     0    0   0
    #define MCP9803_ADDRESS_A2A1A0_001          (0x49)          // 1001     0    0   1
    #define MCP9803_ADDRESS_A2A1A0_010          (0x4A)          // 1001     0    1   0
    #define MCP9803_ADDRESS_A2A1A0_011          (0x4B)          // 1001     0    1   1
    #define MCP9803_ADDRESS_A2A1A0_100          (0x4C)          // 1001     1    0   0
    #define MCP9803_ADDRESS_A2A1A0_101          (0x4D)          // 1001     1    0   1
    #define MCP9803_ADDRESS_A2A1A0_110          (0x4E)          // 1001     1    1   0
    #define MCP9803_ADDRESS_A2A1A0_111          (0x4F)          // 1001     1    1   1

/**************************************************************************
    CONVERSION DELAY (in mS)
**************************************************************************/
    #define MCP9803_CONVERSIONDELAY             (100)

/**************************************************************************
    POINTER REGISTER
**************************************************************************/
    #define MCP9803_REG_POINTER_MASK            (0x03)
    #define MCP9803_REG_POINTER_TEMP            (0x00)
    #define MCP9803_REG_POINTER_CONFIG          (0x01)
    #define MCP9803_REG_POINTER_THYST           (0x02)
    #define MCP9803_REG_POINTER_TLIMIT          (0x03)

/**************************************************************************
    CONFIG REGISTER
**************************************************************************/

    #define MCP9803_REG_CONFIG_OS_MASK          (0x80)      // One-Shot bit
    #define MCP9803_REG_CONFIG_OS_ENABLED       (0x80)      // Enabled
    #define MCP9803_REG_CONFIG_OS_DISABLED      (0x00)      // Disabled (Power-up default)

    #define MCP9803_REG_CONFIG_RES_MASK         (0x60)      // ADC Resolution bit
    #define MCP9803_REG_CONFIG_RES_9BIT         (0x00)      // 9 bit (Power-up default)
    #define MCP9803_REG_CONFIG_RES_10BIT        (0x20)      // 10 bit
    #define MCP9803_REG_CONFIG_RES_11BIT        (0x40)      // 11 bit
    #define MCP9803_REG_CONFIG_RES_12BIT        (0x60)      // 12 bit

    #define MCP9803_REG_CONFIG_FAULTQUE_MASK    (0x18)      // Fault Queue bit
    #define MCP9803_REG_CONFIG_FAULTQUE_1       (0x00)      // 1 (Power-up default)
    #define MCP9803_REG_CONFIG_FAULTQUE_2       (0x08)      // 2
    #define MCP9803_REG_CONFIG_FAULTQUE_4       (0x10)      // 4
    #define MCP9803_REG_CONFIG_FAULTQUE_6       (0x18)      // 6

    #define MCP9803_REG_CONFIG_ALERTPOL_MASK    (0x04)      // Alert Polarity bit
    #define MCP9803_REG_CONFIG_ALERTPOL_HIGH    (0x04)      // Active-high
    #define MCP9803_REG_CONFIG_ALERTPOL_LOW     (0x00)      // Active-low (Power-up default)

    #define MCP9803_REG_CONFIG_COMPINT_MASK     (0x02)      // Comp/Int bit
    #define MCP9803_REG_CONFIG_COMPINT_INTR     (0x02)      // Interrupt output
    #define MCP9803_REG_CONFIG_COMPINT_COMP     (0x00)      // Comparator output (Power-up default)

    #define MCP9803_REG_CONFIG_SHDN_MASK        (0x01)      // Shutdown bit
    #define MCP9803_REG_CONFIG_SHDN_ENABLE      (0x01)      // Enable
    #define MCP9803_REG_CONFIG_SHDN_DISABLE     (0x00)      // Disable (Power-up default)


typedef enum
{
    ONESHOT_ENABLED     = MCP9803_REG_CONFIG_OS_ENABLED,
    ONESHOT_DISABLED    = MCP9803_REG_CONFIG_OS_DISABLED

} mcpOneShot_t;

typedef enum
{
    RESOLUTION_9        = MCP9803_REG_CONFIG_RES_9BIT,
    RESOLUTION_10       = MCP9803_REG_CONFIG_RES_10BIT,
    RESOLUTION_11       = MCP9803_REG_CONFIG_RES_11BIT,
    RESOLUTION_12       = MCP9803_REG_CONFIG_RES_12BIT
    
} mcpResolution_t;

typedef enum
{
    FAULTQUE_1          = MCP9803_REG_CONFIG_FAULTQUE_1,
    FAULTQUE_2          = MCP9803_REG_CONFIG_FAULTQUE_2,
    FAULTQUE_4          = MCP9803_REG_CONFIG_FAULTQUE_4,
    FAULTQUE_6          = MCP9803_REG_CONFIG_FAULTQUE_6
    
} mcpFaultQue_t;

typedef enum
{
    ALERTPOL_HIGH       = MCP9803_REG_CONFIG_ALERTPOL_HIGH,
    ALERTPOL_LOW        = MCP9803_REG_CONFIG_ALERTPOL_LOW
    
} mcpAlertPol_t;

typedef enum
{
    MOD_INTR            = MCP9803_REG_CONFIG_COMPINT_INTR,
    MOD_COMP            = MCP9803_REG_CONFIG_COMPINT_COMP

} mcpMod_t;

typedef enum
{
    SHUTDOWN_ENABLE     = MCP9803_REG_CONFIG_SHDN_ENABLE,
    SHUTDOWN_DISABLE    = MCP9803_REG_CONFIG_SHDN_DISABLE
    
} mcpShutdown_t;


class MCP9803
{
    protected:
        // Instance-specific properties
        uint8_t mcp_conversionDelay;
        mcpOneShot_t mcp_oneshot;
        mcpResolution_t mcp_resolution;
        mcpFaultQue_t mcp_faultqueue;
        mcpAlertPol_t mcp_alertpol;
        mcpMod_t mcp_mod;
        mcpShutdown_t mcp_shutdown;

    public:
        uint8_t mcp_i2cAddress;
        void getAddr_MCP9803(uint8_t i2cAddress);
        void begin(void);
        int16_t Measure_Temp(void);
        void setOneShot(mcpOneShot_t oneshot);
        mcpOneShot_t getOneShot(void);
        void setResolution(mcpResolution_t resolution);
        mcpResolution_t getResolution(void);
        void setFaultQue(mcpFaultQue_t faultque);
        mcpFaultQue_t getFaultQue(void);
        void setAlertPol(mcpAlertPol_t alertpol);
        mcpAlertPol_t getAlertPol(void);
        void setMod(mcpMod_t mod);
        mcpMod_t getMod(void);
        void setShutdown(mcpShutdown_t shutdown);
        mcpShutdown_t getShutdown(void);
  
    private:
};