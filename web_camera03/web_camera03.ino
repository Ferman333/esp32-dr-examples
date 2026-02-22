/**
* Uso de la API de Telegram para mandar una foto tomada desde Esp32-Cam
*/
#include <esp_camera.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
//#include <HTTPClient.h>

// ===========================
// Select camera model in board_config.h
// ===========================
#include "board_config.h"
#include "credentials.h" //File with the "ssid" and "password" credentials of your WiFi

#define LED 33 //Led GPIO pin
#define TOUCH_PIN 12 //GPIO pin used for touch trigger
#define INVERTED //For led inverted logic. Comment if not inverted logic at led pin

bool touched = false;
bool previousTouched = false;

// Telegram server
const char* host = "api.telegram.org";
const int httpsPort = 443;

//unsigned long currentMillis;

//#define CONFIG_CAMERA_TASK_STACK_SIZE 8192


void config_camera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA; //XGA = 1024x768, SXGA = 1280x1024, UXGA = 1600x1200
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA; //HVGA = 480x320, VGA = 640x480, SVGA = 800x600
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    //s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  }
  // drop down frame size for higher initial frame rate (just for video stream)
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_VGA);
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif
}


String urlencode(String str) {
    String out="";
    for(int i=0; i<str.length(); i++) {
        if((str[i]>='a' && str[i]<='z') || (str[i]>='A' && str[i]<='Z') || (str[i]>='0' && str[i]<='9')) out += str[i];
        else {
            out+= "%"+ String(str[i], 16);
        }
    }
    return out;
}


bool requestTelegram(camera_fb_t* fb) {
  String boundary = "----ESP32Boundary7MA4YWxkTrZu0gW";

  // Header for chat_id and photo
  String headers =
    // chat_id
    String("--") + boundary + "\r\n" +
    "Content-Disposition: form-data; name=\"chat_id\"\r\n\r\n" +
    chatId + "\r\n" +
    // Photo
    "--" + boundary + "\r\n" +
    "Content-Disposition: form-data; name=\"photo\"; filename=\"photo.jpg\"\r\n" +
    "Content-Type: image/jpeg\r\n\r\n";
  
  String footer = "\r\n--" + boundary + "--\r\n";

  size_t contentLength = headers.length() + fb->len + footer.length();

  String requestHeader = String("POST /bot") + BOTtoken + "/sendPhoto HTTP/1.1\r\n"
                 + "Host: " + host + "\r\n"
                 + "User-Agent: ESP32-Cam\r\n"
                 + "Connection: close\r\n"
                 + "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n"
                 + "Content-Length: " + String(contentLength) + "\r\n\r\n";
  
  //Create secure client & connect
  WiFiClientSecure wifiClient;
  // Optional: not verify certificate (not recommended in production)
  wifiClient.setInsecure();

  if (!wifiClient.connect(host, httpsPort)) {
    Serial.println("Connection to host failed");
    return false;
  }
  Serial.println("WiFiClient connected");

  // Create HttpClient on the WiFiClientSecure
  //HTTPClient http = HttpClient(wifiClient, host, httpsPort);

  //Write headers
  wifiClient.print(requestHeader);
  wifiClient.print(headers);
  
  //Write binary content of the image
  size_t written = 0;
  const uint8_t* img = fb->buf;
  const size_t chunkSize = 1024;
  while (written < fb->len) {
    size_t toWrite = fb->len - written;
    if (toWrite > chunkSize) toWrite = chunkSize;
    wifiClient.write(img + written, toWrite);
    written += toWrite;
  }

  //Write footer
  wifiClient.print(footer);

  // Read HTTP response (simple)
  // Wait response (max 5 s)
  unsigned long timeout = millis();
  while (wifiClient.connected() && !wifiClient.available()) {
    if (millis() - timeout > 5000) {
      Serial.println("Response timeout");
      break;
    }
    delay(10);
  }

  // Read status line
  String line = ""; //Status line
  if (wifiClient.available()) {
    line = wifiClient.readStringUntil('\n');
    line.trim();
    Serial.println("Status: " + line);
  } else { //No response = photo not sended
    Serial.println("Photo not sended to Telegram");
    wifiClient.stop();
    return false;
  }

  //Read headers & body (optional)
  //bool isBody = false;
  //String responseBody = "";
  //while (wifiClient.available()) {
  //  line = wifiClient.readStringUntil('\n');
  //  if (!isBody) {
  //    if (line == "\r" || line == "") {
  //      isBody = true; // header-body separator
  //    }
  //  } else {
  //    responseBody += line + "\n";
  //  }
  //}
  //
  //Serial.println("Response body:");
  //Serial.println(responseBody);

  // Close wifi client
  wifiClient.stop();
  return true;
}


void takePicture() {
  camera_fb_t* fb = esp_camera_fb_get(); //Take photo
  
  Serial.println("Sending photo to Telegram... ("+String(fb->len)+" bytes)");
  if(requestTelegram(fb)) {
    Serial.println("Photo sended successfully!");
  }

  esp_camera_fb_return(fb);
}


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(800);
  
  pinMode(LED, OUTPUT);
  #if defined(INVERTED)
  digitalWrite(LED, 1);
  #endif
  
  config_camera();
  for(uint16_t i=0; i<16; i++) { //Take some photos to stabilize the image
    camera_fb_t* fb= esp_camera_fb_get();
    esp_camera_fb_return(fb);
  }
  Serial.println("Camera started!");
  
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  
  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  
  Serial.print("Use this local IP: 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  touched= (touchRead(TOUCH_PIN) < 20);
  //currentMillis=millis();
  if(touched != previousTouched) { //Detect change in touched state
    if(touched) { //OnTouched pin
      #if defined(INVERTED)
      digitalWrite(LED, 0); //Turn on led (inverted logic)
      #else
      digitalWrite(LED, 1); //Turn on led
      #endif
      previousTouched = true;
      takePicture();
    } else { //OnUntouched pin
      #if defined(INVERTED)
      digitalWrite(LED, 1); //Turn off led (inverted logic)
      #else
      digitalWrite(LED, 0); //Turn off led
      #endif
      previousTouched = false;
    }
  }
}
