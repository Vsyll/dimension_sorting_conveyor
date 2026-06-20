#ifndef SORTER_H
#define SORTER_H

#include <Arduino.h>
#include <ESP32Servo.h>
#include "config.h"

class PengarahChute {
  public:
    PengarahChute();
    void begin();
    void keSudut(int derajat);
    void keKategori(int kategori);

  private:
    Servo _servo;
};

#endif