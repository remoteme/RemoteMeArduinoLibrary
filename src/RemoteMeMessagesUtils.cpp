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

void RemoteMeMessagesUtils::putUint8(uint8_t* data, uint16_t &pos, uint8_t number) {
	data[pos++] = number;
}

void RemoteMeMessagesUtils::putInt8(uint8_t* data, uint16_t &pos, int8_t number) {
	data[pos++] = number;
}

void RemoteMeMessagesUtils::putBoolean(uint8_t* data, uint16_t &pos, boolean value){
	RemoteMeMessagesUtils::putInt8(data, pos, value? 1:0);
}


void RemoteMeMessagesUtils::putUint16(uint8_t* data, uint16_t &pos, uint16_t number) {
	putBigEndian(data, pos, &number, sizeof(uint16_t));
}

void RemoteMeMessagesUtils::putInt16(uint8_t* data, uint16_t &pos, int16_t number) {
	putBigEndian(data, pos, &number, sizeof(int16_t));
}

void RemoteMeMessagesUtils::putUint64(uint8_t* data, uint16_t &pos, uint64_t number) {
	putBigEndian(data, pos, &number, sizeof(uint64_t));
}

void RemoteMeMessagesUtils::putUint32(uint8_t* data, uint16_t &pos, uint32_t number) {
	putBigEndian(data, pos, &number, sizeof(uint32_t));
}
void RemoteMeMessagesUtils::putInt32(uint8_t* data, uint16_t &pos, int32_t number) {
	putBigEndian(data, pos, &number, sizeof(int32_t));
}



void RemoteMeMessagesUtils::putBigEndian(uint8_t* data, uint16_t &pos, void* value, uint16_t size) {
	uint8_t* reversed=getReverseBytes(value, size);
	
	putArray(data, pos, reversed, size);
	free(reversed);
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

uint16_t RemoteMeMessagesUtils::getUint16(uint8_t* payload, uint16_t& pos) {

	uint8_t* temp = getArray(payload, pos, sizeof(uint16_t));

	reverseBytes(temp, sizeof(uint16_t));
	pos += sizeof(uint16_t);
	
	uint16_t retX=*((uint16_t*)temp);
	free(temp);
	return  retX;
}


int16_t RemoteMeMessagesUtils::getInt16(uint8_t* payload, uint16_t& pos) {

	uint8_t* temp = getArray(payload, pos, sizeof(int16_t));

	reverseBytes(temp, sizeof(int16_t));
	pos += sizeof(int16_t);
	
	int16_t retX=*((int16_t*)temp);
	free(temp);
	return  retX;

}




int32_t RemoteMeMessagesUtils::getInt32(uint8_t* payload, uint16_t& pos) {

	uint8_t* temp = getArray(payload, pos, sizeof(int32_t));

	reverseBytes(temp, sizeof(int32_t));
	pos += sizeof(int32_t);

	int32_t retX = *((int32_t*)temp);
	free(temp);
	return  retX;

}

uint32_t RemoteMeMessagesUtils::getUint32(uint8_t *payload, uint16_t& pos) {
	uint8_t* temp = getArray(payload, pos, sizeof(uint32_t));
	reverseBytes(temp, sizeof(uint32_t));
	pos += sizeof(uint32_t);
	
	uint32_t retX=*((uint32_t*)temp);
	free(temp);
	return  retX;
}

double RemoteMeMessagesUtils::getDouble(uint8_t *payload, uint16_t& pos) {
	uint8_t* temp = getArray(payload, pos, sizeof(double));
	reverseBytes(temp, sizeof(double));
	pos += sizeof(double);
	
	double retX=*((double*)temp);
	free(temp);
	return  retX;
}
uint64_t RemoteMeMessagesUtils::getInt64(uint8_t *payload, uint16_t& pos) {
	uint8_t* temp = getArray(payload, pos, sizeof(uint64_t));
	reverseBytes(temp, sizeof(uint64_t));
	pos += sizeof(uint64_t);
	
	uint64_t retX=*((uint64_t*)temp);
	free(temp);
	return  retX;

}

String RemoteMeMessagesUtils::getString(uint8_t* data, uint16_t& pos) {

	String ret = String((char*)(data + pos));
	pos += ret.length() + 1;

	return  ret;
}



uint8_t RemoteMeMessagesUtils::getUint8(uint8_t* data, uint16_t& pos) {
	return  data[pos++];
}

int8_t RemoteMeMessagesUtils::getInt8(uint8_t* data, uint16_t& pos) {
	return  data[pos++];
}

void RemoteMeMessagesUtils::reverseBytes(uint8_t*  arr, uint16_t size) {
	uint8_t temp;
	for (uint16_t i = 0; i < size/2; ++i) {
		temp = arr[size-i-1];
		arr[size-i-1] = arr[i];
		arr[i] = temp;
	}
}
uint8_t* RemoteMeMessagesUtils::getReverseBytes(void* start, uint16_t size) {
	uint8_t* ret = (uint8_t*)malloc(size);

	for (uint8_t i = 0; i < size; i++) {
		ret[i] = ((uint8_t*)start)[size - i - 1];

	}


	return ret;
}

uint16_t RemoteMeMessagesUtils::getSyncUserMessage(uint16_t receiverDeviceId, uint16_t senderDeviceId, uint64_t messageId, const uint8_t *data, uint16_t length, uint8_t* &payload) {
	uint16_t size = 12 + length;
	payload = (uint8_t*)malloc(size+4);
	uint16_t index = 0;
	RemoteMeMessagesUtils::putUint16(payload, index, RemotemeStructures::USER_SYNC_MESSAGE);
	RemoteMeMessagesUtils::putUint16(payload, index, size);

	RemoteMeMessagesUtils::putUint16(payload, index, receiverDeviceId);
	RemoteMeMessagesUtils::putUint16(payload, index, senderDeviceId);
	RemoteMeMessagesUtils::putUint64(payload, index, messageId);


	RemoteMeMessagesUtils::putArray(payload, index, data, length);

	return size + 4;
}

uint16_t RemoteMeMessagesUtils::getUserMessage(RemotemeStructures::WSUserMessageSettings renevalWhenFailType, uint16_t receiverDeviceId, uint16_t senderDeviceId, uint16_t messageId, const uint8_t *data, uint16_t length, uint8_t* &payload) {
	uint16_t size = 7 + length;
	payload = (uint8_t*)malloc(size+4);


	uint16_t index = 0;


	RemoteMeMessagesUtils::putUint16(payload, index, RemotemeStructures::USER_MESSAGE);
	RemoteMeMessagesUtils::putUint16(payload, index, size);

	RemoteMeMessagesUtils::putUint8(payload, index, renevalWhenFailType);
	RemoteMeMessagesUtils::putUint16(payload, index, receiverDeviceId);
	RemoteMeMessagesUtils::putUint16(payload, index, senderDeviceId);
	RemoteMeMessagesUtils::putUint16(payload, index, messageId);

	RemoteMeMessagesUtils::putArray(payload, index, data, length);

	return size+4;

}


uint16_t  RemoteMeMessagesUtils::getAddDataMessage(uint16_t seriesId, RemotemeStructures::AddDataMessageSetting settings, uint64_t time, double value, uint8_t* &payload)
{
	uint16_t size = 2 + 1 + 8 + 8;
	payload = (uint8_t*)malloc(size+4);


	uint16_t pos = 0;


	RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::ADD_DATA);
	RemoteMeMessagesUtils::putUint16(payload, pos, size);

	RemoteMeMessagesUtils::putUint64(payload, pos, time);
	RemoteMeMessagesUtils::putUint8(payload, pos, settings);

	RemoteMeMessagesUtils::putUint16(payload, pos, seriesId);
	RemoteMeMessagesUtils::putDouble(payload, pos, value);

	return size+4;

}

