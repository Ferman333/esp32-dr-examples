/**
* Este programa pretende medir la distancia a un cierto objeto usando un sensor de ultrasonido, y luego mostrarla en una pantalla LCD. Aún en construcción...
*/

#include <LiquidCrystal_I2C.h>

#define TRIG 12
#define ECHO 13
#define LED 2


LiquidCrystal_I2C lcd= LiquidCrystal_I2C(0x27, 16,2);
long t;
float d;

void setup() {
  // initialize serial communication:
  //Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  
  pinMode(LED, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  
}





void loop() {
  // establish variables for duration of the ping, and the distance result
  // in inches and centimeters:
  
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  //Shot the PING
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  
  // The same pin is used to read the signal from the PING))): a HIGH pulse
  // whose duration is the time (in microseconds) from the sending of the ping
  // to the reception of its echo off of an object.
  t = pulseIn(ECHO, HIGH, 1000000);
  //d = v*t/2 (in cm)
  d = 0.0343*t/2;
  
  
  lcd.setCursor(3,0);
  lcd.print("Distancia:");
  lcd.setCursor(3,1);
  lcd.print( String(d,2)+ " cm" );

  if(d<7) digitalWrite(LED, HIGH);
  else digitalWrite(LED, LOW);

  delay(500);
}
