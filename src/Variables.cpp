// 
// 
// 


#include "Variables.h"



Variables::Variables(RemoteMe* remoteMe) {
	this->remoteMe = remoteMe;
}

void Variables::onChangeVariableMessage(uint8_t *payload) {

	uint16_t pos =4;//type and size

	uint16_t senderDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);
	uint16_t receiverDeviceId = remoteMe->getDeviceId();

	uint8_t ignoreReceiversCount= RemoteMeMessagesUtils::getUint8(payload, pos);

	pos += 2 * ignoreReceiversCount;


	DEBUG_REMOTEME("[RMM] Change variables by direct conenction \n");
	onChangePropagateMessage(senderDeviceId, receiverDeviceId, pos, payload,0,0,0);
}



void Variables::onChangePropagateMessage(uint16_t senderDeviceId, uint16_t receiverDeviceId, uint16_t pos, uint8_t *payload, uint16_t sessionId, uint16_t credit, uint16_t time) {
	uint16_t count = RemoteMeMessagesUtils::getUint16(payload, pos);
	DEBUG_REMOTEME("[RMM] Change variables count %d \n", count);
	while (count != 0) {
		count--;

		uint16_t type = RemoteMeMessagesUtils::getUint16(payload, pos);
		String name = RemoteMeMessagesUtils::getString(payload, pos);


		DEBUG_REMOTEME("[RMM] Change variable name %s type: %d \n", name.c_str(), type);
		if (type == RemotemeStructures::VariableOberverType::BOOLEAN) {
			boolean value = RemoteMeMessagesUtils::getInt8(payload, pos) == 1;
			for (std::list<BooleanVariable>::iterator it = this->booleanVariables.begin(); it != this->booleanVariables.end(); ++it) {
				if ((*it).name == name) {
					if ((*it).toCall != nullptr) {
						(*it).toCall(value);
					}
					if ((*it).toCall_rental != nullptr) {
						(*it).toCall_rental(value, sessionId,credit,time);
					}
					break;
				}
			}
			DEBUG_REMOTEME("[RMM] BOOLEAN varialbe value %d \n", value);
		}
		else if (type == RemotemeStructures::VariableOberverType::INTEGER) {
			int32_t value = RemoteMeMessagesUtils::getInt32(payload, pos);
			for (std::list<IntegerVariable>::iterator it = this->integerVariables.begin(); it != this->integerVariables.end(); ++it) {
				if ((*it).name == name) {
					if ((*it).toCall != nullptr) {
						(*it).toCall(value);
					}
					if ((*it).toCall_rental != nullptr) {
						(*it).toCall_rental(value, sessionId, credit, time);
					}
					break;
				}
			}
			DEBUG_REMOTEME("[RMM] INTEGER varialbe value %d \n", value);
		}
		else if (type == RemotemeStructures::VariableOberverType::TEXT) {
			String value = RemoteMeMessagesUtils::getString(payload, pos);
			for (std::list<TextVariable>::iterator it = this->textVariables.begin(); it != this->textVariables.end(); ++it) {
				if ((*it).name == name) {
					if ((*it).toCall != nullptr) {
						(*it).toCall(value);
					}
					if ((*it).toCall_rental != nullptr) {
						(*it).toCall_rental(value, sessionId, credit, time);
					}
					break;
				}
			}
			DEBUG_REMOTEME("[RMM] TEXT varialbe value %s \n", value.c_str());
		}
		else if (type == RemotemeStructures::VariableOberverType::SMALL_INTEGER_3) {
			uint16_t val1 = RemoteMeMessagesUtils::getInt16(payload, pos);
			uint16_t val2 = RemoteMeMessagesUtils::getInt16(payload, pos);
			uint16_t val3 = RemoteMeMessagesUtils::getInt16(payload, pos);
			for (std::list<SmallInteger3Variable>::iterator it = this->smallInteger3Variables.begin(); it != this->smallInteger3Variables.end(); ++it) {
				if ((*it).name == name) {
					if ((*it).toCall != nullptr) {
						(*it).toCall(val1, val2, val3);
					}
					if ((*it).toCall_rental != nullptr) {
						(*it).toCall_rental(val1, val2, val3, sessionId, credit, time);
					}
					break;
				}
			}
			DEBUG_REMOTEME("[RMM] SMALL_INTEGER_3 varialbe value %d %d %d \n", val1, val2, val3);
		}
		else if (type == RemotemeStructures::VariableOberverType::SMALL_INTEGER_2) {
			uint16_t val1 = RemoteMeMessagesUtils::getInt16(payload, pos);
			uint16_t val2 = RemoteMeMessagesUtils::getInt16(payload, pos);

			for (std::list<SmallInteger2Variable>::iterator it = this->smallInteger2Variables.begin(); it != this->smallInteger2Variables.end(); ++it) {
				if ((*it).name == name) {
					if ((*it).toCall != nullptr) {
						(*it).toCall(val1, val2);
					}
					if ((*it).toCall_rental != nullptr) {
						(*it).toCall_rental(val1, val2, sessionId, credit, time);
					}
					break;
				}
			}
			DEBUG_REMOTEME("[RMM] SMALL_INTEGER_2 varialbe value %d %d \n", val1, val2);
		}
		else if (type == RemotemeStructures::VariableOberverType::INTEGER_BOOLEAN) {
			int32_t value = RemoteMeMessagesUtils::getInt32(payload, pos);
			boolean  b = RemoteMeMessagesUtils::getInt8(payload, pos) == 1;

			for (std::list<IntegerBooleanVariable>::iterator it = this->integerBooleanVariables.begin(); it != this->integerBooleanVariables.end(); ++it) {
				if ((*it).name == name) {
					if ((*it).toCall != nullptr) {
						(*it).toCall(value, b);
					}
					if ((*it).toCall_rental != nullptr) {
						(*it).toCall_rental(value, b, sessionId, credit, time);
					}
					break;
				}
			}
			DEBUG_REMOTEME("[RMM] INTEGER_BOOLEAN varialbe value %d %d \n", value, b);
		}
		else if (type == RemotemeStructures::VariableOberverType::DOUBLE) {
			double value = RemoteMeMessagesUtils::getDouble(payload, pos);
			for (std::list<DoubleVariable>::iterator it = this->doubleVariables.begin(); it != this->doubleVariables.end(); ++it) {
				if ((*it).name == name) {
					if ((*it).toCall != nullptr) {
						(*it).toCall(value);
					}
					if ((*it).toCall_rental != nullptr) {
						(*it).toCall_rental(value, sessionId, credit, time);
					}
					break;
				}
			}
		}
		else if (type == RemotemeStructures::VariableOberverType::TEXT_2) {
			String value1 = RemoteMeMessagesUtils::getString(payload, pos);
			String value2 = RemoteMeMessagesUtils::getString(payload, pos);
			for (std::list<Text2Variable>::iterator it = this->text2Variables.begin(); it != this->text2Variables.end(); ++it) {
				if ((*it).name == name) {
					if ((*it).toCall != nullptr) {
						(*it).toCall(value1, value2);
					}
					if ((*it).toCall_rental != nullptr) {
						(*it).toCall_rental(value1, value2, sessionId, credit, time);
					}
					break;
				}
			}
			DEBUG_REMOTEME("[RMM] TEXT_2 varialbe value %s %s \n", value1.c_str(), value2.c_str());
		}
		else if (type == RemotemeStructures::VariableOberverType::SMALL_INTEGER_2_TEXT_2) {

			uint16_t val1 = RemoteMeMessagesUtils::getInt16(payload, pos);
			uint16_t val2 = RemoteMeMessagesUtils::getInt16(payload, pos);

			String val3 = RemoteMeMessagesUtils::getString(payload, pos);
			String val4 = RemoteMeMessagesUtils::getString(payload, pos);
			for (std::list<SmallInteger2Text2Variable>::iterator it = this->smallInteger2Text2Variables.begin(); it != this->smallInteger2Text2Variables.end(); ++it) {
				if ((*it).name == name) {
					if ((*it).toCall != nullptr) {
						(*it).toCall(val1, val2, val3, val4);
					}
					if ((*it).toCall_rental != nullptr) {
						(*it).toCall_rental(val1, val2, val3, val4, sessionId, credit, time);
					}
					break;
				}
			}
			DEBUG_REMOTEME("[RMM] SMALL_INTEGER_2_TEXT_2 varialbe value %d %d  %s %s \n", val1, val2, val3.c_str(), val4.c_str());
		}


	}
}
void Variables::onChangePropagateMessage(uint8_t *payload) {
	uint16_t pos = 0;
	pos += 4;//type and size

	uint16_t senderDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);
	uint16_t receiverDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);

	onChangePropagateMessage(senderDeviceId, receiverDeviceId, pos, payload,0,0,0);
}

