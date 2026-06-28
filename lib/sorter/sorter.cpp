#include "Sorter.h"

// Constructor
PengarahChute::PengarahChute() {
    _sudutSekarang = SUDUT_SLOT[0]; // Set awal sesuai slot pertama
    _sudutTarget = SUDUT_SLOT[0];
    _waktuLangkahTerakhir = 0;
}

// Inisialisasi servo pada ESP32
void PengarahChute::begin() {
    _servo.setPeriodHertz(50); 
    _servo.attach(PIN_SERVO, 500, 2400); 
    
    // Set posisi fisik awal ke slot pertama
    _servo.write(_sudutSekarang);
}

// Mengubah target sudut secara aman
void PengarahChute::keSudut(int derajat) {
    _sudutTarget = constrain(derajat, 0, 180);
}

// Menggerakkan servo berdasarkan indeks kategori sorting (0 s.d 7)
void PengarahChute::keKategori(int kategori) {
    int katAman = constrain(kategori, 0, 7);
    int targetSudut = SUDUT_SLOT[katAman];
    keSudut(targetSudut);
}

// FUNGSI NON-BLOCKING UTAMA UNTUK UPDATE GERAKAN STEP SERVO (VERSI DINAMIS)
void PengarahChute::update() {
    // Jika posisi sekarang sudah sama dengan target, tidak perlu melangkah
    if (_sudutSekarang == _sudutTarget) return;

    // Cek apakah sudah waktunya melangkah berdasarkan interval di config.h
    if (millis() - _waktuLangkahTerakhir >= SERVO_STEP_INTERVAL) {
        _waktuLangkahTerakhir = millis();

        // 1. HITUNG JARAK (ERROR) ANTARA POSISI SEKARANG DENGAN TARGET
        int selisihSudut = abs(_sudutTarget - _sudutSekarang);

        // 2. TENTUKAN STEP SECARA DINAMIS (Makin jauh, makin besar step-nya)
        int stepDinamis = SERVO_STEP_DEG; // Nilai default dari config.h (7 derajat)

        if (selisihSudut > 90) {
            stepDinamis = 20; // Jarak sangat jauh (beda > 90 derajat): Lompat 20 derajat sekaligus biar ngebut
        } else if (selisihSudut > 45) {
            stepDinamis = 12; // Jarak sedang (beda > 45 derajat): Lompat 12 derajat
        } else if (selisihSudut < 15) {
            stepDinamis = 3;  // Sudah dekat (beda < 15 derajat): Ngerem halus, melangkah 3 derajat saja biar presisi
        }

        // 3. JALANKAN PERGERAKAN BERDASARKAN STEP DINAMIS
        // Jika target lebih besar, tambah sudut saat ini
        if (_sudutSekarang < _sudutTarget) {
            _sudutSekarang += stepDinamis;
            if (_sudutSekarang > _sudutTarget) _sudutSekarang = _sudutTarget; // Proteksi kebablasan
        } 
        // Jika target lebih kecil, kurangi sudut saat ini
        else {
            _sudutSekarang -= stepDinamis;
            if (_sudutSekarang < _sudutTarget) _sudutSekarang = _sudutTarget; // Proteksi kebablasan
        }

        // Eksekusi pergerakan fisik servo ke step baru
        _servo.write(_sudutSekarang);
    }
}