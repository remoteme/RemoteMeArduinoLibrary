// 
// 
// 



#define HOST "192.168.0.30"
#define PORT 8082


#include "RemoteMe.h"



	RemoteMe::RemoteMe(char * token, uint16_t deviceId) {
		this->token = std::move(token);
		this->deviceId = deviceId;

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

			//Serial.print("websocket conencted");
			break;
		case WStype_TEXT:
			
			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			if (length > 0) {
				uint16_t pos = 0;
				RemoteMe& rm = RemoteMe::getInstance("", 0);

				RemotemeStructures::MessageType messageType = static_cast<RemotemeStructures::MessageType> (RemoteMeMessagesUtils::getShort(payload, pos));
				uint16_t size = RemoteMeMessagesUtils::getShort(payload, pos);
				if (messageType == RemotemeStructures::USER_MESSAGE) {
					RemotemeStructures::WSUserMessageSettings userMessageSettings = static_cast<RemotemeStructures::WSUserMessageSettings>(RemoteMeMessagesUtils::getByte(payload, pos));
					uint16_t receiverDeviceId = RemoteMeMessagesUtils::getShort(payload, pos);
					uint16_t senderDeviceId = RemoteMeMessagesUtils::getShort(payload, pos);
					uint16_t messageId = RemoteMeMessagesUtils::getShort(payload, pos);

					uint16_t dataSize = size - pos + 4;// -4 2 for suze 2 for bytes becasuse size is without this
					uint8_t* data = RemoteMeMessagesUtils::getArray(payload, pos, dataSize);
					if (rm.onUserMessage!=nullptr) {
						rm.onUserMessage(senderDeviceId,dataSize, data);
					}
				}
				else if (messageType == RemotemeStructures::USER_SYNC_MESSAGE) {
					uint16_t receiverDeviceId = RemoteMeMessagesUtils::getShort(payload, pos);
					uint16_t senderDeviceId = RemoteMeMessagesUtils::getShort(payload, pos);

					uint64_t messageId = RemoteMeMessagesUtils::getLong(payload, pos);

					uint16_t dataSize = size - pos + 4;
					uint8_t* data = RemoteMeMessagesUtils::getArray(payload, pos, dataSize);


					if (rm.onUserSyncMessage != nullptr) {
						uint16_t returnDataSize;
						uint8_t* returnData;

						rm.onUserSyncMessage(senderDeviceId, dataSize, data,returnDataSize,returnData);
						rm.sendSyncResponseUserMessage(messageId, returnDataSize, returnData);

						free(returnData);

					}


				}
				else {
					//Serial.println("message type  is not supported");
				}
			}
			
		}

	}

	
	String RemoteMe::callRest(String restUrl){
		HttpClient* httpClient;
		if (httpClient == nullptr) {
			WiFiClient wifiClient;
			httpClient = new HttpClient(wifiClient, HOST, PORT);
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
	
	void RemoteMe::sendByRest(uint8_t * payload, size_t length){

		//secure
		//WiFiClient  tcp = new WiFiClientSecure();

		
		HttpClient* httpClient;
		if (httpClient == nullptr) {
			WiFiClient wifiClient;

			httpClient = new HttpClient(wifiClient, HOST, PORT);

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
		twoWayCommunicationEnabled = true;
		createWebSocket();
		waitForWebSocketConnect();
	}
	void RemoteMe::createWebSocket() {
		

		if (webSocket != nullptr) {
			webSocket->disconnect();
		}
		webSocket = new WebSocketsClient();

		char* buf = new char[20];
		snprintf(buf, 20, "/api/ws/v1/%d",deviceId);

		

		webSocket->begin(REMOTEME_HOST, REMOTEME_PORT, buf, "as");
		webSocket->setAuthorization("token", token); // HTTP Basic Authorization

		webSocket->setReconnectInterval(500);


		webSocket->onEvent(RemoteMe::webSocketEvent);
		webSocket->sendPing();

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



	void RemoteMe::sendUserMessage(RemotemeStructures::WSUserMessageSettings renevalWhenFailType, uint16_t receiverDeviceId, uint16_t senderDeviceId, uint16_t messageId, const uint8_t *payload, uint16_t length) {
		uint16_t size = 7 + length;
		uint8_t* data = new uint8_t[size + 4];


		uint16_t index = 0;


		RemoteMeMessagesUtils::putShort(data, index, RemotemeStructures::USER_MESSAGE);
		RemoteMeMessagesUtils::putShort(data, index, size);

		RemoteMeMessagesUtils::putByte(data, index, renevalWhenFailType);
		RemoteMeMessagesUtils::putShort(data, index, receiverDeviceId);
		RemoteMeMessagesUtils::putShort(data, index, senderDeviceId);
		RemoteMeMessagesUtils::putShort(data, index, messageId);

		RemoteMeMessagesUtils::putArray(data, index, payload, length);


		sendByWebSocket(data, size + 4);

	}

	void RemoteMe::sendAddDataMessage(uint16_t seriesId, RemotemeStructures::AddDataMessageSetting settings, uint64_t time, double value)
	{
		uint16_t size = 2+1+8+8;
		uint8_t* data = new uint8_t[size + 4];


		uint16_t pos = 0;


		RemoteMeMessagesUtils::putShort(data, pos, RemotemeStructures::ADD_DATA);
		RemoteMeMessagesUtils::putShort(data, pos, size);

		RemoteMeMessagesUtils::putLong(data, pos, time);
		RemoteMeMessagesUtils::putByte(data, pos, settings);
		
		RemoteMeMessagesUtils::putShort(data, pos, seriesId);
		RemoteMeMessagesUtils::putDouble(data, pos, value);



		sendByWebSocket(data, size + 4);
	}

	void RemoteMe::sendSyncResponseUserMessage(uint64_t messageId, uint16_t dataSize, uint8_t * data)
	{
		uint16_t size = 8 + dataSize;

		uint16_t pos = 0;

		uint8_t* dataToSent = new uint8_t[size + 4];
		RemoteMeMessagesUtils::putShort(dataToSent, pos, RemotemeStructures::SYNC_RESPONSE_MESSAGE);
		RemoteMeMessagesUtils::putShort(dataToSent, pos, dataSize);




		RemoteMeMessagesUtils::putLong(dataToSent, pos, messageId);
		RemoteMeMessagesUtils::putArray(dataToSent, pos, data, dataSize);

		sendByWebSocket(dataToSent, size + 4);


	}

	
	void RemoteMe::sendByWebSocket(uint8_t * payload, size_t length) {
		if (webSocketConnected) {
			webSocket->sendBIN(payload, length);
		}else {
			sendByRest(payload, length);
		}
	
	}

	

	void  RemoteMe::sendRegisterDeviceMessage(uint16_t deviceId, String deviceName, RemotemeStructures::DeviceType deviceType, RemotemeStructures::NetworkDeviceType networkDeviceType) {
		uint16_t size = 2+2 + deviceName.length()+1+1;
		uint8_t* data = new uint8_t[size + 4];


		uint16_t pos = 0;


		RemoteMeMessagesUtils::putShort(data, pos, RemotemeStructures::REGISTER_DEVICE);
		RemoteMeMessagesUtils::putShort(data, pos, size);

		RemoteMeMessagesUtils::putShort(data, pos, deviceId);
		RemoteMeMessagesUtils::putString(data, pos, deviceName);
		RemoteMeMessagesUtils::putByte(data, pos, deviceType);
		RemoteMeMessagesUtils::putShort(data, pos, networkDeviceType);





		sendByWebSocket(data, size + 4);

	}

	void RemoteMe::sendRegisterDeviceMessage(String deviceName) {
		sendRegisterDeviceMessage(deviceId, deviceName, RemotemeStructures::NETWORK, RemotemeStructures::ND_ARDUINO);
	}

	


	void RemoteMe::sendRegisterChildDeviceMessage(uint16_t parentDeviceId, uint16_t deviceId, String deviceName) {
		uint16_t size = 5 + deviceName.length();//4 short  + 1 for 0 for string
		uint8_t* data = new uint8_t[size + 4];


		uint16_t pos = 0;


		RemoteMeMessagesUtils::putShort(data, pos, RemotemeStructures::REGISTER_CHILD_DEVICE);
		RemoteMeMessagesUtils::putShort(data, pos, size);

		RemoteMeMessagesUtils::putShort(data, pos, parentDeviceId);
		RemoteMeMessagesUtils::putShort(data, pos, deviceId);
		RemoteMeMessagesUtils::putString(data, pos, deviceName);



		sendByWebSocket(data, size + 4);
	}
	void RemoteMe::sendRegisterChildDeviceMessage(uint16_t deviceId, String deviceName) {
		sendRegisterChildDeviceMessage(this->deviceId, deviceId, "");
	}
	void RemoteMe::sendRegisterChildDeviceMessage(uint16_t deviceId) {
		sendRegisterChildDeviceMessage(deviceId, "");
	}




	void RemoteMe::sendLogMessage(RemotemeStructures::LogLevel logLevel, String str) {
		uint16_t size = 2 + str.length();
		uint8_t* data = new uint8_t[size + 4];

		uint16_t pos = 0;

		RemoteMeMessagesUtils::putShort(data, pos, RemotemeStructures::LOGG);
		RemoteMeMessagesUtils::putShort(data, pos, size);


		RemoteMeMessagesUtils::putByte(data, pos, logLevel);
		RemoteMeMessagesUtils::putString(data, pos, str);

		sendByWebSocket(data, size + 4);
	}

	void RemoteMe::setUserMessageListener(void(*onUserMessage)(uint16_t senderDeviceId, uint16_t dataSize, uint8_t *data))
	{
		this->onUserMessage = onUserMessage;
	}

	void RemoteMe::setUserSyncMessageListener(void(*onUserSyncMessage)(uint16_t senderDeviceId, uint16_t dataSize, uint8_t *, uint16_t &returnDataSize, uint8_t *&returnData))
	{
		this->onUserSyncMessage = onUserSyncMessage;
	}





	void RemoteMe::waitForWebSocketConnect() {
	
		while (!webSocketConnected) {
			webSocket->loop();
		}


	}

	void RemoteMe::loop() {

		if (twoWayCommunicationEnabled) {
			static unsigned long firstTimeFail = 0;
			static unsigned long lastTimePing = millis();
			static bool restarted = true;

			webSocket->loop();


			if (lastTimePing + 60000 < millis() || firstTimeFail != 0) {
				lastTimePing = millis();

				if (webSocket->sendPing()) {
					firstTimeFail = 0;
				}
				else {
					if (firstTimeFail == 0) {//happen first time after connection established
						firstTimeFail = millis();
						restarted = false;
					}

					if (firstTimeFail + (restarted ? 1800 : 60) * 1000 < millis()) {// if it was already restarted we are allow websocket to do job but until half hour we restart again
						firstTimeFail = millis();
						restarted = true;
						createWebSocket();//shoud hapopen 60s after first fail and then after halfhour
					}

				}


			}
		}
		
	}
