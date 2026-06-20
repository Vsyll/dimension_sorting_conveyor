#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ===== Pin Servo & Relay =====
#define PIN_SERVO           13   // Servo 180 deg posisional (chute pengarah)
#define PIN_RELAY           27   // Relay cut-off (emergency)

// ===== Motor Config (L298N IN1) =====
#define PIN_MOTOR           14   // PWM langsung ke IN1 L298N (IN2 -> GND, ENA -> 5V)
#define INITIAL_MOTOR_DUTY  140  // Kecepatan awal (0-255)
#define MOTOR_PWM_FREQ      4000 // Frekuensi PWM dalam Hz (4 kHz)

// ====== Pin Sensor ======
#define PIN_IR_PANJANG      32   // Samping: mendeteksi durasi objek lewat (panjang)
#define PIN_IR_LEBAR_L      33   // Atas kiri: deteksi lebar luar kiri
#define PIN_IR_LEBAR_R      25   // Atas kanan: deteksi lebar luar kanan
#define PIN_IR_TINGGI       26   // Atas tengah: deteksi ambang tinggi objek
#define PIN_FLAME           34   // KY-026 AO (ADC, input-only)

// ===== Konfigurasi Layar OLED =====
#define OLED_SCREEN_WIDTH   128  // Lebar OLED dalam piksel
#define OLED_SCREEN_HEIGHT  64   // Tinggi OLED dalam piksel
#define OLED_ADDRESS        0x3C // Alamat I2C umum OLED SSD1306

// ===== Logika Sensor / Aktuator =====
#define IR_ON               LOW
#define RELAY_AKTIF         LOW
#define RELAY_PUTUS         HIGH

// ===== Virtual Pin Blynk =====
#define VP_START            V0
#define VP_RESET            V1
#define VP_SPEED            V2
#define VP_LED              V3
#define VP_STATUS           V4
#define VP_TOTAL            V5
#define VP_EMERG            V6
#define VP_BARANG           V7

// ===== Kalibrasi  =====
inline constexpr float         BELT_CM_PER_S     = 8.0;    // Kecepatan belt (cm/detik)
inline constexpr int           FLAME_AMBANG      = 1600;   // ADC 12-bit; < ambang = ADA API
inline constexpr unsigned long WAKTU_KE_UJUNG_MS = 3000;   // Travel time ke chute sorting (ms)
inline constexpr float         PANJANG_AMBANG_MM = 50.0;   // >= 50 mm (5 cm) = Kategori "Panjang"
inline constexpr float         DIM_MAKS          = 100.0;  // Batas atas hasil ukur (mm)

// 8 sudut -> tengah tiap sekat (tray 1/2 lingkaran, 8 sekat @ 22,5 deg)
inline constexpr int SUDUT_SLOT[8] = {11, 34, 56, 79, 101, 124, 146, 169};


inline const char* const NAMA_KAT[8] = {
 "K-C-R", "K-C-T", "K-L-R", "K-L-T",
  "P-C-R", "P-C-T", "P-L-R", "P-L-T"
};

#endif