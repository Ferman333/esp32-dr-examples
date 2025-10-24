/**
* Este programa crea un AP Wifi con un server para controlar un motor con un módulo L298N. Usa esto en un ESP32
*/

#include "credentials.h" //File with custom "ssid" and "password" for the AP
#include <WiFi.h>
#include <WebServer.h>

#define USE_PWM true //If true, the motor's velocity is controlled with PWM; if false, velocity is fixed to maximum and controlled as digital
#if USE_PWM
#define ENA 0 //Pin for ENA in L298N. If false, yous must use the jumper in ENA
#endif
#define IN1 4
#define IN2 5


// Create a web server object
WebServer server(80);

String html01= R"rawliteral(
<html>
<head>
<meta charset="UTF-8">
<title>Motor Driver</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body { background-color: #666;
  display: flex;
  justify-content: center;
  flex-direction: column;
  align-items: center;
  gap: 40px; }

p {
  color: white;
  font: normal normal 24px calligra, verdana;
}

button {
  width: 8vw;
  height: 8vw;
  background: radial-gradient(circle, rgb(0, 255, 44) 70%, rgb(255, 255, 64));
  border: 0px;
  cursor: pointer;
}

.up {
  display: inline-block;
  width: 100%;
  height: 100%;
  background-color: transparent;
  background-image: url('data:image/svg+xml;utf8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100"><polygon points="50,10 10,90 90,90" fill="darkblue" /></svg>');
}

.down {
  display: inline-block;
  width: 100%;
  height: 100%;
  background-color: transparent;
  background-image: url('data:image/svg+xml;utf8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100"><polygon points="10,20 90,20 50,100" fill="darkblue" /></svg>');
}

.panel {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 40px;
}

.buttons {
  display: flex;
  justify-content: center;
  flex-direction: column;
  align-items: center;
  gap: 1vw;
}

.bar-interval {
  width: 30px;
  height: 300px;
  padding-top: 0;
  background: linear-gradient(to bottom, rgb(128, 228, 228) 0%, rgb(33, 43, 187) 85%), lightblue;
  border-radius: 15px;
}

.circle-marker {
  position: relative;
  display: inline-block;
  left: -3px;
  height: 30px;
  width: 30px;
  background-color: rgba(0,0,0,0);
  border: 3px solid #fff;
  border-radius: 50%;
  
}
</style>
</head>

<body>
<p>Motor control</p>

<div class="panel">
  <div class="buttons">
    <button id="boton-up" onmousedown="run_motor('up');" ontouchstart="run_motor('up');" onmouseup="stop();" ontouchend="stop();"><i class="up"></i></button>
    <button id="boton-down" onmousedown="run_motor('down');" ontouchstart="run_motor('down');" onmouseup="stop();" ontouchend="stop();"><i class="down"></i></button>
  </div>
)rawliteral";

String htmlPWM_Bar= R"rawliteral(
  <div class="buttons">
    <div class="bar-interval"></div>
    <p id="PWMlevel" style="margin-top:-0.5vw;"></p>
  </div>
)rawliteral";

String html02= R"rawliteral(
</div>


<script>
var levels= [1]; //Levels for each control bar (range [0:1])
var pressed_bars= [];
var clicked_bars= [];

var pwm_lvl= document.getElementById("PWMlevel");

/* *****
 AJAX functions
****** */
function run_motor(dir) {
  var xhr=new XMLHttpRequest();
  xhr.open('POST', '/run', true);
  xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  
  xhr.onreadystatechange = function() {
    if(xhr.readyState === 4 && xhr.status === 200) {
      console.log( xhr.responseText );
    } else if(xhr.status !== 200) { alert("Error: not response from server. Status: "+ xhr.status); }
  };
  
  xhr.send('dir=' + encodeURIComponent(dir)+ '&pwm='+ encodeURIComponent(levels[0]));
}

function stop() {
  var xhr=new XMLHttpRequest();
  xhr.open('POST', '/stop', true);
  xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  
  xhr.onreadystatechange = function() {
    if(xhr.readyState === 4 && xhr.status === 200) {
      console.log( xhr.responseText );
    } else if(xhr.status !== 200) { alert("Error: not response from server. Status: "+ xhr.status); }
  };
  
  xhr.send('code=' + encodeURIComponent("0"));
}

//**************
// LISTENERS
//**************

//Actions when document is loaded
addEventListener("load", (e)=> {
  //Set position of circle markers for each level bar
  document.querySelectorAll(".bar-interval").forEach((i_bar,j) => {
      //When the circle marks are loaded, position them at their initial values
      var mark= document.createElement('span');
      mark.className= "circle-marker";
      i_bar.appendChild(mark);
      var d_circ= mark.getBoundingClientRect().width; //Circle diameter
      
      mark.style.top= ( (1-levels[j]) * (i_bar.offsetHeight - d_circ) )+ "px";
      pressed_bars[j]= false;
      clicked_bars[j]= false;
  });
  
  pwm_lvl.innerText=Math.round(levels[0]*100)+"%";
});

addEventListener("mousemove", (e) => {moving_cursor(e);} );

addEventListener("touchmove", (e) => {moving_cursor(e);}, {passive:false});

addEventListener("mouseup", unpress_bar);

addEventListener("touchend", unpress_bar);

