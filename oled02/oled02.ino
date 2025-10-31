/**
* Este programa crea un AP Wifi con un server para interactuar con el microcontrolador desde un celular, para escribir (y en un futuro dibujar) en una pantalla Oled. Se ha usado el micro Esp32-C3 supermini con pantalla Oled incluida de 72x40 (que en realidad resultó ser de 70x40).
*/

#include "credentials.h" //File with custom "ssid" and "password" for the AP
#include <WiFi.h>
#include <WebServer.h>
//#include <MUIU8g2.h>
#include <U8g2lib.h>
//#include <U8x8lib.h>
#include <Wire.h>

#define SDA_OLED 5
#define CLK_OLED 6


// Create a web server object
WebServer server(80);

//OLed object, there is no 72x40 constructor in u8g2 hence the 72x40 screen is
// mapped in the middle of the 132x64 pixel buffer of the SSD1306 controller
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, CLK_OLED, SDA_OLED);

String textContent ="";
//index.html
String html01= R"rawliteral(
<html>
<head>
<meta charset="UTF-8">
<title>Oled screen-ESP32 Web server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body {
  background-color: #666;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  gap: 8px;
}

textarea {
  background-color: #333;
  color: white;
  font: normal normal 28px calligra, verdana;
}

p {
  color: white;
  font: normal normal 24px calligra, verdana;
}

button {
  width: 80px;
  height: 35px;
  background-color: #00AA50;
  color: white;
  font-size: 18px;
}
</style>
</head>

<body>
<p>Escribe algo...</p>
<textarea rows="4" cols="8" name="text01" id="text01">
)rawliteral";

String html02= R"rawliteral(
</textarea><br>
<button id="button01" onclick="send_text();">Enviar</button>

<script>
function send_text() {
  var txt_area= document.getElementById("text01");
  var xhr= new XMLHttpRequest();
  
  xhr.open("POST", "/send_text", true);
  xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded; charset=UTF-8');
  
  xhr.onreadystatechange = function() {
    if(xhr.readyState===4 && xhr.status===200) {
      console.log(xhr.responseText);
      txt_area.value = xhr.responseText;
    }
  };
  
  xhr.send("text01="+ encodeURIComponent(txt_area.value));
}
</script>
</body>
</html>
)rawliteral";

//Heart symbol
/*uint8_t heart[8] = {
  0b00000000,
  0b11000011,
  0b11100111,
  0b01111110,
  0b01111110,
  0b00111100,
  0b00011000,
  0b00000000
};*/

int width = 70;
int height = 40;
int xOffset = 30; // = (132-w)/2 : X coord. of the left-upper corner
int yOffset = 24; // = (64-h)/2 : Y coord. of the left-upper corner




//Splits an input text in lines, and prints every line in the LCD screen
void printSplitString(String txt, uint8_t rows) {
  int8_t startIndex =0;
  int8_t endIndex= txt.indexOf('\n');
  int8_t hChMax;
  uint8_t i=0;
  String substr;
  
  u8g2.setFont(u8g2_font_unifont_t_latin);
  hChMax= u8g2.getMaxCharHeight();

  while (endIndex != -1 && i<rows) {
        substr = txt.substring(startIndex, endIndex);

        u8g2.setCursor(xOffset, yOffset+ (i+1)*12);
        u8g2.print(substr); // Imprimir cada subcadena
        
        startIndex = endIndex + 1; // Mover el índice de inicio
        endIndex = txt.indexOf('\n', startIndex); // Buscar el siguiente \n
        i+=1;
  }
  
  //Print last line (if it doesn't end in \n)
  if(startIndex<txt.length()) {
    substr = txt.substring(startIndex, txt.length());
    
    u8g2.setCursor(xOffset, yOffset+ (i+1)*12);
    u8g2.print( substr );
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
    u8g2.clearBuffer();
    printSplitString(textContent, 4);
    u8g2.sendBuffer();
  }
  server.send(200, "text/plain", textContent);
}



void setup(void)
{
    Serial.begin(9600);
    
    //Begin Wifi AP
    Serial.println("Creating Access Point");
    WiFi.softAP(ssid, password);
    Serial.println("Access point created. Connect to the IP "+ WiFi.softAPIP().toString());
    
    // Set up the web server to handle different routes
    server.on("/", handleRoot);
    server.on("/send_text", handleText);
    
    // Start the web server
    server.begin();
    Serial.println("HTTP server started");
    
    //Begin Oled
    delay(1000);
    u8g2.begin();
    u8g2.setContrast(255); // set contrast to maximum 
    u8g2.setBusClock(400000); //400kHz I2C 
    u8g2.enableUTF8Print();
    //u8g2.setFont(u8g2_font_unifont_t_latin); //(u8g2_font_ncenB10_tr);
    Serial.println("OLed iniciada.");
}


void loop(void)
{
    server.handleClient();
}