void Variables::onChangePropagateMessage_rental(uint8_t *payload) {
	uint16_t pos = 0;
	pos += 4;//type and size

	uint16_t senderDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);
	uint16_t receiverDeviceId = RemoteMeMessagesUtils::getUint16(payload, pos);

	uint16_t sessionId = RemoteMeMessagesUtils::getUint16(payload, pos);
	uint16_t credit = RemoteMeMessagesUtils::getUint16(payload, pos);
	uint16_t time = RemoteMeMessagesUtils::getUint16(payload, pos);


	onChangePropagateMessage(senderDeviceId, receiverDeviceId, pos, payload, sessionId, credit, time);
}





uint16_t Variables::getVariableObserveMessage(uint8_t* &payload) {

		uint16_t size = 4;
		uint16_t variablesCount = 0;
		
		for (std::list<BooleanVariable>::iterator it = this->booleanVariables.begin(); it != this->booleanVariables.end(); ++it) {
			size += 2 + (*it).name.length() + 1;
			variablesCount++;
		}

		for (std::list<IntegerVariable>::iterator it = this->integerVariables.begin(); it != this->integerVariables.end(); ++it) {
			size += 2 + (*it).name.length() + 1;
			variablesCount++;
		}

		for (std::list<TextVariable>::iterator it = this->textVariables.begin(); it != this->textVariables.end(); ++it) {
			size += 2 + (*it).name.length() + 1;
			variablesCount++;
		}


		for (std::list<SmallInteger3Variable>::iterator it = this->smallInteger3Variables.begin(); it != this->smallInteger3Variables.end(); ++it) {
			size += 2 + (*it).name.length() + 1;
			variablesCount++;
		}

		for (std::list<SmallInteger2Variable>::iterator it = this->smallInteger2Variables.begin(); it != this->smallInteger2Variables.end(); ++it) {
			size += 2 + (*it).name.length() + 1;
			variablesCount++;
		}


		for (std::list<IntegerBooleanVariable>::iterator it = this->integerBooleanVariables.begin(); it != this->integerBooleanVariables.end(); ++it) {
			size += 2 + (*it).name.length() + 1;
			variablesCount++;
		}


		for (std::list<DoubleVariable>::iterator it = this->doubleVariables.begin(); it != this->doubleVariables.end(); ++it) {
			size += 2 + (*it).name.length() + 1;
			variablesCount++;
		}


		for (std::list<Text2Variable>::iterator it = this->text2Variables.begin(); it != this->text2Variables.end(); ++it) {
			size += 2 + (*it).name.length() + 1;
			variablesCount++;
		}


		for (std::list<SmallInteger2Text2Variable>::iterator it = this->smallInteger2Text2Variables.begin(); it != this->smallInteger2Text2Variables.end(); ++it) {
			size += 2 + (*it).name.length() + 1;
			variablesCount++;
		}


		payload = (uint8_t*)malloc(size + 4);


		uint16_t pos = 0;


		RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::OBSERVER_REGISTER_MESSAGE);
		RemoteMeMessagesUtils::putUint16(payload, pos, size);



		RemoteMeMessagesUtils::putUint16(payload, pos, remoteMe->getDeviceId());
		RemoteMeMessagesUtils::putUint16(payload, pos, variablesCount);



		for (std::list<BooleanVariable>::iterator it = this->booleanVariables.begin(); it != this->booleanVariables.end(); ++it) {
			RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::VariableOberverType::BOOLEAN);
			RemoteMeMessagesUtils::putString(payload, pos, (*it).name);
		}

		for (std::list<IntegerVariable>::iterator it = this->integerVariables.begin(); it != this->integerVariables.end(); ++it) {
			RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::VariableOberverType::INTEGER);
			RemoteMeMessagesUtils::putString(payload, pos, (*it).name);
		}

		for (std::list<TextVariable>::iterator it = this->textVariables.begin(); it != this->textVariables.end(); ++it) {
			RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::VariableOberverType::TEXT);
			RemoteMeMessagesUtils::putString(payload, pos, (*it).name);
		}


		for (std::list<SmallInteger3Variable>::iterator it = this->smallInteger3Variables.begin(); it != this->smallInteger3Variables.end(); ++it) {
			RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::VariableOberverType::SMALL_INTEGER_3);
			RemoteMeMessagesUtils::putString(payload, pos, (*it).name);
		}

		for (std::list<SmallInteger2Variable>::iterator it = this->smallInteger2Variables.begin(); it != this->smallInteger2Variables.end(); ++it) {
			RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::VariableOberverType::SMALL_INTEGER_2);
			RemoteMeMessagesUtils::putString(payload, pos, (*it).name);
		}


		for (std::list<IntegerBooleanVariable>::iterator it = this->integerBooleanVariables.begin(); it != this->integerBooleanVariables.end(); ++it) {
			RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::VariableOberverType::INTEGER_BOOLEAN);
			RemoteMeMessagesUtils::putString(payload, pos, (*it).name);
		}


		for (std::list<DoubleVariable>::iterator it = this->doubleVariables.begin(); it != this->doubleVariables.end(); ++it) {
			RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::VariableOberverType::DOUBLE);
			RemoteMeMessagesUtils::putString(payload, pos, (*it).name);
		}


		for (std::list<Text2Variable>::iterator it = this->text2Variables.begin(); it != this->text2Variables.end(); ++it) {
			RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::VariableOberverType::TEXT_2);
			RemoteMeMessagesUtils::putString(payload, pos, (*it).name);
		}


		for (std::list<SmallInteger2Text2Variable>::iterator it = this->smallInteger2Text2Variables.begin(); it != this->smallInteger2Text2Variables.end(); ++it) {
			RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::VariableOberverType::SMALL_INTEGER_2_TEXT_2);
			RemoteMeMessagesUtils::putString(payload, pos, (*it).name);
		}

	


		return size + 4;


	

}

