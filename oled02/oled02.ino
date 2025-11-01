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
//#include <ArduinoJson.h>

#define SDA_OLED 5
#define CLK_OLED 6


// Create a web server object
WebServer server(80);

//OLed object, there is no 72x40 constructor in u8g2 hence the 72x40 screen is
// mapped in the middle of the 132x64 pixel buffer of the SSD1306 controller
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, CLK_OLED, SDA_OLED);

String textContent ="";
//index.html
String PROGMEM html01= R"rawliteral(
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

canvas {
  background-color: #000;
  width: 216px;
  height: 120px;
  border-color: green;
}

.toggle-button {
  display: inline-block;
  width: auto;
  background-color: #0080F8;
}

.panel {
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  gap: 8px;
}

.panel01 {
  display: flex;
  flex-direction: horizontal;
  justify-content: center;
  align-items: center;
  gap: 10px;
}


.frame {
  width: 20px;
  height: 20px;
  border: 3px;
  border-color: orange;
}
</style>
</head>

<body>
<button class="toggle-button" onclick="toggle_interface();">Dibujo/Texto</button>

<div class="panel" id="text-interface">
<p>Escribe algo...</p>
<textarea rows="4" cols="8" name="text01" id="text01">
)rawliteral";

String PROGMEM html02= R"rawliteral(
</textarea>
<button id="button01" onclick="send_text();">Enviar</button>
</div>

<div class="panel" id="img-interface" style="display:none;">
<p>Dibuja algo...</p>
<div class="panel01">
  <canvas width="216" height="120" id="img_area01"></canvas>
  <div class="panel">
    <div class="frame" style="background:white; border-style: solid;" onclick="change_color('white');" id="white-frame"></div>
    <div class="frame" style="background:black;" onclick="change_color('black');" id="black-frame"></div>
  </div>
</div>
<button id="button01" onclick="send_img();">Enviar</button>
</div>

<script>
var cnv= document.getElementById("img_area01");
var ctx= cnv.getContext("2d");

var cnv_pressed=false;
var color="white";
var X0, Y0;
var W, H;

var BMP_img = []; //Array for the BMP image


// ********* AJAX functions **********
//Send the text of <textarea> to the server
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

//Send the BMP image to server
function send_img() {
  var xhr= new XMLHttpRequest();
  
  xhr.open("POST", "/send_img", true);
  xhr.setRequestHeader('Content-Type', 'application/json');
  
  xhr.onreadystatechange = function() {
    if(xhr.readyState===4 && xhr.status===200) {
      console.log("Response: "+ xhr.responseText);
    }
  };
  
  xhr.send(JSON.stringify(BMP_img));
}


// ******** Listeners *********
addEventListener("load", ()=> {
                              W= cnv.width;
                              H= cnv.height;
                              ctx.fillStyle= cnv.style.background;
                              ctx.fillRect(0,0, W,H);
                              
                              for(k=0; k<(W/24)*H/3; k++) BMP_img[k]=0; //Fill the image array with zeros
                              });

addEventListener("mouseup", end_draw);
addEventListener("touchend", end_draw);

cnv.addEventListener("mousedown", start_draw);
cnv.addEventListener("touchstart", start_draw);

cnv.addEventListener("mousemove", (e)=> {draw(e);});
cnv.addEventListener("touchmove", (e)=> {draw(e);}, {passive:false});


// ******** Util functions ***********
//Set the byte in the BMP image array corresponding to a pixel when the pixel is painted
function setPixel(arrX,arrY) {
  var x= arrX >> 3; // Entire division floor( pxX/8 )
  var bitX= arrX & 0b111; // pxX % 8
  
  BMP_img[9*arrY + x] = (color=="white"? BMP_img[9*arrY + x] | (128>>bitX) : BMP_img[9*arrY + x] & (0xFF7F>>bitX) );
}

//Change color of pencil (black or white)
function change_color(col) {
  color = col;
  if(color=="white") {
    document.getElementById("white-frame").style.borderStyle = "solid";
    document.getElementById("black-frame").style.borderStyle = "none";
  } else {
    document.getElementById("white-frame").style.borderStyle = "none";
    document.getElementById("black-frame").style.borderStyle = "solid";
  }
}

//Paint a single pixel at (absX,absY) coordinates, where the coordinates are absolute (i.e., the window client coords.)
function paintPixel(absX,absY) {
  var x= absX-X0;
  var y= absY-Y0;
  
  x -= x%3; y -= y%3;
  
  ctx.fillStyle= color;
  ctx.fillRect(x,y, 3,3); //Paint the pixel at (x,y);
  
  setPixel(x/3, y/3); //Set the corresponding pixel in the BMP_img array
}

//Draw while dragging the cursor (if you are in draw-mode)
function draw(e) {
  if(cnv_pressed) {
    if(e.type=="touchmove") e.preventDefault();
    
    var x= e.type=="mousemove"? e.clientX : e.touches[0].clientX;
    var y= e.type=="mousemove"? e.clientY : e.touches[0].clientY;
    
    paintPixel(x,y);
  }
}

//Start the draw-mode
function start_draw() {
  cnv_pressed= true;
}

//End the draw-mode
function end_draw() {
  cnv_pressed= false;
}

//Toggle between text and image interface
function toggle_interface() {
  var img_intrfc= document.getElementById("img-interface");
  var txt_intrfc= document.getElementById("text-interface");
  
  if(txt_intrfc.style.display=="none") {
    //Show text interface
    txt_intrfc.style.display= "flex";
    img_intrfc.style.display= "none";
  }
  else {
    //Show img interface
    txt_intrfc.style.display= "none";
    img_intrfc.style.display= "flex";
    
    X0= cnv.getBoundingClientRect().left;
    Y0= cnv.getBoundingClientRect().top;
  }
}

//Show the BMP array in console
function show_BMP() {
  var str="";
  for(i=0; i<H/3; i++) {
    for(j=0; j<W/24; j++) str+= BMP_img[9*i+j]+" ";
    console.log(str);
    str="";
  }
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

const uint8_t width = 70;
const uint8_t height = 40;
const uint8_t xOffset = 30; // = (132-w)/2 : X coord. of the left-upper corner
const uint8_t yOffset = 24; // = (64-h)/2 : Y coord. of the left-upper corner

uint8_t BMP_img[height * ((width>>3) + ((width & 7)==0? 0:1))]; //Array for the BMP image


void decodeJSON(String str, uint8_t* arr) {
  int16_t startIndex =1; //Start at 1 to eliminate the '['
  int16_t endIndex= str.indexOf(',');
  size_t i=0;
  
  while(endIndex != -1) {
    *(arr+i) = (uint8_t) str.substring(startIndex,endIndex).toInt();
    startIndex = endIndex+1;
    endIndex = str.indexOf(',', startIndex);
    i++;
  }

  //Last number
  *(arr+i) = (uint8_t) str.substring(startIndex, str.length()-1).toInt();
}


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


// Manejar la solicitud POST
void handleImg() {
    if (server.args() > 0) {
        // Obtiene el contenido de la solicitud
        decodeJSON(server.arg(0), BMP_img); // Primer argumento

        u8g2.clearBuffer();
        u8g2.drawBitmap(xOffset, yOffset, (width>>3)+ (width &7 == 0? 0:1), height, (const uint8_t*) BMP_img);
        u8g2.sendBuffer();

        server.send(200, "text/plain", "Datos recibidos");
    }
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
    server.on("/send_img", handleImg);
    
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
