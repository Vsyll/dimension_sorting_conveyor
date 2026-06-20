#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ===== Pin ESP32 (Sesuai Konfigurasi Asli) =====
#define PIN_FLAME   34   // KY-026 AO (ADC, input-only)
#define PIN_IR_L1   32   // presence + timer panjang
#define PIN_IR_L2   33   // lebar luar
#define PIN_IR_T1   25   // tinggi bawah (cadangan)
#define PIN_IR_T2   26   // tinggi atas (ambang tinggi)

// ===== Logika sensor =====
#define IR_ON        LOW
const int FLAME_AMBANG = 1600; // < 1600 = ADA API

// ===== Inisialisasi LCD I2C =====
// Alamat I2C umum: 0x27 atau 0x3F. Ukuran: 16 kolom, 2 baris (sesuaikan jika pakai 20x4)
LiquidCrystal_I2C lcd(0x27, 16, 2); 

void setup() {
  Serial.begin(9600);
  
  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TES SENSOR ESP32");
  delay(2000);

  // Konfigurasi Pin Sensor
  pinMode(PIN_IR_L1, INPUT);
  pinMode(PIN_IR_L2, INPUT);
  pinMode(PIN_IR_T1, INPUT);
  pinMode(PIN_IR_T2, INPUT);
  pinMode(PIN_FLAME, INPUT);
}

void loop() {
  // 1. Membaca Data Sensor IR
  bool irL1 = (digitalRead(PIN_IR_L1) == IR_ON);
  bool irL2 = (digitalRead(PIN_IR_L2) == IR_ON);
  bool irT1 = (digitalRead(PIN_IR_T1) == IR_ON);
  bool irT2 = (digitalRead(PIN_IR_T2) == IR_ON);

  // 2. Membaca Data Sensor Api
  int nilaiFlame = analogRead(PIN_FLAME);
  bool adaApi = (nilaiFlame < FLAME_AMBANG);

  // 3. Menampilkan Data ke Serial Monitor (Untuk Diagnostik)
  Serial.print("IR_L1:"); Serial.print(irL1); Serial.print(" | ");
  Serial.print("IR_L2:"); Serial.print(irL2); Serial.print(" | ");
  Serial.print("IR_T1:"); Serial.print(irT1); Serial.print(" | ");
  Serial.print("IR_T2:"); Serial.print(irT2); Serial.print(" | ");
  Serial.print("Flame ADC:"); Serial.print(nilaiFlame); 
  Serial.println(adaApi ? " [ADA API!]" : " [AMAN]");

  // 4. Menampilkan Data ke LCD (Dibuat Singkat agar Muat di LCD 16x2)
  lcd.clear();
  
  // Baris 1: Status IR (1 jika terdeteksi/terhalang, 0 jika bebas)
  lcd.setCursor(0, 0);
  lcd.print("IR:");
  lcd.print(irL1 ? "1" : "0"); lcd.print(" ");
  lcd.print(irL2 ? "1" : "0"); lcd.print(" ");
  lcd.print(irT1 ? "1" : "0"); lcd.print(" ");
  lcd.print(irT2 ? "1" : "0");
  
  // Baris 2: Status Sensor Api
  lcd.setCursor(0, 1);
  lcd.print("F:"); lcd.print(nilaiFlame);
  if (adaApi) {
    lcd.print(" *AWAS API*");
  } else {
    lcd.print(" AMAN");
  }

  delay(500); // Update data setiap 0.5 detik
}