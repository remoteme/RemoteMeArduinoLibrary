// 
// 
// 

#include "RemoteMeWebsocketConnector.h"

RemoteMeWebsocketConnector*  RemoteMeWebsocketConnector::thiz;

RemoteMeWebsocketConnector::RemoteMeWebsocketConnector(void) {
	RemoteMeWebsocketConnector::thiz = this;
}
String  RemoteMeWebsocketConnector::getIp() {
	return WiFi.localIP().toString();
}


	void RemoteMeWebsocketConnector::send(uint8_t * payload,uint16_t size ) {
		Serial.println("sending message");
		if (webSocketConnected) {
			Serial.println("sending message XXX");
			webSocket->sendBIN(payload, size);
		}
	
	}



	bool RemoteMeWebsocketConnector::isWebsocketConnected(){
		if (!webSocketConnected){
			//Serial.println("webSocketConnected is false");
			return false;
		}
		else {
			return true;
		}
	
		
	}



	void RemoteMeWebsocketConnector::ping() {
		if (webSocketConnected) {
			if (!webSocket->sendPing()) {
				webSocketConnected = false;
			}
		}
	}

	
	void RemoteMeWebsocketConnector::waitForConnection() {
		static unsigned long lastTimePing = 0;

		if (lastTimePing + PING_SEND < deltaMillis() && isWebsocketConnected()) {
			ping();
			lastTimePing = deltaMillis();
		}
		if (!isWebsocketConnected())  {
		
			Serial.println("not connected or didnt got ping ");
			bool continousRestarting = false;
				
			webSocketConnected = false;
			if (webSocket != nullptr) {
				webSocket->disconnect();
				delete webSocket;
			}
			webSocket = new WebSocketsClient();

			char* buf = new char[20];
			snprintf(buf, 20, "/api/ws/v1/%d", getDeviceId());

			webSocket->begin(REMOTEME_HOST, REMOTEME_HTTP_PORT, buf, "as");
			webSocket->setAuthorization("token", getToken()); // HTTP Basic Authorization

			webSocket->setReconnectInterval(500);
			webSocket->onEvent(RemoteMeWebsocketConnector::webSocketEvent);
			lastTimePingReceived = deltaMillis();
			while (!isWebsocketConnected()) {
				webSocket->loop();
				Serial.println("connecting websocket");
				delay(100);
			}
			Serial.println("connected");
			sendVariableObserveMessage();
		}
		
		
	}

	void RemoteMeWebsocketConnector::webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
		static bool zm = true;

		switch (type) {
		case WStype_DISCONNECTED:
			//Serial.print("disConnected web socket");
			RemoteMeWebsocketConnector::thiz->webSocketConnected = false;
			break;
		case WStype_CONNECTED:
			Serial.println("connected websocket");
			RemoteMeWebsocketConnector::thiz->webSocketConnected = true;
			RemoteMeWebsocketConnector::thiz->webSocket->setReconnectInterval(20000);

			break;
		case WStype_TEXT:

			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			if (length > 0) {
				RemoteMeWebsocketConnector::thiz->processMessage(payload);
			}

		}


	}

	void RemoteMeWebsocketConnector::loop() {
		if (webSocket!=nullptr) {
			webSocket->loop();
		}
	}
	void RemoteMeWebsocketConnector::disconnect() {
		
		if (webSocket != nullptr) {
			webSocket->disconnect();
			delete webSocket;
			webSocket = nullptr;
		}
	}
	

	
