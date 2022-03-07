#define BLE_MESSAEG_SIZE_MAX 65
#include <BLEUtils.h>

class BLEServer;
class BLECharacteristic;
class BLEAdvertising;
class ble: public BLECharacteristicCallbacks, public BLEServerCallbacks {
public:
  ble();
  ~ble();

  void onConnect(BLEServer* pServer);
  void onDisconnect(BLEServer* pServer);
  void onRead(BLECharacteristic *pCharacteristic);
  void onWrite(BLECharacteristic *pCharacteristic);
  void onIndicate(BLECharacteristic *pCharacteristic);

  void setManufactureData(const unsigned char value);

  bool isConnected();
  bool isReceiveMessage();
  void getMessage(char* buffer, uint8_t length);
  void notify(const char* message);

private:
  BLEAdvertising* pAdvertising = NULL;
  BLECharacteristic* pCharacteristic = NULL;
  char message[BLE_MESSAEG_SIZE_MAX];
  bool messageReceived = false;
  bool connected = false;
  unsigned char manufactureData = 0;
};
