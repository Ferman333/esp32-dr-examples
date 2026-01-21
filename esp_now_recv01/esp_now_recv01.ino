#include <esp_now.h>
#include <WiFi.h>

#define OLED //Comment if received data won't be showed in an OLed screen
#ifdef OLED
#include <U8g2lib.h>

#define SDA_OLED 5
#define CLK_OLED 6

//OLed object, there is no 72x40 constructor in u8g2 hence the 72x40 screen is
// mapped in the middle of the 132x64 pixel buffer of the SSD1306 controller
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, CLK_OLED, SDA_OLED);

int width = 70;
int height = 40;
int xOffset = 30; // = (132-w)/2
int yOffset = 24; // = (64-h)/2
#endif

// Structure example to send data. It must match the receiver structure
typedef struct struct_message {
  char a[32];
  int b;
};
struct_message myData;


void onDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
  memcpy(&myData, incomingData, len); //Get and copy data

  Serial.print("Text: ");
  Serial.println(myData.a);
  Serial.print("Number: ");
  Serial.println(myData.b);

  #ifdef OLED
  //Write received data in the OLed
  u8g2.clearBuffer();
  int8_t hMax= u8g2.getMaxCharHeight();
  u8g2.setCursor(xOffset, yOffset+hMax);
  u8g2.print(String(myData.a));

  u8g2.setCursor(xOffset+25, yOffset+2*hMax);
  u8g2.print(String(myData.b));
  u8g2.sendBuffer();
  #endif
}


void setup() {
  Serial.begin(115200);
  //Set WiFi mode as STA station
  WiFi.mode(WIFI_STA);
  
  //Init ESP-NOW
  if(esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Serial.println("ESP-NOW initialized");

  //Register CB function
  esp_now_register_recv_cb((esp_now_recv_cb_t) onDataRecv);

  #ifdef OLED
  // Oled init
  delay(1000);
  u8g2.begin();
  u8g2.setContrast(255); // set contrast to maximum 
  u8g2.setBusClock(400000); //400kHz I2C 
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_unifont_t_latin);
  Serial.println("Oled initialized");
  #endif

}


void loop() {
  // Nothing to do, al lis donde by the receiver CB function
}
