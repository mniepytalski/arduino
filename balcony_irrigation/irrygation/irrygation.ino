#include "RTCDS1307.h"
#include "LowPower.h"
#include <stdio.h>
#include "pump.h"
#include "clock.h"
#include "watchdog.h"
#include "display.h"
#include "key.h"

#define REAL_SYSTEM

Watchdog watchdog;
Clock clock;
Display display;
Pump pump1(7, 8, 0);
Pump pump2(6, 8, 5);

Key key1(4);
Key key2(8);
int timerCounter = 0;
int systemCounter = 0;

char buffer[20];

void setup()
{
  display.init();  
  clock.start();
//  clock.setTime(18, 14, 0);

  watchdog.init();
}

void loop()
{
  clock.getTime();

  for ( ; ; ) {
    sprintf(buffer,"System%02x %06x,%x", 11, systemCounter, clock.getDayCounter());
    display.print(2, 0, buffer);

    watchdog.reset();
    clock.getTime();
    display.print(0, 1, clock.getFullTimeToString());

    printKeysStatus();
    if(checkConditionToOnEngines(pump1.getWorkHour(),pump1.getWorkMinute()) || key1.isKeyPressed()) {
      pump1.on();
    }
    if(checkConditionToOnEngines(pump2.getWorkHour(),pump2.getWorkMinute()) || key2.isKeyPressed()) {
      pump2.on();
    }
    
    checkPumps();
    printInfo();
    printTemerature(0);
    if ( timerCounter>0 ) {
      timerCounter--;
    } 

    sleep1s();
    systemCounter++;
  }
}

void sleep1s() {
    if ( systemCounter>=0 && systemCounter<(5*60) ) {
      delay(1000);
    } else {
      LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
      LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
    }  
}

void printKeysStatus() {
  display.print(0, 0, key1.read());
  display.print(1, 0, key2.read());   
}

boolean checkConditionToOnEngines(int hour,int minute) {
  if ( clock.getDayCounter()>=3 ) {
    return false;
  }
  if ( timerCounter>0 ) {
    return false;
  } 
  boolean test = clock.getHour() == hour && clock.getMinute() == minute ;
  if ( test ) {
    timerCounter=60; // block it on X seconds
  }
  return test;
}

void checkPumps() {
  pump1.checkAndOff();
  pump2.checkAndOff();
}

void printInfo() {
  printInfo(2, &pump1);
  printInfo(3, &pump2);
}

void printInfo(int lcdRow, Pump *pump) {
  sprintf(buffer,"%X>%02d,%02d:%02d>%d", pump->getWorkCounter(), pump->getEngineCounter(), 
              pump->getWorkHour(), pump->getWorkMinute(), timerCounter);
  display.print(0, lcdRow, buffer);  
}

void printTemerature(int temperature) {
  sprintf(buffer,"%d%cC", temperature, 0xDF);
  display.print(16, 2, buffer);  

  //buffer[2] = 0xDF;
  sprintf(buffer,"%c%d%cC", 0xF6, temperature, 0xDF);
  display.print(15,3, buffer);
}
