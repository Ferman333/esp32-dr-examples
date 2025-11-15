/**
* Este programa lee la temperatura con un sensor DS18B20, y luego la muestra en una pantalla LCD.
*/

#include <OneWire.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>


// Data wire is plugged into pin 10 on the Esp32 
#define ONE_WIRE_BUS 10


// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

//LCD_I2 object (liquid crystal screen)
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);



void setup(void) 
{ 
 // start serial port 
 Serial.begin(9600); 
 Serial.println("Dallas Temperature IC Control Library Demo"); 
 
 // Initiate the LCD:
  lcd.init();
  lcd.backlight();
 
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
 lcd.print(String(sensors.getTempCByIndex(0), 2)); //Get and write temperature
 lcd.write((char)0xDF); //Degree symbol in Hitachi A-00 ROM
 lcd.print("C");
 Serial.println("Temp.: "+ String(sensors.getTempCByIndex(0), 2)+ " deg"); //Show temperature in Serial
 
 delay(1000); //Request temperatures once at each second
} 
