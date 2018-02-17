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

				RemotemeStructures::MessageType messageType = static_cast<RemotemeStructures::MessageType> (RemoteMe::getShort(payload, pos));
				uint16_t size = RemoteMe::getInstance("", 0).getShort(payload, pos);
				if (messageType == RemotemeStructures::USER_MESSAGE) {
					RemotemeStructures::WSUserMessageSettings userMessageSettings = static_cast<RemotemeStructures::WSUserMessageSettings>(RemoteMe::getByte(payload, pos));
					uint16_t receiverDeviceId = RemoteMe::getShort(payload, pos);
					uint16_t senderDeviceId = RemoteMe::getShort(payload, pos);
					uint16_t messageId = RemoteMe::getShort(payload, pos);

					uint16_t dataSize = size - pos + 4;// -4 2 for suze 2 for bytes becasuse size is without this
					uint8_t* data = RemoteMe::getArray(payload, pos, dataSize);
					if (rm.onUserMessage!=nullptr) {
						rm.onUserMessage(senderDeviceId,dataSize, data);
					}
				}
				else if (messageType == RemotemeStructures::USER_SYNC_MESSAGE) {
					uint16_t receiverDeviceId = RemoteMe::getShort(payload, pos);
					uint16_t senderDeviceId = RemoteMe::getShort(payload, pos);

					uint64_t messageId = RemoteMe::getLong(payload, pos);

					uint16_t dataSize = size - pos + 4;
					uint8_t* data = RemoteMe::getArray(payload, pos, dataSize);


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

	


	void RemoteMe::putString(uint8_t* data, uint16_t &pos, String string) {
		const uint8_t* dataString = reinterpret_cast<const uint8_t*>(&string[0]);
		putArray(data, pos, dataString, string.length());
		data[pos++] = 0;


	}
	void RemoteMe::putArray(uint8_t* data, uint16_t &pos, const void* arrayT, uint16_t length) {

		memcpy(&data[pos], arrayT, length);

		pos += length;
	}

	void RemoteMe::putByte(uint8_t* data, uint16_t &pos, uint8_t number) {
		data[pos++] = number;


	}

	void RemoteMe::putShort(uint8_t* data, uint16_t &pos, uint16_t number) {
		putBigEndian(data, pos, &number, sizeof(uint16_t));
	}

	void RemoteMe::putLong(uint8_t* data, uint16_t &pos, uint64_t number) {
		putBigEndian(data, pos, &number, sizeof(uint64_t));
	}


	uint8_t* RemoteMe::getReverseBytes(void* start, uint16_t size) {
		uint8_t* ret = new uint8_t[size];

		for (uint8_t i = 0; i < size; i++) {
			ret[i] = ((uint8_t*)start)[size - i - 1];

		}


		return ret;
	}

	void RemoteMe::putBigEndian(uint8_t* data, uint16_t &pos, void* value, uint16_t size) {
		putArray(data, pos, getReverseBytes(value, size), size);
	}

	void RemoteMe::putDouble(uint8_t* data, uint16_t &pos, double value) {
		return putBigEndian(data, pos, &value, sizeof(double));
	}

	uint8_t* RemoteMe::getArray(uint8_t* data, uint16_t& pos, uint16_t length) {
		if (length != 0) {
			uint8_t* ret = (uint8_t*)malloc(length);
			memcpy(ret, &data[pos], length);
			return ret;
		}
		else {
			return (uint8_t*)malloc(1);
		};

	}

	uint16_t RemoteMe::getShort(uint8_t* payload, uint16_t& pos) {

		uint8_t* temp = getArray(payload, pos, sizeof(uint16_t));

		temp = getReverseBytes(temp, sizeof(uint16_t));
		pos += sizeof(uint16_t);
		uint16_t* ret = (uint16_t*)temp;
		return  *ret;
	}

	uint32_t RemoteMe::getInt(uint8_t *payload, uint16_t& pos) {
		uint8_t* temp = getArray(payload, pos, sizeof(uint32_t));
		temp = getReverseBytes(temp, sizeof(uint32_t));
		pos += sizeof(uint32_t);
		uint32_t* ret = (uint32_t*)temp;

		return  *ret;
	}

	double RemoteMe::getDouble(uint8_t *payload, uint16_t& pos) {
		uint8_t* temp = getArray(payload, pos, sizeof(double));
		temp = getReverseBytes(temp, sizeof(double));
		pos += sizeof(double);
		double* ret = (double*)temp;

		return  *ret;
	}
	uint64_t RemoteMe::getLong(uint8_t *payload, uint16_t& pos) {
		uint8_t* temp = getArray(payload, pos, sizeof(uint64_t));
		temp = getReverseBytes(temp, sizeof(uint64_t));
		pos += sizeof(uint64_t);
		uint64_t* ret = (uint64_t*)temp;

		return  *ret;
	}

	String RemoteMe::getString(uint8_t* data, uint16_t& pos) {

		String ret =String((char*)(data + pos));
		pos += ret.length() + 1;

		return  ret;
	}



	uint8_t RemoteMe::getByte(uint8_t* data, uint16_t& pos) {
		return  data[pos++];
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


		putShort(data, index, RemotemeStructures::USER_MESSAGE);
		putShort(data, index, size);

		putByte(data, index, renevalWhenFailType);
		putShort(data, index, receiverDeviceId);
		putShort(data, index, senderDeviceId);
		putShort(data, index, messageId);


		putArray(data, index, payload, length);


		sendByWebSocket(data, size + 4);

	}

	void RemoteMe::sendAddDataMessage(uint16_t seriesId, RemotemeStructures::AddDataMessageSetting settings, uint64_t time, double value)
	{
		uint16_t size = 11 + sizeof(double);
		uint8_t* data = new uint8_t[size + 4];


		uint16_t pos = 0;


		putShort(data, pos, RemotemeStructures::ADD_DATA);
		putShort(data, pos, size);

		putShort(data, pos, seriesId);
		putByte(data, pos, settings);
		putLong(data, pos, time);
		putDouble(data, pos, value);



		sendByWebSocket(data, size + 4);
	}

	void RemoteMe::sendSyncResponseUserMessage(uint64_t messageId, uint16_t dataSize, uint8_t * data)
	{
		uint16_t size = 8 + dataSize;

		uint16_t pos = 0;

		uint8_t* dataToSent = new uint8_t[size + 4];
		putShort(dataToSent, pos, RemotemeStructures::SYNC_RESPONSE_MESSAGE);
		putShort(dataToSent, pos, dataSize);




		putLong(dataToSent, pos, messageId);
		putArray(dataToSent, pos, data, dataSize);

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


		putShort(data, pos, RemotemeStructures::REGISTER_DEVICE);
		putShort(data, pos, size);

		putShort(data, pos, deviceId);
		putString(data, pos, deviceName);
		putByte(data, pos, deviceType);
		putShort(data, pos, networkDeviceType);





		sendByWebSocket(data, size + 4);

	}

	void RemoteMe::sendRegisterDeviceMessage(String deviceName) {
		sendRegisterDeviceMessage(deviceId, deviceName, RemotemeStructures::NETWORK, RemotemeStructures::ND_ARDUINO);
	}

	


	void RemoteMe::sendRegisterChildDeviceMessage(uint16_t parentDeviceId, uint16_t deviceId, String deviceName) {
		uint16_t size = 5 + deviceName.length();//4 short  + 1 for 0 for string
		uint8_t* data = new uint8_t[size + 4];


		uint16_t pos = 0;


		putShort(data, pos, RemotemeStructures::REGISTER_CHILD_DEVICE);
		putShort(data, pos, size);

		putShort(data, pos, parentDeviceId);
		putShort(data, pos, deviceId);
		putString(data, pos, deviceName);



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

		putShort(data, pos, RemotemeStructures::LOGG);
		putShort(data, pos, size);


		putByte(data, pos, logLevel);
		putString(data, pos, str);

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
