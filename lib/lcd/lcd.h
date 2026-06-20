#ifndef TAMPILAN_H
#define TAMPILAN_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

class Tampilan {
  public:
    Tampilan() : _oled(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire, -1) {}

    void begin() {
        if(!_oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
            Serial.println(F("Gagal mendeteksi OLED SSD1306!"));
            for(;;); 
        }
        clear();
    }

    // Tampilan Standby / Idle awal
    void displayStandby() {
        _oled.clearDisplay();
        
        // Header Bar
        _oled.fillRect(0, 0, 128, 13, SSD1306_WHITE);
        _oled.setTextColor(SSD1306_BLACK);
        _oled.setTextSize(1);
        _oled.setCursor(22, 3);
        _oled.print("CONVEYOR SYSTEM");
        
        // Main Status
        _oled.setTextColor(SSD1306_WHITE);
        _oled.setTextSize(2);
        _oled.setCursor(28, 30);
        _oled.print("STANDBY");
        
        _oled.setTextSize(1);
        _oled.setCursor(20, 53);
        _oled.print("Tekan START di HP");
        
        _oled.display();
    }

    // Tampilan saat sedang mengukur benda di gerbang sensor
    void displayMengukur() {
        _oled.clearDisplay();
        
        _oled.setTextSize(1);
        _oled.setTextColor(SSD1306_WHITE);
        _oled.setCursor(0, 5);
        _oled.print("STATUS OPR:");
        
        _oled.setTextSize(2);
        _oled.setCursor(0, 25);
        _oled.print("MENGUKUR...");
        
        // Gambar Loading Bar sederhana di bawah
        _oled.drawRect(0, 52, 128, 7, SSD1306_WHITE);
        _oled.fillRect(2, 54, 40, 3, SSD1306_WHITE); // Representasi proses
        
        _oled.display();
    }

    // Tampilan Hasil Sorting Terkini (Sangat Komplit & Estetik)
    void displayHasil(int slot, const char* namaKat, float panjang, bool lebar, bool tinggi) {
        _oled.clearDisplay();
        
        // Sisi Kiri: Nomor Slot Sorting (Besar)
        _oled.setTextSize(1);
        _oled.setTextColor(SSD1306_WHITE);
        _oled.setCursor(0, 4);
        _oled.print("SORT SLOT");
        
        _oled.setTextSize(5); // Font sangat besar untuk angka slot
        _oled.setCursor(12, 20);
        _oled.print(slot);
        
        // Garis Pembatas Vertikal di Tengah (Membagi layar kiri dan kanan)
        _oled.drawFastVLine(60, 0, 64, SSD1306_WHITE);
        
        // Sisi Kanan: Detail Spesifikasi Dimensi Fisik Benda
        _oled.setTextSize(1);
        _oled.setCursor(68, 4);
        _oled.print("KODE: ");
        _oled.print(namaKat); // Misal: P-L-T atau K-C-R
        
        _oled.drawFastHLine(64, 16, 64, SSD1306_WHITE); // Garis pembatas horizontal kecil
        
        // Data Panjang
        _oled.setCursor(68, 22);
        _oled.print("P: ");
        _oled.print(panjang, 1);
        _oled.print(" mm");
        
        // Data Lebar
        _oled.setCursor(68, 36);
        _oled.print("L: ");
        _oled.print(lebar ? "LEBAR" : "CIUT");
        
        // Data Tinggi
        _oled.setCursor(68, 50);
        _oled.print("T: ");
        _oled.print(tinggi ? "TINGGI" : "RENDAH");
        
        _oled.display();
    }

    // Tampilan jika terjadi Emergency Kebakaran
    void displayEmergency() {
        _oled.clearDisplay();
        
        // Kotak Bingkai Bahaya
        _oled.drawRect(0, 0, 128, 64, SSD1306_WHITE);
        _oled.drawRect(2, 2, 124, 60, SSD1306_WHITE);
        
        _oled.setTextSize(2);
        _oled.setTextColor(SSD1306_WHITE);
        _oled.setCursor(12, 12);
        _oled.print("EMERGENCY");
        
        _oled.setTextSize(1);
        _oled.setCursor(26, 40);
        _oled.print("FIRE DETECTED!");
        
        _oled.display();
    }

    void clear() {
        _oled.clearDisplay();
        _oled.display();
    }

  private:
    Adafruit_SSD1306 _oled;
};

#endif