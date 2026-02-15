/*

SETTING MODE 

ESP32 connect with the application with BLE in slave mode

lifecycle of the connection:

    setting BLE -> start BLE -> get first connection -> authentication - if password ok -> waits device request ESP Service  
                                                          |        |
                                                          --else----

  Bluetooth will be on waiting for other 

  ESP service  (one at time):
  - getting esp32 status (to define..)
  - change ESP32 password
  - change web server page
  - getting info 


*/

// UUID identify wich BLE service is used, in this case Nordic UART Service
#define SERVICE_UUID           "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID_RX "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID_TX "6e400003-b5a3-f393-e0a9-e50e24dcca9e"


BLECharacteristic *pTX;
BLECharacteristic *pRX;

bool deviceConnected = false;
bool deviceLogged=false;
String rxload="";

char *psw_bluetooth ="1234";



void loopCore0(void* p){
  Serial.begin(115200);
  setupBLE("ESPSERVER");
  // handle bluetooth connection
  while (true){
    commandHandler();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

  }
}

// server callback
class ESPServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      deviceLogged=false;
      Serial.println("[CORE 0] -> Device connected");
    };
    void onDisconnect(BLEServer* pServer) {
      // not connected -> not logged
      deviceConnected = false;
      deviceLogged=false;
      Serial.println("[CORE 0] -> Device disconnected");
      pServer->getAdvertising()->start(); //Reopen the pServer and wait for the connection.
    }
};

// when another device send a message to ESP, it save in the variable "rxload" 
class RXCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      String rxValue = String(value.c_str());

      if (rxValue.length() > 0) {
        rxload="";
        for (int i = 0; i < rxValue.length(); i++){
          rxload +=(char)rxValue[i];
        }
      }
    }
};


// in setup function
void setupBLE(String BLEName){
  // set server name and callback
  const char *ble_name=BLEName.c_str();
  BLEDevice::init(ble_name);

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ESPServerCallbacks());
  
  // create BLE service Nordic UART 
  BLEService *pService = pServer->createService(SERVICE_UUID); 
    // set characteristics
  pTX= pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pTX->addDescriptor(new BLE2902()); 

  
  pRX = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX,
    BLECharacteristic::PROPERTY_WRITE
  );

  pRX->setCallbacks(new RXCallbacks()); 

  
  pService->start();


  // starting server and service
  pServer->getAdvertising()->start();
  Serial.println("[CORE 0]  -> Waiting a client connection to notify...");
}


// send asynchronous msg to client 
bool a_send(String value){
  // maybe we need a buffer but at the moment is better using the string 
  const char*newValue=value.c_str();
  Serial.print("[CORE 0] -> ESP32: ");Serial.println(newValue);

  pTX->setValue(newValue);
  pTX->notify();

  return true;
}

// receive synchronous msg from client 
String s_recv(){
  // rxload will be initialited by the class RXCallback with "onWrite"
  while (rxload=="" || !deviceConnected){
    vTaskDelay(500 / portTICK_PERIOD_MS);

  }

  String out=String(rxload);
  out.trim();
  Serial.println("[CORE 0] -> DEVICE : " +out);
  rxload="";
  return out;
}


void commandHandler(){
  // the device will send a command first
  String cmd=s_recv();
  if (!deviceLogged){
    authBLEConnection();
  }else{
    // all services
  }
}

// in core 
void authBLEConnection(){
  bool ok=false;
  while (!ok){
    bool sent=a_send("Need a password to access");

    if (sent){
      String psw=s_recv();

      if (psw==String(psw_bluetooth)){ 
        ok=true;
        deviceLogged=true; 
      }
    }
    
  }

  a_send("User autenticated");
}