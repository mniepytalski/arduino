#include "clock.h"
#include <Arduino.h>
#include "RTCDS1307.h"
#include <stdio.h>

RTCDS1307 rtc(0x68);

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