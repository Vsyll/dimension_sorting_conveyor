#ifndef PENGARAH_CHUTE_H
#define PENGARAH_CHUTE_H
#include <ESP32Servo.h>

// Servo 180 deg posisional penggerak chute/lengan pengarah
class PengarahChute {
  public:
    PengarahChute(uint8_t pin) : _pin(pin) {}

    void mulai() {
      _servo.setPeriodHertz(50);
      _servo.attach(_pin, 500, 2500);
    }

    // arahkan chute ke sudut (derajat) lalu beri jeda agar servo sampai
    void keSudut(int derajat) {
      _servo.write(constrain(derajat, 0, 180));
      delay(400);
    }

  private:
    uint8_t _pin;
    Servo   _servo;
};

#endif