void Variables::observeBoolean(String name,void(*toCall)(boolean)) {
	BooleanVariable bo;
	bo.toCall = toCall;
	bo.name = name;
	this->booleanVariables.push_back(bo);
}

void Variables::observeInteger(String name, void(*toCall)(int32_t)) {
	IntegerVariable bo;
	bo.toCall = toCall;
	bo.name = name;
	this->integerVariables.push_back(bo);

}


void Variables::observeText(String name, void(*toCall)(String)) {
	TextVariable bo;
	bo.toCall = toCall;
	bo.name = name;
	this->textVariables.push_back(bo);

}

void Variables::observeText2(String name, void(*toCall)(String,String)) {
	Text2Variable bo;
	bo.toCall = toCall;
	bo.name = name;
	this->text2Variables.push_back(bo);

}
void Variables::observeSmallInteger3(String name, void(*toCall)(int16_t, int16_t, int16_t)) {
	SmallInteger3Variable bo;
	bo.toCall = toCall;
	bo.name = name;
	this->smallInteger3Variables.push_back(bo);

}
void Variables::observeSmallInteger2(String name, void(*toCall)(int16_t, int16_t)) {
	SmallInteger2Variable bo;
	bo.toCall = toCall;
	bo.name = name;
	this->smallInteger2Variables.push_back(bo);

}
void Variables::observeIntegerBoolean(String name, void(*toCall)(int32_t, boolean)) {
	IntegerBooleanVariable bo;
	bo.toCall = toCall;
	bo.name = name;
	this->integerBooleanVariables.push_back(bo);

}
void Variables::observeDouble(String name, void(*toCall)(double)) {
	DoubleVariable bo;
	bo.toCall = toCall;
	bo.name = name;
	this->doubleVariables.push_back(bo);

}

