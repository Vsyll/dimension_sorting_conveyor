#include "FireDetect.h"

// Constructor
PengamanApi::PengamanApi() {
    _emergency = false;
}

void PengamanApi::debug() {
    Serial.printf("[DEBUG FIRE] ADC: %d | Status: %s\n", 
                  analogRead(PIN_FLAME), _emergency ? "EMERGENCY" : "AMAN");
}

// Inisialisasi
void PengamanApi::begin() {
    pinMode(PIN_RELAY, OUTPUT);
    pinMode(PIN_FLAME, INPUT); // Pin 34 input-only
    
    // Nyalakan daya aktuator di awal (kondisi aman)
    digitalWrite(PIN_RELAY, RELAY_AKTIF); 
    _emergency = false;
}

// Cek kondisi sensor api
bool PengamanApi::cek() {
    // Membaca nilai ADC dari sensor KY-026
    // Jika nilai ADC < ambang, berarti sensor mendeteksi api (Active LOW)
    if (analogRead(PIN_FLAME) < FLAME_AMBANG) {
        _emergency = true;
        digitalWrite(PIN_RELAY, RELAY_PUTUS); // Putus aliran daya utama secara instan
    }
    return _emergency;
}

// Reset dari kondisi darurat
void PengamanApi::reset() {
    _emergency = false;
    digitalWrite(PIN_RELAY, RELAY_AKTIF); // Hubungkan kembali daya utama
}

// Mengambil status terkini
bool PengamanApi::sedangEmergency() const { 
    return _emergency; 
}