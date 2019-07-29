// 
// 
// 




#include "RemoteMe.h"
#include "RemoteMeConnector.h"
#include "RemoteMeDirectConnector.h"

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
		processUserMessage(senderDeviceId, dataSize, data, 0, 0, 0);
		free(data);
	}
	else if (messageType == RemotemeStructures::USER_MESSAGE_WEBPAGE_TOKEN) {
		RemotemeStructures::WSUserMessageSettings userMessageSettings = static_cast<RemotemeStructures::WSUserMessageSettings>(RemoteMeMessagesUtils::getUint8(payload, pos));
		uint16_t receiverDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);
		uint16_t senderDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);

		uint16_t sessionId = RemoteMeMessagesUtils::getUint16(payload, pos);
		uint16_t credit = RemoteMeMessagesUtils::getUint16(payload, pos);
		uint16_t time = RemoteMeMessagesUtils::getUint16(payload, pos);

		uint16_t dataSize = size - pos + 4;// -4 2 for suze 2 for bytes becasuse size is without this
		uint8_t* data = RemoteMeMessagesUtils::getArray(payload, pos, dataSize);
		processUserMessage(senderDeviceId, dataSize, data, sessionId, credit, time);
		free(data);
	}
	else if (messageType == RemotemeStructures::USER_SYNC_MESSAGE) {
		uint16_t receiverDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);
		uint16_t senderDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);

		uint64_t messageId = RemoteMeMessagesUtils::getInt64(payload, pos);

		uint16_t dataSize = size - pos + 4;
		uint8_t* data = RemoteMeMessagesUtils::getArray(payload, pos, dataSize);


		processSyncUserMessage(senderDeviceId, dataSize, data, 0, 0, 0);

		free(data);

	}
	else if (messageType == RemotemeStructures::USER_SYNC_MESSAGE_WEBPAGE_TOKEN) {
		uint16_t receiverDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);
		uint16_t senderDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);

		uint16_t sessionId = RemoteMeMessagesUtils::getUint16(payload, pos);
		uint16_t credit = RemoteMeMessagesUtils::getUint16(payload, pos);
		uint16_t time = RemoteMeMessagesUtils::getUint16(payload, pos);

		uint64_t messageId = RemoteMeMessagesUtils::getInt64(payload, pos);

		uint16_t dataSize = size - pos + 4;
		uint8_t* data = RemoteMeMessagesUtils::getArray(payload, pos, dataSize);


		processSyncUserMessage(senderDeviceId, dataSize, data, sessionId, credit, time);

		free(data);

	}
	else if (messageType == RemotemeStructures::SYNC_MESSAGE) {

		int8_t type = RemoteMeMessagesUtils::getInt8(payload, pos);
		uint16_t receiverDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);
		uint64_t messageId = RemoteMeMessagesUtils::getInt64(payload, pos);

		uint16_t dataSize = size - pos + 4;
		uint8_t* data = RemoteMeMessagesUtils::getArray(payload, pos, dataSize);

		if (type == RemotemeStructures::GET_WEBSOCKET_SERVER_LOCAL) {
			uint16_t posR = 0;
			uint16_t returnDataSize = 1 + 2 + 16;

			uint8_t* returnData = (uint8_t*)malloc(returnDataSize);

			if (remoteMeDirectConnector == nullptr) {
				RemoteMeMessagesUtils::putUint8(returnData, posR, 0);
				RemoteMeMessagesUtils::putUint16(returnData, posR, 0);
			}
			else {
				RemoteMeMessagesUtils::putUint8(returnData, posR, 1);
				RemoteMeMessagesUtils::putUint16(returnData, posR, LOCAL_SERVER_PORT);
			}


			RemoteMeMessagesUtils::putString(returnData, posR, this->connector->getIp());
			sendSyncResponseMessage(messageId, returnDataSize, returnData);
			free(returnData);
		}
		free(data);

	}
	else if (messageType == RemotemeStructures::SYNC_MESSAGE_RESPONSE) {

		messageId = RemoteMeMessagesUtils::getInt64(payload, pos);

		//rm.syncResponseData = RemoteMeMessagesUtils::getArray(payload, pos, size);
		//rm.syncResponseDataSize = size;


	}
	else if (messageType == RemotemeStructures::VARIABLE_CHANGE_PROPAGATE_MESSAGE) {

		if (rm.variables != nullptr) {
			variables->onChangePropagateMessage(payload);
		}


	}
	else if (messageType == RemotemeStructures::VARIABLE_CHANGE_PROPAGATE_MESSAGE_WEBPAGE_TOKEN) {
		if (variables != nullptr) {
			variables->onChangePropagateMessage_rental(payload);
		}
	}
	else if (messageType == RemotemeStructures::VARIABLE_CHANGE_MESSAGE) {//inner connectoin

		if (variables != nullptr) {
			variables->onChangeVariableMessage(payload);
		}
	}
	else {
		DEBUG_REMOTEME("[RMM] message type  is not supported  \n");
	}
}

