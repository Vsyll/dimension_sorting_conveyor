#include "Sorter.h"

// Constructor
PengarahChute::PengarahChute() {}

// Inisialisasi servo pada ESP32
void PengarahChute::begin() {
    _servo.setPeriodHertz(50); // Standard servo analog/digital pada umumnya (50Hz)
    
    // Attach pin menggunakan pulse width standard (0 deg = 500us, 180 deg = 2500us)
    _servo.attach(PIN_SERVO, 500, 2500); 
    
    // Set posisi awal servo ke slot pertama (standby) saat dinyalakan
    keSudut(SUDUT_SLOT[0]);
}

// Menggerakkan servo ke sudut tertentu
void PengarahChute::keSudut(int derajat) {
    int sudutAman = constrain(derajat, 0, 180);
    _servo.write(sudutAman);
}

// Menggerakkan servo berdasarkan indeks kategori sorting (0 s.d 7)
void PengarahChute::keKategori(int kategori) {
    // Proteksi jika indeks kategori di luar rentang array (0-7)
    int katAman = constrain(kategori, 0, 7);
    
    // Ambil nilai derajat dari array SUDUT_SLOT di config.h
    int targetSudut = SUDUT_SLOT[katAman];
    
    // Eksekusi pergerakan
    keSudut(targetSudut);
}