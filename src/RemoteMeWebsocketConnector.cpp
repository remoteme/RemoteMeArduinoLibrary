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
		if (webSocketConnected) {
			webSocket->sendBIN(payload, size);
		}
	}

	

	void RemoteMeWebsocketConnector::ping() {
		if (webSocketConnected) {
			if (!webSocket->sendPing()) {
				webSocketConnected = false;
			}
			else {
				
				uint8_t *buffer = (uint8_t*)malloc(4);
				buffer[0] = 0;
				buffer[1] = 0;
				buffer[2] = 0;
				buffer[3] = 0;
				send(buffer, 4);
				free(buffer);
				DEBUG_REMOTEME("[RMM] ping send\n");
			}
		}
	}

	
	void RemoteMeWebsocketConnector::waitForConnection() {
		static unsigned long lastTimePing = 0;

		if (lastTimePing + PING_SEND < deltaMillis() && webSocketConnected) {
			ping();
			lastTimePing = deltaMillis();
		}
		if (!webSocketConnected || (lastTimePingReceived + PING_RECEIVE_TIMEOUT < deltaMillis()))  {
		
			DEBUG_REMOTEME("[RMM] not connected or didnt got ping webSocketConnected: %d last time ping received \n",webSocketConnected,(lastTimePingReceived + PING_RECEIVE_TIMEOUT < deltaMillis()));
			
	
				
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
			lastTimePingReceived = deltaMillis() + PING_RECEIVE_TIMEOUT;
			int continousReconnect = 0;
			while (!webSocketConnected) {
				if (continousReconnect++>3000){//5min
					DEBUG_REMOTEME("[RMM] restarting\n");
					ESP.restart();
				}
				webSocket->loop();
				DEBUG_REMOTEME("[RMM] attemp connect\n");
				delay(100);
			}
			DEBUG_REMOTEME("[RMM] connected\n");
			sendVariableObserveMessage();
		}
		
		
	}

	void RemoteMeWebsocketConnector::webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
		static bool zm = true;

		switch (type) {
		case WStype_DISCONNECTED:
			DEBUG_REMOTEME("[RMM] disconnected web socket\n");
			RemoteMeWebsocketConnector::thiz->webSocketConnected = false;
			break;
		case WStype_CONNECTED:
			DEBUG_REMOTEME("[RMM] connected websocket\n");
			RemoteMeWebsocketConnector::thiz->webSocketConnected = true;
			RemoteMeWebsocketConnector::thiz->webSocket->setReconnectInterval(20000);

			break;
		case WStype_TEXT:

			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			if (length > 0) {
				if ((payload[0]==0)&&(payload[1]==0)){
					DEBUG_REMOTEME("[RMM] ping received\n");
					RemoteMeWebsocketConnector::thiz->lastTimePingReceived = RemoteMeWebsocketConnector::thiz->deltaMillis();
				}else{
					RemoteMeWebsocketConnector::thiz->processMessage(payload);
				}
				
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
	

	
