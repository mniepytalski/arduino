#ifndef IRRYGATION_WATCHDOG_H
#define IRRYGATION_WATCHDOG_H

class Watchdog {
public:
    explicit Watchdog();
    void reset();
    void init();
};

#endif //IRRYGATION_WATCHDOG_H