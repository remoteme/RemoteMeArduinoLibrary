// RemoteMe.h
//----------------------------------------
//#define DIRECT_CONNECTIONS
//#define REST_CONNECTIONS




//----------------------------------------

#include <Arduino.h>

#ifdef  DIRECT_CONNECTIONS
	#include <WebSocketsServer.h>
#endif



#ifndef _REMOTEME_h
#define _REMOTEME_h



#include "Variables.h"
#include "RemoteMeMessagesUtils.h"


	class Variables;
	class RemoteMeConnector;

	class RemoteMe
	{
		friend class RemoteMeConnector;

		const char* token;
		uint16_t deviceId;
		RemoteMeConnector* connector;
		

		Variables* variables = nullptr;

	
		#ifdef  DIRECT_CONNECTIONS
			WebSocketsServer* webSocketServer = nullptr;
		#endif
		
	

		uint64_t messageId = 0;//used for reponse
		uint8_t* syncResponseData; //used for reponse
		uint16_t syncResponseDataSize;

	private:

		

		RemoteMe(char * token, uint16_t deviceId);
	
		void processMessage(uint8_t *payload);
		
		void(*onUserMessage)(uint16_t senderDeviceId , uint16_t dataSize , uint8_t* data) = nullptr;
		void(*onUserSyncMessage)(uint16_t senderDeviceId, uint16_t dataSize, uint8_t*, uint16_t& returnDataSize, uint8_t*& returnData ) = nullptr;

		void sendSyncResponseMessage(uint64_t messageId, uint16_t dataSize, uint8_t* data);


		

		long deltaMillis();

	protected:
		void sendVariableObserveMessage();
	public:
		static RemoteMe& getInstance(char * token, int deviceId)
		{
			static RemoteMe    instance(token, deviceId); // Guaranteed to be destroyed.
								  // Instantiated on first use.
			return instance;
		}


		RemoteMe(RemoteMe const&) = delete;
		void operator=(RemoteMe const&) = delete;

		#ifdef  DIRECT_CONNECTIONS
		static void webSocketServerEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
		void setupDirectConnections();
		#endif
		void setConnector(RemoteMeConnector* remoteMeConnector);
		inline const char* getToken() { return token; }

		void loop();
		void disconnect();
		void send(uint8_t * payload, uint16_t size);
		
		uint16_t getDeviceId();
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

		//------------

		void sendVariableRegisterMessage(String name, uint16_t type);
	
		Variables* getVariables();

		//-----------
	

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

	