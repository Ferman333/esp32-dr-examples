//#include <MUIU8g2.h>
#include <U8g2lib.h>
//#include <U8x8lib.h>
#include <Wire.h>

#define SDA_OLED 5
#define CLK_OLED 6

//OLed object, there is no 72x40 constructor in u8g2 hence the 72x40 screen is
// mapped in the middle of the 132x64 pixel buffer of the SSD1306 controller
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, CLK_OLED, SDA_OLED);


//Heart symbol
byte heart[8] = {
  0b00000000,
  0b11000011,
  0b11100111,
  0b01111110,
  0b01111110,
  0b00111100,
  0b00011000,
  0b00000000
};

int width = 70;
int height = 40;
int xOffset = 30; // = (132-w)/2
int yOffset = 24; // = (64-h)/2



void setup(void)
{
     //Serial.begin(9600);
     delay(1000);
     u8g2.begin();
     u8g2.setContrast(255); // set contrast to maximum 
     u8g2.setBusClock(400000); //400kHz I2C 
     u8g2.enableUTF8Print();
     //u8g2.setFont(u8g2_font_unifont_t_latin); //(u8g2_font_ncenB10_tr);
     //Serial.println("OLed iniciada. Para cambiar el tamanio del marco, ingresa (w,h,xOffset,yOffset) en ese orden");
}


void loop(void)
{
    u8g2.clearBuffer(); // clear the internal memory
    //u8g2.drawFrame(xOffset+0, yOffset+0, width, height); //draw a frame around the border
    //u8g2.drawBox(xOffset+0, yOffset+0, width, height); //Fill a box
    //u8g2.setCursor(xOffset+15, yOffset+25);
    //u8g2.printf("%dx%d", width, height);
    u8g2.setCursor(xOffset+4, yOffset+18);
    u8g2.setFont(u8g2_font_unifont_t_latin);
    u8g2.print("ESP32-C3");
    u8g2.drawBitmap(xOffset+22, yOffset+20, 1,8, heart); //u8g2.drawBitMap(x0,y0,byte cnt, h, byte[] bitmap);
    u8g2.drawBitmap(xOffset+32, yOffset+20, 1,8, heart);
    u8g2.drawBitmap(xOffset+42, yOffset+20, 1,8, heart);
    u8g2.sendBuffer(); // transfer internal memory to the display
}
