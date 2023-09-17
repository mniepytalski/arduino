#ifndef IRRYGATION_CLOCK_H
#define IRRYGATION_CLOCK_H

#include <stdint.h>

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

#endif //IRRYGATION_CLOCK_H