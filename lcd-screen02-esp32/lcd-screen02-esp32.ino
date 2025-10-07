/**
* Este programa crea un AP Wifi con un server para interactuar con el microcontrolador desde el celular, y cambiar el texto en una pantalla LCD con el chip HD44780U de Hitachi. Toma en cuenta que hay 2 versiones de la memoria ROM para los Hitachi: la A00 que sólo trae algunos caracteres katakana y las letras latinas básicas, y la A02 que cambia los katakana por letras latinas extendidas (con diacríticos para soporte de varios idiomas europeos), griegas y cirílicas. Esto sólo ha sido probado en el chip con la ROM A00, lo cual ha obligado a cambiar algunos caracteres del español por otras letras para que se muestren en pantalla (ejemplo, las letras acentuadas se cambian por su respectiva vocal sin acento).

*/


#include "credentials.h" //File with custom "ssid" and "password" for the AP
#include <WiFi.h>
#include <WebServer.h>
#include <LiquidCrystal_I2C.h>



// Create a web server object
WebServer server(80);

//LCD_I2 object (liquid crystal screen)
// LiquidCrystal_I2C is the class for the LC2_I2C standard class
//LCDI2C_Latin_Symbols is the class for extended latin symbols
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

String textContent ="";
//index.html
String html01= R"rawliteral(
<html>
<head>
<title>LCD screen-ESP32 Web server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body { background-color: #555; }

textarea {
  background-color: #0033FF;
  color: white;
  font: normal normal 22px calligra, verdana;
}

p {
  color: white;
  font: normal normal 18px calligra, verdana;
}

</style>
</head>
<body>
<p>Escribe algo...</p>

<form method="GET" action="/send_text">
<textarea rows="2" cols="16" name="text01">
)rawliteral";

String html02= R"rawliteral(
</textarea><br>
<input type="submit" />
</form>

</body>
</html>
)rawliteral";


// Ñ symbol
const uint8_t utf8_0xD1[8] = { 0b01101, 0b10110, 0b00000, 0b10001, 0b11001, 0b10101, 0b10011, 0b10001 };

//Ü symbol
const uint8_t utf8_0xDC[8] = { 0b01010, 0b00000, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110 };

//Heart symbol
uint8_t heart[8] = {
  0b00000000,
  0b11000011,
  0b11100111,
  0b01111110,
  0b01111110,
  0b00111100,
  0b00011000,
  0b00000000
};


//Replaces the diacritical letters with its corresponding letter without diacritic, or with the most similar letter in the Hitachi A00 ROM
void convertSpanishText(String& str) {
  //str.replace("\r", ""); //Delete car return caracter

  //Replace lowercase accented vowels
  str.replace(String((char) 0xE1), "a"); //á
  str.replace(String((char) 0xE9), "e"); //é
  str.replace(String((char) 0xED), "i"); //í
  str.replace(String((char) 0xF3), "o"); //ó
  str.replace(String((char) 0xFA), "u"); //ú

  //Replace uppercase accented vowels
  str.replace(String((char) 0xC1), "A"); //Á
  str.replace(String((char) 0xC9), "E"); //É
  str.replace(String((char) 0xCD), "I"); //Í
  str.replace(String((char) 0xD3), "O"); //Ó
  str.replace(String((char) 0xDA), "U"); //Ú

  //Replace Ñ and ñ
  str.replace(String((char) 0xF1), String((char) 0xEE) ); //Code for the pseudo-ñ in the A00 ROM
  str.replace(String((char) 0xD1), "N" ); //Use the RAM defined Ñ symbol (by now just using "N")

  //Replace ü and Ü
  str.replace(String((char) 0xFC), String((char) 0xF5) ); //Code for the pseudo-ü in the A00 ROM
  str.replace(String((char) 0xDC), "U" ); //Use the RAM Ü defined symbol (by now just using "U")

  //Some math symbols (using the ROM)
  str.replace(String((char) 0xB0), String((char) 0xDF) ); //Degrees
  str.replace(String((char) 0xF7), String((char) 0xFD) ); //Division
  str.replace(String((char) 0xD7), String((char) 0xEB) ); //Multiplication
  //str.replace(String((char) 0x221A), String((char) 0xE8) ); //Square root
  //str.replace(String((char) 0x221E), String((char) 0xF3) ); //Infinity
  str.replace(String((char) 0xA2), String((char) 0xEC) ); //Cents
}


//Splits an input text in lines, and prints every line in the LCD screen
void printSplitString(String txt, uint8_t rows) {
  int8_t startIndex =0;
  int8_t endIndex= txt.indexOf('\n');
  uint8_t i=0;
  String substr;
  

  while (endIndex != -1 && i<rows) {
        substr = txt.substring(startIndex, endIndex);
        convertSpanishText(substr);

        lcd.setCursor(0,i);
        lcd.print(substr); // Imprimir cada subcadena

        startIndex = endIndex + 1; // Mover el índice de inicio
        endIndex = txt.indexOf('\n', startIndex); // Buscar el siguiente \n
        i+=1;
  }
  
  //Print last line (if it doesn't end in \n)
  if(startIndex<txt.length()) {
    substr = txt.substring(startIndex, txt.length());
    convertSpanishText(substr);
    
    lcd.setCursor(0,i);
    lcd.print( substr );
  }
}


void handleRoot() {
  //Send the HTML
  server.send(200, "text/html", html01+textContent+html02);
  
}


void handleText() {
  //Get text from the textarea
  if (server.hasArg("text01")) {
    textContent = server.arg("text01"); //Get value from the client
    textContent.replace("\r", "");
    Serial.println(textContent);

    //Print on screen
    lcd.clear();
    printSplitString(textContent, 2);
  }
  handleRoot();
}



void setup(void) 
{ 
  // Start serial port 
  Serial.begin(9600);
  
  
  //Begin Wifi AP
  Serial.println("Creating Access Point");
  WiFi.softAP(ssid, password);
  Serial.println("Access point created. Connect to the IP "+ WiFi.softAPIP().toString());
  
  // Set up the web server to handle different routes
  server.on("/", handleRoot);
  server.on("/send_text", HTTP_GET, handleText);
  //server.on("/off", HTTP_GET, handleGPIOOff);
  
  // Start the web server
  server.begin();
  Serial.println("HTTP server started");
  

  // Initiate the LCD:
  lcd.init();
  lcd.backlight();
  Serial.println("Screen started");
  lcd.createChar(6, (uint8_t*) utf8_0xD1); //Create Ñ symbol
  lcd.createChar(7, (uint8_t*) utf8_0xDC); //Create Ü symbol
} 




void loop(void) 
{
  server.handleClient();
}
