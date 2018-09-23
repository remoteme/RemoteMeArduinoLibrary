// 
// 
// 

#include "RemoteMeSocketConnector.h"

String  RemoteMeSocketConnector::getIp() {
	return WiFi.localIP().toString();
}


	void RemoteMeSocketConnector::send(uint8_t * payload,uint16_t size ) {
		Serial.println("sending message");
		if (socketConnected) {
			Serial.println("sending message XXX");
			wifiClient->write((unsigned char*)payload, size);
		}
	
	}



	bool RemoteMeSocketConnector::isSocketConnected(){
		if (!socketConnected){
			//Serial.println("socketConnected is false");
			return false;
		}
		if (this->wifiClient==nullptr){
			//Serial.println("wifi client is nullpointer");
			return false;
		}
		if (this->wifiClient->connected()){
			return true;
		}else{
			//Serial.println("wifi client not connected");
			return false;
		}
		
	}



	void RemoteMeSocketConnector::ping() {
		if (socketConnected) {
			uint8_t *buffer = (uint8_t*)malloc(4);
			buffer[0] = 0;
			buffer[1] = 0;
			buffer[2] = 0;
			buffer[3] = 0;
			wifiClient->write((unsigned char*)buffer, 4);
			//Serial.println("ping send");
				
			free(buffer);
		}
	}

	
	void RemoteMeSocketConnector::waitForConnection() {
		static unsigned long lastTimePing = 0;

		if (lastTimePing + PING_SEND < deltaMillis() && isSocketConnected()) {
			ping();
			lastTimePing = deltaMillis();
		}
		if (!isSocketConnected() || (lastTimePingReceived+ PING_RECEIVE_TIMEOUT < deltaMillis()))  {
				
			Serial.println("not connected or didnt got ping ");
			bool continousRestarting = false;
				
			socketConnected = false;
			while (!isSocketConnected()) {
				if (wifiClient != nullptr) {
					wifiClient->stop();
					delete wifiClient;
				}
				Serial.println(continousRestarting);
				if (continousRestarting){
					delay(500);
				}else{
					continousRestarting=true;
				}
					
				wifiClient = new WiFiClient();
				//Serial.println("connecting ...");
				int status=wifiClient->connect(REMOTEME_HOST, REMOTEME_SOCKET_PORT);
				if (status==1) {

					String tokenS = String(this->remoteMe->getToken());
					uint16_t sizeToSend = 2 + tokenS.length() + 1;
					uint8_t* buffer = (uint8_t*)malloc(sizeToSend);
					uint16_t pos = 0;
					RemoteMeMessagesUtils::putUint16(buffer, pos, remoteMe->getDeviceId());	
					RemoteMeMessagesUtils::putString(buffer, pos, tokenS);
					socketConnected = sizeToSend == wifiClient->write((unsigned char*)buffer, sizeToSend);
					lastTimePingReceived = deltaMillis() + 2 * PING_RECEIVE_TIMEOUT;

					Serial.println("sending variables");

					
					sendVariableObserveMessage();
					
					Serial.println("connected");
					free(buffer);
				}
				else {
						//Serial.printf("not connected");
						 
					//Serial.printf("WiFi.status():  %d\n",WiFi.status());
						
						//Serial.printf("wifiClient->status():  %d\n",wifiClient->status());
						
					
				}
			}
	
		}
		
		
	}

	

	void RemoteMeSocketConnector::loop() {
		if (wifiClient->available() >= 4) {
			uint8_t *buffer = (uint8_t*)malloc(4);
			wifiClient->read(buffer, 4);
			uint16_t pos = 0;
			uint16_t messageId = RemoteMeMessagesUtils::getUint16(buffer, pos);
			uint16_t size = RemoteMeMessagesUtils::getUint16(buffer, pos);
			free(buffer);

			if (messageId==0 && size == 0) {
				lastTimePingReceived = deltaMillis();
				//Serial.println("ping received");
				return;
			}

			uint16_t bufferPos = 0;
			buffer = (uint8_t*)malloc(size + 4);
			RemoteMeMessagesUtils::putUint16(buffer, bufferPos, messageId);
			RemoteMeMessagesUtils::putUint16(buffer, bufferPos, size);
			unsigned long time = deltaMillis();

			while (wifiClient->available() <size) {
				if (deltaMillis()<time + 3000) {//timeout
					//Serial.println("timout readoing rest message");
					return;
				}
			}

			wifiClient->read(&buffer[pos], size);
			processMessage(buffer);
			free(buffer);
		}
	}
	void RemoteMeSocketConnector::disconnect() {
		
		if (wifiClient != nullptr) {
			wifiClient->stop();
			wifiClient=nullptr;
		}
	}
	

	
