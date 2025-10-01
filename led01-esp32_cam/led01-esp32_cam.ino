const int pinLed = 4;

void setup()
{
  pinMode(pinLed, OUTPUT); // Configurar el pin del LED incorporado como salida
}

void loop()
{
  Serial.begin(115200);
  digitalWrite(pinLed, HIGH); // Encender el LED
  Serial.println("Led On");
  delay(1000); // Esperar un segundo
  
  digitalWrite(pinLed, LOW); // Apagar el LED
  Serial.println("Led Off");
  delay(1000); // Esperar un segundo
  
}
