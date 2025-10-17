/**
* Este programa pretende medir la distancia a un cierto objeto usando un sensor de ultrasonido, y luego mostrarla en una pantalla LCD. Aún en construcción...
*/

//#include <LiquidCrystal_I2C.h> //Shows the distance in an LCD screen. Comment if you don't want use an LCD
#include <U8g2lib.h> //Shows the distance in an Oled screen. Comment if you don't want use an Oled

#define TRIG 0 //Number pin for the ultrasound trigger
#define ECHO 1 //Number pin to receive the ultrasound echo
#define LED 8 //Number pin of a led. If your board has a built-in led, you can use it
#define SDA_OLED 5 //Sda of the Oled screen. Comment if you won't use an Oled
#define CLK_OLED 6 //Clk of the Oled screen. Comment if you won't use an Oled


//LiquidCrystal_I2C lcd= LiquidCrystal_I2C(0x27, 16,2);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, CLK_OLED, SDA_OLED);

//Sizes of the Oled
uint8_t width = 70;
uint8_t height = 40;
uint8_t xOffset = 30; // = (132-w)/2 : X coord. of the left-upper corner
uint8_t yOffset = 24; // = (64-h)/2 : Y coord. of the left-upper corner

long t;
float d;

void setup() {
  //Initialize serial communication:
  Serial.begin(9600);

  //lcd.init();
  //lcd.backlight();

  //Begin Oled
  delay(1000);
  u8g2.begin();
  u8g2.setContrast(255); // set contrast to maximum 
  u8g2.setBusClock(400000); //400kHz I2C 
  u8g2.enableUTF8Print();
  Serial.println("OLed iniciada.");
  
  pinMode(LED, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  digitalWrite(LED, HIGH); //Just use in Esp32-C3, because of the inverted logic!
  
}



void loop() {
  // establish variables for duration of the ping, and the distance result
  // in inches and centimeters:
  
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  //Shot the PING
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  
  // The same pin is used to read the signal from the PING))): a HIGH pulse
  // whose duration is the time (in microseconds) from the sending of the ping
  // to the reception of its echo off of an object.
  t = pulseIn(ECHO, HIGH, 1000000);
  //d = v*t/2 (in cm)
  d = 0.0343*t/2;
  
  //Show distance in serial
  Serial.println("Distance: "+ String(d,2)+ " cm");
  
  //Turn-on the led if a near object is detected
  if(d<7) digitalWrite(LED, LOW); //In Esp32-C3, use inverted logic!
  else digitalWrite(LED, HIGH);
  
  // Write distance in the LCD
  // lcd.setCursor(3,0);
  // lcd.print("Distancia:");
  // lcd.setCursor(3,1);
  // lcd.print( String(d,2)+ " cm" );

  //Write distance in the Oled
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_t_latin); //(u8g2_font_ncenB10_tr);
  u8g2.setCursor(xOffset, yOffset+ 1.5*u8g2.getMaxCharHeight());
  u8g2.print(String(d,2)+" cm");
  u8g2.sendBuffer();
  
  delay(500);
}
