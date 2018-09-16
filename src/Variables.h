// Variables.h

#include <Arduino.h>
#include <list>
#include "RemoteMe.h"
#include "RemoteMeMessagesUtils.h"


#ifndef _REMOTEME_VARIABLES_h
#define _REMOTEME_VARIABLES_h



	class RemoteMe;


	struct BooleanVariable {
		String name;
		void(*toCall)(boolean);
	};
	struct IntegerVariable {
		String name;
		void(*toCall)(int32_t);
	};
	struct TextVariable {
		String name;
		void(*toCall)(String);
	};
	struct SmallInteger3Variable {
		String name;
		void(*toCall)(int16_t, int16_t, int16_t);
	};
	struct SmallInteger2Variable {
		String name;
		void(*toCall)(int16_t, int16_t);
	};
	struct IntegerBooleanVariable {
		String name;
		void(*toCall)(int32_t, boolean);
	};
	struct DoubleVariable {
		String name;
		void(*toCall)(double);
	};
	struct Text2Variable {
		String name;
		void(*toCall)(String,String);
	};
	struct SmallInteger2Text2Variable {
		String name;
		void(*toCall)(int16_t, int16_t,String,String);
	};
	class Variables
	{
		friend RemoteMe;

		std::list<BooleanVariable> booleanVariables;
		std::list<IntegerVariable> integerVariables;
		std::list<TextVariable> textVariables;
		std::list<SmallInteger3Variable> smallInteger3Variables;
		std::list<SmallInteger2Variable> smallInteger2Variables;
		std::list<IntegerBooleanVariable> integerBooleanVariables;
		std::list<DoubleVariable> doubleVariables;
		std::list<Text2Variable> text2Variables;
		std::list<SmallInteger2Text2Variable> smallInteger2Text2Variables;

		

		RemoteMe* remoteMe;
	
	private:
		uint16_t prepareSetMessage(uint8_t* &payload, uint16_t &pos, boolean ignoreCurrent, String name, uint16_t type, uint8_t additionalSize);
	protected:
		void onChangePropagateMessage(uint8_t *payload);
	public:
		Variables(RemoteMe* remoteMe);
	
		void observeBoolean(String name,void(*toCall)(boolean));
		void observeInteger(String name,void(*toCall)(int32_t));
		void observeText(String name, void(*toCall)(String));
		void observeText2(String name, void(*toCall)(String,String));
		void observeSmallInteger3(String name, void(*toCall)(int16_t, int16_t, int16_t));
		void observeSmallInteger2(String name, void(*toCall)(int16_t, int16_t));
		void observeIntegerBoolean(String name, void(*toCall)(int32_t, boolean));
		void observeDouble(String name, void(*toCall)(double));
		void observeSmallInteger2Text2(String name, void(*toCall)(int16_t, int16_t,String,String));
		
		void setBoolean(String name,boolean value, boolean ignoreCurrent);
		inline void setBoolean(String name, boolean value) {setBoolean(name, value, false);	}

		void setInteger(String name, int32_t value, boolean ignoreCurrent);
		inline void setInteger(String name, int32_t value) { setInteger(name, value, false); }

		void setText(String name, String value, boolean ignoreCurrent);
		inline void setText(String name, String value) { setText(name, value, false); }

		void setText2(String name, String value1, String value2, boolean ignoreCurrent);
		inline void setText2(String name, String value1, String value2) { setText2(name, value1,value2, false); }

		
		void setSmallInteger3(String name, uint16_t val1, uint16_t val2, uint16_t val3, boolean ignoreCurrent);
		inline void setSmallInteger3(String name, uint16_t val1, uint16_t val2, uint16_t val3) { setSmallInteger3(name, val1, val2, val3, false); }

		void setSmallInteger2(String name, uint16_t val1, uint16_t val2 , boolean ignoreCurrent);
		inline void setSmallInteger2(String name, uint16_t val1, uint16_t val2 ) { setSmallInteger2(name, val1, val2, false); }

		void setIntegerBoolean(String name, int32_t val1, boolean val2, boolean ignoreCurrent);
		inline void setIntegerBoolean(String name, int32_t val1, boolean val2 ) { setIntegerBoolean(name, val1, val2, false); }

		void setDouble(String name, double value, boolean ignoreCurrent);
		inline void setDouble(String name, double value) { setDouble(name, value, false); }
		
		void setSmallInteger2Text2(String name, int16_t val1, int16_t val2,String val3,String val4, boolean ignoreCurrent);
		inline void setSmallInteger2Text2Variable(String name, int16_t val1, int16_t val2,String val3,String val4) { setSmallInteger2Text2(name, val1,val2,val3,val4, false); }
	};


#endif

	