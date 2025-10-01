#define BTN 4
#define LED 8

uint8_t previous_st = LOW;
int counter = 0;

void setup() {
  // Button input
  pinMode(BTN, INPUT);
  //Onboard led
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH); //Turn off LED (inverted logic)
  //Serial
  Serial.begin(115200);
  Serial.print("Counter: ");
  Serial.println(counter);
}

void loop() {
  if(previous_st==LOW && digitalRead(BTN)==HIGH) {
    previous_st=HIGH;
    counter +=1;
    //Turn on led
    digitalWrite(LED, LOW); //INVERTED LOGIC!
    //Print counter
    Serial.print("Counter: ");
    Serial.println(counter);
  } else if(previous_st==HIGH && digitalRead(BTN)==LOW) {
    previous_st=LOW;
    digitalWrite(LED, HIGH); //INVERTED LOGIC!
  }

}
