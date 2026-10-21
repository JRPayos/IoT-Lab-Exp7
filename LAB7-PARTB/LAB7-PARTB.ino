#include <Wire.h>
#include <Adafruit_BMP280.h>

#define SDA_PIN 21
#define SCL_PIN 22

// Reference declarations from Lab Manual Part B
Adafruit_BMP280 bmpA, bmpB;

// Reference function from Lab Manual Part B
void readBoth() {
  Serial.printf("A (0x76): %.2f hPa   B (0x77): %.2f hPa\n",
                bmpA.readPressure() / 100.0F,
                bmpB.readPressure() / 100.0F);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  // Scan I2C connections + Initialize BMP280s (Lab Reference Setup)
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.println("I2C Bus Scanner & Part B Dual BMP280 Test");

  if (!bmpA.begin(0x76)) Serial.println("BMP A not found at 0x76");
  if (!bmpB.begin(0x77)) Serial.println("BMP B not found at 0x77");
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
      
      // Helper identification based on your Lab 7 components (No SHT31)
      Serial.print("  (");
      if (address == 0x23 || address == 0x5C) {
        Serial.print("BH1750 Light Sensor");
      } else if (address == 0x27 || address == 0x3F) {
        Serial.print("16x2 LCD I2C Backpack");
      } else if (address == 0x76) {
        Serial.print("BMP280 Pressure Sensor A - SDO tied to GND");
      } else if (address == 0x77) {
        Serial.print("BMP280 Pressure Sensor B - SDO Unconnected/Default");
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

  // Part B: Attempt to initialize and read both BMP280 sensors at 0x76 and 0x77
  Serial.println("\nDual BMP280 Reading");
  bool okA = bmpA.begin(0x76);
  bool okB = bmpB.begin(0x77);

  if (!okA) Serial.println("BMP A not found at 0x76 (Connect one BMP280 SDO pin to GND to resolve conflict!)");
  if (!okB) Serial.println("BMP B not found at 0x77");

  if (okA && okB) {
    readBoth(); // Both addresses active -> print independent pressure values
  }

  delay(5000); // Wait 5 seconds before next cycle
}