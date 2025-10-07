/**
* Programa para probar las funciones básicas de lectura y escritura para tarjetas MIFARE_ISO14443A mediante NFC, usando el módulo PN532.
*/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>


//Conexiones SPI del ESP32-C3 Supermini
#define SCK  (4)
#define MISO (5)
#define MOSI (6)
#define SS   (7)


Adafruit_PN532 nfc(SCK, MISO, MOSI, SS);

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing...");
  delay(3000);
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  Serial.println(versiondata);
  if (!versiondata) {
    Serial.println("PN532 not found");
    while (1); // halt
  }
  // Configure the PN532 as RFID/NFC reader
  nfc.SAMConfig();
  Serial.println("PN532 is ready");
}


void loop() {
  // Check for NFC tags and print their UID
  uint8_t success;
  uint8_t uid[7];
  uint8_t uidLength;
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    Serial.print("Found a tag with UID: ");
    for(uint8_t i = 0; i < uidLength; i++) {
      Serial.print(uid[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    delay(1000);
  }
}
