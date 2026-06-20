#ifndef PENGGERAK_BELT_H
#define PENGGERAK_BELT_H
#include <Arduino.h>

// Motor belt via PWM ke gate MOSFET
class PenggerakBelt {
  public:
    PenggerakBelt(uint8_t pin) : _pin(pin) {}

    void mulai() {
      pinMode(_pin, OUTPUT);
      analogWrite(_pin, 0);
    }

    void jalan(bool aktif, int duty) {
      analogWrite(_pin, aktif ? constrain(duty, 0, 255) : 0);
    }

  private:
    uint8_t _pin;
};

#endif