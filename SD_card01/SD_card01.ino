/*
 * Adapted from the original in:
 * https://github.com/espressif/arduino-esp32/tree/master/libraries/SD
 *
 * Important methods of File System fs::FS class (commonly, fs=SD for SD cards):

 * bool fs.open(const char* path, const char* mode="r"); //Opens the file at the given path
   The modes for open() are "r" (default, for reading files), "w" (for writing files, replacing the current data), and "a" (for appending files, it just writes without replacing data)
 
 * bool fs.mkdir(const char* path); //Makes a directory with the given path
 * bool fs.rmdir(const char* path); //Removes the directory at the given path
 * bool fs.rename(const char* path1, const char* path2); //Renames the file at path1 as the path in path2. Also allows move files
 * bool fs.remove(const char* path_file); //Removes the FILE at the given path

 *Important methods in File class:
 * f.close();
 * bool f.isDirectory();
 * File f.openNextFile(); //Opens the next file at the given directory f. Util for listing files
 * f.path(); //Gets the full path of the file
 * f.name();
 * f.size(); //size in bytes
 
 * f.available(); //Byumber of available bytes in f for reading
 * f.read(); //Reads a byte from the file
 * f.print(const char* str); //Writes the given string in the file
 */

#include <FS.h>
#include <SD.h>
#include <SPI.h>

//Pins for SPI. Redefine according to your board model
#define SCK (4)
#define MISO (5)
#define MOSI (6)
#define CS (7)


void listDir(fs::FS &fs, const char* dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}


void readFile(fs::FS &fs, const char* path) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}


void writeFile(fs::FS &fs, const char* path, const char* message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}


void appendFile(fs::FS &fs, const char* path, const char* message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}


/*void testFileIO(fs::FS &fs, const char* path) {
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file) {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %lu ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }

  file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++) {
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %lu ms\n", 2048 * 512, end);
  file.close();
}*/


void setup() {
  Serial.begin(115200);
  
  //Begin SPI
  SPI.begin(SCK, MISO, MOSI, CS);

  //Begin SD
  if (!SD.begin(CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  
  Serial.print("SD Card Type: ");
  switch(cardType) {
    case CARD_MMC:
      Serial.println("MMC");
    break;
    case CARD_SD:
      Serial.println("SDSC");
    break;
    case CARD_SDHC:
      Serial.println("SDHC");
    break;
    default:
      Serial.println("UNKNOWN");
    break;
  }
  
  uint64_t cardSize = SD.cardSize() / (1024 * 1024); //Size in MB
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  listDir(SD, "/", 0);
  writeFile(SD, "/hello.txt", "Hello ");
  appendFile(SD, "/hello.txt", "World!\n");
  readFile(SD, "/hello.txt");
  readFile(SD, "/test.txt");
  //testFileIO(SD, "/test.txt");

  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}


void loop() {
  if(Serial.available()) {
    String txt= Serial.readStringUntil('\n'); //get text from Serial
    
    File f= SD.open("/serial.txt", "w");
    if(!f) {
      Serial.println("Error opening file");
      return;
    }

    if( f.write((const uint8_t*) txt.c_str(), txt.length()) ) {
      Serial.println("Serial data written successfully!");
    } else {
      Serial.println("Error writing file");
    }
    f.close();
  }
}
