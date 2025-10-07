/*********
 Conexión mediante AP (access point) WiFi para controlar el encendido de un Led desde el celular, usando un Esp32.
*********/

#include "credentials.h" //File with the custom credentials of your AP
#include <WiFi.h>
#include <WebServer.h>


// Assign led_pin variable to GPIO led pin (it works with inverted logic in the C3 model)
const int led_pin = 8; //Led pin. 8 in C3-supermini models, 31 in Ai-thinker CAM, and 4 for the flash led in the Ai-thinker CAM
String outputState = "OFF";

// Create a web server object
WebServer server(80);

// Function to handle turning GPIO on
void handleGPIOOn() {
  outputState = "ON";
  digitalWrite(led_pin, LOW);
  handleRoot();
}

// Function to handle turning GPIO off
void handleGPIOOff() {
  outputState = "OFF";
  digitalWrite(led_pin, HIGH);
  handleRoot();
}

// Function to handle the root URL and show the current state
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<link rel=\"icon\" href=\"data:,\">";
  html += "<style>";
  html += "html { font-family: Helvetica; text-align: center; background: #f5f7fa; margin: 0; padding: 20px; }";
  html += "body { max-width: 600px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }";
  html += "h1 { color: #333; font-size: 28px; margin-bottom: 20px; }";
  html += "p { color: #555; font-size: 18px; margin: 10px 0; }";
  html += ".button { background: #4CAF50; border: none; color: white; padding: 12px 24px; text-decoration: none; font-size: 20px; border-radius: 8px; cursor: pointer; transition: background 0.2s ease; display: inline-block; width: 120px; box-sizing: border-box; }";
  html += ".button:hover { background: #45a049; }";
  html += ".button2 { background: #555555; }";
  html += ".button2:hover { background: #666666; }";
  html += "</style></head>";
  html += "<body><h1>ESP32 Web Server</h1>";

  // Display GPIO controls
  html += "<p>GPIO - State " + outputState + "</p>";
  if (outputState == "ON") {
    html += "<p><a href=\"/off\"><button class=\"button button2\">OFF</button></a></p>";
  } else {
    html += "<p><a href=\"/on\"><button class=\"button\">ON</button></a></p>";
  }

  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  // Initialize the led_pin variable as OUTPUT
  pinMode(led_pin, OUTPUT);
  // Turn Off the onboard LED (inverted logic)
  digitalWrite(led_pin, HIGH);

  // Create AP
  //Serial.print("Connecting to ");
  //Serial.println(ssid);
  Serial.println("Creating Access Point");
  WiFi.softAP(ssid, password);
  Serial.println("Access point created. Connect to the IP "+ WiFi.softAPIP().toString());
  
  
  // Set up the web server to handle different routes
  server.on("/", handleRoot);
  server.on("/on", handleGPIOOn);
  server.on("/off", handleGPIOOff);

  // Start the web server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle incoming client requests
  server.handleClient();
}
