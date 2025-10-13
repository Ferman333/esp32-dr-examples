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


//Function to get the UID
void get_UID() {
  uint8_t uid[7];
  uint8_t uidLength;
  
  //Looking for a MiFare_ISO14443A card
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    Serial.print("Found a tag with UID: ");
    for(uint8_t i = 0; i < uidLength; i++) {
      Serial.print(uid[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    delay(1000);
  }
}


//Function to read the data in the card
void read_data() {
  uint8_t uid[7];
  uint8_t uidLength;
  
  //Looking for a MiFare_ISO14443A card
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    Serial.println("Found a tag!");

    //Authenticate block data
    Serial.println("Trying to authenticate block 4");
    uint8_t keya[6]= {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //Default key
    if(nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya)) {

      //Trying to read data
      Serial.println("Sector 1 (blocks 4 to 7) authentified!");
      uint8_t data[16];
      
      //Read data
      if(nfc.mifareclassic_ReadDataBlock(4, data)) {
        Serial.println("Data read from sector 4:");
        nfc.PrintHexChar(data,16);
        Serial.println("");
        delay(3000);

      } else Serial.println("Fail while reading card.");
    } else Serial.println("Error while authenticating card.");
    
  }
}



//Function to write string data in the card
void write_data(String str) {
  uint8_t uid[7];
  uint8_t uidLength;
  
  //Looking for a MiFare_ISO14443A card
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    Serial.println("Found a tag!");

    //Authenticate block data
    Serial.println("Trying to authenticate block 4");
    uint8_t keya[6]= {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //Default key
    if(nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya)) {

      //Trying to write data
      Serial.println("Sector 1 (blocks 4 to 7) authentified!");
      uint8_t data[16];
      
      //Write data
      if(str.length()<=16) {
        for(uint8_t i=0; i<16-str.length(); i++) {
          str += " "; //Fill with whitespaces to get a 16 length text
        }
        memcpy(data, (const uint8_t*) str.c_str(), 16);
        if(nfc.mifareclassic_WriteDataBlock(4, data)) {
          Serial.println("Data written in sector 4:");
          delay(3000);
        
        } else Serial.println("Fail while writing card.");
      } else Serial.println("Text must have less than 16 characters");
    } else Serial.println("Error while authenticating card.");
    
  }
}




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
  //get_UID();
  read_data();
  //write_data("Hola mundo");
}

