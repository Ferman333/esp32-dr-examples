/**********
  WiFi server + GPS
*********/

//For the Wifi connection
#include "credentials.h" //File with custom network credentials
#include <WiFi.h>
#include <WebServer.h>

// Define the RX and TX pins for Serial 1
#define RXD 20
#define TXD 21

#define GPS_BAUD 9600

// Create an instance of the HardwareSerial class for Serial 1
HardwareSerial gpsSerial(1);
String dataGPS ="No data (initializing)";


//Server
WebServer server(80);

//Times
int stopTime=-1000;


/*
Handle functions
*/
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<link rel=\"icon\" href=\"data:,\">";
  html += "<style>";
  html += "html { font-family: Helvetica; text-align: center; background: #f5f7fa; margin: 0; padding: 20px; }";
  html += "body { max-width: 600px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }";
  html += "h1 { color: #333; font-size: 28px; margin-bottom: 20px; }";
  html += "h2 { color: #444; font-size: 24px; margin-bottom: 10px; }";
  html += "p { color: #555; font-size: 18px; margin: 10px 0; }";
  //html += ".button { background: #4CAF50; border: none; color: white; padding: 12px 24px; text-decoration: none; font-size: 20px; border-radius: 8px; cursor: pointer; transition: background 0.2s ease; display: inline-block; width: 120px; box-sizing: border-box; }";
  html += "</style></head>";
  html += "<body><h1>ESP32 GPS Web Server</h1>";

  //Show GPS data
  html+= "<h2>GPS data:</h2>";
  html+= "<p>"+ dataGPS+ " Milliseconds: "+ stopTime +"</p>";
  html+= "<p>Satellites: "+ String(gpsSerial.available()) +"</p>";

  html+= "</body></html>";
  
  server.send(200, "text/html", html);
}


void setup(){
  // Serial Monitor
  Serial.begin(115200);
  
  // Start Serial 2 with the defined RX and TX pins and a baud rate of 9600
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD, TXD);
  Serial.println("Serial 1 started at 9600 baud rate");
  
  //Create access point
  Serial.println("Creating Access Point");
  WiFi.softAP(ssid, password);
  Serial.println("Access point created. Connect to the IP "+ WiFi.softAPIP().toString());
  
  //Configure events
  server.on("/", handleRoot);

  // Start the web server
  server.begin();
  Serial.println("HTTP server started");
}



void loop(){
  // Handle incoming client requests
  server.handleClient();

  if(gpsSerial.available() > 0 && (millis() >= stopTime+1000)){ //Wait a second after the stopTime
    // get the byte data from the GPS
    char gpsData = gpsSerial.read();
    dataGPS= gpsData;
    Serial.print(gpsData);
  } else {
    //Wait 1 second for the GPS signal
    //dataGPS="Not satellites found";
    stopTime= millis();
  }
  
  Serial.println("-------------------------------");
}
