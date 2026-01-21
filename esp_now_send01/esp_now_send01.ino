#include <esp_now.h>
#include <WiFi.h>

uint8_t receiverAddr[]= {0x28, 0x37, 0x2F, 0x6B, 0x26, 0x94}; //(WiFi-STA) MAC of receiver device
// Structure example to send data. It must match the receiver structure
typedef struct struct_message {
  char a[32];
  int b;
};
struct_message myData;

esp_now_peer_info_t peerInfo;


//Callback function when data sent
void onDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
  Serial.print("Last package status: ");
  Serial.println(status==ESP_NOW_SEND_SUCCESS ? "Success":"Fail");
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
  esp_now_register_send_cb((esp_now_send_cb_t) onDataSent);

  //Configure peer (receiver device)
  memcpy(peerInfo.peer_addr, receiverAddr, 6);
  peerInfo.channel=0;
  peerInfo.encrypt=false;

  //Add peer
  if(esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  Serial.println("Added peer sucessfully!");
}


void loop() {
  if(Serial.available()>0) {
    strcpy(myData.a, Serial.readStringUntil('\n').c_str()); //Read string from Serial
    myData.b= random(1,100);

    //Send data
    if(esp_now_send(receiverAddr, (uint8_t*) &myData, sizeof(myData)) == ESP_OK) {
      Serial.println("Data sent successfully");
    } else {
      Serial.println("Error while sending data. Try again");
    }
  }
}