uint16_t RemoteMeMessagesUtils::getSyncResponseMessage(uint64_t messageId, uint16_t dataSize, uint8_t * data, uint8_t* &payload)
{
	uint16_t size = 8 + dataSize;

	uint16_t pos = 0;

	payload = (uint8_t*)malloc(size+4);
	RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::SYNC_RESPONSE_MESSAGE);
	RemoteMeMessagesUtils::putUint16(payload, pos, size);



	RemoteMeMessagesUtils::putUint64(payload, pos, messageId);
	RemoteMeMessagesUtils::putArray(payload, pos, data, dataSize);

	return size+4;


}



uint16_t   RemoteMeMessagesUtils::getRegisterDeviceMessage(uint16_t deviceId, String deviceName, RemotemeStructures::DeviceType deviceType, RemotemeStructures::NetworkDeviceType networkDeviceType, uint8_t* &payload) {
	uint16_t size = 2 + 2 + deviceName.length() + 1 + 1;
	payload = (uint8_t*)malloc(size+4);


	uint16_t pos = 0;


	RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::REGISTER_DEVICE);
	RemoteMeMessagesUtils::putUint16(payload, pos, size);

	RemoteMeMessagesUtils::putUint16(payload, pos, deviceId);
	RemoteMeMessagesUtils::putString(payload, pos, deviceName);
	RemoteMeMessagesUtils::putUint8(payload, pos, deviceType);
	RemoteMeMessagesUtils::putUint16(payload, pos, networkDeviceType);





	return size+4;

}
uint16_t RemoteMeMessagesUtils::getLogMessage(RemotemeStructures::LogLevel logLevel, String str, uint8_t* &payload) {
	uint16_t size = 2 + str.length();
	payload = (uint8_t*)malloc(size+4);

	uint16_t pos = 0;

	RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::LOGG);
	RemoteMeMessagesUtils::putUint16(payload, pos, size);


	RemoteMeMessagesUtils::putUint8(payload, pos, logLevel);
	RemoteMeMessagesUtils::putString(payload, pos, str);

	return size+4;
}

uint16_t RemoteMeMessagesUtils::getRegisterChildDeviceMessage(uint16_t parentDeviceId, uint16_t deviceId, String deviceName, uint8_t* &payload) {
	uint16_t size = 5 + deviceName.length();//4 short  + 1 for 0 for string
	payload = (uint8_t*)malloc(size+4);


	uint16_t pos = 0;


	RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::REGISTER_CHILD_DEVICE);
	RemoteMeMessagesUtils::putUint16(payload, pos, size);

	RemoteMeMessagesUtils::putUint16(payload, pos, parentDeviceId);
	RemoteMeMessagesUtils::putUint16(payload, pos, deviceId);
	RemoteMeMessagesUtils::putString(payload, pos, deviceName);



	return size+4;
}
uint16_t RemoteMeMessagesUtils::getAuthentificateMessage(uint16_t deviceId, String token, uint8_t* &payload){
	uint16_t size = 2 + token.length()+1;//2 short  
	payload = (uint8_t*)malloc(size+4);


	uint16_t pos = 0;


	RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::AUTHENTIFICATE);
	RemoteMeMessagesUtils::putUint16(payload, pos, size);

	RemoteMeMessagesUtils::putUint16(payload, pos, deviceId);
	RemoteMeMessagesUtils::putString(payload, pos, token);
	
	return size+4;
}

