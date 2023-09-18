#ifndef IRRYGATION_PUMP_H
#define IRRYGATION_PUMP_H

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
    int getWorkCounter();

private:
    void off();
};

#endif //IRRYGATION_PUMP_H