#define BLYNK_TEMPLATE_ID   "Tubes INSTRU"
#define BLYNK_TEMPLATE_NAME "TUBES"
#define BLYNK_AUTH_TOKEN    "c1quVSxtfWCxQF4IEdBvp48NHHedn5CZ"
#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>

#include "config.h"
#include "lcd.h" 
#include "dimensionDetect.h"
#include "sorter.h"
#include "belt.h"
#include "fireDetect.h"

// ---- Kredensial WiFi ----
char ssid[] = "mufid";
char pass[] = "mufidfamily";

// ---- Instansiasi Objek Modul ----
Tampilan         oled;
PengukurDimensi detektor;
PengarahChute   sorter;
Belt            belt;       
PengamanApi     pengaman;

// ---- Variabel Global Kontrol ----
bool blynkEmergencyTrigger = false; 
long totalBarang = 0;

// Variabel Tuning Dinamis (Otomatisasi Kecepatan & Waktu)
int kecepatanMotorSekarang = INITIAL_MOTOR_DUTY; 
unsigned long waktuTravelAktif = WAKTU_KE_UJUNG_MS; 

// Variabel Penahan Aksi Sorter (Travel Time)
bool butuhSorting = false;
unsigned long waktuSelesaiUkur = 0;
int kategoriTertunda = 0;

// ---- Fungsi Proteksi Utama ----
bool cekEmergency() {
    if (pengaman.cek()) {
        oled.displayEmergency();
        Blynk.virtualWrite(VP_STATUS, "EMERGENCY: API!");
        Blynk.virtualWrite(VP_EMERG, 255);
        return true;
    }
    if (blynkEmergencyTrigger) {
        oled.displayEmergency(); 
        Blynk.virtualWrite(VP_STATUS, "EMERGENCY: BLYNK STOP!");
        return true;
    }
    return false;
}

// ---- Handler Widget Blynk (Slider & Tombol) ----

// Slider Kecepatan di Blynk (Setting rentang di Blynk: 100 s.d 255)
BLYNK_WRITE(VP_SPEED) { 
    kecepatanMotorSekarang = param.asInt();
    
    // Jika tidak sedang emergency, ubah kecepatan motor secara realtime
    if (!pengaman.sedangEmergency() && !blynkEmergencyTrigger) {
        belt.setSpeed(kecepatanMotorSekarang);
    }
    
    // OTOMATISASI: Menghitung travel time berdasarkan kecepatan slider
    waktuTravelAktif = map(kecepatanMotorSekarang, 100, 255, 4200, 1600);
    Serial.printf("[TUNING] Kecepatan: %d | Waktu Travel Otomatis: %ld ms\n", kecepatanMotorSekarang, waktuTravelAktif);
}

// Tombol Emergency Manual dari Blynk
BLYNK_WRITE(VP_EMERG) { 
    if (param.asInt() > 0) {
        blynkEmergencyTrigger = true;
    }
}

// Tombol Reset Emergency dari Blynk
BLYNK_WRITE(VP_RESET) {
    if (param.asInt() == 1) { 
        pengaman.reset();              
        blynkEmergencyTrigger = false; 
        Blynk.virtualWrite(VP_EMERG, 0);
        Blynk.virtualWrite(VP_STATUS, "SISTEM NORMAL");
        oled.displayStandby();
        
        // Kembalikan motor ke kecepatan semula setelah aman
        belt.setSpeed(kecepatanMotorSekarang); 
    }
}

BLYNK_CONNECTED() { 
    Blynk.virtualWrite(VP_TOTAL, totalBarang); 
}

// ---- Setup Utama ----
void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22); 
    
    oled.begin();
    oled.displayStandby();

    detektor.begin();
    belt.begin();      
    pengaman.begin();
    sorter.begin();

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    Blynk.virtualWrite(VP_STATUS, "SISTEM SIAP");
}

// ---- Loop Utama Operasional (NON-BLOCKING) ----
void loop() {
    Blynk.run(); // Selalu responsif, anti disconnect
    sorter.update();
    
    // 1. Cek Emergency
    if (cekEmergency()) return; 

    Blynk.virtualWrite(VP_LED, 255);
    Blynk.virtualWrite(VP_STATUS, "CONVEYOR RUNNING");

    // 2. Pembacaan Sensor Menggunakan State Machine (Update Terus-Menerus)
    HasilUkur dataBenda;
    if (detektor.update(dataBenda, cekEmergency)) {
        // JIKA MASUK SINI: Berarti benda baru saja LOLOS dari pintu keluar terowongan
        totalBarang++;
        
        sorter.keKategori(dataBenda.kategori);
        
        // Tampilkan info estetik
        oled.displayHasil(dataBenda.kategori + 1, NAMA_KAT[dataBenda.kategori], dataBenda.P, dataBenda.lebar, dataBenda.tinggi);
        detektor.debug(dataBenda, totalBarang);

        // Update ke Blynk Cloud
        char statusStr[32];
        snprintf(statusStr, sizeof(statusStr), "%s (Slot %d)", NAMA_KAT[dataBenda.kategori], dataBenda.kategori + 1);
        Blynk.virtualWrite(VP_STATUS, statusStr);
        Blynk.virtualWrite(VP_BARANG, NAMA_KAT[dataBenda.kategori]);
        Blynk.virtualWrite(VP_TOTAL, totalBarang);

        butuhSorting = true;
        waktuSelesaiUkur = millis();
        kategoriTertunda = dataBenda.kategori;
    }

    // 3. Eksekusi Pergerakan Servo Korsel (Mengikuti Waktu Perjalanan Dinamis)
    if (butuhSorting && (millis() - waktuSelesaiUkur >= waktuTravelAktif)) {
        butuhSorting = false; // Reset penanda, siap tunggu benda berikutnya
    }
}