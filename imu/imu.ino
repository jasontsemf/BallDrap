#include <Arduino_LSM6DS3.h>
#include <MadgwickAHRS.h>

// initialize a Madgwick filter:
Madgwick filter;
// sensor's sample rate is fixed at 104 Hz:
const float sensorRate = 104.00;
 
// values for orientation:
float roll = 0.0;
float pitch = 0.0;
float heading = 0.0;

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
  // start the filter to run at the sample rate:
  filter.begin(sensorRate);
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
    // read accelerometer and gyrometer:
    IMU.readAcceleration(xAcc, yAcc, zAcc);
    IMU.readGyroscope(xGyro, yGyro, zGyro);

    // update the filter, which computes orientation:
    filter.updateIMU(xGyro, yGyro, zGyro, xAcc, yAcc, zAcc);
    
    // print the heading, pitch and roll
    roll = filter.getRoll();
    pitch = filter.getPitch();
    heading = filter.getYaw();

//    // print the filter's results:
//    Serial.print(heading);
//    Serial.print(",");
//    Serial.print(pitch);
//    Serial.print(",");
//    //x
//    Serial.println(roll);

    if(roll > 0 + thresh){
      dirX = -1;
    }else if(roll < 0 - thresh){
      dirX = 1;
    }else{
      dirX = 0;
    }
    if(pitch > 0 + thresh){
      dirY = -1;
    }else if(pitch < 0 - thresh){
      dirY = 1;
    }else{
      dirY = 0;
    }
    
//    if(xGyro > pX + thresh && xGyro > 0){
//      dirX = -1;
//    }else if(xGyro < pX - thresh && xGyro < 0){
//      dirX = 1;
//    }else{
//      dirX = 0;
//    }
//    if(yGyro > pY + thresh && yGyro > 0){
//      dirY = -1;
//    }else if(yGyro < pY - thresh && yGyro < 0){
//      dirY = 1;
//    }else{
//      dirY = 0;
//    }
//    
//    pX = xGyro;
//    pY = yGyro;
//    Serial.print(xGyro);
    Serial.print("\t");
    Serial.print("dirX = ");
    Serial.print(dirX);
    Serial.print("\t");
    Serial.print("dirY = ");
    Serial.println(dirY); 
  }
}