void Variables::observeSmallInteger2Text2(String name, void(*toCall)(int16_t, int16_t,String,String)){
	SmallInteger2Text2Variable bo;
	bo.toCall = toCall;
	bo.name = name;
	this->smallInteger2Text2Variables.push_back(bo);

}
//-----------WB version

void Variables::observeBoolean(String name, void(*toCall)(boolean, uint16_t, uint16_t, uint16_t)) {
	BooleanVariable bo;
	bo.toCall_rental = toCall;
	bo.name = name;
	this->booleanVariables.push_back(bo);
}

void Variables::observeInteger(String name, void(*toCall)(int32_t, uint16_t, uint16_t, uint16_t)) {
	IntegerVariable bo;
	bo.toCall_rental = toCall;
	bo.name = name;
	this->integerVariables.push_back(bo);

}


void Variables::observeText(String name, void(*toCall)(String, uint16_t, uint16_t, uint16_t)) {
	TextVariable bo;
	bo.toCall_rental = toCall;
	bo.name = name;
	this->textVariables.push_back(bo);

}

void Variables::observeText2(String name, void(*toCall)(String, String, uint16_t, uint16_t, uint16_t)) {
	Text2Variable bo;
	bo.toCall_rental = toCall;
	bo.name = name;
	this->text2Variables.push_back(bo);

}
void Variables::observeSmallInteger3(String name, void(*toCall)(int16_t, int16_t, int16_t, uint16_t, uint16_t, uint16_t)) {
	SmallInteger3Variable bo;
	bo.toCall_rental = toCall;
	bo.name = name;
	this->smallInteger3Variables.push_back(bo);

}
void Variables::observeSmallInteger2(String name, void(*toCall)(int16_t, int16_t, uint16_t, uint16_t, uint16_t)) {
	SmallInteger2Variable bo;
	bo.toCall_rental = toCall;
	bo.name = name;
	this->smallInteger2Variables.push_back(bo);

}
void Variables::observeIntegerBoolean(String name, void(*toCall)(int32_t, boolean, uint16_t, uint16_t, uint16_t)) {
	IntegerBooleanVariable bo;
	bo.toCall_rental = toCall;
	bo.name = name;
	this->integerBooleanVariables.push_back(bo);

}
void Variables::observeDouble(String name, void(*toCall)(double, uint16_t, uint16_t, uint16_t)) {
	DoubleVariable bo;
	bo.toCall_rental = toCall;
	bo.name = name;
	this->doubleVariables.push_back(bo);

}

