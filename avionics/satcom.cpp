/* @file    satcom.cpp
 * @author  UBC Rocket Avionics 2018/2019
 * @description   Initializes the satcom module.  Sends a message over SatCom when
 * the SatComSend function is called.
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
#include <string.h>
#include <Arduino.h>
#include "IridiumSBD.h"
#include <HardwareSerial.h>

/*Variables-------------------------------------------------------------*/
// Declare the IridiumSBD object
IridiumSBD modem(IridiumSerial);

/*Functions------------------------------------------------------------*/

/* bool SatComSetup(void){}
 * @brief  Sets up the SatCom serial connection and the SatCom modem
 * @param  void
 * @return setup success flag -> boolean false = failed, true = success.
 */
bool SatComSetup()
{
  bool SatComSuccess = true;
  int signalQuality = -1;
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

  int err = modem.getSignalQuality(SignalQuality);

  #ifdef TESTING
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
    int gps_array1[2];
    int gps_array2[2];
    char sendbuf[BUFSIZE];
    String send_timestamp = String(*timestamp);

    for (int i = 0; i < 2; i++){
      gps_array1[i] = GPS_data[i];
      gps_array2[i] = (GPS_data[i]-gps_array1[i]) * 1000;
    }

    String lat1 = String(gps_array1[0]);
    String lat2 = String(gps_array2[0]);
    String long1 = String(gps_array1[1]);
    String long2 = String(gps_array2[1]);

    // String longitude = String(GPS_data[1]); //NOT SURE
    String altitude = String(GPS_data[2]);
    String satsend = String("timestamp: " + send_timestamp + " Lat: " + lat1 + "." + lat2 +
                                      " Long: " + long1 + "." + long2 + " Alt: " + altitude);

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


/* UNUSED
// bool SatComReceive(char *) {}
// @brief Receives message from satellite via SatCom module
// @param satComCommandArray[] buffer for message to be received
// @return bool indicating if a message was received or not

bool SatComReceive(char satComCommandArray[])
{
  int err;
  uint8_t buffer[SAT_COM_DATA_ARRAY_SIZE];
  size_t bufferSize = sizeof(buffer);

  // Check to see if there are remaining messages not retrieved
  if (modem.getWaitingMessageCount() > 0)
  {
    err = modem.sendReceiveSBDText(NULL, buffer, bufferSize);

    if (err != ISBD_SUCCESS)
    {
      #ifdef TESTING
      SerialUSB.print("sendReceiveSBD* failed: error ");
      SerialUSB.println(err);
      if (err == ISBD_SENDRECEIVE_TIMEOUT)
        SerialUSB.println("Try again with a better view of the sky.");
      #endif
      return false;
    }

    else
    {
      #ifdef TESTING
      SerialUSB.print("Inbound buffer size is: ");
      SerialUSB.println(bufferSize);
      #endif

      for (unsigned int i = 0; i < bufferSize; i++)
      {
        satComCommandArray[i] = (char) buffer[i];
        #ifdef TESTING
        SerialUSB.write(buffer[i]);
        #endif
      }

      #ifdef TESTING
      SerialUSB.println();
      #endif

      return true;
    }
  }
  else
    return false;

  return false;
}
*/

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
