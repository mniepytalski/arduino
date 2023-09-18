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

class Display {
private:
    int pin;

public:
    explicit Display();
    void init();
    void print(int col, int row, int digit);
};

Display::Display() {
}

void Display::init() {
  if (lcdStatus==1 ) {
    lcdStatus = lcd.begin(LCD_COLS, LCD_ROWS);
    if(lcdStatus) {
      hd44780::fatalError(lcdStatus); // does not return
    }
  }  
}

void Display::print(int col, int row, int digit) {
  if (lcdStatus!=1 ) {
    lcd.setCursor(col, row);  
    lcd.print(digit);
  }
}

Watchdog watchdog;
Clock clock;
Display display;
Pump pump1(7);
Pump pump2(6);

int keyDriver = 5;
Key key1(4);
Key key2(8);
int timerCounter = 0;
int systemCounter = 0;

char buffer[20];

void setup()
{
  display.init();
  pinMode(keyDriver, OUTPUT);
  digitalWrite(keyDriver, LOW);
  
  clock.start();
//  clock.setTime(21, 40, 0);

  watchdog.init();
}

void loop()
{
  clock.getTime();

  for ( ; ; ) {
    if (lcdStatus!=1 ) {
      lcd.setCursor(3, 0);
      sprintf(buffer,"System v07 %6x", systemCounter);
      lcd.print(buffer);
    }

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

    if ( systemCounter>=0 && systemCounter<(2*60) ) {
      delay(1000);
    } else {
      LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
      LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
    }
    systemCounter++;
  }
}

void printKeysStatus() {
  display.print(0, 0, key1.read());
  display.print(1, 0, key2.read());   
}

boolean checkEnginesStatus(int hour,int minute) {
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
  if ( lcdStatus==1 ) {
    return;
  }
  lcd.setCursor(0, lcdRow);
  sprintf(buffer,"%2X>%3d, timer:%2d", workCounter, engineCounter, timerCounter);
  lcd.print(buffer);  
}
