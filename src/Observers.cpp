// 
// 
// 


#include "Observers.h"



Observers::Observers(RemoteMe* remoteMe) {
	this->remoteMe = remoteMe;
}

void Observers::onChangePropagateMessage(uint8_t *payload) {
	
	uint16_t pos = 0;
	pos += 4;//type and size

	uint16_t senderDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);

	uint16_t count = RemoteMeMessagesUtils::getUint16(payload, pos);
	while(count!=0){
		count--;

		uint16_t type= RemoteMeMessagesUtils::getUint16(payload, pos);
		String name= RemoteMeMessagesUtils::getString(payload, pos);

		if (type == RemotemeStructures::VariableOberverType::BOOLEAN) {
			boolean value = RemoteMeMessagesUtils::getInt8(payload, pos) == 1;
			for (std::list<BooleanObserver>::iterator it = this->booleanObservers.begin(); it != this->booleanObservers.end(); ++it) {
				if ((*it).name == name) {
					(*it).toCall(value);
					break;
				}
			}

		}else if (type == RemotemeStructures::VariableOberverType::INTEGER) {
			int32_t value =RemoteMeMessagesUtils::getInt32(payload, pos);
			for (std::list<IntegerObserver>::iterator it = this->integerObservers.begin(); it != this->integerObservers.end(); ++it) {
				if ((*it).name == name) {
					(*it).toCall(value);
					break;
				}
			}

		}
	}
}
void Observers::observeBoolean(String name,void(*toCall)(boolean)) {
	BooleanObserver bo;
	bo.toCall = toCall;
	bo.name = name;
	this->booleanObservers.push_back(bo);

	this->remoteMe->sendObserverRegisterMessage(name, RemotemeStructures::VariableOberverType::BOOLEAN);

}

void Observers::observeInteger(String name, void(*toCall)(int32_t)) {
	IntegerObserver bo;
	bo.toCall = toCall;
	bo.name = name;
	this->integerObservers.push_back(bo);

	this->remoteMe->sendObserverRegisterMessage(name, RemotemeStructures::VariableOberverType::INTEGER);

}


uint16_t Observers::prepareSetMessage(uint8_t* &payload, uint16_t &pos, boolean ignoreCurrent, String name, uint16_t type, uint8_t additionalSize) {
	uint16_t size = 5 + name.length() +1+2 + additionalSize;

	if (ignoreCurrent) {
		size += 2;
	}

	payload = (uint8_t*)malloc(size + 4);


	RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::OBSERVER_CHANGE_MESSAGE);
	RemoteMeMessagesUtils::putUint16(payload, pos, size);

	RemoteMeMessagesUtils::putUint16(payload, pos, remoteMe->getDeviceId());
	
	if (ignoreCurrent){
		RemoteMeMessagesUtils::putUint8(payload, pos, 1);
		RemoteMeMessagesUtils::putUint16(payload, pos, remoteMe->getDeviceId());
	}
	else {
		RemoteMeMessagesUtils::putUint8(payload, pos, 0);
	}

	RemoteMeMessagesUtils::putUint16(payload, pos, 1);//we sends only one


	RemoteMeMessagesUtils::putUint16(payload, pos,type);

	RemoteMeMessagesUtils::putString(payload, pos, name);

	return size;

}
void Observers::setBoolean(String name, boolean value,boolean ignoreCurrent) {
	
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size =  prepareSetMessage(payload, pos, ignoreCurrent,name, RemotemeStructures::VariableOberverType::BOOLEAN ,1);


	RemoteMeMessagesUtils::putBoolean(payload, pos, value );

	remoteMe->send(payload, size+4);
	free(payload);
}

void Observers::setInteger(String name, int32_t value, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::INTEGER, 4);


	RemoteMeMessagesUtils::putInt32(payload, pos, value);

	remoteMe->send(payload, size + 4);
	free(payload);
}