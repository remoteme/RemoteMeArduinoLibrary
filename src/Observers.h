// RemoteMe.h

#include <Arduino.h>
#include "RemoteMe.h"
#include "RemoteMeMessagesUtils.h"


#ifndef _REMOTEME_OBSERVERS_h
#define _REMOTEME_OBSERVERS_h


#include <list>


	struct BooleanObserver {
		String name;
		void(*toCall)(boolean);
	};
	struct IntegerObserver {
		String name;
		void(*toCall)(int32_t);
	};

	class Observers
	{
		friend RemoteMe;

		std::list<BooleanObserver> booleanObservers;
		std::list<IntegerObserver> integerObservers;
		RemoteMe* remoteMe;
	
	private:
		uint16_t prepareSetMessage(uint8_t* &payload, uint16_t &pos, boolean ignoreCurrent, String name, uint16_t type, uint8_t additionalSize);
	protected:
		void onChangePropagateMessage(uint8_t *payload);
	public:
		Observers(RemoteMe* remoteMe);
	
		void observeBoolean(String name,void(*toCall)(boolean));
		void observeInteger(String name,void(*toCall)(int32_t));

		void setBoolean(String name,boolean value, boolean ignoreCurrent);
		inline void setBoolean(String name, boolean value) {
			setBoolean(name, value, false);
		}
		void setInteger(String name, int32_t value, boolean ignoreCurrent);
		inline void setInteger(String name, int32_t value) { setInteger(name, value, false); }

	};


#endif

	