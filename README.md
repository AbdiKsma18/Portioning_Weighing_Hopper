# Automated Portioning & Weighing Hopper

Sistem penakaran dan penimbangan material curah otomatis berbasis **Arduino Uno**, modul sensor beban **HX711**, aktuator **Servo** (pengatur katup bukaan *hopper*), **OLED SSD1306** (antarmuka informasi *real-time*), serta **Potensiometer** (pengatur berat target dalam satuan **kilogram (`kg`)**).

Proyek ini dirancang menggunakan **PlatformIO** di **Visual Studio Code** dan diuji melalui simulasi **Wokwi** dengan kontrol pengisian dua tahap (*two-stage filling*) untuk menekan risiko *overshoot* akibat momentum aliran material.

---

## Fitur Utama

- **Pengaturan Target Berbasis `kg`:** Target berat diatur menggunakan potensiometer dalam satuan kilogram (`kg`) dengan tampilan langsung di layar OLED.
- **Pengisian Dua Tahap (`FAST MODE` & `SLOW MODE`):**
  - **`FAST MODE`:** Buka katup penuh (`100%`) saat berat aktual masih jauh di bawah batas ambang ($< 80\%$ dari berat target) untuk mempercepat proses penuangan.
  - **`SLOW MODE`:** Memperkecil bukaan katup saat berat aktual mendekati target ($\ge 80\%$ dari berat target) agar aliran melambat dan hasil akhir lebih presisi.
- **Status & Indikator Jelas:** Menampilkan status sistem di OLED (`READY`, `FAST MODE`, `SLOW MODE`, `COMPLETE`, dan `OVERLOAD!`) beserta *progress bar* pengisian, ditambah indikator LED, *buzzer*, serta tombol `TARE` dan `START/STOP`.

---

## Perangkat Keras & Komponen

| Komponen | Keterangan / Fungsi |
| :--- | :--- |
| **Mikrokontroler** | Arduino Uno |
| **Sensor Berat** | HX711 + *Load Cell* (diuji menggunakan simulasi Wokwi HX711 dalam satuan `kg`) |
| **Aktuator Katup** | Motor Servo (mengatur bukaan pintu *hopper*: tutup, lambat, dan cepat) |
| **Display** | OLED 128x64 (I2C, `SSD1306`) |
| **Input & Indikator** | Potensiometer (set target `kg`), tombol `TARE`, tombol `START/STOP`, LED indikator, dan *buzzer* |

---

## Struktur Proyek

```text
Portioning_Weighing_Hopper/
├── include/          # File header tambahan (jika digunakan)
├── lib/              # Library lokal proyek
├── src/
│   └── main.cpp      # Logika utama state machine, pembacaan sensor, servo, dan OLED
├── test/             # Unit test (jika ada)
├── diagram.json      # Skema rangkaian & konfigurasi komponen simulasi Wokwi
├── wokwi.toml        # Konfigurasi koneksi binary firmware untuk Wokwi
├── platformio.ini    # Konfigurasi environment & dependensi library PlatformIO
└── README.md         # Dokumentasi proyek
```

---

## Cara Menjalankan & Mensimulasikan Proyek

### 1. *Build* Firmware via PlatformIO
Pastikan proyek terbuka di **Visual Studio Code** pada direktori `Portioning_Weighing_Hopper`, lalu jalankan *build* melalui terminal:

```powershell
pio run
```

*(Jika perintah `pio` belum dikenali di path terminal tertentu, jalankan menggunakan executable PlatformIO secara langsung)*:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" run
```

### 2. Menjalankan Simulasi Wokwi
1. Pastikan proses *build* berhasil hingga menghasilkan berkas *firmware* (`firmware.hex`).
2. Buka berkas `diagram.json` di VS Code.
3. Tekan tombol **Start / Play** pada simulator Wokwi.
4. Atur target berat (`kg`) menggunakan potensiometer saat kondisi `READY`, tekan `TARE` bila perlu, lalu tekan `START/STOP` untuk memulai siklus penimbangan.

---

## Catatan Pengembangan

- Seluruh parameter penimbangan dan tampilan antarmuka menggunakan satuan **`kg`** agar selaras dengan kontrol simulasi dan kebutuhan demonstrasi.
- Penamaan mode pada layar OLED (`READY`, `FAST MODE`, `SLOW MODE`, `COMPLETE`, `OVERLOAD!`) disederhanakan agar mudah dipahami saat pengujian maupun presentasi.