void RemoteMe::processUserMessage(uint16_t senderDeviceId, uint16_t dataSize, uint8_t* data, uint16_t sessionId, uint16_t credit, uint16_t time) {

	if (onUserMessage != nullptr) {
		onUserMessage(senderDeviceId, dataSize, data);
	}
	if (onUserMessage_rental != nullptr) {
		onUserMessage_rental(senderDeviceId, dataSize, data, sessionId, credit, time);
	}
}
void RemoteMe::processSyncUserMessage(uint16_t senderDeviceId, uint16_t dataSize, uint8_t* data, uint16_t sessionId, uint16_t credit, uint16_t time) {
	if (onUserSyncMessage != nullptr || onUserSyncMessage_rental != nullptr) {
		uint16_t returnDataSize;
		uint8_t* returnData;

		if (onUserSyncMessage_rental != nullptr) {
			onUserSyncMessage_rental(senderDeviceId, dataSize, data, returnDataSize, returnData, sessionId, credit, time);
		}
		else {
			onUserSyncMessage(senderDeviceId, dataSize, data, returnDataSize, returnData);
		}
		sendSyncResponseMessage(messageId, returnDataSize, returnData);

		free(returnData);

	}
}
void RemoteMe::setConnector(RemoteMeConnector* connector) {
	this->connector = connector;
	this->connector->setRemoteMe(this);
	this->connector->waitForConnection();
}

void RemoteMe::setDirectConnector(RemoteMeDirectConnector* remoteMeDirectConnector) {
	this->remoteMeDirectConnector = remoteMeDirectConnector;
	this->remoteMeDirectConnector->setRemoteMe(this);
}

