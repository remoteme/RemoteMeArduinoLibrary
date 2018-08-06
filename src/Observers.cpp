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
	uint16_t receiverDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);

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
		}else if (type == RemotemeStructures::VariableOberverType::TEXT) {
			String value =RemoteMeMessagesUtils::getString(payload, pos);
			for (std::list<TextObserver>::iterator it = this->textObservers.begin(); it != this->textObservers.end(); ++it) {
				if ((*it).name == name) {
					(*it).toCall(value);
					break;
				}
			}
		}else if (type == RemotemeStructures::VariableOberverType::SMALL_INTEGER_3) {
			uint16_t val1=RemoteMeMessagesUtils::getInt16(payload, pos);
			uint16_t val2=RemoteMeMessagesUtils::getInt16(payload, pos);
			uint16_t val3=RemoteMeMessagesUtils::getInt16(payload, pos);
			for (std::list<SmallInteger3Observer>::iterator it = this->smallInteger3Observers.begin(); it != this->smallInteger3Observers.end(); ++it) {
				if ((*it).name == name) {
					(*it).toCall(val1,val2,val3);
					break;
				}
			}
		}else if (type == RemotemeStructures::VariableOberverType::SMALL_INTEGER_2) {
			uint16_t val1 = RemoteMeMessagesUtils::getInt16(payload, pos);
			uint16_t val2 = RemoteMeMessagesUtils::getInt16(payload, pos);

			for (std::list<SmallInteger2Observer>::iterator it = this->smallInteger2Observers.begin(); it != this->smallInteger2Observers.end(); ++it) {
				if ((*it).name == name) {
					(*it).toCall(val1, val2);
					break;
				}
			}
		}else if (type == RemotemeStructures::VariableOberverType::INTEGER_BOOLEAN) {
			int32_t value =RemoteMeMessagesUtils::getInt32(payload, pos);
			boolean  b = RemoteMeMessagesUtils::getInt8(payload, pos) == 1;

			for (std::list<IntegerBooleanObserver>::iterator it = this->integerBooleanObservers.begin(); it != this->integerBooleanObservers.end(); ++it) {
				if ((*it).name == name) {
					(*it).toCall(value, b);
					break;
				}
			}
		}else if (type == RemotemeStructures::VariableOberverType::DOUBLE) {
			double value =RemoteMeMessagesUtils::getDouble(payload, pos);
			for (std::list<DoubleObserver>::iterator it = this->doubleObservers.begin(); it != this->doubleObservers.end(); ++it) {
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


void Observers::observeText(String name, void(*toCall)(String)) {
	TextObserver bo;
	bo.toCall = toCall;
	bo.name = name;
	this->textObservers.push_back(bo);

	this->remoteMe->sendObserverRegisterMessage(name, RemotemeStructures::VariableOberverType::TEXT);
}
void Observers::observeSmallInteger3(String name, void(*toCall)(uint16_t, uint16_t, uint16_t)) {
	SmallInteger3Observer bo;
	bo.toCall = toCall;
	bo.name = name;
	this->smallInteger3Observers.push_back(bo);

	this->remoteMe->sendObserverRegisterMessage(name, RemotemeStructures::VariableOberverType::SMALL_INTEGER_3);
}
void Observers::observeSmallInteger2(String name, void(*toCall)(uint16_t, uint16_t)) {
	SmallInteger2Observer bo;
	bo.toCall = toCall;
	bo.name = name;
	this->smallInteger2Observers.push_back(bo);

	this->remoteMe->sendObserverRegisterMessage(name, RemotemeStructures::VariableOberverType::SMALL_INTEGER_2);
}
void Observers::observeIntegerBoolean(String name, void(*toCall)(int32_t, boolean)) {
	IntegerBooleanObserver bo;
	bo.toCall = toCall;
	bo.name = name;
	this->integerBooleanObservers.push_back(bo);

	this->remoteMe->sendObserverRegisterMessage(name, RemotemeStructures::VariableOberverType::INTEGER_BOOLEAN);
}
void Observers::observeDouble(String name, void(*toCall)(double)) {
	DoubleObserver bo;
	bo.toCall = toCall;
	bo.name = name;
	this->doubleObservers.push_back(bo);

	this->remoteMe->sendObserverRegisterMessage(name, RemotemeStructures::VariableOberverType::DOUBLE);
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



void Observers::setText(String name, String value, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::TEXT, value.length()+1);


	RemoteMeMessagesUtils::putString(payload, pos, value);

	remoteMe->send(payload, size + 4);
	free(payload);
}

void Observers::setSmallInteger3(String name, uint16_t val1, uint16_t val2, uint16_t val3, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::SMALL_INTEGER_3, 6);


	RemoteMeMessagesUtils::putInt16(payload, pos, val1);
	RemoteMeMessagesUtils::putInt16(payload, pos, val2);
	RemoteMeMessagesUtils::putInt16(payload, pos, val3);

	remoteMe->send(payload, size + 4);
	free(payload);
}

void Observers::setSmallInteger2(String name, uint16_t val1, uint16_t val2, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::SMALL_INTEGER_2, 4);


	RemoteMeMessagesUtils::putInt16(payload, pos, val1);
	RemoteMeMessagesUtils::putInt16(payload, pos, val2);
	
	remoteMe->send(payload, size + 4);
	free(payload);
}

void Observers::setIntegerBoolean(String name, int32_t val1, boolean val2, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::INTEGER_BOOLEAN, 4);


	RemoteMeMessagesUtils::putInt32(payload, pos, val1);
	RemoteMeMessagesUtils::putInt8(payload, pos, val2?1:0);

	remoteMe->send(payload, size + 4);
	free(payload);

}

void Observers::setDouble(String name, double value, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::DOUBLE, 4);


	RemoteMeMessagesUtils::putDouble(payload, pos, value);


	remoteMe->send(payload, size + 4);
	free(payload);

}