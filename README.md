# Solar-Powered-Water-Quality-Monitor-and-Control-System-For-Fish-Farming
EEE4196 - Capstone Project (Undergraduate)

An IoT-enabled, solar-assisted embedded system that **monitors** and **controls** key water quality parameters for aquaculture. The system targets small-to-medium fish farms and supports both **on-site monitoring (LCD)** and **remote monitoring (ThingSpeak)** to help maintain stable conditions for fish survival and growth.

---

## Overview

Fish farming productivity depends heavily on maintaining suitable water conditions. Manual testing is time-consuming and prone to error, especially when multiple parameters must be checked frequently. This project automates water quality management by continuously sensing and controlling:

- **Temperature**
- **pH**
- **Dissolved Oxygen (DO)**
- **Total Dissolved Solids (TDS)** (monitored only)

The system is designed to be energy-efficient using **solar charging + battery power**, making it suitable for **off-grid** or resource-limited environments.

---

## Key Features

- **Real-time monitoring** of pH, Temperature, DO, and TDS
- **Automatic control** of:
  - **Temperature** using an AC heater via relay
  - **pH** using a buffer ejector pump (sodium bicarbonate buffer)
  - **DO** using a 12V aerator pump
- **On-site display** using a 16×2 I2C LCD
- **Remote monitoring** using **ESP8266 Wi-Fi** with **ThingSpeak**
- **Solar-powered architecture** (solar panel → charge controller → 12V battery → buck converter)

---

## System Architecture

### Power Flow
- **20W Solar Panel** charges a **12V battery** through a **solar charge controller**
- **LM2596 buck converter** steps down 12V → 5V to power:
  - microcontroller + sensors + LCD + relay logic
- **12V rail** powers:
  - aerator pump
  - buffer ejector pump
- **AC supply** powers:
  - heater coil (controlled via relay)

> Note: The heater is AC-powered to avoid high DC current draw and additional voltage regulation complexity.

---

## Hardware Components

### Sensors
- **pH Sensor:** PH4502C (0–14 pH range)
- **Temperature Sensor:** DS18B20 (±0.5°C accuracy)
- **TDS Sensor:** analog TDS sensor (ppm / mg/L)
- **Dissolved Oxygen Sensor:** Gravity analog DO sensor (0–20 mg/L)

### Controller & Interface
- **NodeMCU Mega WiFi R3 (ATmega2560 + ESP8266)**
- **16×2 I2C LCD**
- **4-Channel Relay Module**

### Actuators
- **Aerator Pump (12V DC)** for DO control
- **Buffer Ejector Pump (12V DC)** for pH control
- **Water Heater Coil (AC)** for temperature control

### Power
- **20W Solar Panel**
- **Solar Charge Controller (20A)**
- **12V 1.3Ah Lead-Acid Battery**
- **LM2596 Buck Converter (12V → 5V)**

---

## Control Logic (Threshold-Based)

Sensor readings are sampled every ~20 seconds. If values go out of range, the system activates the corresponding actuator via relay.

### Temperature
- If **Temperature < 24°C** → Heater **ON**
- Heater turns **OFF** when **Temperature ≥ 24°C**

### pH
- If **pH < 6.0** or **pH > 8.5** → Buffer Ejector **ON**
- Buffer Ejector turns **OFF** when **6.0 ≤ pH ≤ 8.5**

### Dissolved Oxygen (DO)
- If **DO < 5 mg/L** → Aerator **ON**
- Aerator turns **OFF** when **DO ≥ 5 mg/L**

### TDS
- **Monitored only**
- If TDS exceeds acceptable range, mitigation requires **manual water change** or **RO filtration**, so automated control is not included.

---

## Cloud Monitoring (ThingSpeak)

The integrated ESP8266 uploads the following fields to ThingSpeak:
- pH
- Temperature
- TDS
- Dissolved Oxygen

ThingSpeak provides:
- real-time graphs and history
- remote access from anywhere
- optional alerting (based on channel configuration)

---

## Calibration Summary

- **pH Sensor:** calibrated using known buffer solutions (e.g., pH 6.86 and verification with pH 4.01) and validated against a pH meter
- **Temperature Sensor:** verified with a thermometer (no calibration required)
- **TDS Sensor:** verified using a handheld TDS meter
- **DO Sensor:** single-point calibration with membrane cap fill solution (0.5 mol/L NaOH) and saturation steps

---

## Experiment Summary (Tilapia)

- System was tested in a **120-liter tank**
- Parameters were recorded at **6-hour intervals** and visualized over time
- Two tanks were used to compare survivability:
  - manual/conservative monitoring tank showed high mortality within a few days
  - system-controlled tank achieved **90% survival over 20 days** (1 casualty out of 10 fish)

---

You can organize the repo like this:

