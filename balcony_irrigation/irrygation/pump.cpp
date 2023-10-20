#include "pump.h"
#include <Arduino.h>

Pump::Pump(int pumpPin, int hour, int minute) {
    pin = pumpPin;
    status = false;
    #ifdef REAL_SYSTEM
    pinMode(pin, OUTPUT);
    #endif
    workHour = hour;
    workMinute = minute;
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
        #ifdef REAL_SYSTEM
        digitalWrite(pin, LOW);
        #endif
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

int Pump::getWorkHour() {
  return workHour;
}

int Pump::getWorkMinute() {
  return workMinute;
}

void Pump::setWorkHour(int hour) {
  workHour = hour;
}

void Pump::setWorkMinute(int minute) {
  workMinute = minute;
}

void Pump::off() {
    status = false;
    #ifdef REAL_SYSTEM
    digitalWrite(pin, HIGH);
    #endif
}
