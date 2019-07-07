/*
 * SatCom Source
 *
 * @file    satcom.cpp
 * @author  UBC Rocket Avionics 2018/2019
 * @description   Initializes the satcom module.  Sends a message over SatCom when
 * the SatComSend function is called.  This source utilizes the IridiumSBD.cpp
 * and .h files also included in this distro.
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

/*Includes------------------------------------------------------------*/
#include "satcom.h"
#include "sensors.h"
#include "options.h"
#include <string.h>
#include <Arduino.h>
#include "IridiumSBD.h"
#include <HardwareSerial.h>

/*Constants------------------------------------------------------------*/
#define BUFSIZE 100

/*Variables-------------------------------------------------------------*/
// Declare the IridiumSBD object
static IridiumSBD modem(IridiumSerial);

/*Functions------------------------------------------------------------*/

/* bool SatComSetup(void){}
 * @brief  Sets up the SatCom serial connection and the SatCom modem
 * @param  void
 * @return setup success flag -> boolean false = failed, true = success.
 */
bool SatComSetup()
{
  bool SatComSuccess = true;
  int err;

  // Start the serial port connected to the satellite modem
  IridiumSerial.begin(19200);
  while (!IridiumSerial){};

  // Begin satellite modem operation
  #ifdef TESTING
  SerialUSB.println("Starting modem...");
  #endif
  err = modem.begin();

  if (err != ISBD_SUCCESS)
  {

    #ifdef TESTING
    SerialUSB.print("Begin failed: error ");
    SerialUSB.println(err);
    if (err == ISBD_NO_MODEM_DETECTED){
      SerialUSB.println("No modem detected: check wiring.");
    }
    #endif

    SatComSuccess = false;
    return SatComSuccess;
  }

  else
  {
    #ifdef TESTING
    SerialUSB.print("Modem Initialized");
    #endif

    SatComSuccess = true;
  }

  // Signal Quality Test:
  #ifdef TESTING
  int signalQuality = -1;

  err = modem.getSignalQuality(signalQuality);
  if (err != ISBD_SUCCESS)
  {
    SerialUSB.print("SignalQuality failed: error ");
    SerialUSB.println(err);
    return SatComSuccess;
  }
  SerialUSB.print("On a scale of 0 to 5, signal quality is currently ");
  SerialUSB.print(signalQuality);
  SerialUSB.println(".");
  #endif

  return SatComSuccess;
}

/* int SatComQuality(void){}
 * @brief  Finds and returns the current Signal Quality of the SatCom connection
 * @param  void
 * @return int signal quality from 0 to 5, -1 if the signal quality check fails.
 * 2 or better is preferred
 */
int SatComQuality()
{
  int SignalQuality = -1;

  #ifdef TESTING
  int err = modem.getSignalQuality(SignalQuality);

  if (err != ISBD_SUCCESS)
  {
    SerialUSB.print("SignalQuality failed: error ");
    SerialUSB.println(err);
    return SignalQuality;
  }
  SerialUSB.print("On a scale of 0 to 5, signal quality is currently ");
  SerialUSB.print(SignalQuality);
  SerialUSB.println(".");
  #endif

  return SignalQuality; // between 0 and 5 if valid, -1 if error

}

/* void SatComSendGPS(unsigned long*, float[]){}
 * @brief  Sends an unsigned long timestamp and GPS array through the SatCom
 * @param  unsigned long *timestamp - the time stamp of the incoming data
 * @param  float GPS_data[] - most recently acquired GPS data
 * @return void.
 */
void SatComSendGPS(unsigned long *timestamp, float GPS_data[])
{
    char sendbuf[BUFSIZE];
    String send_timestamp = String(*timestamp);

    String lat1 = String(GPS_data[0], 5);
    String long1 = String(GPS_data[1], 5);
    String altitude = String(GPS_data[2]);

    String satsend = String("timestamp: " + send_timestamp + " Lat: " + lat1 +
                                      " Long: " + long1 + " Alt: " + altitude);

    satsend.toCharArray(sendbuf, BUFSIZE);
    int err = modem.sendSBDText(sendbuf);
    if (err != ISBD_SUCCESS)
    {
      #ifdef TESTING
      SerialUSB.print("sendSBDText failed: error ");
      SerialUSB.println(err);
      if (err == ISBD_SENDRECEIVE_TIMEOUT)
        SerialUSB.println("Try again with a better view of the sky.");
      #endif
    }

    else
    {
      #ifdef TESTING
      SerialUSB.println("sendSBDText: Success!");
      #endif
    }
}

/* void SatComSendCharArray(char SendArray[]){}
 * @brief  Sends a char array through the SatCom
 * @param  char Send_Array[] - a 1D char array with no new line characters
 * @return void.
 */
void SatComSendCharArray(char Send_Array[])
{
    int err = modem.sendSBDText(Send_Array);

    if (err != ISBD_SUCCESS)
    {
      #ifdef TESTING
      SerialUSB.print("sendSBDText failed: error ");
      SerialUSB.println(err);
      if (err == ISBD_SENDRECEIVE_TIMEOUT)
        SerialUSB.println("Try again with a better view of the sky.");
      #endif
    }

    else
    {
      #ifdef TESTING
      SerialUSB.println("sendSBDText: Success!");
      #endif
    }
}

// Unused SATCOM diagnostics
#ifdef DIAGNOSTICS
void ISBDConsoleCallback(IridiumSBD *device, char c)
{
  SerialUSB.write(c);
}

void ISBDDiagsCallback(IridiumSBD *device, char c)
{
  SerialUSB.write(c);
}
#endif
