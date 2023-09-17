#include "RTCDS1307.h"
#include "LowPower.h"
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
    rtc.setDate(23, 9, 12);
    rtc.setTime(22, 50, 0);
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

class Pump {
private:
    int pin;
    bool status;
    int engineCounter = 0;
    int initWorkTime = 25;
    int workCounter = 0;

public:
    explicit Pump(int pin);
    void on();
    void checkAndOff();
    int getPin();
    int getEngineCounter();

private:
    void off();
};


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

////////////////////////////////////////////////////////////////////////

hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip  
int lcdStatus = 1; // 1 -on, 0-off


Clock clock;

Pump pump1(7);
Pump pump2(6);

int key1 = 5;
int key2 = 4;
int key3 = 8;
int timerCounter = 0;
int systemCounter = 0;

void setup()
{
  if (lcdStatus==1 ) {
    lcdStatus = lcd.begin(LCD_COLS, LCD_ROWS);
    if(lcdStatus) {
      hd44780::fatalError(lcdStatus); // does not return
    }
  }

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
    lcd.print("Podlewaczka v005");
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
      pump1.on();
    }
    if(digitalRead(key3) == LOW ) {
      pump2.on();
    }
    if ( checkEnginesStatus(8,0) ) {
      pump1.on();
    }
    if ( checkEnginesStatus(8,5) ) {
      pump2.on();        
    }
    
    checkPumps();
    if ( timerCounter>0 ) {
      timerCounter--;
    } 

    if ( systemCounter>=0 && systemCounter<60 ) {
      delay(1000);
    } else {}
      LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
      LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
    }
    systemCounter++;
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

void checkPumps() {
  pump1.checkAndOff();
  pump2.checkAndOff();
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
