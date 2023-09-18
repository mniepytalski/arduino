#include "pump.h"
#include <Arduino.h>

Pump::Pump(int pumpPin) {
    pin = pumpPin;
    status = false;
    pinMode(pin, OUTPUT);
    off();
}

int Pump::getPin() {
  return pin;
}

int Pump::getEngineCounter() {
  return engineCounter;
}

int Pump::getWorkCounter() {
  return workCounter;
}

void Pump::on() {
    if ( engineCounter<=0 ) {
        workCounter++;
        status = true;
        digitalWrite(pin, LOW);
        engineCounter = initWorkTime;
    }
}

void Pump::checkAndOff() {
    if ( !status ) {
        return;
    }
    if ( engineCounter>0 ) {
        engineCounter = engineCounter - 1 ;
    } else {
        off();
    }
}

void Pump::off() {
    status = false;
    digitalWrite(pin, HIGH);
}
