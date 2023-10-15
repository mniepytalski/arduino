#include "RTCDS1307.h"
#include "LowPower.h"
#include <stdio.h>
#include "pump.h"
#include "clock.h"
#include "watchdog.h"
#include "display.h"
#include "key.h"

Watchdog watchdog;
Clock clock;
Display display;
Pump pump1(7);
Pump pump2(6);

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
    sprintf(buffer,"System%02x %06x,%x", 9, systemCounter, clock.getDayCounter());
    display.print(2, 0, buffer);

    watchdog.reset();
    clock.getTime();
    display.print(0, 1, clock.getFullTimeToString());

    printKeysStatus();
    if(checkConditionToOnEngines(8,0) || key1.isKeyPressed()) {
      pump1.on();
    }
    if(checkConditionToOnEngines(8,5) || key2.isKeyPressed()) {
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
    if ( systemCounter>=0 && systemCounter<(2*60) ) {
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
  if ( clock.getDay()>3 ) {
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
  printInfo(2, pump1.getWorkCounter(), pump1.getEngineCounter());
  printInfo(3, pump2.getWorkCounter(), pump2.getEngineCounter());
}

void printInfo(int lcdRow, int workCounter, int engineCounter) {
  sprintf(buffer,"%2X>%3d,czas:%2d", workCounter, engineCounter, timerCounter);
  display.print(0, lcdRow, buffer);  
}

void printTemerature(int temperature) {
  sprintf(buffer,"%2dC", temperature);
  display.print(15, 2, buffer);  
}
