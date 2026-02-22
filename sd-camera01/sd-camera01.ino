#include "board_config.h" //Header to set the Esp-Cam model
#include <esp_camera.h>
#include <FS.h>
#include <SD_MMC.h>
//#include <LittleFS.h>
#include <Preferences.h>

#define LED 33 //On board LED (not the flash)
#define TOUCH_PIN 12

Preferences prf; //Preferences object to save the pic's number
bool touched = false;
bool previousTouched = false;

void configCamera() {
  camera_config_t config; //Config object
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
  config.frame_size = FRAMESIZE_UXGA;
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
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }
  
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  
  sensor_t* s = esp_camera_sensor_get();
  s->set_brightness(s, 0);
  s->set_contrast(s, 0);
  s->set_saturation(s, 0);
  s->set_special_effect(s, 0);
  s->set_whitebal(s, 1);
  s->set_awb_gain(s, 1);
  s->set_wb_mode(s, 0);
  s->set_exposure_ctrl(s, 1);
  s->set_aec2(s, 0);
  s->set_ae_level(s, 0);
  s->set_aec_value(s, 300);
  s->set_gain_ctrl(s, 1);
  s->set_agc_gain(s, 0);
  s->set_gainceiling(s, (gainceiling_t)0);
  s->set_bpc(s, 0);
  s->set_wpc(s, 1);
  s->set_raw_gma(s, 1);
  s->set_lenc(s, 1);
  s->set_hmirror(s, 0);
  s->set_vflip(s, 0);
  s->set_dcw(s, 1);
  s->set_colorbar(s, 0);

  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }
  
#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif
}


void takePicture() {
  camera_fb_t* fb = esp_camera_fb_get(); //Take photo
  
  uint32_t cnt = prf.getUInt("count", 0);
  String path = "/pic" + String(cnt) + ".jpg";
  
  //Save photo
  File file = SD_MMC.open(path.c_str(), FILE_WRITE);
  if(!file) {
    Serial.println("Error while saving photo");
    return;
  }
  file.write(fb->buf, fb->len);
  file.close();
  Serial.println("Photo saved at "+path);
  
  prf.putUInt("count", cnt+1); //Increase and save value
  esp_camera_fb_return(fb);
}


void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(LED, OUTPUT);
  
  prf.begin("pic-counter01", false);

  if(!SD_MMC.begin()) { //SD initialization
    Serial.println("SD mount failed.");
    return;
  }

  configCamera();
  for(uint16_t i=0; i<16; i++) { //Take some photos to stabilize the image
    camera_fb_t* fb= esp_camera_fb_get();
    esp_camera_fb_return(fb);
  }
  Serial.println("Camera Started!");
}

void loop() {
  // touched= (touchRead(TOUCH_PIN) < 20);
  // if(touched != previousTouched) { //Detect change in touched state
    // if(touched) { //OnTouched pin
      // digitalWrite(LED, 1); //Turn on led
      // previousTouched = true;
      // takePicture();
    // } else { //OnUntouched pin
      // digitalWrite(LED, 0); //Turn off led
      // previousTouched = false;
    // }
  // }
  
  takePicture();
  digitalWrite(LED, 1);
  delay(1000); //Led on for 1 second
  digitalWrite(LED, 0);
  delay(9000); //Take picture every 10 s (including the led-on time)
}
