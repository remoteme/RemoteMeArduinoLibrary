// RemoteMe.h
//----------------------------------------
//#define DIRECT_CONNECTIONS
//#define REST_CONNECTIONS

#ifdef STM32_DEVICE
	#include <application.h>
	#define bit(b) (1UL << (b)) // Taken directly from Arduino.h
#else
	#include <Arduino.h>
#endif

#include <functional>


#define NETWORK_ESP8266_ASYNC   (0)
#define NETWORK_ESP8266         (1)
#define NETWORK_W5100           (2)
#define NETWORK_ENC28J60        (3)
#define NETWORK_ESP32           (4)



#if !defined(SOCKETS_NETWORK_TYPE)
// select Network type based
	#if defined(ESP8266) || defined(ESP31B)
		#define SOCKETS_NETWORK_TYPE NETWORK_ESP8266
		//#define SOCKETS_NETWORK_TYPE NETWORK_ESP8266_ASYNC
		//#define SOCKETS_NETWORK_TYPE NETWORK_W5100

	#elif defined(ESP32)
		#define SOCKETS_NETWORK_TYPE NETWORK_ESP32

	#else
		#define SOCKETS_NETWORK_TYPE NETWORK_W5100

	#endif
#endif

// Includes and defined based on Network Type
#if (SOCKETS_NETWORK_TYPE == NETWORK_ESP8266_ASYNC)

// Note:
//   No SSL/WSS support for client in Async mode
//   TLS lib need a sync interface!


	#if defined(ESP8266)
		#include <ESP8266WiFi.h>
	#elif defined(ESP32)
		#include <WiFi.h>
		#include <WiFiClientSecure.h>
	#elif defined(ESP31B)
		#include <ESP31BWiFi.h>
	#else
		#error "network type ESP8266 ASYNC only possible on the ESP mcu!"
	#endif

	#include <ESPAsyncTCP.h>
	#include <ESPAsyncTCPbuffer.h>
	#define WEBSOCKETS_NETWORK_CLASS AsyncTCPbuffer


#elif (SOCKETS_NETWORK_TYPE == NETWORK_ESP8266)

	#if !defined(ESP8266) && !defined(ESP31B)
		#error "network type ESP8266 only possible on the ESP mcu!"
	#endif

	#ifdef ESP8266
		#include <ESP8266WiFi.h>
	#else
		#include <ESP31BWiFi.h>
	#endif

	#define WEBSOCKETS_NETWORK_CLASS WiFiClient


#elif (SOCKETS_NETWORK_TYPE == NETWORK_W5100)

	#ifdef STM32_DEVICE
		#define WEBSOCKETS_NETWORK_CLASS TCPClient

	#else
		#include <Ethernet.h>
		#include <SPI.h>
		#define WEBSOCKETS_NETWORK_CLASS EthernetClient

	#endif

#elif (SOCKETS_NETWORK_TYPE == NETWORK_ENC28J60)

	#include <UIPEthernet.h>
	#define WEBSOCKETS_NETWORK_CLASS UIPClient


#elif (SOCKETS_NETWORK_TYPE == NETWORK_ESP32)

	#include <WiFi.h>
	#include <WiFiClientSecure.h>
	#define WEBSOCKETS_NETWORK_CLASS WiFiClient


#else
	#error "no network type selected!"
#endif


//----------------------------------------

#include <Arduino.h>

#ifdef  DIRECT_CONNECTIONS
	#include <WebSocketsServer.h>
#endif
#ifdef REST_CONNECTIONS
	#include <ArduinoHttpClient.h>
#endif


#ifndef _REMOTEME_h
#define _REMOTEME_h


#define REMOTEME_HOST "app.remoteme.org"
#define REMOTEME_HTTP_PORT 80

#define REMOTEME_SOCKET_PORT 18
#ifndef LOCAL_SERVER_PORT
	#define LOCAL_SERVER_PORT 80
#endif

#include "Variables.h"
#include "RemoteMeMessagesUtils.h"


	class Variables;

	class RemoteMe
	{
		const char * token;
		uint16_t deviceId;

		bool socketConnected = false;

		Variables* variables = nullptr;

	
		#ifdef  DIRECT_CONNECTIONS
			WebSocketsServer* webSocketServer = nullptr;
		#endif
		
		WiFiClient* wifiClient = nullptr;

		uint64_t messageId = 0;//used for reponse
		uint8_t* syncResponseData; //used for reponse
		uint16_t syncResponseDataSize;

	private:

		bool socketEnabled = false;

		RemoteMe(char * token, uint16_t deviceId);

		
		void socketLoop();

		
		void processMessage(uint8_t *payload);
		
		void(*onUserMessage)(uint16_t senderDeviceId , uint16_t dataSize , uint8_t* data) = nullptr;
		void(*onUserSyncMessage)(uint16_t senderDeviceId, uint16_t dataSize, uint8_t*, uint16_t& returnDataSize, uint8_t*& returnData ) = nullptr;

		void sendSyncResponseMessage(uint64_t messageId, uint16_t dataSize, uint8_t* data);


		bool ping();
		void waitForConnection();
		long deltaMillis();
	protected:
		#ifdef  REST_CONNECTIONS
		void sendByRest(uint8_t * payload, uint16_t length);
		String callRest(String restUrl);
		#endif
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
		void setupTwoWayCommunication();
			
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

		void sendVariableObserveMessage(String name, uint16_t type);


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

	