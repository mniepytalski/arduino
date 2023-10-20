#ifndef IRRYGATION_PUMP_H
#define IRRYGATION_PUMP_H

class Pump {
private:
    int pin;
    bool status;
    int engineCounter = 0;
    int initWorkTime = 25;
    int workCounter = 0;

    int workHour = 8;
    int workMinute = 0;

public:
    explicit Pump(int pin, int hour, int minute);
    void on();
    void checkAndOff();
    int getPin();
    int getEngineCounter();
    int getWorkCounter();

    int getWorkHour();
    int getWorkMinute();
    void setWorkHour(int hour);
    void setWorkMinute(int minute);

private:
    void off();
};

#endif //IRRYGATION_PUMP_H