#include <Arduino.h>
#include <BLEDevice.h>

#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define LED_PIN 2
 
class MyCallbacks: public BLECharacteristicCallbacks {
   void onWrite(BLECharacteristic *pCharacteristic) {
     std::string value = pCharacteristic->getValue();
 
     if (value.length() > 0) {
       Serial.println("*********");
       Serial.print("New value: ");
       for (int i = 0; i < value.length(); i++)
         Serial.print(value[i]);
 
       Serial.println();
       Serial.println("*********");
       
       if (value == "on") {
        digitalWrite(LED_PIN, HIGH);
       }
       if (value == "off") {
        digitalWrite(LED_PIN, LOW);
       }

     }
   }
};
 
void setup() {
 pinMode(LED_PIN, OUTPUT);

 Serial.begin(9600);
 delay(2500);
 
 Serial.println("1- Download and install an BLE scanner app in your phone");
 Serial.println("2- Scan for BLE devices in the app");
 Serial.println("3- Connect to SDSUCS");
 Serial.println("4- Write 'on' to turn led on, and 'off' to turn off");
 
 BLEDevice::init("SDSUCS");
 BLEServer *pServer = BLEDevice::createServer();
 
 BLEService *pService = pServer->createService(SERVICE_UUID);
 
 BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                        CHARACTERISTIC_UUID,
                                        BLECharacteristic::PROPERTY_READ |
                                        BLECharacteristic::PROPERTY_WRITE
                                      );
 
 pCharacteristic->setCallbacks(new MyCallbacks());
 
 pCharacteristic->setValue("Hello World");
 pService->start();
 
 BLEAdvertising *pAdvertising = pServer->getAdvertising();
 pAdvertising->start();
}
 
void loop() {
 delay(2000);
}