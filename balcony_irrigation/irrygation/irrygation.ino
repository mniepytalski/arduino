#include "RTCDS1307.h"
#include <stdio.h>
#include <avr/wdt.h>
//#include <String.h>
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
int status;


Clock clock;

int pumpPin1 = 7;     // pump 1 
int pumpPin2 = 6;     // pump 2 

int key1 = 5;
int key2 = 4;

boolean pumpStatus = false;

void setup()
{
  status = lcd.begin(LCD_COLS, LCD_ROWS);
  if(status) {
    hd44780::fatalError(status); // does not return
  }  
  Serial.begin(115200);

  pinMode(pumpPin1, OUTPUT);
  pinMode(pumpPin2, OUTPUT);
  pumpsOff();
  
  pinMode(key1, OUTPUT);
  pinMode(key2, INPUT);
  digitalWrite(key1, LOW);
  digitalWrite(key2, HIGH);
  
  clock.start();
//  clock.setTime();
}

void loop()
{
  clock.getTime();
//  clock.sendToSerial();

  lcd.setCursor(0, 0);
  lcd.print("System podlewania");

  for ( ; ; ) {
    wdt_reset();
    lcd.setCursor(0, 1);
    clock.getTime();
    lcd.print(clock.getFullTimeToString());
 
    if(digitalRead(key2) == LOW ) {
      pumpsOn();
    } else {
      if ( checkEnginesStatus() ) {
        pumpsOn();
      } else {
        pumpsOff();
      }
    }
    delay(1000);
  }
}

boolean checkEnginesStatus() {
  return clock.getHour()== 20 && clock.getMinute() ==15 ;
}


void pumpsOn() {
    lcd.setCursor(0, 2);
    lcd.print("ON ");
    digitalWrite(pumpPin1, LOW);
    digitalWrite(pumpPin2, LOW);
    pumpStatus = true;
}

void pumpsOff() {
    lcd.setCursor(0, 2);
    digitalWrite(pumpPin1, HIGH);
    digitalWrite(pumpPin2, HIGH);
    lcd.print("OFF");
    pumpStatus = false;
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
