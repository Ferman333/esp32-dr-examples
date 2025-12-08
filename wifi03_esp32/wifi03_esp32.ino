/* Programa para controlar el led de un ESP32 mediante Wifi, pero usando la biblioteca "esp_http_server.h" de ESP-IDF*/
#include <WiFi.h>
#include <esp_http_server.h>
#include "credentials.h" //File with ssid and password WiFi credentials

#define LED 8 //In Esp32-C3, Led is at GPIO 8
#define inverted true //Inverted logic for the LED (ex. C3 model). "true" to enable, "false" to disable

String state = "off"; //State of the led
uint8_t lvl= 255; //Bright level of the led
httpd_handle_t server = NULL; //Server object

const char* html PROGMEM= R"rawliteral(
<html>
<head>
<meta charset="UTF-8">
<title>LED controller</title>
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
  font-size: 32px;
  border: 0px;
  cursor: pointer;
}


.buttons {
  display: flex;
  justify-content: center;
  flex-direction: column;
  align-items: center;
  gap: 1vw;
}


.panel {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 40px;
}


/* Sizes for small screens */
@media (max-width: 1080px) {

button {
  width: 80px;
  height: 80px;
  font-size: 32px;
  border: 0px;
  cursor: pointer;
}

.buttons {
  display: flex;
  justify-content: center;
  flex-direction: column;
  align-items: center;
  gap: 10px;
}

}


.button-on {
  background: radial-gradient(circle, rgb(0, 255, 44) 70%, rgb(255, 255, 64));
}


.button-off {
  background: radial-gradient(circle, rgb(255, 0, 0) 70%, transparent);
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
<p>Led control</p>

<div class="panel">
  <div class="buttons">
    <button id="boton01" onclick="press();" class="button-on">ON</button>
    <p id="led-status">Status: off</p>
  </div>
  <div class="buttons">
    <div class="bar-interval"></div>
    <p id="PWMlevel" style="margin-top:-0.5vw;"></p>
  </div>
</div>


<script>
var levels= [1]; //Levels for each control bar (range [0:1])
var pressed_bars= [];
var clicked_bars= [];

var pwm_lvl= document.getElementById("PWMlevel");
var btn01= document.getElementById("boton01");

/* *****
 AJAX functions
****** */

//Get data from server
function get_data() {
  var xhr=new XMLHttpRequest();
  xhr.open('POST', '/send_data', false);
  xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  
  xhr.onreadystatechange = function() {
    if(xhr.readyState === 4 && xhr.status === 200) {
      console.log( xhr.responseText );
      
      levels[0]= JSON.parse(xhr.responseText).lvl;
      change_buttons(JSON.parse(xhr.responseText).status);
      
    }
  };
  xhr.send();
}

//Press the On/Off led button
function press() {
  var xhr=new XMLHttpRequest();
  xhr.open('POST', '/turn_led', true);
  xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  
  xhr.onreadystatechange = function() {
    if(xhr.readyState === 4 && xhr.status === 200) {
      console.log( "Status: "+ xhr.responseText );
      change_buttons(xhr.responseText);
      
    } else if(xhr.status !== 200) { alert("Error: not response from server. Status: "+ xhr.status); }
  };
  
  xhr.send('lvl=' + encodeURIComponent(levels[0]));
}


//**************
// LISTENERS
//**************

//Actions when document is loaded
addEventListener("load", (e)=> {
  //Get data from server
  get_data();
  
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

//Change buttons according to LED state
function change_buttons(st) {
  document.getElementById("led-status").innerText= "Status: "+ st; //Change status label
  
  //Change button style
  if(st=="off") {
    btn01.classList.add("button-on");
    btn01.classList.remove("button-off");
    btn01.innerText ="ON";
  } else {
    btn01.classList.add("button-off");
    btn01.classList.remove("button-on");
    btn01.innerText ="OFF";
  }
}


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


//Handle root / request
static esp_err_t handle_root(httpd_req_t* req) {
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, html, strlen(html));
}


//Handle /send_data request
static esp_err_t handle_send(httpd_req_t* req) {
  httpd_resp_set_type(req, "text/plain");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*"); //Avoid CORS error
  
  String out= "{\"status\":\""+ state+ "\",\"lvl\":"+ String(1.0*lvl/255, 2)+ "}";
  return httpd_resp_send(req, out.c_str(), out.length());
}


//Handle /turn_led request
static esp_err_t handle_led(httpd_req_t* req) {
  char* buf= NULL;
  size_t buf_len= req->content_len; //Length of data received
  
  if(buf_len>0) {
    buf= (char*) malloc(buf_len+1);
    
    if(!buf) {
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    
    int r_len= httpd_req_recv(req, buf, buf_len); //Get request data
    if(r_len<0) {
      free(buf);
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    buf[r_len]='\0'; //End of string
    
    float level=1.0;
    sscanf(buf, "lvl=%f", &level); //Obtain level value
    lvl= (uint8_t)round(level*255);
    
    if(state=="off") {
      //Turn-on led
      #if inverted
      ledcWrite(LED, 255-lvl);
      #else
      ledcWrite(LED, lvl);
      #endif
      state="on";
    }
    else {
      //Turn-off led
      #if inverted
      ledcWrite(LED, 255);
      #else
      ledcWrite(LED, 0);
      #endif
      state="off";
    }
    
    free(buf);

    httpd_resp_set_type(req, "text/plain");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, state.c_str(), state.length());
  }
  else return ESP_FAIL;
}


//Config and init server
void start_server() {
  // Config object
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  /*
  config= {
        .task_priority = 5,
        .stack_size = 4096,
        .document_root = NULL,
        .max_open_sockets = 5,
        .max_uri_handlers = 2,
        .server_port = 80,
        .ctrl_port = 32768,
        .event_handler = NULL,
        .http_ctx = NULL,
    };
  */
  config.max_uri_handlers = 4;
  
  //URI handlers
  httpd_uri_t root_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = handle_root,
    .user_ctx = NULL
  };

  httpd_uri_t send_uri = {
    .uri = "/send_data",
    .method = HTTP_POST,
    .handler = handle_send,
    .user_ctx = NULL
  };

  httpd_uri_t led_uri = {
    .uri = "/turn_led",
    .method = HTTP_POST,
    .handler = handle_led,
    .user_ctx = NULL
  };

  //Start server
  if(httpd_start(&server, &config)==ESP_OK) {
    //Register URI's
    httpd_register_uri_handler(server, &root_uri);
    httpd_register_uri_handler(server, &send_uri);
    httpd_register_uri_handler(server, &led_uri);
  } else {
    Serial.println("Error: server couldn't initialize");
  }
}


void setup() {
  Serial.begin(115200);

  //Led
  ledcAttach(LED, 5000, 8);
  #if inverted
  ledcWrite(LED, 255); //Inverted logic!
  #endif
  
  //Connecting WiFi
  WiFi.begin(ssid,password);
  Serial.print("Conectando a WiFi...");
  while(WiFi.status()!=WL_CONNECTED) {
    delay(1000);  
    Serial.print(".");
  }
  Serial.println("");
  
  Serial.print("Conectado a WiFi en la IP: ");
  Serial.println(WiFi.localIP());
  
  start_server(); //Begin server
}

void loop() {
  // Nothing to do, all is managed by the server
}
