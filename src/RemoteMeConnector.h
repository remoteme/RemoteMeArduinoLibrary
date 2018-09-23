
#include <Arduino.h>

#ifndef _REMOTEMEConnector_h
#define _REMOTEMEConnector_h


#define REMOTEME_HOST "app.remoteme.org"
#define REMOTEME_HTTP_PORT 80

#define REMOTEME_SOCKET_PORT 18
#ifndef LOCAL_SERVER_PORT
#define LOCAL_SERVER_PORT 80
#endif


#include <RemoteMe.h>

class RemoteMeConnector{
	friend RemoteMe;

	protected:
		RemoteMe* remoteMe;
		inline long deltaMillis() { return this->remoteMe->deltaMillis(); }
		inline uint16_t getDeviceId() { return this->remoteMe->getDeviceId(); }
		inline const char* getToken() { return this->remoteMe->getToken(); }
		inline void sendVariableObserveMessage() { this->remoteMe->sendVariableObserveMessage(); }
		inline void processMessage(uint8_t *payload) { this->remoteMe->processMessage(payload); }
		inline void setRemoteMe(RemoteMe* remoteMe) { this->remoteMe = remoteMe; };

		virtual String getIp() = 0;

		virtual void loop() = 0;
		virtual void waitForConnection() = 0;
		virtual void disconnect() = 0;
		virtual void send(uint8_t * payload, uint16_t size) = 0;
		
		
	};

#endif //_REMOTEMEConnector_h