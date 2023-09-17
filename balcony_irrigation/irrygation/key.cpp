#include "key.h"
#include <Arduino.h>

Key::Key(int keyPin) {
  pin = keyPin;
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
}

bool Key::isKeyPressed() {
  return read() == LOW;
}

int Key::read() {
  return digitalRead(pin);
}