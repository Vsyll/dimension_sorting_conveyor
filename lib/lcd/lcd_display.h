#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

class LcdDisplay {
  public:
    LcdDisplay() : _lcd(0x27, 16, 2) {}

    void begin() {
        Wire.begin(21, 22); 
        _lcd.init();
        _lcd.backlight();
        clear();
    }

    void clear() { _lcd.clear(); }

    void displayBooting() {
        _lcd.clear();
        _lcd.setCursor(0, 0);
        _lcd.print("DIMENSION SORTER");
        _lcd.setCursor(4, 1);
        _lcd.print("UNSOED");
        delay(2000);
        clear();
    }

    void displayStandby() {
        _lcd.clear();
        _lcd.setCursor(0, 0);
        _lcd.print("SYSTEM READY");
        _lcd.setCursor(0, 1);
        _lcd.print("Menunggu Benda..");
    }

    void displayHasil(int slot, const char* namaKat, float panjang, bool lebar, bool tinggi) {
        _lcd.clear();
        _lcd.setCursor(0, 0);
        _lcd.printf("Slot%d:%s", slot, namaKat);
        _lcd.setCursor(0, 1);
        _lcd.printf("P:%.0fmm %s %s",
            panjang,
            lebar  ? "L" : "S",
            tinggi ? "T" : "R");
    }

    void displayEmergency() {
        _lcd.clear();
        _lcd.setCursor(0, 0);
        _lcd.print("!!! BAHAYA !!!");
        _lcd.setCursor(0, 1);
        _lcd.print("API TERDETEKSI!");
    }

  private:
    LiquidCrystal_I2C _lcd;
};

#endif