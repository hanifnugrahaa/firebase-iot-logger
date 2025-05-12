# Firebase IoT Data Logger (ESP32)

Proyek ini menghubungkan ESP32 ke Firebase Realtime Database untuk mencatat data integer dan float secara periodik dengan timestamp.

## Peralatan
- ESP32 board
- Firebase Realtime Database
- WiFi

## Library yang Digunakan
- Firebase ESP Client by Mobizt
- WiFi.h (ESP32)

## Setup
1. Clone repo ini
2. Buka `firebase_realtime_logger.ino`
3. Masukkan SSID, Password WiFi, API_KEY, dan DATABASE_URL
4. Upload ke board

## Output
Data akan dikirim ke:
- `/test/intHistory/[timestamp]`
- `/test/float`
