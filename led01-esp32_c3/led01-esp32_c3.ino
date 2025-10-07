/**
* Código de ejemplo del ESP-32 para hacer parpadear el Led on-board cada segundo. Sin embargo, esto aplica en otros controladores con built-in led (o en placas sin éste, pero conectando un led externo), sólo cambia el número de pin correspondiente al led en pinLed.

*/

const int pinLed = 8;

void setup()
{
  pinMode(pinLed, OUTPUT); // Configurar el pin del LED incorporado como salida
}

void loop()
{
  Serial.begin(115200);
  digitalWrite(pinLed, HIGH); // Encender el LED
  Serial.println("Led Off");
  delay(1000); // Esperar un segundo
  
  digitalWrite(pinLed, LOW); // Apagar el LED
  Serial.println("Led On");
  delay(1000); // Esperar un segundo
  
}
