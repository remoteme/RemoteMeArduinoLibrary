// RemoteMe.h

#include <ArduinoHttpClient.h>
#include <WebSocketsClient.h>
#include <Hash.h>

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


	enum AddDataMessageSetting { NO_ROUND = 0, _1S = 1, _2S = 2, _5S = 3, _10S = 4, _15S = 5, _20S = 6, _30S = 7 };
	enum DeviceType { NETWORK = 1, SMARTPHONE = 2, WEBPAGE = 3, JSSCRIPT = 4 };
	enum LogLevel { INFO = 1, WARN = 2, ERROR = 3 };
	enum LeafDeviceType { LD_OTHER_SOCKET = 1, LD_EXTERNAL_SCRIPT = 2, LD_SERIAL = 3, LD_NRF24 = 4, LD_GPIO = 5 };
	enum NetworkDeviceType { ND_UNDEFINED = 0, ND_RASPBERRY_PI = 1, ND_ARDUINO = 2 };

	enum SyncMessageType { USER = 0, GET_WEBRTC_CONENCTED_DEVICE_ID = 1, GET_FILES = 2, GET_FILE_CONTENT = 3, SAVE_FILE_CONTENT = 4, REMOVE_FILE = 5, REMOVE_DEVICE_DIRECTORY = 9, GET_FILE_SIZE = 6, RENAME_FILE = 7, GET_CONNECTED_DEVICES = 8 };
	enum SystemMessageType { RESTART = 1, DEVICE_CONNECT_CHANGE = 2 };


}

#ifndef _REMOTEME_h
#define _REMOTEME_h

#define LED     D5   

#define REMOTEME_HOST "192.168.0.30"
#define REMOTEME_PORT 8082


	class RemoteMe
	{
		const char * token;
		uint16_t deviceId;
		bool webSocketConnected;

		WebSocketsClient* webSocket = nullptr;


	private:
		bool twoWayCommunicationEnabled = false;

		RemoteMe(char * token, uint16_t deviceId);

		void static putShort(uint8_t* data, uint16_t &pos, uint16_t number);
		void static putByte(uint8_t* data, uint16_t &pos, uint8_t number);
		void static putArray(uint8_t* data, uint16_t &pos, const void* number, uint16_t length);
		void static putString(uint8_t * data, uint16_t &pos, String string);
		void static putLong(uint8_t * data, uint16_t &pos, uint64_t number);
		void static putDouble(uint8_t * data, uint16_t &pos, double value);
		void static putBigEndian(uint8_t * data, uint16_t &pos, void * value, uint16_t size);


		static uint8_t *getReverseBytes(void *start, uint16_t size);
		static uint8_t *getArray(uint8_t *data, uint16_t &pos, uint16_t length);
		static uint16_t getShort(uint8_t *payload, uint16_t &pos);
		static uint32_t getInt(uint8_t *payload, uint16_t &pos);
		static uint8_t getByte(uint8_t* data, uint16_t& pos);
		static String getString(uint8_t* data, uint16_t& pos);
		static uint64_t getLong(uint8_t *payload, uint16_t& pos);
		static double getDouble(uint8_t *payload, uint16_t& pos);
		
		
		void(*onUserMessage)(uint16_t senderDeviceId , uint16_t dataSize , uint8_t* data) = nullptr;
		void(*onUserSyncMessage)(uint16_t senderDeviceId, uint16_t dataSize, uint8_t*, uint16_t& returnDataSize, uint8_t*& returnData ) = nullptr;

		void sendSyncResponseUserMessage(uint64_t messageId, uint16_t dataSize, uint8_t* data);

		void createWebSocket();
		void waitForWebSocketConnect();
	protected:
		void sendByWebSocket(uint8_t * payload, size_t length);
		void sendByRest(uint8_t * payload, size_t length);
	public:
		static RemoteMe& getInstance(char * token, int deviceId)
		{
			static RemoteMe    instance(token, deviceId); // Guaranteed to be destroyed.
								  // Instantiated on first use.
			return instance;
		}


		RemoteMe(RemoteMe const&) = delete;
		void operator=(RemoteMe const&) = delete;

		static void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);
		void setupTwoWayCommunication();
		void loop();

		
		

		void sendAddDataMessage(uint16_t seriesId, RemotemeStructures::AddDataMessageSetting settings, uint64_t time, double value);

		void sendUserMessage(RemotemeStructures::WSUserMessageSettings renevalWhenFailType, uint16_t receiverDeviceId, uint16_t messageId, const uint8_t * payload, uint16_t length);
		void sendUserMessage(RemotemeStructures::WSUserMessageSettings renevalWhenFailType, uint16_t receiverDeviceId, uint16_t senderDeviceId, uint16_t messageId, const uint8_t *payload, uint16_t length);
		void sendUserMessage(RemotemeStructures::WSUserMessageSettings renevalWhenFailType, uint16_t receiverDeviceId, uint16_t messageId, String message);
		void sendUserMessage(RemotemeStructures::WSUserMessageSettings renevalWhenFailType, uint16_t receiverDeviceId, uint16_t senderDeviceId, uint16_t messageId, String message);
		void sendUserMessage(uint16_t receiverDeviceId, String message);
		void sendUserMessage(uint16_t receiverDeviceId, const uint8_t * payload, uint16_t length);

		void sendRegisterDeviceMessage(uint16_t deviceId, String deviceName, RemotemeStructures::DeviceType deviceType, RemotemeStructures::NetworkDeviceType networkDeviceType);
		void sendRegisterDeviceMessage(String deviceName);
	

		void sendRegisterChildDeviceMessage(uint16_t parentDeviceId, uint16_t deviceId, String deviceName);
		void sendRegisterChildDeviceMessage(uint16_t deviceId, String deviceName);
		void sendRegisterChildDeviceMessage(uint16_t deviceId);
		void sendLogMessage(RemotemeStructures::LogLevel logLevel, String str);

		void setUserMessageListener(void(*onUserMessage)(uint16_t senderDeviceId, uint16_t dataSize, uint8_t* data));
		void setUserSyncMessageListener(void(*onUserSyncMessage)(uint16_t senderDeviceId, uint16_t dataSize, uint8_t*, uint16_t& returnDataSize, uint8_t*& returnData));



		template<typename... Args> void sendLogMessage(RemotemeStructures::LogLevel logLevel, const char* fmt, Args... args) {
			char* buf = new char[100];
			int cx = snprintf(buf, 100, fmt, args...);
			if (cx > 100) {
				buf = new char[cx];
				snprintf(buf, cx, fmt, args...);
			}
			sendLogMessage(logLevel, String(buf));
		}


		template<typename... Args> void sendLogMessage(const char* fmt, Args... args) {
			sendLogMessage(RemotemeStructures::INFO, fmt, args...);
		}



	};


#endif

	