std::list<uint16_t>* RemoteMe::getDirectConnected() {
	if (this->remoteMeDirectConnector == nullptr) {
		return nullptr;
	}
	else {
		return this->remoteMeDirectConnector->getDirectConnected();
	}
}
uint16_t RemoteMe::sendUserSyncMessage(uint16_t receiverDeviceId, const uint8_t * payload, uint16_t length, uint8_t*& returnData) {

	this->messageId = 0;


	uint64_t messageId = deltaMillis() + 20;;
	uint8_t *payloadSyncMessage;

	uint16_t payloadSyncMessageSize = RemoteMeMessagesUtils::getSyncUserMessage(receiverDeviceId, deviceId, messageId, payload, length, payloadSyncMessage);
	send(payloadSyncMessage, payloadSyncMessageSize);

	this->messageId = 0;
	for (uint16_t i = 0; i < 8000; i++) {//8000 give 8s becasue there is delay 1ms

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
	sendUserMessage(renevalWhenFailType, receiverDeviceId, senderDeviceId, messageId, data, message.length() + 1);
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

	uint8_t * data;
	uint16_t size = RemoteMeMessagesUtils::getUserMessage(renevalWhenFailType, receiverDeviceId, senderDeviceId, messageId, payload, length, data);


	if (!this->sendDirect(receiverDeviceId, data, size)) {
		send(data, size);
	}
	free(data);
}

bool RemoteMe::sendDirect(uint16_t receiverDeviceId, uint8_t *payload, uint16_t length) {

	if (this->remoteMeDirectConnector != nullptr) {
		return this->remoteMeDirectConnector->send(receiverDeviceId, payload, length);
	}
	else {
		return false;
	}
}

void RemoteMe::sendDirect(uint8_t *payload, uint16_t length) {
	if (this->remoteMeDirectConnector != nullptr) {
		this->remoteMeDirectConnector->send(payload, length);
	}
}

void RemoteMe::sendAddDataMessage(uint16_t seriesId, RemotemeStructures::AddDataMessageSetting settings, uint64_t time, double value)
{
	uint8_t * data;
	uint16_t size = RemoteMeMessagesUtils::getAddDataMessage(seriesId, settings, time, value, data);


	send(data, size);

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
	send(data, size);
	free(data);
}

void RemoteMe::send(uint8_t * payload, uint16_t size) {
	this->connector->send(payload, size);
}




void RemoteMe::sendRegisterDeviceMessage(String deviceName) {
	sendRegisterDeviceMessage(deviceId, deviceName, RemotemeStructures::NETWORK, RemotemeStructures::ND_ARDUINO);
}




void RemoteMe::sendRegisterChildDeviceMessage(uint16_t parentDeviceId, uint16_t deviceId, String deviceName) {
	uint8_t* data;
	uint16_t  size = RemoteMeMessagesUtils::getRegisterChildDeviceMessage(parentDeviceId, deviceId, deviceName, data);
	send(data, size);
	free(data);
}

void RemoteMe::sendRegisterChildDeviceMessage(uint16_t deviceId, String deviceName) {
	sendRegisterChildDeviceMessage(this->deviceId, deviceId, "");
}
void RemoteMe::sendRegisterChildDeviceMessage(uint16_t deviceId) {
	sendRegisterChildDeviceMessage(deviceId, "");
}

void RemoteMe::sendPushNotificationMessage(uint16_t webPageDeviceId, String title, String body, String badge, String icon, String image) {
	this->sendPushNotificationMessage(webPageDeviceId, title, body, badge, icon, image, 0);
}
void RemoteMe::sendPushNotificationMessage(uint16_t webPageDeviceId, String title, String body, String badge, String icon, String image, int vibrateCount, ...) {
	uint8_t* data;

	va_list valst;
	va_start(valst, vibrateCount);

	uint8_t* vibrateArr = (uint8_t*)malloc(vibrateCount);
	for (int i = 0; i < vibrateCount; i++) {
		vibrateArr[i] = va_arg(valst, int) / 10;
	}
	uint16_t size = RemoteMeMessagesUtils::getPushNotificationMessage(webPageDeviceId, title, body, badge, icon, image, vibrateCount, vibrateArr, data);
	send(data, size);
	free(data);
	free(vibrateArr);
}


void RemoteMe::setFileContent(uint16_t deviceId, String fileName, int dataSize, const uint8_t* content) {
	uint16_t dataPerOnePackage = 256 * 256 - 4 - 2 - 1 - fileName.length() - 1;
	int offset = 0;
	bool first = true;
	while (dataSize > 0) {
		uint16_t currentPackageSize = dataSize < dataPerOnePackage ? dataSize : dataPerOnePackage;
		dataSize -= currentPackageSize;

		uint8_t* singelData = (uint8_t*)malloc(currentPackageSize);
		memcpy(singelData, &content[offset], currentPackageSize);

		offset += currentPackageSize;

		uint8_t* data;
		uint16_t size = RemoteMeMessagesUtils::getSetFileContentMessage(deviceId, fileName, !first, dataSize == 0, currentPackageSize, singelData, data);
		send(data, size);

		free(data);
		free(singelData);
		first = false;
	}

}


void RemoteMe::sendLogMessage(RemotemeStructures::LogLevel logLevel, String str) {
	uint8_t* data;
	uint16_t size = RemoteMeMessagesUtils::getLogMessage(logLevel, str, data);
	send(data, size);
	free(data);
}

void RemoteMe::setUserMessageListener(void(*onUserMessage)(uint16_t senderDeviceId, uint16_t dataSize, uint8_t *data))
{
	this->onUserMessage = onUserMessage;
}

void RemoteMe::setUserMessageListener(void(*onUserMessage)(uint16_t senderDeviceId, uint16_t dataSize, uint8_t *data, uint16_t sessionId, uint16_t credit, uint16_t time))
{
	this->onUserMessage_rental = onUserMessage;
}

void RemoteMe::setUserSyncMessageListener(void(*onUserSyncMessage)(uint16_t senderDeviceId, uint16_t dataSize, uint8_t *, uint16_t &returnDataSize, uint8_t *&returnData))
{
	this->onUserSyncMessage = onUserSyncMessage;
}

void RemoteMe::setUserSyncMessageListener(void(*onUserSyncMessage)(uint16_t senderDeviceId, uint16_t dataSize, uint8_t *, uint16_t &returnDataSize, uint8_t *&returnData, uint16_t sessionId, uint16_t credit, uint16_t time))
{
	this->onUserSyncMessage_rental = onUserSyncMessage;
}

void RemoteMe::loop() {

	this->connector->waitForConnection();
	this->connector->loop();


	if (remoteMeDirectConnector != nullptr) {
		remoteMeDirectConnector->loop();
	}

}



void RemoteMe::disconnect() {
	this->connector->disconnect();
}

long RemoteMe::deltaMillis() {
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

		DEBUG_REMOTEME("[RMM] sending variables remoteme \n");
		send(data, size);
		free(data);
	}
	else {
		DEBUG_REMOTEME("[RMM] variables are null \n");
	}


}

void RemoteMe::sendDecreaseWebPageTokenCreditMessage(uint16_t sessionId, int16_t credit, int16_t time) {
	DEBUG_REMOTEME("[RMM] sendDecreaseWebPageTokenCreditMessage \n");
	uint16_t size = 12;
	uint8_t* payload = (uint8_t*)malloc(size);
	uint16_t pos = 0;
	RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::DECREASE_WEBPAGE_TOKEN_CREDIT);
	RemoteMeMessagesUtils::putUint16(payload, pos, 8);
	RemoteMeMessagesUtils::putUint16(payload, pos, deviceId);
	RemoteMeMessagesUtils::putUint16(payload, pos, sessionId);
	RemoteMeMessagesUtils::putInt16(payload, pos, credit);
	RemoteMeMessagesUtils::putInt16(payload, pos, time);
	
	send(payload, size);
	free(payload);
}
