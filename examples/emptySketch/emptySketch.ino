#define WIFI_NAME ""
#define WIFI_PASSWORD ""
#define DEVICE_ID 201
#define DEVICE_NAME ""
#define TOKEN ""

#include <ArduinoHttpClient.h>
#include <RemoteMe.h>
#include <ESP8266WiFi.h>

#include <ESP8266WiFiMulti.h>


ESP8266WiFiMulti WiFiMulti;
RemoteMe& remoteMe= RemoteMe::getInstance(TOKEN,DEVICE_ID);

void onUserSyncMessage(uint16_t senderDeviceId, uint16_t dataSize, uint8_t* data, uint16_t &returnDataSize, uint8_t *&returnData) {
  
}

void onUserMessage(uint16_t senderDeviceId, uint16_t dataSize, uint8_t *data) {
  

}

void setup() {

  
  WiFiMulti.addAP(WIFI_NAME, WIFI_PASSWORD);
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }


  remoteMe.setUserMessageListener(onUserMessage);
  remoteMe.setUserSyncMessageListener(onUserSyncMessage);

  remoteMe.setupTwoWayCommunication();

  remoteMe.sendRegisterDeviceMessage(DEVICE_NAME);  

}




void loop() {

  remoteMe.loop();
  
  

}
