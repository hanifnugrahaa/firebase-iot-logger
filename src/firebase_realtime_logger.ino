#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8622)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <time.h>



#define WIFI_SSID "Masukkan SSID WiFi"
#define WIFI_PASSWORD "Masukkan Password SSID WiFi"
#define API_KEY "Gunakan API Key Firebase"
#define DATABASE_URL "Gunakan reference URL project Firebase"


//Dekralasi variabel global
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

void setup(){
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);
  Serial.println("Scan WiFi...");

  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("No networks found");
  } else {
    Serial.println("Networks found:");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(WiFi.SSID(i));
      delay(10);
    }
  }
 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Menghubungkan ke Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED){
    Serial.println("Gagal Terkoneksi");
    Serial.print("Status WiFi: ");
    Serial.println(WiFi.status());
    delay(1000);
  }
  Serial.println();
  Serial.print("Terhubung dengan Wi-Fi : ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Konfigurasi waktu nyata (NTP)
  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov"); // UTC+7 (WIB)
  Serial.println("Menunggu sinkronisasi waktu");
  while(time(nullptr) < 100000){
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWaktu tersinkronisasi");
  
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);

    char timestamp[25];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H_%M_%S", timeinfo); // Format waktu
    
    // Write sebuah angka integer pada database dengan path dinamis
    String pathInt = "/test/intHistory/" + String(timestamp);
    Serial.print("Mengirim ke : ");
    Serial.println(pathInt);

    if (Firebase.RTDB.setInt(&fbdo, pathInt, count)){
      Serial.println("Integer berhasil disimpan dengan timestamp..");
    } else{
      Serial.println("Failed send data..");
      Serial.println("What makes : " + fbdo.errorReason());
    }
  count++;

  
    //Write data float ke firebase dengan path /test/floatHistory
    // Kode baru
    String pathFloat = "/test/floatHistory/" + String(timestamp);
    Serial.print("Mengirim ke : ");
    Serial.print(pathFloat);

    if (Firebase.RTDB.setFloat(&fbdo, pathFloat, 0.01 + random(0, 100))){
      Serial.println("PATH : " + fbdo.dataPath() + ", Data type : " + fbdo.dataType());
      Serial.println("Float berhasil disimpan dengan timestamp...");
    }
    else{
      Serial.println("FAILED..");
      Serial.println("What makes : " + fbdo.errorReason());
    }

  }
}
