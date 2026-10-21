#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>
#include <SD.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define SD_CS   5   // SPI Chip Select pin

Adafruit_BMP280 bmpA, bmpB;

void readBoth() {
  Serial.printf("A: %.2f hPa   B: %.2f hPa\n",
                bmpA.readPressure() / 100.0F,
                bmpB.readPressure() / 100.0F);
}

// Part C: Write and read back a short test file on microSD
void testSDCard() {
  Serial.println("\nSPI microSD Test");
  if (!SD.begin(SD_CS)) {
    Serial.println("SD init failed! Check SPI wiring (18,19,23,5) & FAT32 card.");
    return;
  }
  Serial.println("SD card initialized.");

  // Write test file
  File f = SD.open("/test.txt", FILE_WRITE);
  if (f) {
    f.println("Lab 7 SPI microSD test OK!");
    f.close();
    Serial.println("Wrote to /test.txt");
  } else {
    Serial.println("Write failed!");
    return;
  }

  // Read back test file
  f = SD.open("/test.txt", FILE_READ);
  if (f) {
    Serial.print("Read back: ");
    while (f.available()) {
      Serial.write(f.read());
    }
    f.close();
  }
  Serial.println("SPI pins used: 4 (GPIO 18, 19, 23, CS=5) vs I2C bus: 2 (GPIO 21, 22)");
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  // Scan I2C connections
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.println("I2C & SPI Bus Scanner");

  if (!bmpA.begin(0x76)) Serial.println("BMP A not found at 0x76");
  if (!bmpB.begin(0x77)) Serial.println("BMP B not found at 0x77");

  // Run Part C SD card write/read test once at startup
  testSDCard();
}

void loop() {
  byte error, address;
  int nDevices = 0;

  Serial.println("\nScanning I2C bus...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print(" -> I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      
      // Helper identification
      Serial.print("  (");
      if (address == 0x23 || address == 0x5C) {
        Serial.print("BH1750 Light Sensor");
      } else if (address == 0x27 || address == 0x3F) {
        Serial.print("16x2 LCD I2C Backpack");
      } else if (address == 0x76) {
        Serial.print("BMP280 A - 0x76");
      } else if (address == 0x77) {
        Serial.print("BMP280 B - 0x77");
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
  }

  // Part B: Read both BMP280s
  if (bmpA.begin(0x76) && bmpB.begin(0x77)) {
    readBoth();
  }

  delay(5000);
}