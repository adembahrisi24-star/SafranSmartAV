#include <WiFi.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2); 
Servo ventValve;

const int SERVO_PIN = 18;
const float TEMP_LIMIT = 40.0; 

// 🔑 PLUGGED IN DIRECTLY FROM YOUR SCREENSHOT:
#define AIO_USERNAME    "adambahri"
#define AIO_KEY         "aio_XeET98IHHu1bxDN5HVwxzq237oSW"

// Wi-Fi and Server Setup for Wokwi
const char* WLAN_SSID     = "Wokwi-GUEST"; 
const char* WLAN_PASS     = "";
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883

// Setup Cloud Connection Clients
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish tempPublish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish predPublish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/prediction");

// Arrays to store history for our AI calculation
float tempHistory[5] = {0, 0, 0, 0, 0};
int sampleCount = 0;

void MQTT_connect(); // Helper function declaration

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  // Initialize LCD Screen cleanly for Wokwi
  lcd.begin(16, 2); 
  lcd.backlight();
  lcd.print("Connecting WiFi...");
  
  // Initialize Servo
  ventValve.attach(SERVO_PIN);
  ventValve.write(0); 

  // Connect to the virtual network
  Serial.print("Connecting to virtual Wi-Fi");
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi Connected!");
  
  lcd.clear();
  lcd.print("Safran Node ON");
  delay(2000);
  lcd.clear();
}

void loop() {
  MQTT_connect(); // Ensure cloud database connection is alive

  float currentTemp = dht.readTemperature();
  
  if (isnan(currentTemp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Shift history array to make room for new data
  for (int i = 0; i < 4; i++) {
    tempHistory[i] = tempHistory[i + 1];
  }
  tempHistory[4] = currentTemp;
  if (sampleCount < 5) sampleCount++;

  float predictedTemp = currentTemp; 

  // AI STEP: Calculate the trend (Linear Regression Slope)
  if (sampleCount >= 5) {
    float sumX = 0, sumY = 0, sumXY = 0, sumXX = 0;
    for (int i = 0; i < 5; i++) {
      sumX += i;
      sumY += tempHistory[i];
      sumXY += i * tempHistory[i];
      sumXX += i * i;
    }
    
    float slope = (5 * sumXY - sumX * sumY) / (5 * sumXX - sumX * sumX);
    predictedTemp = currentTemp + (slope * 5); 
  }

  // Print data to Serial Monitor
  Serial.print("Current: "); Serial.print(currentTemp);
  Serial.print("°C | Predicted: "); Serial.print(predictedTemp);
  Serial.println("°C");

  // Display on Virtual LCD
  lcd.setCursor(0, 0);
  lcd.print("Now: "); lcd.print(currentTemp, 1); lcd.print("C   ");
  lcd.setCursor(0, 1);
  lcd.print("Pred: "); lcd.print(predictedTemp, 1); lcd.print("C  ");

  // AI Decision Logic
  if (predictedTemp >= TEMP_LIMIT) {
    lcd.setCursor(12, 1);
    lcd.print("[⚠️]");
    ventValve.write(90); 
    Serial.println("⚠️ WARNING: Predictive Overheating! Valve open.");
  } else {
    lcd.setCursor(12, 1);
    lcd.print("[OK]");
    ventValve.write(0); 
  }

  // 📡 TELEMETRY UPGRADE: Push numbers over the internet to your Dashboard
  Serial.print("📡 Broadcasting telemetry data...");
  if (! tempPublish.publish(currentTemp)) Serial.print(" Temp fail.");
  if (! predPublish.publish(predictedTemp)) Serial.print(" Pred fail.");
  Serial.println(" Data pushed successfully!");

  delay(5000); // Wait 5 seconds between transmission bursts
}

// System function to maintain internet connection stability
void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) return;
  Serial.print("Connecting to cloud server... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
    retries--;
    if (retries == 0) while (1); 
  }
  Serial.println("Connected to Dashboard! ✅");
}
