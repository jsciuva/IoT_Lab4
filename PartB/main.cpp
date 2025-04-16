#include <Arduino.h>
#include <BLEDevice.h>
#include "SparkFunLSM6DSO.h"
#include "Wire.h"


#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

float threshold = 30.0; // default rad/s of threshold of a step
int stepCount = 0;
float filter = 15; // used for calibration to filter out non-steps


LSM6DSO myIMU; //Default constructor is I2C, addr 0x6B
BLECharacteristic *pCharacteristic;

void setup() {


  Serial.begin(9600);
  delay(1000); 
  Serial.println("Starting BLE work!");
 
  BLEDevice::init("SDSUCS");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
                                  

 pCharacteristic->setValue(stepCount);
 pService->start();
 BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
 pAdvertising->addServiceUUID(SERVICE_UUID);
 pAdvertising->setScanResponse(true);
 pAdvertising->setMinPreferred(0x0); 
 pAdvertising->setMinPreferred(0x12);
 BLEDevice::startAdvertising();
 Serial.println("Characteristic defined! Now you can read it in your phone!");
  
  Wire.begin();
  delay(10);
  if( myIMU.begin() )
    Serial.println("Ready.");
  else { 
    Serial.println("Could not connect to IMU.");
    Serial.println("Freezing");
  }

  if( myIMU.initialize(BASIC_SETTINGS) )
    Serial.println("Loaded Settings.");
  
  // calibrate to find threshold amount
  Serial.println("Calibrating...");
  Serial.println("take some steps");
  float minRadPerSec = 100; // don't exceed this val for threshold
  // calibrate for about 10 sec
  for (int i = 0; i < 30; i++) {
    float maxReading = 0; // a step is at least 15 rad/s
    // take maximum reading in 300 ms period
    for (int j = 0; j < 15; j++) {
      float reading = abs(myIMU.readFloatGyroY());
      if (reading > maxReading && reading > filter) {
        maxReading = reading;
      }
      delay(20);
    }
    // take the minimum of the peaks as the threshold
    if (maxReading < minRadPerSec && maxReading != 0) {
      minRadPerSec = maxReading;
    }
    delay(20);
  }
  Serial.println("Finished calibration");
  threshold = minRadPerSec * 0.8; // Reduce min slightly
  Serial.println("Threshold: ");
  Serial.println(threshold);
}


void loop()
{

  // detect a step
  if (myIMU.readFloatGyroY() > threshold) {
    stepCount++;
    Serial.println("Step count: ");
    Serial.println(stepCount);
    // send data to phone over BLE
    String str = "Steps: " + String(stepCount);
    pCharacteristic->setValue(str.c_str());
    pCharacteristic->notify();
    delay(300); // delay to avoid multiple counts per step
  }
  delay(20); // sample every 20 miliseconds
}