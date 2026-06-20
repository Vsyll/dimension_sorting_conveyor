#ifndef TAMPILAN_H
#define TAMPILAN_H
#include <LiquidCrystal_I2C.h>

// Pembungkus LCD I2C 16x2 (dua baris teks)
class Tampilan {
  public:
    Tampilan(uint8_t addr = 0x27, uint8_t kolom = 16, uint8_t baris = 2)
      : _lcd(addr, kolom, baris) {}

    void mulai() {
      _lcd.init();
      _lcd.backlight();
    }

    void dua(const char* baris1, const char* baris2) {
      _lcd.clear();
      _lcd.setCursor(0, 0); _lcd.print(baris1);
      _lcd.setCursor(0, 1); _lcd.print(baris2);
    }

  private:
    LiquidCrystal_I2C _lcd;
};

#endif