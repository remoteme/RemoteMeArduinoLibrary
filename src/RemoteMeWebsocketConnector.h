


#include <Arduino.h>

#include <RemoteMeConnector.h>
#include <WebSocketsClient.h>

#ifndef _REMOTEMEWebsocketConnector_h
#define _REMOTEMEWebsocketConnector_h



class RemoteMe;
class RemoteMeWebsocketConnector;

class RemoteMeWebsocketConnector : public RemoteMeConnector
	{

	private:
		
		bool webSocketConnected = false;
		WebSocketsClient* webSocket = nullptr;
	
		static void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);
		void ping();
		long lastTimePingReceived;
	public:
		static RemoteMeWebsocketConnector*  thiz;


		RemoteMeWebsocketConnector(void);

		String  getIp();
		
		
		void loop();
		void waitForConnection();
		void disconnect();
		void send(uint8_t * payload, uint16_t size);
		

	};


#endif //_REMOTEMESocketConnector_h

	