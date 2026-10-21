#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 22

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  // Scan I2C connetions
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.println("I2C Bus Scanner");
}

void loop() {
  byte error, address;
  int nDevices = 0;

  Serial.println("\nScanning I2C bus...");

  for (address = 1; address < 127; address++) {
    // Wire.endTransmission() returns 0 if a device acknowledges (ACK) the address
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print(" -> I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      
      // Helper identification based on Lab 7 components
      Serial.print("  (");
      if (address == 0x23 || address == 0x5C) {
        Serial.print("BH1750 Light Sensor");
      } else if (address == 0x27 || address == 0x3F) {
        Serial.print("16x2 LCD I2C Backpack");
      } else if (address == 0x76 || address == 0x77) {
        Serial.print("BMP280 Pressure Sensor");
      } else {
        Serial.print("Unknown I2C Device");
      }
      Serial.println(")");

      nDevices++;
    } 
    else if (error == 4) {
      Serial.print(" -> Unknown bus error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0) {
    Serial.println("WARNING: No I2C devices found! Check wiring and 4.7k pull-up resistors.");
  } else {
    Serial.printf("Scan complete. Total I2C devices responding: %d\n", nDevices);
    Serial.println("Total ESP32 GPIO pins consumed by I2C bus: 2 (GPIO 21, GPIO 22)");
  }

  delay(5000); // Wait 5 seconds before scanning again
}