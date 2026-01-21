//#include <WiFi.h>
#include <esp_wifi.h>

/*
* A program to get the MAC address of the board
*/

void setup() {
  //Begin Serial
  Serial.begin(115200);
  Serial.println("Begin...");
  
  //Begin Wifi in STA mode
  wifi_init_config_t defaultConfig= WIFI_INIT_CONFIG_DEFAULT();
  if(esp_wifi_init(&defaultConfig) != ESP_OK) {
    Serial.println("It couldn't init WiFi");
    return;
  }
  
  Serial.println("Reading MAC address...");
  //Get and read MAC address
  uint8_t sta_MAC[6];
  if(esp_wifi_get_mac(WIFI_IF_STA, sta_MAC) == ESP_OK) {
    Serial.print("MAC address: ");
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n", sta_MAC[0],sta_MAC[1],sta_MAC[2],sta_MAC[3],sta_MAC[4],sta_MAC[5]);
  }
  else {
    Serial.println("Failed to read MAC address");
  }

}

void loop() {
  // put your main code here, to run repeatedly (nothing)
}
