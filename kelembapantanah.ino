#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
// Ganti dengan SSID dan password Wi-Fi Anda
const char* ssid = "aa";
const char* password = "12345678";
// Ganti dengan URL Supabase API dan API Key
const String supabaseURL = "https://iqdnpgggupisxhhfepbi.supabase.co";
const String apiKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImlxZG5wZ2dndXBpc3h
oaGZlcGJpIiwicm9sZSI6ImFub24iLCJpYXQiOjE3MzE0NzE3NDEsImV4cCI6MjA0NzA0Nzc0MX0.s2aDzTXB0Q4OQF9YSO8amZ8syCVkCNWU5ORfLf_v8l0";
// Pin untuk sensor kelembapan tanah (output analog)
#define SENSOR_PIN A0
// Interval pengiriman data ke Supabase dalam milidetik
int sendingInterval = 3000; // Pengiriman data setiap 3 detik
HTTPClient https;
WiFiClientSecure client;
void setup() {
 // Mengatur pin LED bawaan untuk menunjukkan status pengiriman data
 pinMode(LED_BUILTIN, OUTPUT);
 digitalWrite(LED_BUILTIN, HIGH); // LED menyala saat tidak mengirim data
 // Menghubungkan ke Wi-Fi
 Serial.begin(115200);
 connectToWiFi();
 // HTTPS tanpa validasi sertifikat
 client.setInsecure();
}
void loop() {
 // Cek status koneksi Wi-Fi
 if (WiFi.status() == WL_CONNECTED) {
 digitalWrite(LED_BUILTIN, LOW); // LED menyala saat mengirim data
 // Membaca nilai kelembapan tanah
 int moistureValue = readSoilMoisture();
 
 // Mengirim data ke Supabase

 sendToSupabase(moistureValue);
 // Matikan LED setelah pengiriman data
 digitalWrite(LED_BUILTIN, HIGH); // LED mati
 } else {
 Serial.println("Error in Wi-Fi connection");
 }
 // Tunggu beberapa detik sebelum membaca dan mengirim data lagi
 delay(sendingInterval); // 3 detik sebelum membaca sensor lagi
}
// Fungsi untuk menghubungkan ke Wi-Fi
void connectToWiFi() {
 Serial.print("Connecting to Wi-Fi");
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
 Serial.println("");
 Serial.println("Wi-Fi connected.");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());
}
// Fungsi untuk membaca nilai kelembapan tanah
int readSoilMoisture() {
 int moistureValue = analogRead(SENSOR_PIN);
 int moisture = 1024 - moistureValue; // Biasanya 1024 - nilai sensor untuk kelembapan
 Serial.print("Soil Moisture: ");
 Serial.println(moisture);
 return moisture;
}
// Fungsi untuk mengirim data ke Supabase
void sendToSupabase(int moistureValue) {
 // Menyusun data JSON untuk dikirim
 String postData = "{\"kelembapan\":" + String(moistureValue) + "}";
 // Mengirim permintaan POST ke Supabase
 https.begin(client, supabaseURL + "/rest/v1/tanah");
 https.addHeader("Content-Type", "application/json");
 https.addHeader("Prefer", "return=representation");
 https.addHeader("apikey", apiKey);
 https.addHeader("Authorization", "Bearer " + apiKey);
 // Mengirim data JSON
 int httpCode = https.POST(postData);
 String payload = https.getString(); 
4
 // Menampilkan hasil HTTP
 Serial.println(httpCode);
 Serial.println(payload);
 https.end(); // Menutup koneksi
}
