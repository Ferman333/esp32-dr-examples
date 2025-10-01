#include <LiquidCrystal_I2C.h>
//#include <characters.h>


//LCD_I2 object (liquid crystal screen)
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);


//Heart symbol
uint8_t heart[8] = {
  0b00000000,
  0b00010001,
  0b00011011,
  0b00011111,
  0b00001110,
  0b00001110,
  0b00000100,
  0b00000000
};



void setup(void) 
{ 
 // start serial port 
 //Serial.begin(9600);
 
 // Initiate the LCD:
  lcd.init();
  lcd.backlight();
  
  lcd.createChar(0, heart);
  //lcd.createChar(0, (uint8_t*) utf8_0xD1);
  //lcd.setCursor(0,0);
  //lcd.write(0);
  //lcd.createChar(0, (uint8_t*) utf8_0xC1);
  //lcd.setCursor(0,1);
  //lcd.write(0);
   
} 




void loop(void) 
{ 
 String txt1="Hola niños";
 String txt2= "I💚U";

 txt1.replace("ñ", String((char) 0xEE) );
 txt2.replace("💚", String((char) 0) );

 lcd.setCursor(3, 0); // Set the cursor on the third column and first row.
 lcd.print(txt1);
 //lcd.print("Hola desde");
 //lcd.setCursor(4,1);
 //lcd.print("ESP-32!!!");
 lcd.setCursor(5,1);
 for(uint8_t i=0; i<3; i++)
    lcd.write(txt2[i]);
 
 //delay(1000);
} 