void Variables::observeSmallInteger2Text2(String name, void(*toCall)(int16_t, int16_t, String, String, uint16_t, uint16_t, uint16_t)) {
	SmallInteger2Text2Variable bo;
	bo.toCall_rental = toCall;
	bo.name = name;
	this->smallInteger2Text2Variables.push_back(bo);

}


//end wb version




uint16_t Variables::prepareSetMessage(uint8_t* &payload, uint16_t &pos, boolean ignoreCurrent, String name, uint16_t type, uint8_t additionalSize) {
	uint16_t size = 5 + name.length() +1+2 + additionalSize;


	std::list<uint16_t>* direct = remoteMe->getDirectConnected();
	uint8_t ignoreCount = 0;
	if (direct != nullptr) {
		ignoreCount = direct->size();

	}

	if (ignoreCurrent) {
		ignoreCount++;
	}
	
	size += 2 * ignoreCount;

	payload = (uint8_t*)malloc(size + 4);

	RemoteMeMessagesUtils::putUint16(payload, pos, RemotemeStructures::VARIABLE_CHANGE_MESSAGE);
	RemoteMeMessagesUtils::putUint16(payload, pos, size);
	RemoteMeMessagesUtils::putUint16(payload, pos, remoteMe->getDeviceId());
	RemoteMeMessagesUtils::putUint8(payload, pos, ignoreCount);

	if (ignoreCurrent){
		RemoteMeMessagesUtils::putUint16(payload, pos, remoteMe->getDeviceId());
	}
	
	if (direct!=nullptr) {
		for (std::list<uint16_t>::iterator it = direct->begin(); it != direct->end(); ++it) {
			RemoteMeMessagesUtils::putUint16(payload, pos, (*it));
		}
		delete(direct);
	}

	RemoteMeMessagesUtils::putUint16(payload, pos, 1);//we sends only one


	RemoteMeMessagesUtils::putUint16(payload, pos,type);
	RemoteMeMessagesUtils::putString(payload, pos, name);

	return size;

}

