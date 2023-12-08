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

#include "MCP9803.h"

/**************************************************************************/
/*
        Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static uint8_t i2cread(void)
{
    #if ARDUINO >= 100
        return Wire.read();
    #else
        return Wire.receive();
    #endif
}

/**************************************************************************/
/*
        Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static void i2cwrite(uint8_t x)
{
    #if ARDUINO >= 100
        Wire.write((uint8_t)x);
    #else
        Wire.send(x);
    #endif
}

/**************************************************************************/
/*
        Writes 8-bits to the specified destination register
*/
/**************************************************************************/
static void writeRegister(uint8_t i2cAddress, uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(i2cAddress);
    i2cwrite((uint8_t)reg);
    i2cwrite((uint8_t)(value));
    Wire.endTransmission();
}

/**************************************************************************/
/*
        Reads 16-bits to the specified destination register
*/
/**************************************************************************/
static uint16_t readRegister(uint8_t i2cAddress, uint8_t reg)
{
    Wire.beginTransmission(i2cAddress);
    i2cwrite((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom(i2cAddress, (uint8_t)2);
    return (int16_t)((i2cread()<< 8) | i2cread());
}

/**************************************************************************/
/*
        Instantiates a new MCP9803 class with appropriate properties
*/
/**************************************************************************/
void MCP9803::getAddr_MCP9803(uint8_t i2cAddress)
{
    mcp_i2cAddress = i2cAddress;
    mcp_conversionDelay = MCP9803_CONVERSIONDELAY;
}

/**************************************************************************/
/*
        Sets up the Hardware
*/
/**************************************************************************/
void MCP9803::begin()
{
    Wire.begin();
}

/**************************************************************************/
/*
        Sets the One-Shot Mode
        The One-shot mode performs a single temperature
        measurement and returns to Shutdown mode
*/
/**************************************************************************/
void MCP9803::setOneShot(mcpOneShot_t oneshot)
{
    mcp_oneshot = oneshot;
}

/**************************************************************************/
/*
        Gets the One-Shot Mode
*/
/**************************************************************************/
mcpOneShot_t MCP9803::getOneShot()
{
    return mcp_oneshot;
}


/**************************************************************************/
/*
        Sets the ADC Resolution
        The user can gain better insight into the trends and characteristics
        of the ambient temperature by using a finer resolution.
*/
/**************************************************************************/
void MCP9803::setResolution(mcpResolution_t resolution)
{
    mcp_resolution = resolution;
}

/**************************************************************************/
/*
        Gets the ADC Resolution
*/
/**************************************************************************/
mcpResolution_t MCP9803::getResolution()
{
    return mcp_resolution;
}

/**************************************************************************/
/*
        Sets the Fault Queue
        The fault queue feature can be used as a filter to lessen
        the probability of spurious activation of the ALERT pin
*/
/**************************************************************************/
void MCP9803::setFaultQue(mcpFaultQue_t faultqueue)
{
    mcp_faultqueue = faultqueue;
}

/**************************************************************************/
/*
        Gets the Fault Queue
*/
/**************************************************************************/
mcpFaultQue_t MCP9803::getFaultQue()
{
    return mcp_faultqueue;
}

/**************************************************************************/
/*
        Sets the Alert Polarity
        The ALERT output can be configured as either a
        comparator output or as Interrupt Output mode
*/
/**************************************************************************/
void MCP9803::setAlertPol(mcpAlertPol_t alertpol)
{
    mcp_alertpol = alertpol;
}

/**************************************************************************/
/*
        Gets the Alert Polarity
*/
/**************************************************************************/
mcpAlertPol_t MCP9803::getAlertPol()
{
    return mcp_alertpol;
}

/**************************************************************************/
/*
        Sets the Comparator/Interrupt Mode
*/
/**************************************************************************/
void MCP9803::setMod(mcpMod_t mod)
{
    mcp_mod = mod;
}

/**************************************************************************/
/*
        Gets the the Comparator/Interrupt Mode
*/
/**************************************************************************/
mcpMod_t MCP9803::getMod()
{
    return mcp_mod;
}

/**************************************************************************/
/*
        Sets the Shutdown Mode
*/
/**************************************************************************/
void MCP9803::setShutdown(mcpShutdown_t shutdown)
{
    mcp_shutdown = shutdown;
}

/**************************************************************************/
/*
        Gets the Shutdown Mode
*/
/**************************************************************************/
mcpShutdown_t MCP9803::getShutdown()
{
    return mcp_shutdown;
}

/**************************************************************************/
/*
        Reads the results, measuring the 16-bit temperature register
*/
/**************************************************************************/
int16_t MCP9803::Measure_Temp()
{
  
    // Start with default values
    uint16_t config =   MCP9803_REG_CONFIG_FAULTQUE_1    |      // 1 (Power-up default)
                        MCP9803_REG_CONFIG_ALERTPOL_LOW  |      // Active-low (Power-up default)
                        MCP9803_REG_CONFIG_COMPINT_COMP;        // Comparator output (Power-up default)
    
    // Set One-Shot Mode
    config |= mcp_oneshot;
    
    // Set ADC Resolution
    config |= mcp_resolution;
    
    // Set Shutdown Mode
    config |= mcp_shutdown;

    // Write config register to the MCP
    writeRegister(mcp_i2cAddress, MCP9803_REG_POINTER_CONFIG, config);

    // Wait for the conversion to complete
    delay(mcp_conversionDelay);

    // Read the conversion results
    // 16-bit unsigned results for the MCP9803
    uint16_t raw_temp = readRegister(mcp_i2cAddress, MCP9803_REG_POINTER_TEMP);
    return (int16_t)raw_temp;
}