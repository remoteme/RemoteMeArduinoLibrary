// 
// 
// 

#include "RemoteMeSocketConnector.h"

String  RemoteMeSocketConnector::getIp() {
	return WiFi.localIP().toString();
}


	void RemoteMeSocketConnector::send(uint8_t * payload,uint16_t size ) {
		if (socketConnected) {
			DEBUG_REMOTEME("[RMM] sending message\n");
			wifiClient->write((unsigned char*)payload, size);
		}
	
	}



	bool RemoteMeSocketConnector::isSocketConnected(){
		if (!socketConnected){
			return false;
		}
		if (this->wifiClient==nullptr){
			return false;
		}
		return this->wifiClient->connected();
		
	}



	void RemoteMeSocketConnector::ping() {
		if (socketConnected) {
			uint8_t *buffer = (uint8_t*)malloc(4);
			buffer[0] = 0;
			buffer[1] = 0;
			buffer[2] = 0;
			buffer[3] = 0;
			wifiClient->write((unsigned char*)buffer, 4);
			DEBUG_REMOTEME("[RMM] ping send\n");
				
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
				
			DEBUG_REMOTEME("[RMM] not connected or didnt got ping \n");
			int continousRestarting = 0;
				
			socketConnected = false;
			while (!isSocketConnected()) {
				if (wifiClient != nullptr) {
					wifiClient->stop();
					delete wifiClient;
				}
				
				if (continousRestarting>0){
					delay(100);
				}
				continousRestarting++;
				if (continousRestarting>300){
					DEBUG_REMOTEME("[RMM] Restarting esp\n");
					ESP.restart();
				}
				
				wifiClient = new WiFiClient();
				DEBUG_REMOTEME("[RMM] connecting ...\n");
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

					DEBUG_REMOTEME("[RMM] sending variables\n");

					
					sendVariableObserveMessage();
					
					DEBUG_REMOTEME("[RMM] connected\n");
					free(buffer);
				}
				else {
					DEBUG_REMOTEME("[RMM]not connected\n");
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
				DEBUG_REMOTEME("[RMM] ping received\n");
				return;
			}

			uint16_t bufferPos = 0;
			buffer = (uint8_t*)malloc(size + 4);
			RemoteMeMessagesUtils::putUint16(buffer, bufferPos, messageId);
			RemoteMeMessagesUtils::putUint16(buffer, bufferPos, size);
			unsigned long time = deltaMillis();

			while (wifiClient->available() <size) {
				if (deltaMillis()<time + 3000) {//timeout
					DEBUG_REMOTEME("[RMM] timeout readoing rest of message\n");
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
	

	
