#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <LiquidCrystal_I2C.h>

//MPU6050 object
Adafruit_MPU6050 mpu;
//LCD_I2 object (liquid crystal screen)
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  //while (!Serial)
  //  delay(10);

  // Initiate the LCD:
  lcd.init();
  lcd.backlight();
  
  // Try to initialize MPU sensor!
  if (!mpu.begin(0x68)) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G); //Possible ranges: 2G, 4G, 8G, 16G
  mpu.setGyroRange(MPU6050_RANGE_500_DEG); //Possible ranges: 250deg, 500deg, 1000deg, 2000deg
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ); //Ranges: 5 Hz, 10 Hz, 21 Hz, 44 Hz, 94 Hz, 184 Hz, 260 Hz

  Serial.println("");
  delay(100);
}



void loop() {
  // Get sensors' values
  sensors_event_t a, gy, temp;
  mpu.getEvent(&a, &gy, &temp);
  
  Serial.print("Aceleracion X: ");  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotacion X: ");  Serial.print(gy.gyro.x);
  Serial.print(", Y: ");  Serial.print(gy.gyro.y);
  Serial.print(", Z: ");  Serial.print(gy.gyro.z);
  Serial.println(" rad/s");
  
  Serial.print("Temperatura: "); Serial.print(temp.temperature); Serial.println(" degC");
  
  Serial.println("");
  
  //Write in the LCD
  lcd.setCursor(2,0);
  lcd.print("Temperatura:");
  lcd.setCursor(4,1);
  lcd.print(String(temp.temperature, 2)); //Write temperature
  lcd.write((char) 0xDF); //Degree symbol in Hitachi A-00 ROM
  lcd.print("C");
  
  delay(1000);
}
