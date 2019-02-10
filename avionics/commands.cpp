#include "commands.h"
//#include <Arduino.h>

void communicateThroughSerial(HardwareSerial SerialVar,FlightStates * state)
{
    char command[DATA_ARRAY_SIZE];
    char recognitionRadio[DATA_ARRAY_SIZE];
    char goodResponse[] = {'G','x','x','x','x'};
    const char badResponse[] = {'B','B','B','B','B'};


        #ifdef TESTING
        SerialUSB.print("Received Message: ");
        #endif
        while (SerialVar.available()){
            for(int i = 0; i< DATA_ARRAY_SIZE; i++){
                command[i] = SerialVar.read();
            }
            bool correctCommand = checkForValidty(command);

            if(correctCommand){
                // radiolog.print(goodResponse);
                for(int i =1; i<5; i++)
                {
                    goodResponse[i] = command[0];
                }
                #ifdef TESTING

                SerialUSB.println(command);
                doCommand(command[0], state);

                #endif

                sendRadioResponse(goodResponse);
                SerialUSB.println(goodResponse);
            }
            else{
                //radiolog.print(badResponse);
                SerialUSB.println(command);
                SerialUSB.println(goodResponse);
                sendRadioResponse(badResponse);
            }
        }
}

void doCommand(char command, FlightStates * state){
    char satComResponse[] = {'O','K','A','Y','!'};
    switch (command){
        case ARM:
        //switchState(*state, ARMED);
        case CAMERAS_ON:
        //turn on the cameras
        case CAMERAS_OFF:
        //turn off the cameras
        case HALO:
        //play HALO
        case SATCOM_TEST:
            for(int i=0; i<5; i++){
                SerialSatCom.write(satComResponse[i]);
            }
        break;
        case RESET:
        //not sure
        case MAIN:
        //testing purposes
        digitalWrite(LED_BUILTIN, HIGH);
        // delay(400);
        // digitalWrite(LED_BUILTIN,LOW);
        break;
        case DROGUE:
        //testing purposes
        // digitalWrite(LED_BUILTIN, HIGH);
        // delay(400);
        digitalWrite(LED_BUILTIN,LOW);
        break;
    }

}

void sendRadioResponse(const char* response){
    //teensy should be little endian, which means least significant is stored first, make sure ground station decodes accordingly

      for(int i=0; i<5; i++)
      {
          SerialRadio.write(response[i]);
      }
}

//checks if all indexes are equal for radio commands
bool checkForValidty(char *radioCommand)
 {
    const char a0 = radioCommand[0];

    for (int i = 1; i < 5; i++)
    {
        if (radioCommand[i] != a0)
            return false;
    }
    return true;
}

