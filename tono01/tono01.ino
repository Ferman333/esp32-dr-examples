#define AUD 8

const int c[5]={131,262,523,1046,2093}; // Do (4 escalas)
const int e[5]={165,330,659,1319,2637}; // Mi
//const int a[5]={220,440,880,1760,3520}; // La



void setup() {
  // put your setup code here, to run once:

}





void loop() {
  // Tonos en el pin 8 (AUD)
  tone(AUD, e[2]);
  delay(400);
  noTone(AUD);
  //delay(100);

  tone(AUD, c[2]);
  delay(750);
  noTone(AUD);
  delay(150);
  
  

}
