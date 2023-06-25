#include "RTCDS1307.h"
#include <stdio.h>
#include <avr/wdt.h>
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header


/*-----( Declare Constants )-----*/
// LCD geometry
const int LCD_COLS = 20;
const int LCD_ROWS = 4;


RTCDS1307 rtc(0x68);

class Clock {
  private:
    char date[20];
    uint8_t year, month, weekday, day, hour, minute, second;
    bool period = 0;

  public:
    Clock();
    void start();
    void setTime();
    void getTime();
    void sendToSerial();
    char* getDateToString();
    char* getTimeToString();
    char* getFullTimeToString();
    uint8_t getHour();
    uint8_t getMinute();
};

   Clock::Clock() {
   }

   void Clock::start() {
      rtc.begin();    
   }

   void Clock::setTime() {
    rtc.setDate(21, 6, 28);
    rtc.setTime(16, 13, 0);
   }
   
   void Clock::getTime() {
    rtc.getDate(year, month, day, weekday);
    rtc.getTime(hour, minute, second, period);
    rtc.setMode(0);
    rtc.getTime(hour, minute, second, period);
   }

  void Clock::sendToSerial() {
    Serial.print(getFullTimeToString());
    Serial.println();
  }  

  char* Clock::getDateToString() {
    sprintf(date, "%d-%02d-%02d", year+ 2000,month,day);
    return date;
  }

  char* Clock::getTimeToString() {
    sprintf(date, "%02d:%02d:%02d", hour,minute,second);
    return date;
  }

  char* Clock::getFullTimeToString(){
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d", year+ 2000,month,day,hour,minute,second);
    return date;     
  }

  uint8_t Clock::getHour() {
    return hour;
  }
  uint8_t Clock::getMinute() {
    return minute;
  }


////////////////////////////////////////////////////////////////////////

hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip  
int lcdStatus = 1; // 1 -on, 0-off


Clock clock;

int pumpPin1 = 7;     // pump 1 
int pumpPin2 = 6;     // pump 2 

int key1 = 5;
int key2 = 4;
int key3 = 8;

int engineCounter1 = 0;
int engineCounter2 = 0;
int timerCounter = 0;
int engineWorkTime = 30;

void setup()
{
  if (lcdStatus==1 ) {
    lcdStatus = lcd.begin(LCD_COLS, LCD_ROWS);
    if(lcdStatus) {
      hd44780::fatalError(lcdStatus); // does not return
    }
  }

  pinMode(pumpPin1, OUTPUT);
  pinMode(pumpPin2, OUTPUT);
  pumpOff(pumpPin1);
  pumpOff(pumpPin2);
  
  pinMode(key1, OUTPUT);
  pinMode(key2, INPUT);
  pinMode(key3, INPUT);
  digitalWrite(key1, LOW);
  digitalWrite(key2, HIGH);
  digitalWrite(key3, HIGH);
  
  clock.start();
//  clock.setTime();

  initWatchdog();
}

void loop()
{
  clock.getTime();

  if (lcdStatus!=1 ) {
    lcd.setCursor(3, 0);
    lcd.print("Podlewaczka v001");
  }
  for ( ; ; ) {
    wdt_reset();
    clock.getTime();
    if ( lcdStatus!=1 ) {
      lcd.setCursor(0, 1);
      lcd.print(clock.getFullTimeToString());
    }

    checkKeysStatus();
    if(digitalRead(key2) == LOW ) {
      pumpsOn(1);
    }
    if(digitalRead(key3) == LOW ) {
      pumpsOn(2);
    }
    if ( checkEnginesStatus(21,02) ) {
      pumpsOn(1);
    }
    if ( checkEnginesStatus(21,03) ) {
      pumpsOn(2);          
    }
    
    checkPumps();
    if ( timerCounter>0 ) {
      timerCounter--;
    } 
    delay(1000);
  }
}

void checkKeysStatus() {
    if (lcdStatus!=1 ) {
      lcd.setCursor(0, 0);
      lcd.print(digitalRead(key2));
      lcd.setCursor(1, 0);
      lcd.print(digitalRead(key3));
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

void pumpsOn(int number) {
    if ( number==1 ) {
      pumpOnIfPossible(&engineCounter1,pumpPin1);
    } 
    if ( number==2 ) {
      pumpOnIfPossible(&engineCounter2,pumpPin2);
    }
}

void pumpOnIfPossible(int *engineCounter,int pumpPin) {
  if ( *engineCounter<=0 ) {
    digitalWrite(pumpPin, LOW);
    *engineCounter = engineWorkTime;
  }
}

void checkPumps() {
  checkPumpAndOff(&engineCounter1,pumpPin1);
  checkPumpAndOff(&engineCounter2,pumpPin2);
  printInfo(2, pumpPin1,engineCounter1);
  printInfo(3, pumpPin2,engineCounter2);
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

void checkPumpAndOff(int *engineCounter,int pumpPin) {
    if ( *engineCounter>0 ) {
      *engineCounter = *engineCounter - 1 ;
    } else {
      pumpOff(pumpPin);
    }
}

void pumpOff(int pumpNumber) {
  digitalWrite(pumpNumber, HIGH);
}

void initWatchdog() {
  cli();         // disable all interrupts
  wdt_reset();   // reset the WDT timer
  /*
  WDTCSR configuration:
  WDIE = 1: Interrupt Enable
  WDE = 1 :Reset Enable
  WDP3 = 1 :For 2000ms Time-out
  WDP2 = 1 :For 2000ms Time-out
  WDP1 = 1 :For 2000ms Time-out
  WDP0 = 1 :For 2000ms Time-out
  */
  // Enter Watchdog Configuration mode:
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  // Set Watchdog settings:
  WDTCSR = (1<<WDIE) | (1<<WDE) | (1<<WDP3) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0);
  sei();
}
