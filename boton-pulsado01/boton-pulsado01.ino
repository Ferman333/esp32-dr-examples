/**
* Programa para contar el número de veces que se pulsó un botón.
* version: 1.0
*
* Este programa puede ejecutarse en cualquier placa de desarrollo que tenga un built-in led, lo que hace es aumentar en 1 un contador de veces pulsadas al apretar un botón conectado a la placa, encender el built-in led mientras esté presionado el botón, y apagarlo al soltar el botón. Si tu placa no tiene built-in led, puedes sustituirlo por un led externo conectado a otro pin.
*/


#define BTN 4 //GPIO for the button
#define LED 8 //Check your board, for ESP32-C3 Supermini the led is at GPIO 8

uint8_t previous_st = LOW; //Button's state, LOW if not pressed (the GPIO pin logic state is 0 or GND), and HIGH if pressed (logic state 1 or VCC)
int counter = 0; // Counter of how many times the button has been pulsed



void setup() {
  // Button input
  pinMode(BTN, INPUT);
  //Onboard led
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH); //Turn off LED (inverted logic in ESP32-C3 supermini)
  
  //Serial
  Serial.begin(115200);
  Serial.print("Counter: ");
  Serial.println(counter);
}



void loop() {
  // Click-pressed
  if(previous_st==LOW && digitalRead(BTN)==HIGH) {
    previous_st=HIGH;
    counter +=1;
    //Turn on led
    digitalWrite(LED, LOW); //INVERTED LOGIC!
    //Print counter
    Serial.print("Counter: ");
    Serial.println(counter);
  }
  // Click-released
  else if(previous_st==HIGH && digitalRead(BTN)==LOW) {
    previous_st=LOW;
    //Turn off led
    digitalWrite(LED, HIGH); //INVERTED LOGIC!
  }

}
