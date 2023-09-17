#include "RTCDS1307.h"
#include "LowPower.h"
#include <stdio.h>
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header
#include "key.h"
#include "pump.h"
#include "clock.h"
#include "watchdog.h"

/*-----( Declare Constants )-----*/
// LCD geometry
const int LCD_COLS = 20;
const int LCD_ROWS = 4;


////////////////////////////////////////////////////////////////////////
hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip  
int lcdStatus = 1; // 1 -on, 0-off

Clock clock;

Pump pump1(7);
Pump pump2(6);

int keyDriver = 5;
Key key1(4);
Key key2(8);
int timerCounter = 0;
int systemCounter = 0;

Watchdog watchdog;

void setup()
{
  if (lcdStatus==1 ) {
    lcdStatus = lcd.begin(LCD_COLS, LCD_ROWS);
    if(lcdStatus) {
      hd44780::fatalError(lcdStatus); // does not return
    }
  }
  pinMode(keyDriver, OUTPUT);
  digitalWrite(keyDriver, LOW);
  
  clock.start();
//  clock.setTime(19, 10, 0);

  watchdog.init();
}

void loop()
{
  clock.getTime();

  if (lcdStatus!=1 ) {
    lcd.setCursor(3, 0);
    lcd.print("Podlewaczka v005");
  }
  for ( ; ; ) {
    watchdog.reset();
    clock.getTime();
    if ( lcdStatus!=1 ) {
      lcd.setCursor(0, 1);
      lcd.print(clock.getFullTimeToString());
    }

    printKeysStatus();
    if(checkEnginesStatus(8,0) || key1.isKeyPressed()) {
      pump1.on();
    }
    if(checkEnginesStatus(8,5) || key2.isKeyPressed()) {
      pump2.on();
    }
    
    checkPumps();
    printInfo();
    if ( timerCounter>0 ) {
      timerCounter--;
    } 

    if ( systemCounter>=0 && systemCounter<60 ) {
      delay(1000);
    } else {
      LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
      LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
    }
    systemCounter++;
  }
}

void printKeysStatus() {
    if (lcdStatus!=1 ) {
      lcd.setCursor(0, 0);
      lcd.print(key1.read());
      lcd.setCursor(1, 0);
      lcd.print(key2.read());
    }
}

boolean checkEnginesStatus(int hour,int minute) {
  if ( timerCounter>0 ) {
    return false;
  } 
  boolean test = clock.getHour() == hour && clock.getMinute() == minute ;
//  boolean test = clock.getMinute()%10 == minute ;
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
  printInfo(2, pump1.getPin(), pump1.getEngineCounter());
  printInfo(3, pump2.getPin(), pump2.getEngineCounter());
}

void printInfo(int lcdRow, int pumpPin, int engineCounter) {
  if ( lcdStatus==1 ) {
    return;
  }
  char info[20];
  lcd.setCursor(0, lcdRow);
  sprintf(info,"%d>%3d, timer:%2d",pumpPin, engineCounter, timerCounter);
  lcd.print(info);  
}
