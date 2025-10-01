

//Pines de entrada
const int ch1=14, ch2=15, ch3=16;
//Pines de salida PWM
const int ou1=9, ou2=10, ou3=11;

//Valores de voltaje
float v1, v2, v3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(ch1,INPUT);
  pinMode(ch2,INPUT);
  pinMode(ch3,INPUT);

  pinMode(ou1,OUTPUT);
  pinMode(ou2,OUTPUT);
  pinMode(ou3,OUTPUT);

  analogWrite(ou1,50);
  analogWrite(ou2,150);
  analogWrite(ou3,220);

}





void loop() {
  // put your main code here, to run repeatedly:
  //Medir voltajes en las entradas
  v1=analogRead(ch1)*5.0/1023;
  v2=analogRead(ch2)*5.0/1023;
  v3=analogRead(ch3)*5.0/1023;
  //
  //
  //
  Serial.println( String(v1,2) );
  
  delayMicroseconds(10);
  //delay(200);
}
