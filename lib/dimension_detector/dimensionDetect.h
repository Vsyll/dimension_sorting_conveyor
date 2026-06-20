#ifndef DIMENSIONDETECT_H
#define DIMENSIONDETECT_H

#include <Arduino.h>
#include "config.h"

// Hasil pengukuran satu benda
struct HasilUkur {
    float P;          // Panjang benda (mm)
    bool panjang;     // True jika P >= ambang panjang
    bool lebar;       // True jika salah satu atau kedua IR lebar terhalang
    bool tinggi;      // True jika IR tinggi terhalang
    int kategori;     // Indeks kategori (0..7)
};

class PengukurDimensi {
  public:
    PengukurDimensi();
    void debug(HasilUkur h, long total);
    void begin();
    bool adaBenda();
    HasilUkur ukur(bool (*adaApi)());

  private:
    bool _terhalang(uint8_t pin);
};

#endif