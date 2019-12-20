/*Includes------------------------------------------------------------*/
#include "hardware/ignitor.h"
#include <Arduino.h>

void Ignitor(uint8_t pin) {

}

void Ignitor::init() {
    /*init ignitor*/
    pinMode(ignitePin, OUTPUT);
    digitalWrite(ignitePin, LOW);

    /*continuity check */
    pinMode(continuityPin, OUTPUT);
    
    digitalWrite(continuityPin, HIGH);
    delayMicroseconds(CONTINUITY_CHECK_DELAY);

    int continuity = analogRead(continuityADCPin);
    digitalWrite(continuityPin, LOW);

    if (continuity <= DISCONTINUOUS_THRESHOLD) {
        status = HardwareStatus::FAILURE;
    } else {
        status = HardwareStatus::NOMINAL;
    }
}

void Ignitor::activate() {
    digitalWrite(ignitePin, HIGH);
    delay(IGNITOR_DELAY);
    digitalWrite(ignitePin, LOW);
}
