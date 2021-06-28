#include "RTCDS1307.h"
#include <String.h>
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
    uint8_t year, month, weekday, day, hour, minute, second;
    bool period = 0;
    String m[12] = {"Styczen", "Luty", "Marzec", "Kwiecien", "Maj", "Czerwic", "Lipiec", "Sierpien", "Wrzesien", "Pazdziernik", "Listopad", "Grudzien"};
    String w[7] = {"Poniedzialek", "Wtorek", "Sroda", "Czwartek", "Piatek", "Sobota", "Niedziela"};

  public:
    Clock();
    void start();
    void setTime();
    void getTime();
    void sendToSerial();
    String getDateToString();
    String getTimeToString();
    String getFullTimeToString();
    uint8_t getHour();
    uint8_t getMinute();
  private:
    String formatTwoDigit(int digit);
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

  String Clock::getDateToString() {
    String str = String(year + 2000);
    str += "-";
    str += formatTwoDigit(month);
    str += "-";
    str += formatTwoDigit(day);
    return str;
  }

  String Clock::getTimeToString() {
    String str = formatTwoDigit(hour)+String(":");
    str += formatTwoDigit(minute)+String(":");
    str += formatTwoDigit(second);
    return str;
  }

  String Clock::getFullTimeToString(){
    return getDateToString() + String(" ") + getTimeToString();
  }


  uint8_t Clock::getHour() {
    return hour;
  }
  uint8_t Clock::getMinute() {
    return minute;
  }

  String Clock::formatTwoDigit(int digit) {
    if (digit<10 ) {
      return String(0)+String(digit);
    } else {
      return String(digit);
    }
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

  lcd.setCursor(0, 4);
  lcd.print("System podlewania");



  for ( ; ; ) {
    lcd.setCursor(0, 0);
    clock.getTime();
    String time = clock.getFullTimeToString();
    lcd.print(time);

    lcd.setCursor(0, 1);
    lcd.print(digitalRead(key2));

 
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
    lcd.print("Koniec");

}

boolean checkEnginesStatus() {
  return clock.getHour()== 20 && clock.getMinute() ==15 ;
}


void pumpsOn() {
    lcd.setCursor(0, 1);
    lcd.print("ON ");
    digitalWrite(pumpPin1, LOW);
    digitalWrite(pumpPin2, LOW);
    pumpStatus = true;
}

void pumpsOff() {
    lcd.setCursor(0, 1);
    digitalWrite(pumpPin1, HIGH);
    digitalWrite(pumpPin2, HIGH);
    lcd.print("OFF");
    pumpStatus = false;
}
