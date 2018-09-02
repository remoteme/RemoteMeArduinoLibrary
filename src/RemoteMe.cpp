// 
// 
// 




#include "RemoteMe.h"
#include "Variables.h"


	RemoteMe::RemoteMe(char * token, uint16_t deviceId) {
		this->token = std::move(token);
		this->deviceId = deviceId;

	}

	void RemoteMe::webSocketServerEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
		static bool zm = true;

		switch (type) {
		case WStype_DISCONNECTED:
			
			break;
		case WStype_CONNECTED:
			
			break;
		case WStype_TEXT:

			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			if (length > 0) {
				RemoteMe::getInstance("", 0).processMessage(payload);
			}

		}
		
	}

	void RemoteMe::webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
		static bool zm = true;

		switch (type) {
		case WStype_DISCONNECTED:
			//Serial.print("disConnected web socket");
			RemoteMe::getInstance("", 0).webSocketConnected = false;
			break;
		case WStype_CONNECTED:
			RemoteMe::getInstance("", 0).webSocketConnected = true;
			RemoteMe::getInstance("", 0).webSocket->setReconnectInterval(20000);
			
			
			break;
		case WStype_TEXT:
			
			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			if (length > 0) {
				RemoteMe::getInstance("", 0).processMessage(payload);
			}
			
		}
		
		
	}


	void RemoteMe::processMessage(uint8_t *payload) {
		uint16_t pos = 0;
		RemoteMe& rm = RemoteMe::getInstance("", 0);

		RemotemeStructures::MessageType messageType = static_cast<RemotemeStructures::MessageType> (RemoteMeMessagesUtils::getUint16(payload, pos));
		uint16_t size = RemoteMeMessagesUtils::getUint16(payload, pos);
		if (messageType == RemotemeStructures::USER_MESSAGE) {
			
			RemotemeStructures::WSUserMessageSettings userMessageSettings = static_cast<RemotemeStructures::WSUserMessageSettings>(RemoteMeMessagesUtils::getUint8(payload, pos));
			uint16_t receiverDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);
			uint16_t senderDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);
			uint16_t messageId = RemoteMeMessagesUtils::getUint16(payload, pos);

			uint16_t dataSize = size - pos + 4;// -4 2 for suze 2 for bytes becasuse size is without this
			uint8_t* data = RemoteMeMessagesUtils::getArray(payload, pos, dataSize);
			if (rm.onUserMessage != nullptr) {
				rm.onUserMessage(senderDeviceId, dataSize, data);
			}
			free(data);
		}else if (messageType == RemotemeStructures::USER_SYNC_MESSAGE) {
			uint16_t receiverDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);
			uint16_t senderDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);

			uint64_t messageId = RemoteMeMessagesUtils::getInt64(payload, pos);

			uint16_t dataSize = size - pos + 4;
			uint8_t* data = RemoteMeMessagesUtils::getArray(payload, pos, dataSize);


			if (rm.onUserSyncMessage != nullptr) {
				uint16_t returnDataSize;
				uint8_t* returnData;

				rm.onUserSyncMessage(senderDeviceId, dataSize, data, returnDataSize, returnData);
				rm.sendSyncResponseMessage(messageId, returnDataSize, returnData);

				free(returnData);

			}
			free(data);

		} else if (messageType == RemotemeStructures::SYNC_MESSAGE) {
			int8_t type = RemoteMeMessagesUtils::getInt8(payload, pos);
			uint16_t receiverDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);	
			uint64_t messageId = RemoteMeMessagesUtils::getInt64(payload, pos);
			
			uint16_t dataSize = size - pos + 4;
			uint8_t* data = RemoteMeMessagesUtils::getArray(payload, pos, dataSize);

			if (type == RemotemeStructures::GET_WEBSOCKET_SERVER_LOCAL) {
				uint16_t posR = 0;
				uint16_t returnDataSize = 1 + 2 + 16;

				uint8_t* returnData= (uint8_t*)malloc(returnDataSize);
				if (webSocketServer==nullptr) {
					RemoteMeMessagesUtils::putUint8(returnData, posR, 0);
				}else {
					RemoteMeMessagesUtils::putUint8(returnData, posR, 1);
				}
				RemoteMeMessagesUtils::putUint16(returnData, posR, LOCAL_SERVER_PORT);
				RemoteMeMessagesUtils::putString(returnData, posR, WiFi.localIP().toString());
				sendSyncResponseMessage(messageId, returnDataSize, returnData);
				free(returnData);
			}
			free(data);

		}	else if (messageType == RemotemeStructures::SYNC_RESPONSE_MESSAGE) {

			rm.messageId = RemoteMeMessagesUtils::getInt64(payload, pos);

			//rm.syncResponseData = RemoteMeMessagesUtils::getArray(payload, pos, size);
			//rm.syncResponseDataSize = size;


		}else if (messageType == RemotemeStructures::VARIABLE_CHANGE_PROPAGATE_MESSAGE) {

			if (rm.variables != nullptr) {
				rm.variables->onChangePropagateMessage(payload);
			}


		}
		else {
			//Serial.println("message type  is not supported");
		}
	}
	
	String RemoteMe::callRest(String restUrl){
		HttpClient* httpClient;
		if (httpClient == nullptr) {
			WiFiClient wifiClient;
			httpClient = new HttpClient(wifiClient, REMOTEME_HOST, REMOTEME_HTTP_PORT);
		}
		
		httpClient->beginRequest();
		httpClient->get(restUrl);
		httpClient->sendHeader("Content-Type", "text/plain");
		httpClient->sendHeader("Content-Length", 0);
		httpClient->sendHeader("token", token);
		httpClient->endRequest();

	
		int statusCode = httpClient->responseStatusCode();
		return httpClient->responseBody();
		
	}
	
	void RemoteMe::sendByRest(uint8_t * payload,uint16_t length ){

		//secure
		//WiFiClient  tcp = new WiFiClientSecure();

		
		HttpClient* httpClient;
		if (httpClient == nullptr) {
			WiFiClient wifiClient;

			httpClient = new HttpClient(wifiClient, REMOTEME_HOST, REMOTEME_HTTP_PORT);

		}

		

	
		httpClient->beginRequest();
		httpClient->post("/api/rest/v1/message/sendMessage/");//longest
		httpClient->sendHeader("Content-Type", "text/plain");
		httpClient->sendHeader("Content-Length", length);
		httpClient->sendHeader("token", token);
		httpClient->endRequest();

		httpClient->write(payload, length);
		int statusCode = httpClient->responseStatusCode();
		String response = httpClient->responseBody();

		
		
		// read the status code and body of the response
	//	int statusCode = httpClient->responseStatusCode();


	}
	void RemoteMe::setupTwoWayCommunication() {
		
		socketEnabled = true;
		waitForConnection();
	}
	void RemoteMe::setupTwoWayWebSocketCommunication() {
		webSocketEnabled = true;
		waitForConnection();
	}
	
	
	uint16_t RemoteMe::sendUserSyncMessage(uint16_t receiverDeviceId, const uint8_t * payload, uint16_t length, uint8_t*& returnData) {

		this->messageId = 0;


		uint64_t messageId = deltaMillis() + 20;;
		uint8_t *payloadSyncMessage;

		uint16_t payloadSyncMessageSize = RemoteMeMessagesUtils::getSyncUserMessage(receiverDeviceId, deviceId, messageId, payload, length, payloadSyncMessage);
		send(payloadSyncMessage, payloadSyncMessageSize);

		this->messageId = 0;
		for (uint16_t i = 0; i < 8000;i++) {//8000 give 8s becasue there is delay 1ms
			
			this->loop();
			if (this->messageId == messageId) {//got reponse
				returnData = this->syncResponseData;
				this->messageId = 0;
				return this->syncResponseDataSize;
			}
			delay(1);

		}

		
		return 0xFFFF;//error
	}


	void RemoteMe::sendUserMessage(RemotemeStructures::WSUserMessageSettings renevalWhenFailType, uint16_t receiverDeviceId, uint16_t messageId, const uint8_t *payload, uint16_t length) {
		sendUserMessage(renevalWhenFailType, receiverDeviceId, deviceId, messageId, payload, length);
	}

	void RemoteMe::sendUserMessage(RemotemeStructures::WSUserMessageSettings renevalWhenFailType, uint16_t receiverDeviceId, uint16_t messageId, String message) {
		sendUserMessage(renevalWhenFailType, receiverDeviceId, deviceId, messageId, message);
	}

	void RemoteMe::sendUserMessage(RemotemeStructures::WSUserMessageSettings renevalWhenFailType, uint16_t receiverDeviceId, uint16_t senderDeviceId, uint16_t messageId, String message) {
		const uint8_t* data = reinterpret_cast<const uint8_t*>(&message[0]);
		sendUserMessage(renevalWhenFailType, receiverDeviceId, senderDeviceId, messageId, data, message.length()+1);
	}

	void RemoteMe::sendUserMessage(uint16_t receiverDeviceId, String message)
	{
		sendUserMessage(RemotemeStructures::NO_RENEWAL, receiverDeviceId, 0, message);
	}

	void RemoteMe::sendUserMessage(uint16_t receiverDeviceId, const uint8_t * payload, uint16_t length)
	{
		sendUserMessage(RemotemeStructures::NO_RENEWAL, receiverDeviceId, 0, payload, length);
	}



	void RemoteMe::sendUserMessage(RemotemeStructures::WSUserMessageSettings renevalWhenFailType, uint16_t receiverDeviceId, 	uint16_t senderDeviceId, uint16_t messageId, const uint8_t *payload, uint16_t length) {
		
		uint8_t * data;
		uint16_t size = RemoteMeMessagesUtils::getUserMessage(renevalWhenFailType, receiverDeviceId, senderDeviceId, messageId, payload, length, data);
		send(data,size);
		free(data);
	}

	void RemoteMe::sendAddDataMessage(uint16_t seriesId, RemotemeStructures::AddDataMessageSetting settings, uint64_t time, double value)
	{
		uint8_t * data;
		uint16_t size = RemoteMeMessagesUtils::getAddDataMessage(seriesId, settings, time, value, data);

		
		send(data,size);
		
		free(data);
	}



	void RemoteMe::sendSyncResponseMessage(uint64_t messageId, uint16_t dataSize, uint8_t * dataS)
	{
		uint8_t* data;
		uint16_t size = RemoteMeMessagesUtils::getSyncResponseMessage(messageId, dataSize, dataS, data);
		send(data, size);
		free(data);
	}

	void  RemoteMe::sendRegisterDeviceMessage(uint16_t deviceId, String deviceName, RemotemeStructures::DeviceType deviceType, RemotemeStructures::NetworkDeviceType networkDeviceType) {
		uint8_t * data;
		uint16_t size = RemoteMeMessagesUtils::getRegisterDeviceMessage(deviceId, deviceName, deviceType, networkDeviceType, data);
		send(data,size);
		free(data);
	}
	
	void RemoteMe::send(uint8_t * payload,uint16_t size ) {
	

		if (webSocketConnected) {
			webSocket->sendBIN(payload, size);
		}else if (socketConnected) {
			wifiClient->write((unsigned char*)payload, size);
		}
		else {
			sendByRest(payload, size);
		}
	
	}

	


	void RemoteMe::sendRegisterDeviceMessage(String deviceName) {
		sendRegisterDeviceMessage(deviceId, deviceName, RemotemeStructures::NETWORK, RemotemeStructures::ND_ARDUINO);
	}

	


	void RemoteMe::sendRegisterChildDeviceMessage(uint16_t parentDeviceId, uint16_t deviceId, String deviceName) {
		uint8_t* data;
		uint16_t  size= RemoteMeMessagesUtils::getRegisterChildDeviceMessage(parentDeviceId, deviceId, deviceName, data);
		send(data, size);
		free(data);
	}

	void RemoteMe::sendRegisterChildDeviceMessage(uint16_t deviceId, String deviceName) {
		sendRegisterChildDeviceMessage(this->deviceId, deviceId, "");
	}
	void RemoteMe::sendRegisterChildDeviceMessage(uint16_t deviceId) {
		sendRegisterChildDeviceMessage(deviceId, "");
	}




	void RemoteMe::sendLogMessage(RemotemeStructures::LogLevel logLevel, String str) {
		uint8_t* data;
		uint16_t size= RemoteMeMessagesUtils::getLogMessage(logLevel, str,data);
		send(data,size);
		free(data);
	}

	void RemoteMe::setUserMessageListener(void(*onUserMessage)(uint16_t senderDeviceId, uint16_t dataSize, uint8_t *data))
	{
		this->onUserMessage = onUserMessage;
	}

	void RemoteMe::setUserSyncMessageListener(void(*onUserSyncMessage)(uint16_t senderDeviceId, uint16_t dataSize, uint8_t *, uint16_t &returnDataSize, uint8_t *&returnData))
	{
		this->onUserSyncMessage = onUserSyncMessage;
	}






	bool RemoteMe::ping() {
		bool ret = false;
		if (webSocketEnabled) {
			if (webSocketConnected) {
				ret = webSocket->sendPing();
			}
		}else if (socketEnabled) {
			if (socketConnected) {
				uint8_t *buffer = (uint8_t*)malloc(4);
				buffer[0] = 0;
				buffer[1] = 0;
				buffer[2] = 0;
				buffer[3] = 0;
				ret = 4 == wifiClient->write((unsigned char*)buffer, 4);
				free(buffer);
			}
		}
		
		if (!ret) {
			socketConnected = false;
			webSocketConnected = false;
		}
		return ret;
	}

	void RemoteMe::waitForConnection() {
		static unsigned long lastTimePing = 0;
		static unsigned long lastTimeRestart = 0;

		if (webSocketEnabled || socketEnabled) {
			if (lastTimePing + 20000 < deltaMillis()) {
				ping();

				lastTimePing = deltaMillis();
			}
			while (!(socketConnected || webSocketConnected)) {

				if (lastTimeRestart + 20000 < deltaMillis()) {//restart every 20s
					lastTimeRestart = deltaMillis();
					if (webSocketEnabled) {
						if (webSocket != nullptr) {
							webSocket->disconnect();
						}
						webSocket = new WebSocketsClient();

						char* buf = new char[20];
						snprintf(buf, 20, "/api/ws/v1/%d", deviceId);

						webSocket->begin(REMOTEME_HOST, REMOTEME_HTTP_PORT, buf, "as");
						webSocket->setAuthorization("token", token); // HTTP Basic Authorization

						webSocket->setReconnectInterval(500);
						webSocket->onEvent(RemoteMe::webSocketEvent);

					}
					else if (socketEnabled) {
						if (wifiClient != nullptr) {
							wifiClient->stop();
						}
						
						wifiClient = new WiFiClient();

						if (wifiClient->connect(REMOTEME_HOST, REMOTEME_SOCKET_PORT)) {

							String tokenS = String(token);
							uint16_t sizeToSend = 2 + tokenS.length() + 1;
							uint8_t* buffer = (uint8_t*)malloc(sizeToSend);
							uint16_t pos = 0;
							RemoteMeMessagesUtils::putUint16(buffer, pos, deviceId);
							RemoteMeMessagesUtils::putString(buffer, pos, tokenS);
							socketConnected = sizeToSend == wifiClient->write((unsigned char*)buffer, sizeToSend);
							free(buffer);
						}
					}
				}
				if (webSocketEnabled && webSocket != nullptr) {
					webSocket->sendPing();
					webSocket->loop();//its setting  webSocketConnected
				}
				delay(100);
			}
		}
		
	}

	void RemoteMe::setupDirectConnections( ) {
	

		webSocketServer = new WebSocketsServer(LOCAL_SERVER_PORT);
		webSocketServer->begin();
		webSocketServer->onEvent(RemoteMe::webSocketServerEvent);

	}
	void RemoteMe::loop() {
		
		waitForConnection();
		if (webSocketEnabled) {
			webSocket->loop();
		}
		else if (socketEnabled) {
			socketLoop();
		}
		if (webSocketServer != nullptr) {
			webSocketServer->loop();
		}

	}


	void RemoteMe::socketLoop() {
		if (wifiClient->available() >= 4) {
			uint8_t *buffer = (uint8_t*)malloc(4);
			wifiClient->read(buffer, 4);
			uint16_t pos = 0;
			uint16_t messageId = RemoteMeMessagesUtils::getUint16(buffer, pos);
			uint16_t size = RemoteMeMessagesUtils::getUint16(buffer, pos);
			free(buffer);

			if (messageId==0 && size == 0) {
				return;
			}

			uint16_t bufferPos = 0;
			buffer = (uint8_t*)malloc(size + 4);
			RemoteMeMessagesUtils::putUint16(buffer, bufferPos, messageId);
			RemoteMeMessagesUtils::putUint16(buffer, bufferPos, size);
			unsigned long time = deltaMillis();
			bool error = false;
			while (wifiClient->available() <size) {
				if (deltaMillis()<time + 3000) {//timeout
					return;
				}
			}

			
			wifiClient->read(&buffer[pos], size);
			processMessage(buffer);
			free(buffer);


		}
	}
	void RemoteMe::disconnect() {
		if (webSocket != nullptr) {
			webSocket->disconnect();
			webSocket=nullptr;
		}
		if (wifiClient != nullptr) {
			wifiClient->stop();
			wifiClient=nullptr;
		}
	}
	
	long RemoteMe::deltaMillis(){
		return millis()+1e6;//so functions to restart conenciton woutn wait 20s without when first time connect
	}
	uint16_t RemoteMe::getDeviceId() {
		return deviceId;

	}

	//---------- variables


	Variables*  RemoteMe::getVariables() {
		if (this->variables == nullptr) {
			this->variables = new Variables(this);
		}
		return this->variables;
	}

	void RemoteMe::sendVariableObserveMessage(String name, uint16_t type) {
		

		uint8_t* data;
		uint16_t size = RemoteMeMessagesUtils::sendVariableObserveMessage(deviceId, name, type, data);
		send(data, size);
		free(data);

	}
