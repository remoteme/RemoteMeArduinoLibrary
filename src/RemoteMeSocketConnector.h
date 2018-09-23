

#ifdef STM32_DEVICE
#include <application.h>
#define bit(b) (1UL << (b)) // Taken directly from Arduino.h
#else
#include <Arduino.h>
#endif

#include <functional>

#define PING_SEND 40000
#define PING_RECEIVE_TIMEOUT 80000

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

#include <Arduino.h>

#include <RemoteMeConnector.h>

#ifndef _REMOTEMESocketConnector_h
#define _REMOTEMESocketConnector_h


#define REMOTEME_HOST "app.remoteme.org"
#define REMOTEME_HTTP_PORT 80

class RemoteMe;

class RemoteMeSocketConnector: public RemoteMeConnector
	{

		bool socketConnected = false;
		WiFiClient* wifiClient = nullptr;
	
	public:

		String  getIp();
		long lastTimePingReceived;
		
		void loop();
		bool isSocketConnected();
		
		void ping();
		void waitForConnection();
	
		
		void disconnect();
		void send(uint8_t * payload, uint16_t size);
		

	};


#endif //_REMOTEMESocketConnector_h

	