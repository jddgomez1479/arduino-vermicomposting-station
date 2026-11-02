# Vermicomposting Station Firmware (Arduino) 

Arduino-based firmware designed to monitor and automate a vermicomposting (lombricultura) system. The software manages environmental sensors, an interactive menu displayed on an I2C LCD, a 4x4 membrane keypad, and automated irrigation routines backed by persistent EEPROM storage and a Real-Time Clock (RTC).

---

## Features & System Capabilities

* **Real-Time Monitoring:** Tracks ambient temperature, soil/substrate moisture, and pH levels using analog inputs and digital RTC data.
* **User Interface (HRI):** Navigable menu system powered by an **I2C LCD (16x2)** and a **4x4 Keypad** with custom cursor indicators and backlight controls.
* **Sensor Calibration:** Calibration routines for humidity and pH sensors with parameter persistence saved directly to the microcontroller's **EEPROM**.
* **Real-Time Clock (RTC):** Integrates a **DS3231 RTC module** to handle precise scheduling, timestamps, and automated time-based events.
* **Automated Irrigation:** Dual-mode irrigation system supporting scheduled daily watering times or automated soil moisture-triggered activation via an output relay.

---

## Hardware Requirements

* **Microcontroller:** Arduino Mega 2560 or Arduino Uno (compatible board with sufficient SRAM/Flash for libraries).
* **Display:** 16x2 LCD Display with I2C interface module (Default Address: `0x27`).
* **Input Device:** 4x4 Matrix Membrane Keypad.
* **Real-Time Clock:** DS3231 RTC Module.
* **Sensors:** 
  * pH Sensor module connected to analog pin `A2`.
  * Soil Moisture Sensor connected to analog pin `A3`.
* **Actuator:** Relay module connected to digital pin `10` for water pump/valve control.

---

## Required Arduino Libraries

To compile and flash this code successfully, ensure you have the following libraries installed via the Arduino IDE Library Manager:

1. **`LiquidCrystal_I2C`** (by Frank de Brabander)
2. **`Keypad`** (by Mark Stanley, Alexander Brevig)
3. **`RTClib`** (by Adafruit)
4. **`EEPROM`** (Native library included with Arduino IDE core)

---

## Pinout Configuration

| Component / Periph. | Pin / Interface | Description |
| :--- | :--- | :--- |
| **I2C LCD** | SDA / SCL | Dedicated I2C bus pins |
| **Keypad Rows** | Pins `2, 3, 4, 5` | Row lines for 4x4 matrix |
| **Keypad Columns** | Pins `6, 7, 8, 9` | Column lines for 4x4 matrix |
| **Relay Control** | Pin `10` | Digital output for irrigation pump/valve |
| **pH Sensor** | Pin `A2` | Analog input for pH reading |
| **Moisture Sensor** | Pin `A3` | Analog input for moisture level |

---

## Getting Started & Usage

1. Clone or download this repository.
2. Open the `.ino` file using the **Arduino IDE**.
3. Install all required dependencies listed above.
4. Select your board model (`Arduino Uno` or `Arduino Mega 2560`) and correct COM port under **Tools > Board**.
5. Compile and upload the sketch to your microcontroller.

---

## License
This project is open-source and available for academic and agricultural automation purposes.