#include <Wire.h>

void setup() {
  Wire.begin();  // Inicia el bus I2C
  Serial.begin(9600);  // Inicia la comunicación serial
  while (!Serial);  // Espera a que se inicie el monitor serial
  Serial.println("\nBuscando dispositivos I2C...");
}


void loop() {
  for (byte i = 1; i < 127; i++) {  // Scanea direcciones de 1 a 127
    Wire.beginTransmission(i);
    byte error = Wire.endTransmission();

    if (error == 0) {
      // Dispositivo encontrado
      Serial.print("Dispositivo encontrado en dirección I2C: 0x");
      Serial.println(i, HEX);  // Imprime la dirección en formato hexadecimal
    } else if (error == 4) {
      // Error desconocido
      Serial.print("Error desconocido en dirección: 0x");
      Serial.println(i, HEX);
    }
    delay(100);  // Retraso para mayor estabilidad
  }
  
  Serial.println("Escaneo completado.");
  while (1);  // Detiene el programa
}
