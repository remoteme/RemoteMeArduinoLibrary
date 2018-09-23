// 
// 
// 




#include "RemoteMe.h"
#include "RemoteMeConnector.h"


	RemoteMe::RemoteMe(char * token, uint16_t deviceId) {
		this->token = std::move(token);
		this->deviceId = deviceId;

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
				#ifdef DIRECT_CONNECTIONS
					if (webSocketServer==nullptr) {
						RemoteMeMessagesUtils::putUint8(returnData, posR, 0);
					}else {
						RemoteMeMessagesUtils::putUint8(returnData, posR, 1);
					}
				#else
					RemoteMeMessagesUtils::putUint8(returnData, posR, 0);
				#endif
				RemoteMeMessagesUtils::putUint16(returnData, posR, LOCAL_SERVER_PORT);
				RemoteMeMessagesUtils::putString(returnData, posR, this->connector->getIp());
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

	void RemoteMe::setConnector(RemoteMeConnector* connector) {
		this->connector = connector;
		this->connector->setRemoteMe(this);
		this->connector->waitForConnection();
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
		this->connector->send(payload, size);
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




	#ifdef DIRECT_CONNECTIONS
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

	
	void RemoteMe::setupDirectConnections( ) {
	

		webSocketServer = new WebSocketsServer(LOCAL_SERVER_PORT);
		webSocketServer->begin();
		webSocketServer->onEvent(RemoteMe::webSocketServerEvent);

	}
	#endif
	
	void RemoteMe::loop() {
		
		this->connector->waitForConnection();
		this->connector->loop();
		

		#ifdef DIRECT_CONNECTIONS
		if (webSocketServer != nullptr) {
			webSocketServer->loop();
		}
		#endif
	}


	
	void RemoteMe::disconnect() {
		this->connector->disconnect();
	}
	
	long RemoteMe::deltaMillis(){
		return millis() + 400000l;
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

	void RemoteMe::sendVariableObserveMessage() {
		if (variables != nullptr) {
			uint8_t* data;
			uint16_t size = this->getVariables()->getVariableObserveMessage(data);

			Serial.println("sending variables remoteme");
			send(data, size);
			free(data);
		}
		else {
			Serial.println("variables are null");
		}
		

	}
