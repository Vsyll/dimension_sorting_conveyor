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
    void update(); // <-- WAJIB dipanggil di loop() utama untuk menggerakkan step servo

  private:
    Servo _servo;
    int _sudutSekarang;           // Menyimpan posisi sudut servo saat ini
    int _sudutTarget;             // Menyimpan sudut tujuan
    unsigned long _waktuLangkahTerakhir; // Untuk check interval millis()
};

#endif