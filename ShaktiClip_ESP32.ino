/* ShaktiClip_ESP32.ino
   Complete firmware for ShaktiClip:
   - SOS Button
   - GPS Location
   - LoRa Communication
   - Fall Detection (MPU6050)
   - Inactivity Detection (15 seconds)
   - Battery + HR Placeholder
   - LoRa + Optional WiFi Fallback
*/

#include <TinyGPSPlus.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "MPU6050.h"

// ====================== CONFIGURATION ======================

// Enable WiFi fallback? (true = yes, false = no)
#define USE_WIFI_FALLBACK false

// WiFi Credentials (only used if fallback enabled)
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASS = "YOUR_WIFI_PASS";

// Backend URL (used only if WiFi fallback enabled)
const char* BACKEND_URL = "http://127.0.0.1:5000/alert";

// LoRa Pins (change if needed)
#define LORA_SS   18
#define LORA_RST  14
#define LORA_DIO0 26
#define LORA_BAND 915E6  // change according to your region (433E6 / 868E6 / 915E6)

// GPS UART Pins
HardwareSerial GPSSerial(1);
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
#define GPS_BAUD   9600

// Button + LED Pins
const int SOS_BUTTON_PIN = 4;  // active LOW
const int LED_PIN = 2;         // built-in LED

// Accelerometer (MPU6050)
MPU6050 mpu;
bool mpuInitialized = false;
float accelThreshold = 2.0; // when fall impact is detected
unsigned long IMMOBILE_TIME = 15000; // 15 seconds

// Debounce
unsigned long lastButtonPress = 0;
const unsigned long debounceMs = 300;

// Device ID
String DEVICE_ID = "PC-01";

// GPS Object
TinyGPSPlus gps;

// ====================== HELPER FUNCTIONS ======================

// Battery placeholder (modify if using ADC)
int readBatteryPercent() {
  return 80; // Mock value for simulation
}

// Heart-rate placeholder (modify if using MAX30102)
float readHeartRate() {
  return 75.0;
}

// Create JSON packet
String makeJSON(String reason, double lat, double lon, float accel) {
  int batt = readBatteryPercent();
  float hr = readHeartRate();
  String json = "{";
  json += "\"deviceID\":\"" + DEVICE_ID + "\",";
  json += "\"reason\":\"" + reason + "\",";
  json += "\"lat\":" + String(lat, 6) + ",";
  json += "\"lon\":" + String(lon, 6) + ",";
  json += "\"battery\":" + String(batt) + ",";
  json += "\"hr\":" + String(hr) + ",";
  json += "\"accel\":" + String(accel, 2) + ",";
  json += "\"ts\":\"" + String(millis()) + "\"";
  json += "}";
  return json;
}

// Send LoRa packet
void sendLoRa(String payload) {
  Serial.println("[LoRa] Sending: " + payload);
  LoRa.beginPacket();
  LoRa.print(payload);
  LoRa.endPacket();
}

// Optional WiFi fallback
bool sendHTTP(String payload) {
  if (!USE_WIFI_FALLBACK) return false;
  if (WiFi.status() != WL_CONNECTED) return false;

  HTTPClient http;
  http.begin(BACKEND_URL);
  http.addHeader("Content-Type", "application/json");
  int code = http.POST(payload);

  Serial.println("[HTTP] Code: " + String(code));
  http.end();
  return (code >= 200 && code < 300);
}

// Main alert trigger function
void triggerAlert(String reason, double lat, double lon, float accel) {
  String json = makeJSON(reason, lat, lon, accel);

  // Send via LoRa
  sendLoRa(json);

  // Send via WiFi fallback (optional)
  sendHTTP(json);

  // Flash LED
  digitalWrite(LED_PIN, HIGH);
  delay(800);
  digitalWrite(LED_PIN, LOW);
}

// SOS button handling
void checkButton() {
  bool pressed = !digitalRead(SOS_BUTTON_PIN);

  if (pressed) {
    unsigned long t = millis();

    if (t - lastButtonPress > debounceMs) {
      lastButtonPress = t;

      Serial.println("[BUTTON] SOS Pressed!");

      double lat = gps.location.isValid() ? gps.location.lat() : 0.0;
      double lon = gps.location.isValid() ? gps.location.lng() : 0.0;

      triggerAlert("ManualSOS", lat, lon, 0);
    }
  }
}

// Fall + 15-second inactivity detection
void checkFall() {
  if (!mpuInitialized) return;

  Vector accel = mpu.readRawAccel();
  float ax = accel.XAxis / 16384.0;
  float ay = accel.YAxis / 16384.0;
  float az = accel.ZAxis / 16384.0;
  float mag = sqrt(ax*ax + ay*ay + az*az);

  if (mag > accelThreshold) {
    Serial.println("[FALL] Impact detected!");

    unsigned long start = millis();

    while (millis() - start < IMMOBILE_TIME) {
      Vector check = mpu.readRawAccel();
      float ax2 = check.XAxis / 16384.0;
      float ay2 = check.YAxis / 16384.0;
      float az2 = check.ZAxis / 16384.0;
      float mag2 = sqrt(ax2*ax2 + ay2*ay2 + az2*az2);

      if (mag2 > 0.4) {
        Serial.println("[FALL] Movement detected → Cancel auto-SOS");
        return;
      }

      delay(200);
    }

    double lat = gps.location.isValid() ? gps.location.lat() : 0.0;
    double lon = gps.location.isValid() ? gps.location.lng() : 0.0;

    triggerAlert("FallAuto", lat, lon, mag);
  }
}

// ====================== SETUP ======================
void setup() {
  Serial.begin(115200);
  delay(200);

  // GPS
  GPSSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

  // Button & LED
  pinMode(SOS_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  // MPU6050
  Wire.begin();
  if (mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_16G)) {
    Serial.println("MPU6050 Ready");
    mpuInitialized = true;
  }

  // LoRa
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (LoRa.begin(LORA_BAND)) {
    Serial.println("LoRa OK");
  } else {
    Serial.println("LoRa FAILED");
  }

  // WiFi fallback
  if (USE_WIFI_FALLBACK) {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting WiFi");

    for (int i = 0; i < 20; i++) {
      if (WiFi.status() == WL_CONNECTED) break;
      delay(300);
      Serial.print(".");
    }

    Serial.println(WiFi.status() == WL_CONNECTED ? "\nWiFi OK" : "\nWiFi FAILED");
  }
}

// ====================== LOOP ======================
void loop() {
  // Read GPS
  while (GPSSerial.available()) gps.encode(GPSSerial.read());

  checkButton();
  checkFall();

  delay(50);
}
