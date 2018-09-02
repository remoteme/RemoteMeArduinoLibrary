// RemoteMe.h

#include <Arduino.h>



#ifndef _REMOTEME_MESSAGES_Utils_h
#define _REMOTEME_MESSAGES_Utils_h

namespace RemotemeStructures
{
	enum VariableOberverType {
		BOOLEAN=0, INTEGER=1,TEXT=2,SMALL_INTEGER_3=3,SMALL_INTEGER_2=4,INTEGER_BOOLEAN=5,DOUBLE=6,TEXT_2=7
	};
	

	enum MessageType {
		USER_MESSAGE = 100, USER_MESSAGE_DELIVER_STATUS = 101, USER_SYNC_MESSAGE = 102,
		VARIABLE_CHANGE_MESSAGE= 103, VARIABLE_CHANGE_PROPAGATE_MESSAGE = 104,
		SYNC_MESSAGE = 120, SYNC_RESPONSE_MESSAGE = 121, 
		VARIABLE_OBSERVE_MESSAGE =122,
		WEBRTC_MESSAGE = 150,
		REGISTER_DEVICE = 200, REGISTER_CHILD_DEVICE = 201, ADD_DATA = 300,

		UPDATEFILE = 400,
		LOGG = 20000, SYSTEM_MESSAGE = 20001
	};


	enum WSUserMessageSettings { NO_RENEWAL = 0, RENEWAL_IF_FAILED = 1 };


	enum AddDataMessageSetting { NO_ROUND = 0, _1S = 1, _2S = 2, _5S = 3, _10S = 4, _15S = 5, _20S = 6, _30S = 7 ,_1M=8,_5M=9,_10M=10,_15M=11,_30M=12,_1H=13,_2H=13,_3H=13,_4H=14,_5H=15,_6H=16};
	enum DeviceType { NETWORK = 1, SMARTPHONE = 2, WEBPAGE = 3, JSSCRIPT = 4 };
	enum LogLevel { INFO = 1, WARN = 2, ERROR = 3 };
	enum LeafDeviceType { LD_OTHER_SOCKET = 1, LD_EXTERNAL_SCRIPT = 2, LD_SERIAL = 3, LD_NRF24 = 4, LD_GPIO = 5 };
	enum NetworkDeviceType { ND_UNDEFINED = 0, ND_RASPBERRY_PI = 1, ND_ARDUINO = 2 };

	enum SyncMessageType { USER = 0, GET_WEBRTC_CONENCTED_DEVICE_ID = 1, GET_FILES = 2, GET_FILE_CONTENT = 3, SAVE_FILE_CONTENT = 4, REMOVE_FILE = 5, REMOVE_DEVICE_DIRECTORY = 9, GET_FILE_SIZE = 6, RENAME_FILE = 7, GET_CONNECTED_DEVICES = 8,GET_WEBSOCKET_SERVER_LOCAL=10 };
	enum SystemMessageType { RESTART = 1, DEVICE_CONNECT_CHANGE = 2 };


}




	class RemoteMeMessagesUtils
	{
	

	private:
		static uint8_t *getReverseBytes(void *start, uint16_t size);
		static void reverseBytes(uint8_t*  arr, uint16_t size);
	public:
	
		static void putUint16(uint8_t* data, uint16_t &pos, uint16_t number);
		static void putInt16(uint8_t* data, uint16_t &pos, int16_t number);
		static void putUint8(uint8_t* data, uint16_t &pos, uint8_t number);
		static void putInt8(uint8_t* data, uint16_t &pos, int8_t number);
		static void putBoolean(uint8_t* data, uint16_t &pos, boolean val);
	
		static void putArray(uint8_t* data, uint16_t &pos, const void* number, uint16_t length);
		static void putString(uint8_t * data, uint16_t &pos, String string);
		static void putUint64(uint8_t * data, uint16_t &pos, uint64_t number);
		static void putUint32(uint8_t * data, uint16_t &pos, uint32_t number);
		static void putInt32(uint8_t * data, uint16_t &pos, int32_t number);
		static void putDouble(uint8_t * data, uint16_t &pos, double value);
		static void putFloat(uint8_t * data, uint16_t &pos, float value);
		
		static void putBigEndian(uint8_t * data, uint16_t &pos, void * value, uint16_t size);
		
		static uint8_t *getArray(uint8_t *data, uint16_t &pos, uint16_t length);
		static uint16_t getUint16(uint8_t *payload, uint16_t &pos);
		static int16_t getInt16(uint8_t* payload, uint16_t& pos);
		static uint32_t getUint32(uint8_t *payload, uint16_t &pos);
		static int32_t getInt32(uint8_t *payload, uint16_t &pos);
		static uint8_t getUint8(uint8_t* data, uint16_t& pos);
		static int8_t getInt8(uint8_t* data, uint16_t &pos);
		static String getString(uint8_t* data, uint16_t& pos);
		static uint64_t getInt64(uint8_t *payload, uint16_t& pos);
		static double getDouble(uint8_t *payload, uint16_t& pos);

		static uint16_t getUserMessage(RemotemeStructures::WSUserMessageSettings renevalWhenFailType, uint16_t receiverDeviceId, uint16_t senderDeviceId, uint16_t messageId, const uint8_t *data, uint16_t length, uint8_t* &payload);
		static uint16_t getSyncUserMessage(uint16_t receiverDeviceId, uint16_t senderDeviceId, uint64_t messageId, const uint8_t *data, uint16_t length, uint8_t* &payload);
		static uint16_t getAddDataMessage(uint16_t seriesId, RemotemeStructures::AddDataMessageSetting settings, uint64_t time, double value, uint8_t* &payload);
		static uint16_t getSyncResponseMessage(uint64_t messageId, uint16_t dataSize, uint8_t * data, uint8_t* &payload);
		static uint16_t getRegisterDeviceMessage(uint16_t deviceId, String deviceName, RemotemeStructures::DeviceType deviceType, RemotemeStructures::NetworkDeviceType networkDeviceType, uint8_t* &payload);
		static uint16_t getLogMessage(RemotemeStructures::LogLevel logLevel, String str, uint8_t* &payload);
		static uint16_t getRegisterChildDeviceMessage(uint16_t parentDeviceId, uint16_t deviceId, String deviceName, uint8_t* &payload);

		static uint16_t sendVariableObserveMessage(uint16_t deviceId,String name, uint16_t type, uint8_t* &payload);
	};


#endif

	