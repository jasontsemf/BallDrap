#include <SPI.h>
#include <WiFiNINA.h>
#include <Arduino_LSM6DS3.h>
#include <MadgwickAHRS.h>
#include "arduino_secrets.h"

#define yLedPin 2
#define bLedPin 3
#define dButton 4

// initialize a Madgwick filter:
Madgwick filter;
// sensor's sample rate is fixed at 104 Hz:
const float sensorRate = 104.00;
 
// values for orientation:
float roll = 0.0;
float pitch = 0.0;
float heading = 0.0;

float thresh;

bool yLed = false;
bool bLed = false;
int dButtonState = 0;

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
//char server[] = "www.google.com";    // name address for Google (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;
//char server[] = "10.18.241.229";
char server[] = "10.18.255.103";

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  thresh = 10.0;
  // attempt to start the IMU:
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU");
    // stop here if you can't access the IMU:
    while (true);
  }

  pinMode(yLedPin, OUTPUT);
  pinMode(bLedPin, OUTPUT);
  pinMode(dButton, INPUT_PULLUP);

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 3 seconds for connection:
    delay(5000);
  }

  digitalWrite(yLedPin, HIGH);
  
  Serial.println("Connected to wifi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:]
  if (client.connect(server, 8080)) {
    Serial.println("connected to server");
    client.println("n=Jason\n");
  }
  // start the filter to run at the sample rate:
  filter.begin(sensorRate);
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:

  // values for acceleration and rotation:
  float xAcc, yAcc, zAcc;
  float xGyro, yGyro, zGyro;

  float pX, pY;
  int dirX, dirY;
  
  while (client.connected()) {
    digitalWrite(bLedPin, HIGH);
    if(client.available()){
      char c = client.read();
      Serial.write(c);
    }
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
      
      if(roll > 0 + thresh){
        dirX = -1;
        client.println("l");
      }else if(roll < 0 - thresh){
        dirX = 1;
        client.println("r");
      }else{
        dirX = 0;
      }
      if(pitch > 0 + thresh){
        dirY = -1;
        client.println("d");
      }else if(pitch < 0 - thresh){
        dirY = 1;
        client.println("u");
      }else{
        dirY = 0;
      }

      dButtonState = digitalRead(dButton);
      if(dButtonState == LOW){
        client.println("x");
      }
      delay(30);
    }

  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    digitalWrite(bLedPin, LOW);
    client.stop();
    // do nothing forevermore:
    dButtonState = digitalRead(dButton);
    if(dButtonState == LOW){
      if (client.connect(server, 8080)) {
        Serial.println("connected to server");
        client.println("n=Jason\n");
      }
    }
  }
}



void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}
