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

		uint64_t messageId = 0;//used for reponse
		uint8_t* syncResponseData; //used for reponse
		uint16_t syncResponseDataSize;

	private:
		bool twoWayCommunicationEnabled = false;

		RemoteMe(char * token, uint16_t deviceId);

		


		
		
		
		void(*onUserMessage)(uint16_t senderDeviceId , uint16_t dataSize , uint8_t* data) = nullptr;
		void(*onUserSyncMessage)(uint16_t senderDeviceId, uint16_t dataSize, uint8_t*, uint16_t& returnDataSize, uint8_t*& returnData ) = nullptr;

		void sendSyncResponseUserMessage(uint64_t messageId, uint16_t dataSize, uint8_t* data);

		void createWebSocket();
		void waitForWebSocketConnect();
	protected:
		
		void sendByRest(uint8_t * payload, uint16_t length);
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
		void disconnect();
		void send(uint8_t * payload, uint16_t size);
		

		void sendAddDataMessage(uint16_t seriesId, RemotemeStructures::AddDataMessageSetting settings, uint64_t time, double value);

		uint16_t sendUserSyncMessage(uint16_t receiverDeviceId, const uint8_t * payload, uint16_t length, uint8_t*& returnData);

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

		
		

		template<typename... Args> void sendLogMessage(const char* fmt, Args... args) {
			sendLogMessage(RemotemeStructures::INFO, fmt, args...);
		}



	};


#endif

	