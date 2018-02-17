#define WIFI_NAME ""
#define WIFI_PASSWORD ""
#define DEVICE_ID 123
#define DEVICE_NAME "my Arduino"
#define TOKEN ""


#include <ArduinoHttpClient.h>
#include <RemoteMe.h>
#include <ESP8266WiFi.h>

#include <ESP8266WiFiMulti.h>


ESP8266WiFiMulti WiFiMulti;
RemoteMe& remoteMe= RemoteMe::getInstance(TOKEN,DEVICE_ID);


#define LED1     D5
#define LED2     D6   



void onUserMessage(uint16_t senderDeviceId, uint16_t dataSize, uint8_t *data) {
	Serial.println("on User  message");

  int ledPin;
  
	if (data[0]==0){
    ledPin=LED1;
	}else{
     ledPin=LED2;
	}
 
	 digitalWrite(ledPin,data[1]? HIGH:LOW);

}

void setup() {

	pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
	Serial.begin(9600);
	Serial.print("started");
	while (!Serial) {
	}
	WiFiMulti.addAP(WIFI_NAME, WIFI_PASSWORD);
	while (WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
		Serial.print(".");
	}
	Serial.print("Connected to wifi");
	Serial.print("Connected, IP address: ");
	Serial.println(WiFi.localIP());


	remoteMe.setUserMessageListener(onUserMessage);

	remoteMe.setupTwoWayCommunication();
	
	remoteMe.sendRegisterDeviceMessage(DEVICE_NAME);	
	Serial.print("setup end");
}




void loop() {
	static int i = 0;
	remoteMe.loop();
	
	if (i++ > 50000) {
		Serial.print(".");//show that something is going on
		i = 0;
	}
	

}
