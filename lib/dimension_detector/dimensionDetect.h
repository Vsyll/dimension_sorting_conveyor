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

enum StateDeteksi { IDLE, MENGUKUR_MASUK, MENUNGGU_KELUAR, MENGUKUR_LEBAR, SELESAI };

class PengukurDimensi {
  public:
    void begin();
    void debug(HasilUkur h, long total);
    bool adaBenda();
    bool update(HasilUkur &hasil, bool (*adaApi)());

  private:
    bool _terhalang(uint8_t pin);

    StateDeteksi stateSekarang;
    unsigned long waktuMasuk;
    unsigned long waktuLepas;
    bool statusTinggi;
    bool statusLebar;
    bool pernahKenaLebar;
};

#endif