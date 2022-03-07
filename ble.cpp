#include <M5Atom.h>
#include "ble.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID "D9E35DB6-4C1A-4E64-A5DC-10DCC6DCA9BC"
#define CHARACTERISTIC_UUID "AB81B493-DD72-48EB-87D3-B8B7CC87F1D6"

ble::ble() {
  Serial.println("BLE start");
  BLEDevice::init("M5AtomEcho");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(this);
  BLEService *pService;
  pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE |
                                         BLECharacteristic::PROPERTY_NOTIFY |
                                         BLECharacteristic::PROPERTY_INDICATE
                                       );
  pCharacteristic->setCallbacks(this);
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setMinInterval(1000);
  pAdvertising->setMaxInterval(1100);
  
  this->setManufactureData(0);

  pAdvertising->start();
  Serial.println("start Advertise");
}

ble::~ble() {
  
}

void ble::setManufactureData(const unsigned char value)
{
  this->manufactureData = value;
  Serial.print("set manufacture data: ");
  Serial.println(this->manufactureData);

  std::string manufactureData = "";
  manufactureData += (char)0xFF; // manufactureID
  manufactureData += (char)0xFF; // manufactureID
  manufactureData += (char)0x00; // data
  manufactureData += this->manufactureData; // data
  BLEAdvertisementData advertisementData = BLEAdvertisementData();
  advertisementData.setManufacturerData(manufactureData);
  pAdvertising->setAdvertisementData(advertisementData);
}

void ble::onConnect(BLEServer* pServer) {
  Serial.print("#connect:");
  Serial.println(pServer->getConnId());
  this->connected = true;
};

void ble::onDisconnect(BLEServer* pServer) {
  Serial.println("#disconnect");
  this->connected = false;
}

void ble::onRead(BLECharacteristic *pCharacteristic) {
  Serial.println("#read");
  int value = (int)this->manufactureData;
  pCharacteristic->setValue(value); 
}

void ble::onWrite(BLECharacteristic *pCharacteristic) {
  Serial.println("#write");
  std::string value = pCharacteristic->getValue();
  Serial.println(value.c_str());
  memset(message, 0, sizeof(message));
  if ((sizeof(message) - 1) < value.length()) {
    memcpy(message, value.c_str(), sizeof(message) - 1);
  } else {
    sprintf(message, "%s", value.c_str());
  }
  this->messageReceived = true;  
}

bool ble::isReceiveMessage() {
  return this->messageReceived;
}

void ble::getMessage(char* buffer, uint8_t length) {
  if (strlen(message) >= length) {
    return;
  }

  strcpy(buffer, message);
  this->messageReceived = false;
}

void ble::onIndicate(BLECharacteristic *pCharacteristic) {
  Serial.println("#indicate");
}

bool ble::isConnected() {
  return this->connected;
}

void ble::notify(const char* message) {
  if (this->connected) {
    pCharacteristic->setValue(message);
    pCharacteristic->notify();
  }
}
