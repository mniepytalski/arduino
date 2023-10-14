#ifndef IRRYGATION_CLOCK_H
#define IRRYGATION_CLOCK_H

#include <stdint.h>

class Clock {
  private:
    char date[20];
    uint8_t year, month, weekday, day, hour, minute, second;
    bool period = 0;
    uint8_t dayCounter;
    uint8_t startDay;

  public:
    Clock();
    void start();
    void setTime(int hour, int minute, int second);
    void getTime();
    void sendToSerial();
    char* getDateToString();
    char* getTimeToString();
    char* getFullTimeToString();
    uint8_t getDay(); 
    uint8_t getHour();
    uint8_t getMinute();
    uint8_t getDayCounter();
};

#endif //IRRYGATION_CLOCK_H