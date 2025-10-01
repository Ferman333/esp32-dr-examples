/**********
GPS + LCD screen
*********/
#include <LiquidCrystal_I2C.h>
#include <TinyGPS++.h>

// Define the RX and TX pins for Serial 1
#define RXD 20
#define TXD 21
#define BTN 4
//#define LED 8
#define GPS_BAUD 9600

//Basic variables
uint8_t previous_st = LOW;
uint8_t mode = 0;

// Create an instance of the HardwareSerial class for Serial 1
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Degree symbol
byte circ[8] = {
  0b00011100,
  0b00010100,
  0b00011100,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000
};


void setup(){
  // Serial Monitor
  Serial.begin(115200);
  
  // Start Serial 2 with the defined RX and TX pins and a baud rate of 9600
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD, TXD);
  Serial.println("Serial 1 started at 9600 baud rate");
  
  // Button input
  pinMode(BTN, INPUT);
  
  //Start lcd
  lcd.init();
  lcd.backlight();
  lcd.createChar(0,circ);

  lcd.setCursor(3,0);
  lcd.print("GPS iniciado");
  
  //Counter
  Serial.print("Counter: ");
  Serial.println(mode);
}



void loop(){

  //Wait a second to read again the GPS
  unsigned long start = millis();
  while (millis() - start < 1000) {
  //On button down event
  if(previous_st==LOW && digitalRead(BTN)==HIGH) {
    previous_st=HIGH;
    mode= (mode+1)%6;
    //Print counter
    Serial.print("Counter: ");
    Serial.println(mode);
  }
  //On button up event
  else if(previous_st==HIGH && digitalRead(BTN)==LOW) {
    previous_st=LOW;
  }
  //Reading GPS data
    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
    }
  }
  
  if(gps.location.isUpdated()) {
    // Print serial
    //Serial.print("LAT: ");
    //Serial.println(gps.location.lat(), 6);
    //Serial.print("LONG: "); 
    //Serial.println(gps.location.lng(), 6); 
    //Serial.print("ALT (min)= "); 
    //Serial.println(gps.altitude.meters());
    //Serial.print("SPEED (km/h) = "); 
    //Serial.println(gps.speed.kmph());
    //Serial.print("HDOP = "); 
    //Serial.println(gps.hdop.value() / 100.0); 
    //Serial.print("Satellites = "); 
    //Serial.println(gps.satellites.value()); 
    //Serial.print("Time in UTC: ");
    //Serial.println(String(gps.date.year()) + "/" + String(gps.date.month()) + "/" + String(gps.date.day()) + "," + String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second()));
    //Serial.println("");
    
    lcd.clear();
    //Print values according to mode value
    switch(mode) {
      case 0:
      //Print date and time
      lcd.setCursor(3, 0);
      lcd.print(String(gps.date.day()) + "/" + String(gps.date.month()) + "/" + String(gps.date.year()));
      lcd.setCursor(3,1);
      lcd.print(String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second())+ " UTC");
      break;
      case 1:
      //Print lat in the LCD
      lcd.setCursor(0, 0);
      lcd.print("Lat:");
      lcd.print(gps.location.lat(), 6);
      lcd.write((byte) 0);
      //Print lng in the LCD
      lcd.setCursor(0, 1);
      lcd.print("Lon:");
      lcd.print(gps.location.lng(), 6);
      lcd.write((byte) 0);
      break;
      case 2:
      //Print alt in LCD
      lcd.setCursor(1, 0);
      lcd.print("Alt:");
      lcd.print(gps.altitude.meters(), 0);
      lcd.print(" msnm");
      break;
      case 3:
      //Print vel in LCD
      lcd.setCursor(2, 0);
      lcd.print("Vel:");
      lcd.print(gps.speed.kmph(), 0);
      lcd.print(" km/h");
      break;
      case 4:
      //Print HDOP in LCD
      lcd.setCursor(3, 0);
      lcd.print("HDOP:"); 
      lcd.print(gps.hdop.value() / 100.0);
      break;
      case 5:
      //Print number of visible satellites
      lcd.setCursor(1, 0);
      lcd.print("Satellites:"); 
      lcd.print(gps.satellites.value());
      break;
    }
  }
  //delay(1000);
  //Serial.println("-------------------------------");
}
