/**
* Programa para probar un módulo Sim800L para tarjetas SIM. La conexión entre el microcontrolador y el módulo funciona, pero no se pudo probar más debido a la falta de conexión para las redes 2G (ahora obsoletas).
*/

#define RXD 20
#define TXD 21

HardwareSerial simSerial(1);

void test_sim800_module()
{
  simSerial.println("AT"); //basic command
  updateSerial();
  Serial.println();
  simSerial.println("AT+CSQ"); //Chech signal strength (it must be greater than 5)
  updateSerial();
  simSerial.println("AT+CCID"); //Check the SIM id number
  updateSerial();
  simSerial.println("AT+CREG?"); //Check if you're registered in the network (1: home network, 5: roaming network)
  updateSerial();
  simSerial.println("ATI"); //get module's name and number
  updateSerial();
  simSerial.println("AT+COPS?"); //check if you're connected to the network
  updateSerial();
  simSerial.println("AT+CBC"); //get battery percentage
  updateSerial();
}



void updateSerial()
{
  delay(1000);
  while (Serial.available())
  {
    simSerial.write(Serial.read()); //Forward what Serial received to Software Serial Port
  }
  while (simSerial.available())
  {
    Serial.print(String((char)simSerial.read())); //Forward what Software Serial received to Serial Port
  }
}



void send_SMS()
{
  simSerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  simSerial.println("AT+CMGS=\"+CCxxxxxxxxxx\"");//change CC with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  simSerial.print("Test message"); //text content
  updateSerial();
Serial.println();
  Serial.println("Message Sent");
  simSerial.write(26);
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  simSerial.begin(115200, SERIAL_8N1, RXD, TXD); //simSerial.begin(9600);
  delay(3000);
  test_sim800_module();
  send_SMS();
}



void loop() {
  // put your main code here, to run repeatedly:
  updateSerial();
}
