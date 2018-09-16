
#define WIFI_NAME ""
#define WIFI_PASSWORD ""
#define DEVICE_ID 205
#define DEVICE_NAME "my arduino "
#define TOKEN ""





#include "Arduino.h"
#include <RemoteMe.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>



RemoteMe& remoteMe = RemoteMe::getInstance(TOKEN, DEVICE_ID);

ESP8266WiFiMulti WiFiMulti;
void setup()
{
	Serial.begin(9600);

	Serial.println("starting");

	WiFiMulti.addAP(WIFI_NAME, WIFI_PASSWORD);
	while (WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}


	remoteMe.setUserMessageListener(onUserMessage);

	remoteMe.setupTwoWayCommunication();
	remoteMe.sendRegisterDeviceMessage(DEVICE_NAME);
	//remoteMe.setupDirectConnections();
	
}





void onUserMessage(uint16_t senderDeviceId, uint16_t dataSize, uint8_t *data) {
	Serial.print("on user message size ");
	Serial.println(dataSize);

	String message = "";
	uint16_t pos = 0;
	for (int i = 0; i < dataSize; i++) {
		Serial.print(RemoteMeMessagesUtils::getUint8(data, pos));
		Serial.print(" ");
	}
	Serial.println("");
}

void loop()
{
	remoteMe.loop();
	

}

