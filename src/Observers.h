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
	struct TextObserver {
		String name;
		void(*toCall)(String);
	};
	struct SmallInteger3Observer {
		String name;
		void(*toCall)(uint16_t , uint16_t , uint16_t );
	};
	struct SmallInteger2Observer {
		String name;
		void(*toCall)(uint16_t , uint16_t );
	};
	struct IntegerBooleanObserver {
		String name;
		void(*toCall)(int32_t, boolean);
	};
	struct DoubleObserver {
		String name;
		void(*toCall)(double);
	};


	class Observers
	{
		friend RemoteMe;

		std::list<BooleanObserver> booleanObservers;
		std::list<IntegerObserver> integerObservers;
		std::list<TextObserver> textObservers;
		std::list<SmallInteger3Observer> smallInteger3Observers;
		std::list<SmallInteger2Observer> smallInteger2Observers;
		std::list<IntegerBooleanObserver> integerBooleanObservers;
		std::list<DoubleObserver> doubleObservers;

		

		RemoteMe* remoteMe;
	
	private:
		uint16_t prepareSetMessage(uint8_t* &payload, uint16_t &pos, boolean ignoreCurrent, String name, uint16_t type, uint8_t additionalSize);
	protected:
		void onChangePropagateMessage(uint8_t *payload);
	public:
		Observers(RemoteMe* remoteMe);
	
		void observeBoolean(String name,void(*toCall)(boolean));
		void observeInteger(String name,void(*toCall)(int32_t));
		void observeText(String name, void(*toCall)(String));
		void observeSmallInteger3(String name, void(*toCall)(uint16_t, uint16_t, uint16_t));
		void observeSmallInteger2(String name, void(*toCall)(uint16_t, uint16_t));
		void observeIntegerBoolean(String name, void(*toCall)(int32_t, boolean));
		void observeDouble(String name, void(*toCall)(double));

		void setBoolean(String name,boolean value, boolean ignoreCurrent);
		inline void setBoolean(String name, boolean value) {setBoolean(name, value, false);	}

		void setInteger(String name, int32_t value, boolean ignoreCurrent);
		inline void setInteger(String name, int32_t value) { setInteger(name, value, false); }

		void setText(String name, String value, boolean ignoreCurrent);
		inline void setText(String name, String value) { setText(name, value, false); }

		void setSmallInteger3(String name, uint16_t val1, uint16_t val2, uint16_t val3, boolean ignoreCurrent);
		inline void setSmallInteger3(String name, uint16_t val1, uint16_t val2, uint16_t val3) { setSmallInteger3(name, val1, val2, val3, false); }

		void setSmallInteger2(String name, uint16_t val1, uint16_t val2 , boolean ignoreCurrent);
		inline void setSmallInteger2(String name, uint16_t val1, uint16_t val2 ) { setSmallInteger2(name, val1, val2, false); }

		void setIntegerBoolean(String name, int32_t val1, boolean val2, boolean ignoreCurrent);
		inline void setIntegerBoolean(String name, int32_t val1, boolean val2 ) { setIntegerBoolean(name, val1, val2, false); }

		void setDouble(String name, double value, boolean ignoreCurrent);
		inline void setDouble(String name, double value) { setDouble(name, value, false); }
	};


#endif

	