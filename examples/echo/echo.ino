#define WIFI_NAME ""
#define WIFI_PASSWORD ""
#define DEVICE_ID 201
#define DEVICE_NAME ""
#define TOKEN ""

#include <RemoteMe.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>


ESP8266WiFiMulti WiFiMulti;
RemoteMe& remoteMe= RemoteMe::getInstance(TOKEN,DEVICE_ID);

void onUserSyncMessage(uint16_t senderDeviceId, uint16_t dataSize, uint8_t* data, uint16_t &returnDataSize, uint8_t *&returnData) {
	Serial.print("on User Sync message");
	
	String ret = "Hello ASYNC ";
	
	returnDataSize = dataSize + ret.length()+1;


	returnData = (uint8_t*)malloc(returnDataSize+1);
	for (int i = 0; i < returnDataSize; i++) {
		returnData[i] = (i+1) * 3;
	}
	
	snprintf((char*)returnData, returnDataSize, "%s%s", ret.c_str(), data);

	returnDataSize--;//dont send terminate 0 at end of string
}

void onUserMessage(uint16_t senderDeviceId, uint16_t dataSize, uint8_t *data) {
	Serial.println("on User  message");

	String ret = "Hello ";

	uint16_t returnDataSize = dataSize + ret.length() + 1;
	
	uint8_t * returnData = (uint8_t*)malloc(returnDataSize);
	

	snprintf((char*)returnData, returnDataSize, "%s%s", ret.c_str(),(char*) data);
	remoteMe.sendUserMessage(senderDeviceId, returnData, returnDataSize-1);//-1 because  we dont want to send terminate 0 at end of stirng
	free(returnData);
	

}

void setup() {
	
	Serial.begin(9600);
	Serial.println("started");
	while (!Serial) {
	}
	WiFiMulti.addAP(WIFI_NAME, WIFI_PASSWORD);
	while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
	}
	Serial.println("Connected to wifi");
	Serial.println("Connected, IP address: ");
	Serial.println(WiFi.localIP());


	remoteMe.setUserMessageListener(onUserMessage);
	remoteMe.setUserSyncMessageListener(onUserSyncMessage);

	remoteMe.setupTwoWayCommunication();

	remoteMe.sendRegisterDeviceMessage(DEVICE_NAME);	
	Serial.println("setup end");
}




void loop() {
	static int i = 0;
	remoteMe.loop();
	
	if (i++ > 50000) {
		Serial.print(".");//show that something is going on
		i = 0;
	}

	

}
