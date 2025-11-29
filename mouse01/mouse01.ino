/* Programa para crear un Mouse por acelerómetro. (Por ahora sólo funciona en el modelo ESP32-S3)
*/
#include <USBHIDMouse.h> //Just available in models with USB-HDI, such as Esp32-S3
//#include <HID-Project.h> //HID-Project by Nico Hood
//#include <HID-Settings.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>


Adafruit_MPU6050 mpu;
USBHIDMouse mouse;

long previousTime = 0;
float dt= 10; //Milliseconds
float a[3]; //Accelerations
float a_pr[3]; //Previous accelerations
float v[3]; //Velocities
float v_pr[3]; //Previous velocities


void setup() {
  //Begin serial
  Serial.begin(115200);
  while(!Serial) delay(10);
  
  //Begin MPU-6050
  if(!mpu.begin(0x68)) {
    Serial.println("Not found MPU-6050 chip.");
    for(;;);
  }
  Serial.println("MPU-6050 started!");
  
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G); //Possible ranges: 2G, 4G, 8G, 16G
  mpu.setGyroRange(MPU6050_RANGE_500_DEG); //Possible ranges: 250deg, 500deg, 1000deg, 2000deg
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ); //Ranges: 5 Hz, 10 Hz, 21 Hz, 44 Hz, 94 Hz, 184 Hz, 260 Hz
  
  mouse.begin(); //Begin mouse
  
  v[0]=0; v[1]=0; v[2]=0; //Initial velocities at zero
  
  Serial.println("");
  delay(100);
}



void loop() {
  // Get sensors' values
  sensors_event_t a_s, gy, temp;
  mpu.getEvent(&a_s, &gy, &temp);
  
  a[0]= a_s.acceleration.x; a[1]= a_s.acceleration.y; a[2]= a_s.acceleration.z;
  a[1]-=9.81; //Gravitational accel. in Y
  for(uint8_t k=0; k<3; k++) v[k]+= a[k]*dt/1000; //Update velocities
  
  if(millis()-previousTime >= 1000) { //Show data after 1000 ms
    Serial.print("Aceleracion X: ");  Serial.print(a[0]);
    Serial.print(", Y: ");  Serial.print(a[1]);
    Serial.print(", Z: ");  Serial.print(a[2]);
    Serial.println(" m/s^2");
    
    Serial.print("Velocity X: ");  Serial.print(v[0]);
    Serial.print(", Y: ");  Serial.print(v[1]);
    Serial.print(", Z: ");  Serial.print(v[2]);
    Serial.println(" m/s");
    
    previousTime = millis(); //Update time
  }
  
  mouse.move(v[0]*dt/1000*100/2.54*800, v[1]*dt/1000*100/2.54*800);
  delay(dt);
}













