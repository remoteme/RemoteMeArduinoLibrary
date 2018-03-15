// RemoteMe.h

#include <ArduinoHttpClient.h>
#include <WebSocketsClient.h>
#include <Hash.h>


#ifndef _REMOTEME_MESSAGES_Utils_h
#define _REMOTEME_MESSAGES_Utils_h

namespace RemotemeStructures
{

	enum MessageType {
		USER_MESSAGE = 100, USER_MESSAGE_DELIVER_STATUS = 101, USER_SYNC_MESSAGE = 102,
		SYNC_MESSAGE = 120, SYNC_RESPONSE_MESSAGE = 121, WEBRTC_MESSAGE = 150,
		REGISTER_DEVICE = 200, REGISTER_CHILD_DEVICE = 201, ADD_DATA = 300,

		UPDATEFILE = 400,
		LOGG = 20000, SYSTEM_MESSAGE = 20001
	};


	enum WSUserMessageSettings { NO_RENEWAL = 0, RENEWAL_IF_FAILED = 1 };


	enum AddDataMessageSetting { NO_ROUND = 0, _1S = 1, _2S = 2, _5S = 3, _10S = 4, _15S = 5, _20S = 6, _30S = 7 ,_1M=8,_5M=9,_10M=10,_15M=11,_30M=12,_1G=13,_2G=13,_3G=13,_4G=14,_5G=15,_6G=16};
	enum DeviceType { NETWORK = 1, SMARTPHONE = 2, WEBPAGE = 3, JSSCRIPT = 4 };
	enum LogLevel { INFO = 1, WARN = 2, ERROR = 3 };
	enum LeafDeviceType { LD_OTHER_SOCKET = 1, LD_EXTERNAL_SCRIPT = 2, LD_SERIAL = 3, LD_NRF24 = 4, LD_GPIO = 5 };
	enum NetworkDeviceType { ND_UNDEFINED = 0, ND_RASPBERRY_PI = 1, ND_ARDUINO = 2 };

	enum SyncMessageType { USER = 0, GET_WEBRTC_CONENCTED_DEVICE_ID = 1, GET_FILES = 2, GET_FILE_CONTENT = 3, SAVE_FILE_CONTENT = 4, REMOVE_FILE = 5, REMOVE_DEVICE_DIRECTORY = 9, GET_FILE_SIZE = 6, RENAME_FILE = 7, GET_CONNECTED_DEVICES = 8 };
	enum SystemMessageType { RESTART = 1, DEVICE_CONNECT_CHANGE = 2 };


}




	class RemoteMeMessagesUtils
	{
	

	private:
		static uint8_t *getReverseBytes(void *start, uint16_t size);
		
	public:
	
		static void putShort(uint8_t* data, uint16_t &pos, uint16_t number);
		static void putByte(uint8_t* data, uint16_t &pos, uint8_t number);
		static void putArray(uint8_t* data, uint16_t &pos, const void* number, uint16_t length);
		static void putString(uint8_t * data, uint16_t &pos, String string);
		static void putLong(uint8_t * data, uint16_t &pos, uint64_t number);
		static void putInt(uint8_t * data, uint16_t &pos, uint32_t number);
		static void putDouble(uint8_t * data, uint16_t &pos, double value);
		static void putFloat(uint8_t * data, uint16_t &pos, float value);
		
		static void putBigEndian(uint8_t * data, uint16_t &pos, void * value, uint16_t size);
		
		static uint8_t *getArray(uint8_t *data, uint16_t &pos, uint16_t length);
		static uint16_t getShort(uint8_t *payload, uint16_t &pos);
		static uint32_t getInt(uint8_t *payload, uint16_t &pos);
		static uint8_t getByte(uint8_t* data, uint16_t& pos);
		static String getString(uint8_t* data, uint16_t& pos);
		static uint64_t getLong(uint8_t *payload, uint16_t& pos);
		static double getDouble(uint8_t *payload, uint16_t& pos);

		

	};


#endif

	