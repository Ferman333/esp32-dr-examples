#include <FS.h>
#include <LittleFS.h>

/* Adapted from the original in:
   https://github.com/lorol/arduino-esp32littlefs-plugin

   If you test two partitions, you need to use a custom
   partition.csv file, see in the sketch folder */

#define FORMAT_LITTLEFS_IF_FAILED true //Just for formatting the first time


void listDir(fs::FS &fs, const char* dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
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
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}


void createDir(fs::FS &fs, const char* path) {
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}


void removeDir(fs::FS &fs, const char* path) {
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path)) {
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
}


void readFile(fs::FS &fs, const char* path, uint8_t* buf) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }
  
  file.read(buf, file.size()); //Read data and save it in the buffer
  buf[file.size()] = '\0'; //End of string
  Serial.println("- read from file:");
  // while (file.available()) {
  //   Serial.write(file.read());
  // }
  for(size_t i=0; i<file.size(); i++) Serial.write(buf[i]);
  Serial.println();
  file.close();
}


void writeFile(fs::FS &fs, const char* path, const char* message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}


void appendFile(fs::FS &fs, const char* path, const char* message) {
  Serial.printf("Appending to file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("- failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("- message appended");
  } else {
    Serial.println("- append failed");
  }
  file.close();
}


void deleteFile(fs::FS &fs, const char* path) {
  Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path)) {
    Serial.println("- file deleted");
  } else {
    Serial.println("- delete failed");
  }
}


void setup() {
  Serial.begin(115200);
  delay(500);
  
  //Begin LittleFS
  if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
    Serial.println("LittleFS Mount Failed");
    return;
  }
  
  listDir(LittleFS, "/", 0);

  if(!LittleFS.exists("/boots.txt")) writeFile(LittleFS, "/boots.txt", "Boots: 1");
  else {
    uint8_t str[15];
    int n;

    readFile(LittleFS, "/boots.txt", str);
    sscanf((const char*) str, "Boots: %d", &n); //Get current number of (re)boots

    String new_str= "Boots: "+String(n+1); //Increase number of (re)boots
    writeFile(LittleFS, "/boots.txt", (const char*) new_str.c_str());
  }
}

void loop() {}
