#ifndef PENGUKUR_DIMENSI_H
#define PENGUKUR_DIMENSI_H
#include <Arduino.h>

// Hasil pengukuran satu benda
struct HasilUkur {
  float P;          // panjang (mm)
  bool  panjang;    // P >= ambang
  bool  lebar;      // IR_L2 terhalang
  bool  tinggi;     // IR_T2 terhalang
  int   kategori;   // 0..7
};

// Baca 4 IR di gerbang -> ukur P/L/T -> tentukan kategori 0..7
class PengukurDimensi {
  public:
    PengukurDimensi(uint8_t pinL1, uint8_t pinL2, uint8_t pinT2, uint8_t irOn,
                    float beltCmPerS, float panjangAmbangMm, float dimMaks)
      : _l1(pinL1), _l2(pinL2), _t2(pinT2), _irOn(irOn),
        _belt(beltCmPerS), _ambang(panjangAmbangMm), _maks(dimMaks) {}

    void mulai() {
      pinMode(_l1, INPUT);
      pinMode(_l2, INPUT);
      pinMode(_t2, INPUT);
    }

    bool adaBenda() { return _kena(_l1); }

    // ukur selama IR_L1 terhalang; adaApi() dipanggil agar emergency responsif
    HasilUkur ukur(bool (*adaApi)()) {
      unsigned long t0 = millis();
      bool lebarLuar = false, atas = false;

      while (_kena(_l1)) {
        if (_kena(_l2)) lebarLuar = true;
        if (_kena(_t2)) atas = true;
        if (adaApi && adaApi()) break;   // emergency -> berhenti mengukur
        delay(2);
      }
      unsigned long durasi = millis() - t0;

      HasilUkur h;
      h.P = _belt * (durasi / 1000.0) * 10.0;   // cm -> mm
      if (h.P > _maks) h.P = _maks;
      h.panjang  = (h.P >= _ambang);
      h.lebar    = lebarLuar;
      h.tinggi   = atas;
      h.kategori = (h.panjang ? 4 : 0) + (h.lebar ? 2 : 0) + (h.tinggi ? 1 : 0);
      return h;
    }

  private:
    uint8_t _l1, _l2, _t2, _irOn;
    float   _belt, _ambang, _maks;
    bool _kena(uint8_t pin) { return digitalRead(pin) == _irOn; }
};

#endif