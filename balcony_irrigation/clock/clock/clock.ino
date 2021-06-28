#include "RTCDS1307.h"

RTCDS1307 rtc(0x68);

uint8_t year, month, weekday, day, hour, minute, second;
bool period = 0;

String m[12] = {"Styczen", "Luty", "Marzec", "Kwiecien", "Maj", "Czerwic", "Lipiec", "Sierpien", "Wrzesien", "Pazdziernik", "Listopad", "Grudzien"};
String w[7] = {"Poniedzialek", "Wtorek", "Sroda", "Czwartek", "Piatek", "Sobota", "Niedziela"};

void setup()
{
  Serial.begin(115200);
  rtc.begin();
//  rtc.setDate(21, 6, 17);
//  rtc.setTime(9, 44, 0);
}

void loop()
{
  rtc.getDate(year, month, day, weekday);
  rtc.getTime(hour, minute, second, period);
  if (!(second % 3)) rtc.setMode(1 - rtc.getMode());
  rtc.getTime(hour, minute, second, period);

//  Serial.print(w[weekday - 1]);
//  Serial.print("  ");
  
  
  Serial.print(year + 2000, DEC);
  Serial.print("-");
  if (month<10 ) {
    Serial.print(0, DEC);  
  }
  Serial.print(month, DEC);
  Serial.print("-");
  Serial.print(day, DEC);
  
  Serial.print("  ");
  Serial.print(hour, DEC);
  Serial.print(":");
  Serial.print(minute, DEC);
  Serial.print(":");
  Serial.print(second, DEC);
  Serial.print(rtc.getMode() ? (period ? " PM" : " AM") : "");
  Serial.println();
  delay(1000);
}
