// RemoteMe.h

#include <ArduinoHttpClient.h>
#include <WebSocketsClient.h>
#include <Hash.h>



#ifndef _REMOTEME_h
#define _REMOTEME_h


#define REMOTEME_HOST "app.remoteme.org"
#define REMOTEME_PORT 80

#include "RemoteMeMessagesUtils.h"
	class RemoteMe
	{
		const char * token;
		uint16_t deviceId;
		bool webSocketConnected;

		WebSocketsClient* webSocket = nullptr;


	private:
		bool twoWayCommunicationEnabled = false;

		RemoteMe(char * token, uint16_t deviceId);

		


		
		
		
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

		
		String callRest(String restUrl);


		template<typename... Args> void sendLogMessage(RemotemeStructures::LogLevel logLevel, const char* fmt, Args... args) {
			char* buf = new char[100];
			int cx = snprintf(buf, 100, fmt, args...);
			if (cx > 100) {
				buf = new char[cx];
				snprintf(buf, cx, fmt, args...);
			}
			sendLogMessage(logLevel, String(buf));
		}

		
		void static putShort(uint8_t* data, uint16_t &pos, uint16_t number);
		void static putByte(uint8_t* data, uint16_t &pos, uint8_t number);
		void static putArray(uint8_t* data, uint16_t &pos, const void* number, uint16_t length);
		void static putString(uint8_t * data, uint16_t &pos, String string);
		void static putLong(uint8_t * data, uint16_t &pos, uint64_t number);
		void static putInt(uint8_t * data, uint16_t &pos, uint32_t number);
		void static putDouble(uint8_t * data, uint16_t &pos, double value);
		void static putFloat(uint8_t * data, uint16_t &pos, float value);
		
		void static putBigEndian(uint8_t * data, uint16_t &pos, void * value, uint16_t size);
		
		static uint8_t *getArray(uint8_t *data, uint16_t &pos, uint16_t length);
		static uint16_t getShort(uint8_t *payload, uint16_t &pos);
		static uint32_t getInt(uint8_t *payload, uint16_t &pos);
		static uint8_t getByte(uint8_t* data, uint16_t& pos);
		static String getString(uint8_t* data, uint16_t& pos);
		static uint64_t getLong(uint8_t *payload, uint16_t& pos);
		static double getDouble(uint8_t *payload, uint16_t& pos);

		template<typename... Args> void sendLogMessage(const char* fmt, Args... args) {
			sendLogMessage(RemotemeStructures::INFO, fmt, args...);
		}



	};


#endif

	