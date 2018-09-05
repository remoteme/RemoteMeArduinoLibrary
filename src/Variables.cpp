// 
// 
// 


#include "Variables.h"



Variables::Variables(RemoteMe* remoteMe) {
	this->remoteMe = remoteMe;
}

void Variables::onChangePropagateMessage(uint8_t *payload) {
	
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
			for (std::list<BooleanVariable>::iterator it = this->booleanVariables.begin(); it != this->booleanVariables.end(); ++it) {
				if ((*it).name == name) {
					(*it).toCall(value);
					break;
				}
			}
		}else if (type == RemotemeStructures::VariableOberverType::INTEGER) {
			int32_t value =RemoteMeMessagesUtils::getInt32(payload, pos);
			for (std::list<IntegerVariable>::iterator it = this->integerVariables.begin(); it != this->integerVariables.end(); ++it) {
				if ((*it).name == name) {
					(*it).toCall(value);
					break;
				}
			}
		}else if (type == RemotemeStructures::VariableOberverType::TEXT) {
			String value =RemoteMeMessagesUtils::getString(payload, pos);
			for (std::list<TextVariable>::iterator it = this->textVariables.begin(); it != this->textVariables.end(); ++it) {
				if ((*it).name == name) {
					(*it).toCall(value);
					break;
				}
			}
		}else if (type == RemotemeStructures::VariableOberverType::SMALL_INTEGER_3) {
			uint16_t val1=RemoteMeMessagesUtils::getInt16(payload, pos);
			uint16_t val2=RemoteMeMessagesUtils::getInt16(payload, pos);
			uint16_t val3=RemoteMeMessagesUtils::getInt16(payload, pos);
			for (std::list<SmallInteger3Variable>::iterator it = this->smallInteger3Variables.begin(); it != this->smallInteger3Variables.end(); ++it) {
				if ((*it).name == name) {
					(*it).toCall(val1,val2,val3);
					break;
				}
			}
		}else if (type == RemotemeStructures::VariableOberverType::SMALL_INTEGER_2) {
			uint16_t val1 = RemoteMeMessagesUtils::getInt16(payload, pos);
			uint16_t val2 = RemoteMeMessagesUtils::getInt16(payload, pos);

			for (std::list<SmallInteger2Variable>::iterator it = this->smallInteger2Variables.begin(); it != this->smallInteger2Variables.end(); ++it) {
				if ((*it).name == name) {
					(*it).toCall(val1, val2);
					break;
				}
			}
		}else if (type == RemotemeStructures::VariableOberverType::INTEGER_BOOLEAN) {
			int32_t value =RemoteMeMessagesUtils::getInt32(payload, pos);
			boolean  b = RemoteMeMessagesUtils::getInt8(payload, pos) == 1;

			for (std::list<IntegerBooleanVariable>::iterator it = this->integerBooleanVariables.begin(); it != this->integerBooleanVariables.end(); ++it) {
				if ((*it).name == name) {
					(*it).toCall(value, b);
					break;
				}
			}
		}else if (type == RemotemeStructures::VariableOberverType::DOUBLE) {
			double value =RemoteMeMessagesUtils::getDouble(payload, pos);
			for (std::list<DoubleVariable>::iterator it = this->doubleVariables.begin(); it != this->doubleVariables.end(); ++it) {
				if ((*it).name == name) {
					(*it).toCall(value);
					break;
				}
			}
		}else if (type == RemotemeStructures::VariableOberverType::TEXT_2) {
			String value1 =RemoteMeMessagesUtils::getString(payload, pos);
			String value2 =RemoteMeMessagesUtils::getString(payload, pos);
			for (std::list<Text2Variable>::iterator it = this->text2Variables.begin(); it != this->text2Variables.end(); ++it) {
				if ((*it).name == name) {
					(*it).toCall(value1, value2);
					break;
				}
			}
		}
	}
}
void Variables::observeBoolean(String name,void(*toCall)(boolean)) {
	BooleanVariable bo;
	bo.toCall = toCall;
	bo.name = name;
	this->booleanVariables.push_back(bo);

	this->remoteMe->sendVariableObserveMessage(name, RemotemeStructures::VariableOberverType::BOOLEAN);

}

void Variables::observeInteger(String name, void(*toCall)(int32_t)) {
	IntegerVariable bo;
	bo.toCall = toCall;
	bo.name = name;
	this->integerVariables.push_back(bo);

	this->remoteMe->sendVariableObserveMessage(name, RemotemeStructures::VariableOberverType::INTEGER);

}


void Variables::observeText(String name, void(*toCall)(String)) {
	TextVariable bo;
	bo.toCall = toCall;
	bo.name = name;
	this->textVariables.push_back(bo);

	this->remoteMe->sendVariableObserveMessage(name, RemotemeStructures::VariableOberverType::TEXT);
}