//Listeners for all the interval bars
document.querySelectorAll('.bar-interval').forEach((i_bar, j) => {
  
  //With click at a point of the bar, set the corresponding level and move the mark circle
  i_bar.addEventListener('click', (e) => {
      
      clicked_bars[j]= true; //Click this bar
      var marker = e.target.querySelector('.circle-marker');
      //If marker is null (it happens when the user clicks over the mark), stop the function
      if(marker === null) { return; }
      
      //Move marker and set its corresponding variable
      move_marker(marker, e.clientY, j);
      
      clicked_bars[j]= false;
  });
  
  
  //If some bar is pressed (when is enabled because img and logo are selected), change its flag to true
  i_bar.addEventListener('mousedown', (e) => {
    pressed_bars[j] = true;
  });
  
  
  //Movility for touch devices, it screams when touch starts
  i_bar.addEventListener('touchstart', (e) => {
    pressed_bars[j] = true;
  });
  
});

/* ******
**** Functions
******* */

//Moves the marker in a pressed bar while moving the cursor
function moving_cursor(e) {
  //Detect the pressed bar
  var j= pressed_bars.findIndex((elem) => elem);
  if(j==-1) return; //Exit if no bar is pressed
  
  if(e.type=="touchmove") e.preventDefault(); //Prevent moving the screen while sliding a level-bar
  
  clicked_bars[j] = false; //Avoid click event in the level-bar if cursor moves
  
  var marker = document.querySelectorAll('.bar-interval')[j].querySelector('.circle-marker');
  
  //Move the mark and set the level of its variable
  if(e.type=="mousemove") move_marker(marker, e.clientY, j);
  else if(e.type=="touchmove") move_marker(marker, e.touches[0].clientY, j);
}

// Moves the circle marker within an interval bar, and sets the reletive level (lvl in [0:1]) according to its position
function move_marker(marker, y, j) {
    var d_circ= marker.getBoundingClientRect().width; //Diameter of circle
    var i_bar= marker.parentNode; //Interval bar
    var L= i_bar.offsetHeight - d_circ; //Length of bar
    
    //Top Y coordinate for the circle
    var markY = y - i_bar.getBoundingClientRect().top - 0.5*d_circ;
    markY= (markY<0)? 0: markY;
    markY= (markY>L)? L: markY;
    
    //Move the circle marker
    marker.style.top = markY + "px";
    
    //Set the level with the percentage
    if(j!=-1) levels[j]= 1-markY/L;
    
    pwm_lvl.innerText = Math.round(levels[j]*100)+"%";
}

//Unpress the pressed level-bar when touch/click ends
function unpress_bar() {
  var j= pressed_bars.findIndex((elem) => elem);
  if(j==-1) return;
  
  //Unpress the bar
  pressed_bars[j] = false;
}
</script>
</body>
</html>
)rawliteral";

String direction="";
String previousDir="";
//bool stopped=true;



void stop_motor() {
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 0);
  
  previousDir="";
}


void move_forward(float pwm_lvl) {
  //If PWM enabled, control velocity
  #if USE_PWM
  ledcWrite(ENA, (uint8_t) round(pwm_lvl*255));
  #endif
  //Stop the motor and wait if it's already running in back direction
  if(previousDir=="down") {
    stop_motor();
    delay(500);
  }
  //Forward motor
  digitalWrite(IN1, 1);
  digitalWrite(IN2, 0);
  
  previousDir="up";
}


void move_back(float pwm_lvl) {
  //If PWM enabled, control velocity
  #if USE_PWM
  ledcWrite(ENA, (uint8_t) round(pwm_lvl*255));
  #endif
  //Stop the motor and wait if it's already running in forward direction
  if(previousDir=="up") {
    stop_motor();
    delay(500);
  }
  //Back motor
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 1);
  
  previousDir="down";
}


void handleRoot() {
  server.send(200, "text/html", html01
             #if USE_PWM
             +htmlPWM_Bar
             #endif
             +html02);
}


void run_motor() {
  if(server.hasArg("dir")) {
    direction = server.arg("dir"); //Get motor's direction from the client
    direction.replace("\r", "");
    Serial.println("Direction: "+direction);
    
    float pwm=1;
    if(server.hasArg("pwm")) {
      pwm = server.arg("pwm").toFloat(); //Get PWM from the client
    }
    Serial.println("PWM velocity: "+ String(pwm*100,0) +"%");
    
    //Move the motor
    if(direction=="up") move_forward(pwm);
    else if(direction=="down") move_back(pwm);
    
    server.send(200, "text/plain", "Success! Motor moving ("+ direction+ ")." );
  }
}


void setup() {
  Serial.begin(9600);
  
  #if USE_PWM
  // Configure PWM
  ledcAttach(ENA, 5000, 8); // ledcAttach(pin_PWM, freq_in_Hz, resolution_in_bits)
  #endif
  //Configure motor controls
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  
  //Begin Wifi AP
  Serial.println("Creating Access Point");
  WiFi.softAP(ssid, password);
  Serial.println("Access point created. Connect to the IP "+ WiFi.softAPIP().toString());
  
  // Set up the web server to handle different routes
  server.on("/", handleRoot);
  server.on("/run", run_motor);
  server.on("/stop", []() { stop_motor(); server.send(200, "text/plain", "Success! Motor stopped."); });
  
  // Start the web server
  server.begin();
  Serial.println("HTTP server started");
}


void loop() {
  server.handleClient();
}
