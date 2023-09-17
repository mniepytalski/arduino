#ifndef IRRYGATION_KEY_H
#define IRRYGATION_KEY_H

class Key {
private:
    int pin;

public:
    explicit Key(int keyPin);
    bool isKeyPressed();
    int read();
};

#endif //IRRYGATION_KEY_H