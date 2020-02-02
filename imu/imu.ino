#include <Arduino_LSM6DS3.h>

float thresh;

void setup() {
  thresh = 10.0;
  Serial.begin(9600);
  // attempt to start the IMU:
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU");
    // stop here if you can't access the IMU:
    while (true);
  }
}
 
void loop() {
  // values for acceleration and rotation:
  float xAcc, yAcc, zAcc;
  float xGyro, yGyro, zGyro;

  float pX, pY;
  int dirX, dirY;
  // check if the IMU is ready to read:
  if (IMU.accelerationAvailable() &&
    IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(xGyro, yGyro, zGyro);
    if(xGyro > pX + thresh && xGyro > 0){
      dirX = -1;
    }else if(xGyro < pX - thresh && xGyro < 0){
      dirX = 1;
    }else{
      dirX = 0;
    }
    if(yGyro > pY + thresh && yGyro > 0){
      dirY = -1;
    }else if(yGyro < pY - thresh && yGyro < 0){
      dirY = 1;
    }else{
      dirY = 0;
    }
    
    pX = xGyro;
    pY = yGyro;
    Serial.print("dirX = ");
    Serial.print(dirX);
    Serial.print("\t");
    Serial.print("dirY = ");
    Serial.println(dirY);
    
  }
}
