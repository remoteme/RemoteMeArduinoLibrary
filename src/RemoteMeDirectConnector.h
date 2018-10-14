
#include <Arduino.h>

#ifndef _REMOTEMEDirectConnector_h
#define _REMOTEMEDirectConnector_h



#define LOCAL_SERVER_PORT 80




#include <RemoteMe.h>

class RemoteMe;

class RemoteMeDirectConnector{
	friend RemoteMe;

	protected:
		RemoteMe* remoteMe;
		inline long deltaMillis() { return this->remoteMe->deltaMillis(); }
		inline uint16_t getDeviceId() { return this->remoteMe->getDeviceId(); }
		inline void processMessage(uint8_t *payload) { this->remoteMe->processMessage(payload); }
		inline void setRemoteMe(RemoteMe* remoteMe) { this->remoteMe = remoteMe; };

		virtual void loop() = 0;

		virtual std::list<uint16_t>* getDirectConnected() = 0;
		virtual bool send(uint16_t deviceId, uint8_t * payload, uint16_t size) = 0;
		virtual void send(uint8_t* payload, uint16_t size) = 0;
		
	};

#endif //_REMOTEMEDirectConnector_h