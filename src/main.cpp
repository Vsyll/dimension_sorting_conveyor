
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
Tampilan        oled;
PengukurDimensi detektor;
PengarahChute   sorter;
Belt            belt;       
PengamanApi     pengaman;

// ---- Variabel Global Kontrol ----
bool blynkEmergencyTrigger = false; // Emergency stop manual dari tombol Blynk
long totalBarang = 0;
unsigned long waktuMulaiSorting = 0;
bool sedangMenungguSorting = false;

// ---- Fungsi Proteksi Utama (Gabungan Sensor & Blynk) ----
bool cekEmergency() {
    // 1. Cek sensor api fisik (otomatis mengunci jika aktif)
    if (pengaman.cek()) {
        belt.run(false, 0);
        oled.displayEmergency();
        Blynk.virtualWrite(VP_STATUS, "EMERGENCY: API!");
        Blynk.virtualWrite(VP_EMERG, 255);
        return true;
    }
    // 2. Cek apakah ada trigger emergency manual dari software Blynk
    if (blynkEmergencyTrigger) {
        belt.run(false, 0);
        oled.displayEmergency(); 
        Blynk.virtualWrite(VP_STATUS, "EMERGENCY: BLYNK STOP!");
        return true;
    }
    return false;
}

// ---- Handler Widget Blynk ----
BLYNK_WRITE(VP_EMERG) { 
    // Jika tombol Emergency di Blynk ditekan (Kondisi Aktif = 1 atau 255)
    int statusTombol = param.asInt();
    if (statusTombol > 0) {
        blynkEmergencyTrigger = true;
    }
}

BLYNK_WRITE(VP_RESET) {
    if (param.asInt() == 1) { 
        pengaman.reset();              // Lepas kunci emergency sensor api fisik
        blynkEmergencyTrigger = false; // Lepas kunci emergency software blynk
        Blynk.virtualWrite(VP_EMERG, 0);
        Blynk.virtualWrite(VP_STATUS, "SISTEM NORMAL");
        oled.displayStandby();
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

    // Langsung hubungkan ke Blynk begitu alat mendapat daya listrik
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    Blynk.virtualWrite(VP_STATUS, "SISTEM SIAP");
}

// ---- Loop Utama Operasional ----
void loop() {
    Blynk.run();
    
    // Jika terdeteksi kondisi emergency, hentikan sisa eksekusi loop
    if (cekEmergency()) return; 

    // Indikator LED di Blynk selalu ON menandakan mesin sedang aktif berjalan
    Blynk.virtualWrite(VP_LED, 255);

    // Jika sistem normal dan tidak sedang menahan barang menuju ujung, jalankan motor
    if (!sedangMenungguSorting) {
        belt.run(true, INITIAL_MOTOR_DUTY);
        Blynk.virtualWrite(VP_STATUS, "CONVEYOR RUNNING");
        
        // JIKA ADA BENDA MASUK GERBANG SENSOR
        if (detektor.adaBenda()) {
            oled.displayMengukur();
            
            // Eksekusi fungsi ukur internal (melempar fungsi cekEmergency sebagai callback)
            HasilUkur h = detektor.ukur(cekEmergency); 
            
            // Jika saat proses pengukuran tidak terjadi interupsi emergency
            if (!pengaman.sedangEmergency() && !blynkEmergencyTrigger) {
                totalBarang++;
                sorter.keKategori(h.kategori); // Posisikan Chute

                // Tampilkan Data Estetik ke Layar OLED & Serial Monitor
                oled.displayHasil(h.kategori + 1, NAMA_KAT[h.kategori], h.P, h.lebar, h.tinggi);
                detektor.debug(h, totalBarang);

                // Kirim Parameter ke Blynk Cloud
                char statusStr[32];
                snprintf(statusStr, sizeof(statusStr), "%s (Slot %d)", NAMA_KAT[h.kategori], h.kategori + 1);
                Blynk.virtualWrite(VP_STATUS, statusStr);
                Blynk.virtualWrite(VP_BARANG, NAMA_KAT[h.kategori]);
                Blynk.virtualWrite(VP_TOTAL, totalBarang);

                // Aktifkan timer non-blocking untuk membiarkan barang berjalan ke ujung tray
                waktuMulaiSorting = millis();
                sedangMenungguSorting = true;
            }
        }
    } 
    // Logika Non-Blocking Pengganti Jeda Waktu Perjalanan Barang (Travel Time)
    else {
        if (millis() - waktuMulaiSorting >= WAKTU_KE_UJUNG_MS) {
            sedangMenungguSorting = false; // Waktu habis, siap mendeteksi barang baru lagi
        }
    }
}