void Variables::observeText2(String name, void(*toCall)(String,String)) {
	Text2Variable bo;
	bo.toCall = toCall;
	bo.name = name;
	this->text2Variables.push_back(bo);

	this->remoteMe->sendVariableObserveMessage(name, RemotemeStructures::VariableOberverType::TEXT_2);
}
void Variables::observeSmallInteger3(String name, void(*toCall)(int16_t, int16_t, int16_t)) {
	SmallInteger3Variable bo;
	bo.toCall = toCall;
	bo.name = name;
	this->smallInteger3Variables.push_back(bo);

	this->remoteMe->sendVariableObserveMessage(name, RemotemeStructures::VariableOberverType::SMALL_INTEGER_3);
}
void Variables::observeSmallInteger2(String name, void(*toCall)(int16_t, int16_t)) {
	SmallInteger2Variable bo;
	bo.toCall = toCall;
	bo.name = name;
	this->smallInteger2Variables.push_back(bo);

	this->remoteMe->sendVariableObserveMessage(name, RemotemeStructures::VariableOberverType::SMALL_INTEGER_2);
}
void Variables::observeIntegerBoolean(String name, void(*toCall)(int32_t, boolean)) {
	IntegerBooleanVariable bo;
	bo.toCall = toCall;
	bo.name = name;
	this->integerBooleanVariables.push_back(bo);

	this->remoteMe->sendVariableObserveMessage(name, RemotemeStructures::VariableOberverType::INTEGER_BOOLEAN);
}
void Variables::observeDouble(String name, void(*toCall)(double)) {
	DoubleVariable bo;
	bo.toCall = toCall;
	bo.name = name;
	this->doubleVariables.push_back(bo);

	this->remoteMe->sendVariableObserveMessage(name, RemotemeStructures::VariableOberverType::DOUBLE);
}


uint16_t Variables::prepareSetMessage(uint8_t* &payload, uint16_t &pos, boolean ignoreCurrent, String name, uint16_t type, uint8_t additionalSize) {
	uint16_t size = 5 + name.length() +1+2 + additionalSize;

	if (ignoreCurrent) {
		size += 2;
	}

	payload = (uint8_t*)malloc(size + 4);


	RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::VARIABLE_CHANGE_MESSAGE);
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
void Variables::setBoolean(String name, boolean value,boolean ignoreCurrent) {
	
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size =  prepareSetMessage(payload, pos, ignoreCurrent,name, RemotemeStructures::VariableOberverType::BOOLEAN ,1);


	RemoteMeMessagesUtils::putBoolean(payload, pos, value );

	remoteMe->send(payload, size+4);
	free(payload);
}

void Variables::setInteger(String name, int32_t value, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::INTEGER, 4);


	RemoteMeMessagesUtils::putInt32(payload, pos, value);

	remoteMe->send(payload, size + 4);
	free(payload);
}



void Variables::setText(String name, String value, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::TEXT, value.length()+1);


	RemoteMeMessagesUtils::putString(payload, pos, value);

	remoteMe->send(payload, size + 4);
	free(payload);
}

void Variables::setText2(String name, String value1, String value2, boolean ignoreCurrent){
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::TEXT_2, value1.length()+1+value2.length()+1);


	RemoteMeMessagesUtils::putString(payload, pos, value1);
	RemoteMeMessagesUtils::putString(payload, pos, value2);
	
	remoteMe->send(payload, size + 4);
	free(payload);
}
void Variables::setSmallInteger3(String name, uint16_t val1, uint16_t val2, uint16_t val3, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::SMALL_INTEGER_3, 6);


	RemoteMeMessagesUtils::putInt16(payload, pos, val1);
	RemoteMeMessagesUtils::putInt16(payload, pos, val2);
	RemoteMeMessagesUtils::putInt16(payload, pos, val3);

	remoteMe->send(payload, size + 4);
	free(payload);
}

void Variables::setSmallInteger2(String name, uint16_t val1, uint16_t val2, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::SMALL_INTEGER_2, 4);


	RemoteMeMessagesUtils::putInt16(payload, pos, val1);
	RemoteMeMessagesUtils::putInt16(payload, pos, val2);
	
	remoteMe->send(payload, size + 4);
	free(payload);
}

void Variables::setIntegerBoolean(String name, int32_t val1, boolean val2, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::INTEGER_BOOLEAN, 5);


	RemoteMeMessagesUtils::putInt32(payload, pos, val1);
	RemoteMeMessagesUtils::putInt8(payload, pos, val2?1:0);

	remoteMe->send(payload, size + 4);
	free(payload);

}

void Variables::setDouble(String name, double value, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::DOUBLE,8);


	RemoteMeMessagesUtils::putDouble(payload, pos, value);


	remoteMe->send(payload, size + 4);
	free(payload);

}