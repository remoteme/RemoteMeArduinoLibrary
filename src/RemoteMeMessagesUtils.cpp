// 
// 
// 




#include "RemoteMeMessagesUtils.h"







void RemoteMeMessagesUtils::putString(uint8_t* data, uint16_t &pos, String string) {
	const uint8_t* dataString = reinterpret_cast<const uint8_t*>(&string[0]);
	putArray(data, pos, dataString, string.length());
	data[pos++] = 0;


}
void RemoteMeMessagesUtils::putArray(uint8_t* data, uint16_t &pos, const void* arrayT, uint16_t length) {

	memcpy(&data[pos], arrayT, length);

	pos += length;
}

void RemoteMeMessagesUtils::putByte(uint8_t* data, uint16_t &pos, uint8_t number) {
	data[pos++] = number;


}

void RemoteMeMessagesUtils::putShort(uint8_t* data, uint16_t &pos, uint16_t number) {
	putBigEndian(data, pos, &number, sizeof(uint16_t));
}

void RemoteMeMessagesUtils::putLong(uint8_t* data, uint16_t &pos, uint64_t number) {
	putBigEndian(data, pos, &number, sizeof(uint64_t));
}

void RemoteMeMessagesUtils::putInt(uint8_t* data, uint16_t &pos, uint32_t number) {
	putBigEndian(data, pos, &number, sizeof(uint32_t));
}



void RemoteMeMessagesUtils::putBigEndian(uint8_t* data, uint16_t &pos, void* value, uint16_t size) {
	putArray(data, pos, getReverseBytes(value, size), size);
}

void RemoteMeMessagesUtils::putDouble(uint8_t* data, uint16_t &pos, double value) {
	return putBigEndian(data, pos, &value, sizeof(double));
}

void RemoteMeMessagesUtils::putFloat(uint8_t * data, uint16_t &pos, float value) {
	return putBigEndian(data, pos, &value, sizeof(float));
}


uint8_t* RemoteMeMessagesUtils::getArray(uint8_t* data, uint16_t& pos, uint16_t length) {
	if (length != 0) {
		uint8_t* ret = (uint8_t*)malloc(length);
		memcpy(ret, &data[pos], length);
		return ret;
	}
	else {
		return (uint8_t*)malloc(1);
	};

}

uint16_t RemoteMeMessagesUtils::getShort(uint8_t* payload, uint16_t& pos) {

	uint8_t* temp = getArray(payload, pos, sizeof(uint16_t));

	temp = getReverseBytes(temp, sizeof(uint16_t));
	pos += sizeof(uint16_t);
	uint16_t* ret = (uint16_t*)temp;
	return  *ret;
}

uint32_t RemoteMeMessagesUtils::getInt(uint8_t *payload, uint16_t& pos) {
	uint8_t* temp = getArray(payload, pos, sizeof(uint32_t));
	temp = getReverseBytes(temp, sizeof(uint32_t));
	pos += sizeof(uint32_t);
	uint32_t* ret = (uint32_t*)temp;

	return  *ret;
}

double RemoteMeMessagesUtils::getDouble(uint8_t *payload, uint16_t& pos) {
	uint8_t* temp = getArray(payload, pos, sizeof(double));
	temp = getReverseBytes(temp, sizeof(double));
	pos += sizeof(double);
	double* ret = (double*)temp;

	return  *ret;
}
uint64_t RemoteMeMessagesUtils::getLong(uint8_t *payload, uint16_t& pos) {
	uint8_t* temp = getArray(payload, pos, sizeof(uint64_t));
	temp = getReverseBytes(temp, sizeof(uint64_t));
	pos += sizeof(uint64_t);
	uint64_t* ret = (uint64_t*)temp;

	//uint32_t u = *(uint32_t*)&x;

	return  *ret;
}

String RemoteMeMessagesUtils::getString(uint8_t* data, uint16_t& pos) {

	String ret = String((char*)(data + pos));
	pos += ret.length() + 1;

	return  ret;
}



uint8_t RemoteMeMessagesUtils::getByte(uint8_t* data, uint16_t& pos) {
	return  data[pos++];
}

int8_t RemoteMeMessagesUtils::getSignedByte(uint8_t* data, uint16_t& pos) {
	return  data[pos++];
}

uint8_t* RemoteMeMessagesUtils::getReverseBytes(void* start, uint16_t size) {
	uint8_t* ret = new uint8_t[size];

	for (uint8_t i = 0; i < size; i++) {
		ret[i] = ((uint8_t*)start)[size - i - 1];

	}


	return ret;
}

uint16_t RemoteMeMessagesUtils::getSyncUserMessage(uint16_t receiverDeviceId, uint16_t senderDeviceId, uint64_t messageId, const uint8_t *data, uint16_t length, uint8_t* &payload) {
	uint16_t size = 12 + length;
	payload = new uint8_t[size + 4];
	uint16_t index = 0;
	RemoteMeMessagesUtils::putShort(payload, index, RemotemeStructures::USER_SYNC_MESSAGE);
	RemoteMeMessagesUtils::putShort(payload, index, size);

	RemoteMeMessagesUtils::putShort(payload, index, receiverDeviceId);
	RemoteMeMessagesUtils::putShort(payload, index, senderDeviceId);
	RemoteMeMessagesUtils::putLong(payload, index, messageId);


	RemoteMeMessagesUtils::putArray(payload, index, data, length);

	return size + 4;
}

