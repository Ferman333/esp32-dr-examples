/**
* Este programa lee la temperatura con un sensor DS18B20, y luego la muestra en una pantalla LCD.
*/

#include <OneWire.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>


// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 2 


// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);


//LCD_I2 object (liquid crystal screen)
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);


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




void setup(void) 
{ 
 // start serial port 
 //Serial.begin(9600); 
 //Serial.println("Dallas Temperature IC Control Library Demo"); 
 
 // Initiate the LCD:
  lcd.init();
  lcd.backlight();

  lcd.createChar(0,circ);
 
 // Start up the library 
 sensors.begin(); 
} 




void loop(void) 
{ 
 // call sensors.requestTemperatures() to issue a global temperature 
 // request to all devices on the bus 
 sensors.requestTemperatures(); // Send the command to get temperature readings 
 
 lcd.setCursor(2, 0); // Set the cursor on the third column and first row.
 lcd.print("Temperatura:");
 lcd.setCursor(4,1);
 lcd.print(String(sensors.getTempCByIndex(0), 2));
 lcd.write((byte) 0);
 lcd.print("C");
 
 
 delay(1000); //Request temperatures once at each second
} 
