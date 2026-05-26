# Safran SmartAV: Industrial IoT & Predictive Edge-AI Safety Loop

An End-to-End Edge-AI and Industrial IoT (IIoT) prototype designed to simulate an automated, predictive cabin climate safety system for aerospace manufacturing and avionics. This system utilizes an ESP32 microcontroller to forecast thermal anomalies and execute preventive mechanical mitigation before critical thresholds are breached.

👉 **Live Simulation Link:** https://wokwi.com/projects/465020161118193665

---

## ☁️ Enterprise IoT Cloud Infrastructure

The core highlight of this project is its transition from an isolated hardware circuit to a fully networked **Industrial IoT (IIoT) Cloud System**. The system bridges local edge computation with a remote live internet server to enable continuous asset tracking from a centralized control room.

### 🌐 Cloud Architecture Highlights:
* **Cloud Broker Platform:** Leveraged **Adafruit IO Cloud Services** as the central cloud environment to ingest, parse, and handle real-time telemetry streams securely.
* **Network Protocol:** Implemented **MQTT (Message Queuing Telemetry Transport)**, an industry-standard, lightweight publish-subscribe network protocol ideal for high-latency, low-bandwidth aerospace communications.
* **Virtual Gateway:** Configured the ESP32 network stack to establish a secure station connection to a virtual Wi-Fi access point (`Wokwi-GUEST`), enabling data packets to bridge seamlessly from a local simulator to a live internet web server.
* **Data Persistence (Feeds):** Architected distinct data channels (`temperature` and `prediction`) to isolate raw sensor telemetry from edge-computed AI forecast arrays.

### 📡 Live Cloud Dashboard In Action
![Safran Cloud Monitor Dashboard](images/cloud_dashboard.jpg)
*Figure 1: Real-time Adafruit IO Cloud Dashboard displaying live temperature gauges and predictive analytics streams matching the local ESP32 output.*

---

## ⚙️ Core Technical Features

### 🧠 Edge Intelligence (Predictive Analytics)
Instead of relying on reactive thresholds, the ESP32 runs a localized **Linear Regression mathematical model** written in C++. 
* Computes the slope (rate of change) across a rolling history of 5 thermal samples.
* Projects the thermal vector 5 steps into the future using the trend equation:
  $$predictedTemp = currentTemp + (slope \times 5)$$
* Proactively commands safety measures early if the forecast crosses the critical limit ($40.0^\circ\text{C}$).

### 🔌 Hardware Interface Stack
* **Microcontroller:** ESP32 (Selected for dual-core processing capability and integrated Wi-Fi stack).
* **Sensors & Actuators:** DHT22 (High-accuracy temperature/humidity sensor), I2C LCD 16x2 Display, and PWM-controlled Servo Motor (Simulating an emergency coolant/vent valve).

---

## 🛠️ How to Replicate This Project

### 1. Hardware Dependencies
Ensure the following libraries are installed in your development environment:
* `DHT sensor library`
* `Adafruit Unified Sensor`
* `LiquidCrystal I2C`
* `ESP32Servo`
* `Adafruit MQTT Library`

### 2. Deployment Instructions
1. Clone this repository.
2. Open `sketch.ino` and update the `AIO_USERNAME` and `AIO_KEY` fields with your specific IoT cloud credentials.
3. Upload the code to your physical ESP32 board or paste it directly into a Wokwi workspace.
4. Set up an Adafruit IO Dashboard containing a gauge block linked to the `temperature` feed and a line graph linked to both `temperature` and `prediction` feeds.