uint16_t RemoteMeMessagesUtils::getUserMessage(RemotemeStructures::WSUserMessageSettings renevalWhenFailType, uint16_t receiverDeviceId, uint16_t senderDeviceId, uint16_t messageId, const uint8_t *data, uint16_t length, uint8_t* &payload) {
	uint16_t size = 7 + length;
	payload = new uint8_t[size + 4];


	uint16_t index = 0;


	RemoteMeMessagesUtils::putShort(payload, index, RemotemeStructures::USER_MESSAGE);
	RemoteMeMessagesUtils::putShort(payload, index, size);

	RemoteMeMessagesUtils::putByte(payload, index, renevalWhenFailType);
	RemoteMeMessagesUtils::putShort(payload, index, receiverDeviceId);
	RemoteMeMessagesUtils::putShort(payload, index, senderDeviceId);
	RemoteMeMessagesUtils::putShort(payload, index, messageId);

	RemoteMeMessagesUtils::putArray(payload, index, data, length);

	return size+4;

}


uint16_t  RemoteMeMessagesUtils::getAddDataMessage(uint16_t seriesId, RemotemeStructures::AddDataMessageSetting settings, uint64_t time, double value, uint8_t* &payload)
{
	uint16_t size = 2 + 1 + 8 + 8;
	payload = new uint8_t[size + 4];


	uint16_t pos = 0;


	RemoteMeMessagesUtils::putShort(payload, pos, RemotemeStructures::ADD_DATA);
	RemoteMeMessagesUtils::putShort(payload, pos, size);

	RemoteMeMessagesUtils::putLong(payload, pos, time);
	RemoteMeMessagesUtils::putByte(payload, pos, settings);

	RemoteMeMessagesUtils::putShort(payload, pos, seriesId);
	RemoteMeMessagesUtils::putDouble(payload, pos, value);

	return size+4;

}

uint16_t RemoteMeMessagesUtils::getSyncResponseUserMessage(uint64_t messageId, uint16_t dataSize, uint8_t * data, uint8_t* &payload)
{
	uint16_t size = 8 + dataSize;

	uint16_t pos = 0;

	payload = new uint8_t[size + 4];
	RemoteMeMessagesUtils::putShort(payload, pos, RemotemeStructures::SYNC_RESPONSE_MESSAGE);
	RemoteMeMessagesUtils::putShort(payload, pos, dataSize);



	RemoteMeMessagesUtils::putLong(payload, pos, messageId);
	RemoteMeMessagesUtils::putArray(payload, pos, data, dataSize);

	return size+4;


}

uint16_t   RemoteMeMessagesUtils::getRegisterDeviceMessage(uint16_t deviceId, String deviceName, RemotemeStructures::DeviceType deviceType, RemotemeStructures::NetworkDeviceType networkDeviceType, uint8_t* &payload) {
	uint16_t size = 2 + 2 + deviceName.length() + 1 + 1;
	payload = new uint8_t[size + 4];


	uint16_t pos = 0;


	RemoteMeMessagesUtils::putShort(payload, pos, RemotemeStructures::REGISTER_DEVICE);
	RemoteMeMessagesUtils::putShort(payload, pos, size);

	RemoteMeMessagesUtils::putShort(payload, pos, deviceId);
	RemoteMeMessagesUtils::putString(payload, pos, deviceName);
	RemoteMeMessagesUtils::putByte(payload, pos, deviceType);
	RemoteMeMessagesUtils::putShort(payload, pos, networkDeviceType);





	return size+4;

}
uint16_t RemoteMeMessagesUtils::getLogMessage(RemotemeStructures::LogLevel logLevel, String str, uint8_t* &payload) {
	uint16_t size = 2 + str.length();
	payload = new uint8_t[size + 4];

	uint16_t pos = 0;

	RemoteMeMessagesUtils::putShort(payload, pos, RemotemeStructures::LOGG);
	RemoteMeMessagesUtils::putShort(payload, pos, size);


	RemoteMeMessagesUtils::putByte(payload, pos, logLevel);
	RemoteMeMessagesUtils::putString(payload, pos, str);

	return size+4;
}

uint16_t RemoteMeMessagesUtils::getRegisterChildDeviceMessage(uint16_t parentDeviceId, uint16_t deviceId, String deviceName, uint8_t* &payload) {
	uint16_t size = 5 + deviceName.length();//4 short  + 1 for 0 for string
	payload = new uint8_t[size + 4];


	uint16_t pos = 0;


	RemoteMeMessagesUtils::putShort(payload, pos, RemotemeStructures::REGISTER_CHILD_DEVICE);
	RemoteMeMessagesUtils::putShort(payload, pos, size);

	RemoteMeMessagesUtils::putShort(payload, pos, parentDeviceId);
	RemoteMeMessagesUtils::putShort(payload, pos, deviceId);
	RemoteMeMessagesUtils::putString(payload, pos, deviceName);



	return size+4;
}