void Variables::send(uint8_t * payload, uint16_t size) {
	remoteMe->send(payload, size );
	remoteMe->sendDirect(payload, size );
}
void Variables::setBoolean(String name, boolean value,boolean ignoreCurrent) {
	
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size =  prepareSetMessage(payload, pos, ignoreCurrent,name, RemotemeStructures::VariableOberverType::BOOLEAN ,1);

	RemoteMeMessagesUtils::putBoolean(payload, pos, value );

	send(payload, size+4);
	free(payload);
}

void Variables::setInteger(String name, int32_t value, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::INTEGER, 4);


	RemoteMeMessagesUtils::putInt32(payload, pos, value);

	send(payload, size + 4);
	free(payload);
}



void Variables::setText(String name, String value, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::TEXT, value.length()+1);


	RemoteMeMessagesUtils::putString(payload, pos, value);

	send(payload, size + 4);
	free(payload);
}

void Variables::setText2(String name, String value1, String value2, boolean ignoreCurrent){
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::TEXT_2, value1.length()+1+value2.length()+1);


	RemoteMeMessagesUtils::putString(payload, pos, value1);
	RemoteMeMessagesUtils::putString(payload, pos, value2);
	
	send(payload, size + 4);
	free(payload);
}
void Variables::setSmallInteger3(String name, uint16_t val1, uint16_t val2, uint16_t val3, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::SMALL_INTEGER_3, 6);


	RemoteMeMessagesUtils::putInt16(payload, pos, val1);
	RemoteMeMessagesUtils::putInt16(payload, pos, val2);
	RemoteMeMessagesUtils::putInt16(payload, pos, val3);

	send(payload, size + 4);
	free(payload);
}

void Variables::setSmallInteger2(String name, uint16_t val1, uint16_t val2, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::SMALL_INTEGER_2, 4);


	RemoteMeMessagesUtils::putInt16(payload, pos, val1);
	RemoteMeMessagesUtils::putInt16(payload, pos, val2);
	
	send(payload, size + 4);
	free(payload);
}

void Variables::setIntegerBoolean(String name, int32_t val1, boolean val2, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::INTEGER_BOOLEAN, 5);


	RemoteMeMessagesUtils::putInt32(payload, pos, val1);
	RemoteMeMessagesUtils::putInt8(payload, pos, val2?1:0);

	send(payload, size + 4);
	free(payload);

}

void Variables::setDouble(String name, double value, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::DOUBLE,8);


	RemoteMeMessagesUtils::putDouble(payload, pos, value);


	send(payload, size + 4);
	free(payload);

}

void Variables::setSmallInteger2Text2(String name, int16_t val1, int16_t val2,String val3,String val4, boolean ignoreCurrent) {
	uint8_t* payload;
	uint16_t pos = 0;
	uint16_t size = prepareSetMessage(payload, pos, ignoreCurrent, name, RemotemeStructures::VariableOberverType::SMALL_INTEGER_2_TEXT_2,4+ val3.length()+1+val4.length()+1);


	RemoteMeMessagesUtils::putInt16(payload, pos, val1);
	RemoteMeMessagesUtils::putInt16(payload, pos, val2);
	RemoteMeMessagesUtils::putString(payload, pos, val3);
	RemoteMeMessagesUtils::putString(payload, pos, val4);
	

	send(payload, size + 4);
	free(payload);

}
		