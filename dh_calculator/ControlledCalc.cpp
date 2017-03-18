#include "ControlledCalc.h"

ISeg* ISeg::interpRoots[MAX_INTERPS] = {};
//#define DEBUG ON


class InterpFunc:public Func{
		bool parse(uint16_t &pointer,char* text){

			if (!Controller::parse_uint8(interpFuncID,pointer,text)){
				controller->getErrorLogger()->println("Could not parse interp id");
				return false;
			}

			if (text[pointer]!='['){
				controller->getErrorLogger()->println("Missing opening bracket for interp");
				return false;
			}

			pointer++;

			eval = createFunc(pointer,text,controller);

			if (eval == 0 ){
				controller->getErrorLogger()->println("Failed to parse function to pass to interp");
				return false;
			}

			if (text[pointer]!=']'){
				controller->getErrorLogger()->print("Found '");
				controller->getErrorLogger()->print(text[pointer]);
				controller->getErrorLogger()->println("' expect closing bracket ']' for interp");
				delete eval;
				eval = 0;
				return false;
			}

			pointer++;

			return true;
		}

		double solve(double in){
				#ifdef DEBUG
				Serial.print("Solving for: ");
				Serial.println(in);
				#endif
				ISeg* start = ISeg::interpRoots[interpFuncID];
				double y = ISeg::eval(start,in,Controller::lastProcessedMSTime);
				#ifdef DEBUG
				Serial.print("Output: ");
				Serial.println(y);
				#endif
				return y;
		}

		ADDRTYPE getType(){
			return A_DOUBLE;
		}
		uint8_t readB(){
			return (uint8_t) readD();
		}
		uint16_t readU(){
			return (uint16_t) readD();
		}
		int16_t readI(){
			return (int16_t) readD();
		}
		int32_t readL(){
			return (int32_t) readD();
		}
		float readF(){
			return (float) readD();
		}
		double readD(){
			return solve(eval->readD());
		}
		uint32_t readT(){
			return (uint32_t) readD();
		}

		const char* getName(){
			return "Interp";
		}

		~InterpFunc(){
			if (eval!=0)
				delete eval;
		};
	private:
		Func* eval;
		uint8_t interpFuncID;
};


class ByteLitFunc:public Func{
	public:
	ByteLitFunc(uint8_t in){
		val = in;
	}

	bool parse(uint16_t &pointer,char* text){
		return false;
	}

	ADDRTYPE getType(){
		return A_BYTE;
	}
	uint8_t readB(){
		return val;
	}
	uint16_t readU(){
		return (uint16_t) val;
	}
	int16_t readI(){
		return (int16_t) val;
	}
	int32_t readL(){
		return (int32_t) val;
	}
	float readF(){
		return (float) val;
	}
	double readD(){
		return (double) val;
	}
	uint32_t readT(){
		return (uint32_t) val;
	}

	~ByteLitFunc(){};
	uint8_t val;
};

class UIntLitFunc:public Func{
		public:
	UIntLitFunc(uint16_t in){
		val = in;
	}

	bool parse(uint16_t &pointer,char* text){
		return false;
	}

	ADDRTYPE getType(){
		return A_UINT;
	}
	uint8_t readB(){
		return (uint8_t)val;
	}
	uint16_t readU(){
		return val;
	}
	int16_t readI(){
		return (int16_t) val;
	}
	int32_t readL(){
		return (int32_t) val;
	}
	float readF(){
		return (float) val;
	}
	double readD(){
		return (double) val;
	}
	uint32_t readT(){
		return (uint32_t) val;
	}

	~UIntLitFunc(){};
	uint16_t val;
};

class IntLitFunc:public Func{
		public:
	IntLitFunc(int16_t in){
		val = in;
	}

	bool parse(uint16_t &pointer,char* text){
		return false;
	}

	ADDRTYPE getType(){
		return A_INT;
	}
	uint8_t readB(){
		return (uint8_t)val;
	}
	uint16_t readU(){
		return (uint16_t) val;
	}
	int16_t readI(){
		return val;
	}
	int32_t readL(){
		return (int32_t) val;
	}
	float readF(){
		return (float) val;
	}
	double readD(){
		return (double) val;
	}
	uint32_t readT(){
		return (uint32_t) val;
	}

	~IntLitFunc(){};
	int16_t val;
};

class LongLitFunc:public Func{
		public:
	LongLitFunc(int32_t in){
		val = in;
	}

	bool parse(uint16_t &pointer,char* text){
		return false;
	}

	ADDRTYPE getType(){
		return A_LONG;
	}
	uint8_t readB(){
		return (uint8_t)val;
	}
	uint16_t readU(){
		return (uint16_t) val;
	}
	int16_t readI(){
		return (int16_t)val;
	}
	int32_t readL(){
		return val;
	}
	float readF(){
		return (float) val;
	}
	double readD(){
		return (double) val;
	}
	uint32_t readT(){
		return (uint32_t) val;
	}

	~LongLitFunc(){};
	int32_t val;
};

class FloatLitFunc:public Func{
		public:
	FloatLitFunc(float in){
		val = in;
	}

	bool parse(uint16_t &pointer,char* text){
		return false;
	}

	ADDRTYPE getType(){
		return A_FLOAT;
	}
	uint8_t readB(){
		return (uint8_t)val;
	}
	uint16_t readU(){
		return (uint16_t) val;
	}
	int16_t readI(){
		return (int16_t)val;
	}
	int32_t readL(){
		return (int32_t)val;
	}
	float readF(){
		return val;
	}
	double readD(){
		return (double) val;
	}
	uint32_t readT(){
		return (uint32_t) val;
	}

	~FloatLitFunc(){};
	float val;
};

class DoubleLitFunc:public Func{
		public:
	DoubleLitFunc(double in){
		val = in;
	}

	bool parse(uint16_t &pointer,char* text){
		return false;
	}

	ADDRTYPE getType(){
		return A_DOUBLE;
	}
	uint8_t readB(){
		return (uint8_t)val;
	}
	uint16_t readU(){
		return (uint16_t) val;
	}
	int16_t readI(){
		return (int16_t)val;
	}
	int32_t readL(){
		return (int32_t)val;
	}
	float readF(){
		return (float)val;
	}
	double readD(){
		return val;
	}
	uint32_t readT(){
		return (uint32_t) val;
	}

	~DoubleLitFunc(){};
	double val;
};

class TimeLitFunc:public Func{
		public:
	TimeLitFunc(uint32_t in){
		val = in;
	}

	bool parse(uint16_t &pointer,char* text){
		return false;
	}

	ADDRTYPE getType(){
		return A_TIME;
	}
	uint8_t readB(){
		return (uint8_t)val;
	}
	uint16_t readU(){
		return (uint16_t) val;
	}
	int16_t readI(){
		return (int16_t)val;
	}
	int32_t readL(){
		return (int32_t)val;
	}
	float readF(){
		return (float) val;
	}
	double readD(){
		return (double) val;
	}
	uint32_t readT(){
		return val;
	}

	~TimeLitFunc(){};
	uint32_t val;
};



class LiteralFunc:public Func{

	bool parse(uint16_t &pointer,char* text){
		ADDRTYPE type;

		if (!ADDR1::parseType(type,text[pointer]))
				return false;
		pointer++;

		switch(type){
			case A_BYTE:{
				uint8_t val;
				if (!Controller::parse_uint8(val,pointer,text))
					return false;
				innerVal = new ByteLitFunc(val);
				break;
			}
			case A_UINT:{
				uint16_t val;
				if (!Controller::parse_uint16(val,pointer,text))
					return false;
				innerVal = new UIntLitFunc(val);
				break;
			}
			case A_INT:{
				int16_t val;
				if (!Controller::parse_int16(val,pointer,text))
					return false;
				innerVal = new IntLitFunc(val);
				break;
			}
			case A_LONG:{
				int32_t val;
				if (!Controller::parse_int32(val,pointer,text))
					return false;
				innerVal = new  LongLitFunc(val);
				break;
			}
			case A_FLOAT:{
				double val;
				if (!Controller::parse_double(val,pointer,text))
					return false;
				innerVal = new FloatLitFunc((float)val);
				break;
			}
			case A_DOUBLE:{
				double val;
				if (!Controller::parse_double(val,pointer,text))
					return false;
				innerVal = new DoubleLitFunc((double)val);
				break;
			}
			case A_TIME:{
				uint32_t val;
				if (!Controller::parse_uint32(val,pointer,text))
					return false;
				innerVal = new TimeLitFunc(val);
				break;
			}
			case A_STRING: return false;
			case BAD_TYPE: return false;
		}

		innerVal->controller = controller;
		return true;
	}

	bool simplify(Func* (&better) ){
		better = innerVal;
		return true;
	}

	ADDRTYPE getType(){
		return A_BYTE; // Doesnt matter, this function is simplified away
	}
	uint8_t readB(){
		return 0;
	}
	uint16_t readU(){
		return 0;
	}
	int16_t readI(){
		return 0;
	}
	int32_t readL(){
		return 0;
	}
	float readF(){
		return 0;
	}
	double readD(){
		return 0;
	}
	uint32_t readT(){
		return 0;
	}

	const char* getName(){
		return "#";
	}


	~LiteralFunc(){};
	private:

	Func* innerVal;
};

class VariableFunc:public Func{

	bool parse(uint16_t &pointer,char* text){
		ADDR1 temp(pointer,text);
		addr1 = temp;
		if (text[pointer]=='.'){
			pointer++;
			if (!Controller::parse_uint8(addr2,pointer,text)){
				controller->getErrorLogger()->println("Expected addr2 number");
				return false;
			}
		}else{
			addr2 = 0;
		}


		controlled = controller->getControlled(addr1.modID);


		if (controlled == 0 )
			return false;
		return true;
	}

	bool simplify(Func* (&better) ){
		return false;
	}

	ADDRTYPE getType(){
		return addr1.type;
	}

	uint8_t readB(){
		switch(addr1.type){
			case A_BYTE: return (uint8_t) (controlled->readB(addr1,addr2));
			case A_UINT: return (uint8_t) controlled->readU(addr1,addr2);
			case A_INT: return (uint8_t) controlled->readI(addr1,addr2);
			case A_LONG: return (uint8_t) controlled->readL(addr1,addr2);
			case A_FLOAT: return (uint8_t) controlled->readF(addr1,addr2);
			case A_DOUBLE: return (uint8_t) controlled->readD(addr1,addr2);
			case A_TIME: return (uint8_t) controlled->readT(addr1,addr2);
			case A_STRING: return 0;
			case BAD_TYPE: return 0;
		}
		return 0;
	}
	uint16_t readU(){
		switch(addr1.type){
			case A_BYTE: return (uint16_t) controlled->readB(addr1,addr2);
			case A_UINT: return (uint16_t) controlled->readU(addr1,addr2);
			case A_INT: return (uint16_t) controlled->readI(addr1,addr2);
			case A_LONG: return (uint16_t) controlled->readL(addr1,addr2);
			case A_FLOAT: return (uint16_t) controlled->readF(addr1,addr2);
			case A_DOUBLE: return (uint16_t) controlled->readD(addr1,addr2);
			case A_TIME: return (uint16_t) controlled->readT(addr1,addr2);
			case A_STRING: return 0;
			case BAD_TYPE: return 0;
		}
		return 0;
	}
	int16_t readI(){
		switch(addr1.type){
			case A_BYTE: return (int16_t) controlled->readB(addr1,addr2);
			case A_UINT: return (int16_t) controlled->readU(addr1,addr2);
			case A_INT: return (int16_t) controlled->readI(addr1,addr2);
			case A_LONG: return (int16_t) controlled->readL(addr1,addr2);
			case A_FLOAT: return (int16_t) controlled->readF(addr1,addr2);
			case A_DOUBLE: return (int16_t) controlled->readD(addr1,addr2);
			case A_TIME: return (int16_t) controlled->readT(addr1,addr2);
			case A_STRING: return 0;
			case BAD_TYPE: return 0;
		}
		return 0;
	}
	int32_t readL(){
		switch(addr1.type){
			case A_BYTE: return (int32_t) controlled->readB(addr1,addr2);
			case A_UINT: return (int32_t) controlled->readU(addr1,addr2);
			case A_INT: return (int32_t) controlled->readI(addr1,addr2);
			case A_LONG: return (int32_t) controlled->readL(addr1,addr2);
			case A_FLOAT: return (int32_t) controlled->readF(addr1,addr2);
			case A_DOUBLE: return (int32_t) controlled->readD(addr1,addr2);
			case A_TIME: return (int32_t) controlled->readT(addr1,addr2);
			case A_STRING: return 0;
			case BAD_TYPE: return 0;
		}
		return 0;
	}
	float readF(){
		switch(addr1.type){
			case A_BYTE: return (float) controlled->readB(addr1,addr2);
			case A_UINT: return (float) controlled->readU(addr1,addr2);
			case A_INT: return (float) controlled->readI(addr1,addr2);
			case A_LONG: return (float) controlled->readL(addr1,addr2);
			case A_FLOAT: return (float) controlled->readF(addr1,addr2);
			case A_DOUBLE: return (float) controlled->readD(addr1,addr2);
			case A_TIME: return (float) controlled->readT(addr1,addr2);
			case A_STRING: return 0;
		  case BAD_TYPE: return 0;
		}
		return 0;
	}
	double readD(){
		switch(addr1.type){
			case A_BYTE: return (double) controlled->readB(addr1,addr2);
			case A_UINT: return (double) controlled->readU(addr1,addr2);
			case A_INT: return (double) controlled->readI(addr1,addr2);
			case A_LONG: return (double) controlled->readL(addr1,addr2);
			case A_FLOAT: return (double) controlled->readF(addr1,addr2);
			case A_DOUBLE: return (double) controlled->readD(addr1,addr2);
			case A_TIME: return (double) controlled->readT(addr1,addr2);
			case A_STRING: return 0;
			case BAD_TYPE: return 0;
		}
		return 0;
	}
	uint32_t readT(){
		switch(addr1.type){
			case A_BYTE: return (uint32_t) controlled->readB(addr1,addr2);
			case A_UINT: return (uint32_t) controlled->readU(addr1,addr2);
			case A_INT: return (uint32_t) controlled->readI(addr1,addr2);
			case A_LONG: return (uint32_t) controlled->readL(addr1,addr2);
			case A_FLOAT: return (uint32_t) controlled->readF(addr1,addr2);
			case A_DOUBLE: return (uint32_t) controlled->readD(addr1,addr2);
			case A_TIME: return (uint32_t) controlled->readT(addr1,addr2);
			case A_STRING: return 0;
			case BAD_TYPE: return 0;
		}
		return 0;
	}

	const char* getName(){
		return "Var";
	}


	~VariableFunc(){

	};

	private:
	Controller::Controlled* controlled;
	ADDR1 addr1;
	uint8_t addr2 = 0;
};



class WriteFunc:public Func{
	bool parse(uint16_t &pointer,char* input){
		if (input[pointer]!='[')
			return false;
		pointer++;
		if (input[pointer]!='$')
			return false;
		pointer++;

		target = new ADDR1(pointer,input);

		pointer++;
		if (input[pointer]=='"'){
				pointer++;
				int i;
				for (i = 0; i < 255; i++){
					char x = input[pointer+i];
					if (x=='"' ){
						text = new char[i+1];
						text[i]='\0';
						i--;
						for (;i>=0;i--){
							text[i] = input[pointer+i];
						}
						pointer++;
						return true;
					}
					if (x=='\0'){
						controller->getErrorLogger()->println("Write text operator missing closing \"");
						return false;
					}
				}
				controller->getErrorLogger()->println("Write text too long");
				return false;
		}else{
			valFunc = createFunc(pointer,input,controller);
			pointer++;
			if (valFunc == 0){
				controller->getErrorLogger()->println("Couldn't parse function to assign for write");
				return false;
			}
			return true;
		}
	}
	ADDRTYPE getType(){
		return target->type;
	}

 double process(){

	 if (text!= 0){
		 writeText();
		 return 0;
	 }

	 Controller::Controlled* targetMod = controller->getControlled(target->modID);

	 double val = 0;
	 switch(target->type){
		 case A_BYTE: {
			 uint8_t temp;
			 temp  =  valFunc->readB();
			 val = (double) temp ;
			 if (targetMod != 0)
			 	targetMod->write(*target,temp);
		 break;
	 }
		 case A_UINT: {
			 uint16_t temp;
			 temp  =  valFunc->readU();
			 val = (double) temp ;
			 if (targetMod != 0)
			 	targetMod->write(*target,temp);
		 break;
	 }
		 case A_INT: {
			 int16_t temp;
			 temp  =  valFunc->readI();
			 val = (double) temp ;
			 if (targetMod != 0)
			 	targetMod->write(*target,temp);
		 break;
	 }
		 case A_LONG: {
			 int32_t temp;
			 temp  =  valFunc->readL();
			 val = (double) temp ;
			 if (targetMod != 0)
			 	targetMod->write(*target,temp);
		 break;
	 }
		 case A_FLOAT:{
			 float temp;
			 temp  =  valFunc->readF();
			 val = (double) temp ;
			 if (targetMod != 0)
			 	targetMod->write(*target,temp);
		 break;
	 }
		 case A_DOUBLE:{
			 double temp;
			 temp  =  valFunc->readD();
			 val = (double) temp ;
			 if (targetMod != 0)
			 	targetMod->write(*target,temp);
		 break;
	 }
		 case A_TIME: {
			 uint32_t temp;
			 temp  =  valFunc->readT();
			 val = (double) temp ;
			 if (targetMod != 0)
			 	targetMod->write(*target,temp);
		 break;
	 }
		 case A_STRING:
		 return 0;
		 case BAD_TYPE: return 0;
	 }
	 return val;
 }

	uint8_t readB(){
		return (uint8_t) process();
	}
	uint16_t readU(){
		return (uint16_t) process();
	}
	int16_t readI(){
		return (int16_t) process();
	}
	int32_t readL(){
		return (int32_t) process();
	}
	float readF(){
				return (float) process();
	}
	double readD(){
			return (double) process();
	}
	uint32_t readT(){
				return (uint32_t) process();
	}

	const char* getName(){
		return "w[]";
	}


	~WriteFunc(){
		if (target!=0)
			delete target;
		if (valFunc!=0)
			delete valFunc;
		if (text!=0)
			delete text;
	};

	private:
		void writeText(){
			Controller::Controlled* targetMod = controller->getControlled(target->modID);
			if (targetMod == 0)
				return;
			targetMod->write(*target,text);
		}
		Func* valFunc=0;
		ADDR1* target=0;
		char* text=0;
};

class TimeFunc:public Func{
	bool parse(uint16_t &pointer,char* text){
		return true;
	}
	ADDRTYPE getType(){
		return A_TIME;
	}
	uint8_t readB(){
		return (uint8_t) Controller::lastProcessedMSTime;
	}
	uint16_t readU(){
		return (uint16_t) Controller::lastProcessedMSTime;
	}
	int16_t readI(){
		return (int16_t) Controller::lastProcessedMSTime;
	}
	int32_t readL(){
		return (int32_t) Controller::lastProcessedMSTime;
	}
	float readF(){
		return (float) Controller::lastProcessedMSTime;
	}
	double readD(){
		return (double) Controller::lastProcessedMSTime;
	}
	uint32_t readT(){
		return (uint32_t) Controller::lastProcessedMSTime;
	}

	const char* getName(){
		return "Time";
	}

	~TimeFunc(){};
};

class IfFunc:public Func{
	bool parse(uint16_t &pointer,char* text){
		eval = createFunc(pointer,text,controller);
		if (eval == 0 )
			return false;
		if (text[pointer] != '[')
			return false;
		pointer++;
		tVal = createFunc(pointer,text,controller);
		if (tVal == 0 )
			return false;
		if (text[pointer] != ',')
			return false;
		pointer++;
		fVal = createFunc(pointer,text,controller);
		if (fVal == 0 )
			return false;
		if (text[pointer] != ']')
			return false;
		pointer++;
		return true;
	}
	ADDRTYPE getType(){
		return tVal->getType();
	}
	uint8_t readB(){
		return (uint8_t) (eval->readB()==0)?fVal->readB():tVal->readB();
	}
	uint16_t readU(){
		return (uint16_t) (eval->readB()==0)?fVal->readU():tVal->readU();
	}
	int16_t readI(){
		return (int16_t) (eval->readB()==0)?fVal->readI():tVal->readI();
	}
	int32_t readL(){
		return (int32_t) (eval->readB()==0)?fVal->readL():tVal->readL();
	}
	float readF(){
		return (float) (eval->readB()==0)?fVal->readF():tVal->readF();
	}
	double readD(){
		return (double) (eval->readB()==0)?fVal->readD():tVal->readD();
	}
	uint32_t readT(){
		return (uint32_t) (eval->readB()==0)?fVal->readT():tVal->readT();
	}

	const char* getName(){
		return "If/Else";
	}

	~IfFunc(){
		if (eval != 0)
			delete eval;
		if (fVal != 0)
			delete fVal;
		if (tVal != 0)
			delete tVal;
	};

	private:

	Func* eval=0;
	Func* fVal=0;
	Func* tVal=0;

};

	enum COMPTYPE{
		EQ,NEQ,LT,GT,LTE,GTE,AND,OR,ADD,SUB,MULT,DIV,POW,MOD
	};

class BiMathFunc:public Func{
public:
	BiMathFunc(COMPTYPE _compType,ADDRTYPE _type,Func* _lVal,Func* _rVal){
		compType = _compType;
		type = _type;
		lVal = _lVal;
		rVal = _rVal;
	}

	bool parse(uint16_t &pointer,char* text){
		return false;
	}

	bool simplify(Func* (&better) ){
		return false;
	}

	ADDRTYPE getType(){
		return type;
	}

	uint8_t evalB(){
		uint8_t l = lVal->readB();
		uint8_t r = rVal->readB();
		switch (compType){
			case ADD: return l+r;
			case SUB: return l-r;
			case MULT:  return l*r;
			case DIV: return l/r;
			case POW: return pow(l,r);
			case MOD: return l%r;
			default: return 0;
		}
			return 0;
	}

	uint16_t evalU(){
		uint16_t l = lVal->readU();
		uint16_t r = rVal->readU();
		switch (compType){
			case ADD: return l+r;
			case SUB: return l-r;
			case MULT:  return l*r;
			case DIV: return l/r;
			case POW: return pow(l,r);
			case MOD: return l%r;
			default: return 0;
		}
			return 0;
	}

	int16_t evalI(){
		int16_t l = lVal->readI();
		int16_t r = rVal->readI();
		switch (compType){
			case ADD: return l+r;
			case SUB: return l-r;
			case MULT:  return l*r;
			case DIV: return l/r;
			case POW: return pow(l,r);
			case MOD: return l%r;
			default: return 0;
		}
			return 0;
	}

	int32_t evalL(){
		int32_t l = lVal->readL();
		int32_t r = rVal->readL();
		switch (compType){
			case ADD: return l+r;
			case SUB: return l-r;
			case MULT:  return l*r;
			case DIV: return l/r;
			case POW: return pow(l,r);
			case MOD: return l%r;
			default: return 0;
		}
			return 0;
	}

	float evalF(){
		float l = lVal->readF();
		float r = rVal->readF();
		switch (compType){
			case ADD: return l+r;
			case SUB: return l-r;
			case MULT:  return l*r;
			case DIV: return l/r;
			case POW: return pow(l,r);
			case MOD: return (long)l%(long)r;
			default: return 0;
		}
			return 0;
	}

	double evalD(){
		double l = lVal->readD();
		double r = rVal->readD();
		switch (compType){
			case ADD: return l+r;
			case SUB: return l-r;
			case MULT:  return l*r;
			case DIV: return l/r;
			case POW: return pow(l,r);
			case MOD: return (long)l%(long)r;
			default: return 0;
		}
			return 0;
	}

	uint32_t evalT(){
		uint32_t l = lVal->readT();
		uint32_t r = rVal->readT();
		switch (compType){
			case ADD: return l+r;
			case SUB: return l-r;
			case MULT:  return l*r;
			case DIV: return l/r;
			case POW: return pow(l,r);
			case MOD: return l%r;
			default: return 0;
		}
			return 0;
	}

	uint8_t readB(){
		switch (type){
			case A_BYTE: return  evalB();
			case A_UINT: return  evalU();
			case A_INT: return  evalI();
			case A_LONG: return  evalL();
			case A_FLOAT: return  evalF();
			case A_DOUBLE: return  evalD();
			case A_TIME: return  evalT();
			case A_STRING: return 0;
			case BAD_TYPE: return 0;
		}
		return 0;
	}
	uint16_t readU(){
		switch (type){
			case A_BYTE: return  evalB();
			case A_UINT: return  evalU();
			case A_INT: return  evalI();
			case A_LONG: return  evalL();
			case A_FLOAT: return  evalF();
			case A_DOUBLE: return  evalD();
			case A_TIME: return  evalT();
			case A_STRING: return 0;
			case BAD_TYPE: return 0;
		}
		return 0;
	}
	int16_t readI(){
		switch (type){
			case A_BYTE: return  evalB();
			case A_UINT: return  evalU();
			case A_INT: return  evalI();
			case A_LONG: return  evalL();
			case A_FLOAT: return  evalF();
			case A_DOUBLE: return  evalD();
			case A_TIME: return  evalT();
			case A_STRING: return 0;
			case BAD_TYPE: return 0;
		}
		return 0;
	}
	int32_t readL(){
		switch (type){
			case A_BYTE: return  evalB();
			case A_UINT: return  evalU();
			case A_INT: return  evalI();
			case A_LONG: return  evalL();
			case A_FLOAT: return  evalF();
			case A_DOUBLE: return  evalD();
			case A_TIME: return  evalT();
			case A_STRING: return 0;
			case BAD_TYPE: return 0;
		}
		return 0;
	}
	float readF(){
		switch (type){
			case A_BYTE: return  evalB();
			case A_UINT: return  evalU();
			case A_INT: return  evalI();
			case A_LONG: return  evalL();
			case A_FLOAT: return  evalF();
			case A_DOUBLE: return  evalD();
			case A_TIME: return  evalT();
			case A_STRING: return 0;
			case BAD_TYPE: return 0;
		}
		return 0;
	}
	double readD(){
		switch (type){
			case A_BYTE: return  evalB();
			case A_UINT: return  evalU();
			case A_INT: return  evalI();
			case A_LONG: return  evalL();
			case A_FLOAT: return  evalF();
			case A_DOUBLE: return  evalD();
			case A_TIME: return  evalT();
			case A_STRING: return 0;
			case BAD_TYPE: return 0;
		}
		return 0;
	}
	uint32_t readT(){
		switch (type){
			case A_BYTE: return  evalB();
			case A_UINT: return  evalU();
			case A_INT: return  evalI();
			case A_LONG: return  evalL();
			case A_FLOAT: return  evalF();
			case A_DOUBLE: return  evalD();
			case A_TIME: return  evalT();
			case A_STRING: return 0;
			case BAD_TYPE: return 0;
		}
		return 0;
	}

	~BiMathFunc(){
		if (lVal != 0)
			delete lVal;
		if (rVal != 0)
			delete rVal;
	}

	private:

	COMPTYPE compType;

	ADDRTYPE type;

	Func* lVal=0;
	Func* rVal=0;

};


class CompareFunc:public Func{

	bool parse(uint16_t &pointer,char* text){
		lVal = createFunc(pointer,text,controller);
		if (lVal == 0 )
			return false;
		switch(text[pointer]){
			case '&':
				pointer++;
				if (text[pointer]!='&')
					return false;
				compType = AND;
				break;
			case '|':
				pointer++;
				if (text[pointer]!='|')
					return false;
				compType = OR;
				break;
			case '=':
				compType = EQ;
				pointer++;
				if (text[pointer]!='=')
					return false;
				break;
			case '!':
				compType = EQ;
				pointer++;
				if (text[pointer]!='=')
					return false;
				break;
			case '<':
				if (text[pointer+1]=='='){
					compType = LTE;
					pointer++;
				} else {
					compType = LT;
				}
				break;
			case '>':
				if (text[pointer+1]=='='){
					compType = GTE;
					pointer++;
				} else {
					compType = GT;
				}
				break;
			case '+':
				compType = ADD;
				break;
			case '-':
				compType = SUB;
				break;
			case '/':
				compType = DIV;
				break;
			case '*':
				compType = MULT;
				break;
			case '^':
				compType = POW;
				break;
			case '%':
				compType = MOD;
				break;
			default:
				return false;
		}
		pointer++;
		rVal = createFunc(pointer,text,controller);
		if (rVal == 0 )
			return false;
		if (text[pointer] != '}')
			return false;
		pointer++;
		ADDRTYPE lType = lVal->getType();
		ADDRTYPE rType = rVal->getType();
		if (rType>lType)
			type = rType;
		else
			type = lType;
		return true;
	}

	bool simplify(Func* (&better) ){
		if (compType == ADD || compType == SUB || compType == MULT || compType == DIV ||
		 compType == POW || compType == MOD ){
			better = new BiMathFunc(compType,type,lVal,rVal);
			lVal = 0;
			rVal = 0;
			return true;
		}
		return false;
	}

	ADDRTYPE getType(){
		return A_BYTE;
	}

	uint8_t evalB(){
		uint8_t l = lVal->readB();
		uint8_t r = rVal->readB();
		switch (compType){
			case EQ: return l==r;
			case NEQ: return l!=r;
			case GT:  return l>r;
			case GTE: return l>=r;
			case LT: return l<r;
			case LTE: return l<=r;
			case AND: return l&&r;
			case OR: return l||r;
			default: return 0;
		}
			return 0;
	}

	uint8_t evalU(){
		uint16_t l = lVal->readU();
		uint16_t r = rVal->readU();
		switch (compType){
			case EQ: return l==r;
			case NEQ: return l!=r;
			case GT:  return l>r;
			case GTE: return l>=r;
			case LT: return l<r;
			case LTE: return l<=r;
			case AND: return l&&r;
			case OR: return l||r;
			default: return 0;
		}
			return 0;
	}

	uint8_t evalI(){
		int16_t l = lVal->readI();
		int16_t r = rVal->readI();
		switch (compType){
			case EQ: return l==r;
			case NEQ: return l!=r;
			case GT:  return l>r;
			case GTE: return l>=r;
			case LT: return l<r;
			case LTE: return l<=r;
			case AND: return l&&r;
			case OR: return l||r;
			default: return 0;
		}
			return 0;
	}

	uint8_t evalL(){
		int32_t l = lVal->readL();
		int32_t r = rVal->readL();
		switch (compType){
			case EQ: return l==r;
			case NEQ: return l!=r;
			case GT:  return l>r;
			case GTE: return l>=r;
			case LT: return l<r;
			case LTE: return l<=r;
			case AND: return l&&r;
			case OR: return l||r;
			default: return 0;
		}
			return 0;
	}

	uint8_t evalF(){
		float l = lVal->readF();
		float r = rVal->readF();
		switch (compType){
			case EQ: return l==r;
			case NEQ: return l!=r;
			case GT:  return l>r;
			case GTE: return l>=r;
			case LT: return l<r;
			case LTE: return l<=r;
			case AND: return l&&r;
			case OR: return l||r;
			default: return 0;
		}
			return 0;
	}

	uint8_t evalD(){
		double l = lVal->readD();
		double r = rVal->readD();
		switch (compType){
			case EQ: return l==r;
			case NEQ: return l!=r;
			case GT:  return l>r;
			case GTE: return l>=r;
			case LT: return l<r;
			case LTE: return l<=r;
			case AND: return l&&r;
			case OR: return l||r;
			default: return 0;
		}
			return 0;
	}

	uint8_t evalT(){
		uint32_t l = lVal->readT();
		uint32_t r = rVal->readT();
		switch (compType){
			case EQ: return l==r;
			case NEQ: return l!=r;
			case GT:  return l>r;
			case GTE: return l>=r;
			case LT: return l<r;
			case LTE: return l<=r;
			case AND: return l&&r;
			case OR: return l||r;
			default: return 0;
		}
			return 0;
	}

	uint8_t readB(){
		switch (type){
			case A_BYTE: return  evalB();
			case A_UINT: return  evalU();
			case A_INT: return  evalI();
			case A_LONG: return  evalL();
			case A_FLOAT: return  evalF();
			case A_DOUBLE: return  evalD();
			case A_TIME: return  evalT();
			default: return 0;
		}
		return 0;
	}
	uint16_t readU(){
		return readB();
	}
	int16_t readI(){
		return readB();
	}
	int32_t readL(){
		return readB();
	}
	float readF(){
		return readB();
	}
	double readD(){
		return readB();
	}
	uint32_t readT(){
		return readB();
	}

	const char* getName(){
		switch (compType){
			case EQ: name[1] = '=';name[2] = '=';break;
			case NEQ: name[1] = '!';name[2] = '=';break;
			case LT: name[1] = '<';name[2] = ' ';break;
			case LTE: name[1] = '<';name[2] = '=';break;
			case GT: name[1] = '>';name[2] = ' ';break;
			case GTE: name[1] = '>';name[2] = '=';break;
			case AND: name[1] = '&';name[2] = '&';break;
			case OR: name[1] = '|';name[2] = '|';break;
			case ADD: name[1] = '+';name[2] = ' ';break;
			case SUB: name[1] = '-';name[2] = ' ';break;
			case MULT: name[1] = '*';name[2] = ' ';break;
			case DIV: name[1] = '/';name[2] = ' ';break;
			case POW: name[1] = '^';name[2] = ' ';break;
			case MOD: name[1] = '%';name[2] = ' ';break;
			default: break;
		}
		return name;
	}

	~CompareFunc(){
		if (lVal != 0)
			delete lVal;
		if (rVal != 0)
			delete rVal;
	}

	private:

	COMPTYPE compType;

	ADDRTYPE type;

	char name[5] = {'{','?','?','}','\0'};

	Func* lVal=0;
	Func* rVal=0;

};

class SqrtFunc:public Func{



	bool parse(uint16_t &pointer,char* text){

	val = createFunc(pointer,text,controller);
		if (val == 0 )
			return false;

		return true;
	}
	ADDRTYPE getType(){
		return A_DOUBLE;
	}
	uint8_t readB(){
		return (uint8_t) sqrt(val->readD());
	}
	uint16_t readU(){
		return (uint16_t) sqrt(val->readD());
	}
	int16_t readI(){
		return (int16_t) sqrt(val->readD());
	}
	int32_t readL(){
		return (int32_t) sqrt(val->readD());
	}
	float readF(){
		return (float) sqrt(val->readD());
	}
	double readD(){
		return (double) sqrt(val->readD());
	}
	uint32_t readT(){
		return (uint32_t) sqrt(val->readD());
	}

	const char* getName(){
		return "Sqrt";
	}

	~SqrtFunc(){
		if (val!=0)
			delete val;
	};

private:
	Func *val;

};

class ColorFunc:public Func{

	uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
		return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	}


	bool parse(uint16_t &pointer,char* text){

	r = createFunc(pointer,text,controller);
		if (r == 0 ){
			controller->getErrorLogger()->println("Failed to parse red");
			return false;
		}
		pointer++;
	g = createFunc(pointer,text,controller);
		if (g == 0 ){
			delete r;
			r = 0;
			controller->getErrorLogger()->println("Failed to parse green");
			return false;
		}
		pointer++;
	b = createFunc(pointer,text,controller);
		if (b == 0 ){
			delete r;
			r = 0;
			delete g;
			g = 0;
			controller->getErrorLogger()->println("Failed to parse blue");
			return false;
		}
		pointer++;
		return true;
	}
	ADDRTYPE getType(){
		return A_UINT;
	}
	uint8_t readB(){
		return (uint8_t) 0;
	}
	uint16_t readU(){
		return  color565(r->readB(),g->readB(),b->readB());
	}
	int16_t readI(){
		return (int16_t) 0;
	}
	int32_t readL(){
		return (int32_t)  readU();
	}
	float readF(){
		return (float)  readU();
	}
	double readD(){
		return (double)  readU();
	}
	uint32_t readT(){
		return (uint32_t)  readU();
	}

	const char* getName(){
		return "color565";
	}

	~ColorFunc(){
		if (r!=0)
			delete r;
		if (g!=0)
			delete g;
		if (b!=0)
			delete b;
	};

private:



	Func *r=0;
	Func *g=0;
	Func *b=0;

};

class NegateFunc:public Func{



	bool parse(uint16_t &pointer,char* text){
		val = createFunc(pointer,text,controller);

		if (val == 0 )
			return false;
		type = val->getType();

		return true;
	}
	ADDRTYPE getType(){
		return type;
	}
	uint8_t readB(){
		return (uint8_t) -(val->readB());
	}
	uint16_t readU(){
		return (uint16_t) -(val->readU());
	}
	int16_t readI(){
		return (int16_t) -(val->readI());
	}
	int32_t readL(){
		return (int32_t) -(val->readL());
	}
	float readF(){
		return (float) -(val->readF());
	}
	double readD(){
		return (double) -(val->readD());
	}
	uint32_t readT(){
		return (uint32_t) -(val->readT());
	}

	const char* getName(){
		return "Negate";
	}

	~NegateFunc(){
		if (val!=0)
			delete val;
	};

private:
	ADDRTYPE type;
	Func *val;

};

class CastFunc:public Func{

	bool parse(uint16_t &pointer,char* text){

		if (!ADDR1::parseType(type,text[pointer]))
				return false;
		pointer++;

		if (text[pointer] != '[')
			return false;

		pointer++;
		val = createFunc(pointer,text,controller);

		if (val == 0 )
			return false;

		if (text[pointer] != ']')
			return false;
		pointer++;

		return true;
	}
	ADDRTYPE getType(){
		return type;
	}
	uint8_t readB(){
		return (val->readB());
	}
	uint16_t readU(){
		return (val->readU());
	}
	int16_t readI(){
		return (val->readI());
	}
	int32_t readL(){
		return (val->readL());
	}
	float readF(){
		return (val->readF());
	}
	double readD(){
		return (val->readD());
	}
	uint32_t readT(){
		return (val->readT());
	}

	const char* getName(){
		return "cast";
	}

	~CastFunc(){
		if (val!=0)
			delete val;
	};

private:
	ADDRTYPE type;
	Func *val;

};

double slope (double x0, double x1,double y0,double y1){
		double b = x1-x0;
		#ifdef DEBUG
		Serial.print("Solving slope:");
		Serial.println(b);
		#endif
		if (b==0){
			if (y0<y1)
				b= 999999;
			else
				b= -999999;
		}else{
			b = (y1 - y0)/b;
		}
		return b;
}

class LinearISeg:public ISeg{
	virtual void updateSeg(){
		b = slope(getX0(),getX1(),getY0(),getY1());
		a = getY0() - b*getX0();
	};

	virtual double calculate(double xTarget){
		return a+b*xTarget;
	};

	double a=0,b=0;
};

class CubicHermiteISeg:public ISeg{
	virtual void updateSeg(){
		//solve d0 by looking back at prev segmant (or forward to p1 if no prev) and solving derivative
		if (prev != 0 ){
			d0 = slope(prev->getX0(),getX1(),prev->getY0(),getY1());
		}else{
			d0 = slope(getX0(),getX1(),getY0(),getY1());
		}

		if (next != 0 ){
			d1 = slope(getX0(),next->getX1(),getY0(),next->getY1());
		}else{
			d1 = slope(getX0(),getX1(),getY0(),getY1());
		}

		x0 = getX0();
		x1 = getX1();

		f0 = getY0();
		f1 = getY1();

		double h = x1 - x0;
		double df = (f1 - f0)/h;

		c2 = - ( 2.0 * d0 - 3.0 * df + d1 ) / h;
  		c3 =   (       d0 - 2.0 * df + d1 ) / h / h;

  		#ifdef DEBUG
  		Serial.print(" h:");
  		Serial.print(h);
  		Serial.print(" df:");
  		Serial.print(df);
  		Serial.print(" d0:");
  		Serial.print(d0);
  		Serial.print(" d1:");
  		Serial.print(d1);
  		Serial.print(" x0:");
  		Serial.print(x0);
  		Serial.print(" x1:");
  		Serial.print(x1);
  		Serial.print(" f0:");
  		Serial.print(f0);
  		Serial.print(" f1:");
  		Serial.print(f1);
  		Serial.print(" c2:");
  		Serial.print(c2);
  		Serial.print(" c3:");
  		Serial.println(c3);
  		#endif

	};

	virtual double calculate(double xTarget){
		return  f0 + ( xTarget - x0 ) * ( d0
                    + ( xTarget - x0 ) * ( c2
                    + ( xTarget - x0 ) *   c3 ) );
	};

	double d0=0,d1=0,c2=0,c3=0,x0=0,x1=0,f0=0,f1=0;
};


ISeg* ISeg::parse(uint16_t &pointer,char* command,Controller* controller){
		ISeg* start=0;
		ISeg* current=0;
		while(command[pointer]!='\0'){
			ISeg* temp = ISeg::getNext(current,pointer,command,controller);

			if (temp == 0){
				ISeg::del(start);
				controller->getErrorLogger()->setParseError(command,pointer,"Failed to parse interpolation");
				return 0;
			}

			if (current == 0){
				current = temp;
				start = temp;
			}else{
				current->next = temp;
				temp->prev = current;
				current = temp;
			}


		}

		return start;

	}

ISeg* ISeg::getNext(ISeg* prev,uint16_t &pointer,char* command,Controller* controller){
		ISeg* next = 0;
		Func* fX = 0;
		Func* fY = 0;
		if (prev == 0){

			fX = createFunc(pointer,command,controller);

			if (fX == 0)
				return 0;
			if (command[pointer]!=','){
				return 0;
			}

			pointer++;
			fY = createFunc(pointer,command,controller);
			if (fY == 0)
				return 0;
		}else{
			fX = prev->fx1;
			fY = prev->fy1;
		}

		switch (command[pointer]){
			case '|': next = new ISeg(); break;
			case '/': next = new LinearISeg(); break;
			case '~': next = new CubicHermiteISeg(); break;
			default:
				controller->getErrorLogger()->setParseError(command,pointer,"Unkown interp type, expected | or / or ~");
				return 0;
		}

		next->fx0= fX;
		next->fy0= fY;

		pointer++;


		fX = createFunc(pointer,command,controller);
		if (fX == 0)
			return 0;
		if (command[pointer]!=','){
			return 0;
		}
		pointer++;
		fY = createFunc(pointer,command,controller);
		if (fY == 0)
			return 0;

		next->fx1= fX;
		next->fy1= fY;
		return next;
	}

double ISeg::eval(ISeg* startSeg,double inX,uint32_t time){
		#ifdef DEBUG
		Serial.print("Starting eval at :");
		Serial.println(inX);
		#endif
		if (startSeg == 0){
			return 0;
		}

		while(true){
			startSeg->validate(time);
			if (inX <= startSeg->getX0())
				return startSeg->getY0();
			if (inX > startSeg->getX1()){
				if (startSeg->next == 0 )
					return startSeg->getY1();
				else
					startSeg = startSeg->next;
				continue;
			}
			return startSeg->interpolate(inX);
		}

	}

void ISeg::del(ISeg* start){
		if (start == 0){
			#ifdef DEBUG
				Serial.println("Delete called null ISEG");
			#endif
			return;
		}

		while(true){
			if (start->next == 0){
				while (true){
					ISeg* back = start->prev;
					#ifdef DEBUG
						Serial.print("Del  ISEG:");
						Serial.println((uint32_t)start);
					#endif
					delete start;
					if (back == 0){
						return;
					}
					start = back;
					back->decoupleFront();
				}
			}
			#ifdef DEBUG
				Serial.print("Del iterating ISEGS to:");
				Serial.println((uint32_t)start->next);
			#endif
			start = start->next;
		}
	}



Func* createFunc(uint16_t &pointer,char* text, Controller *controller){

	Func* res = 0;
	switch (text[pointer]){
		case 't':res = new TimeFunc(); break;
		case 'w':res = new WriteFunc(); break;
		case '#':res = new LiteralFunc(); break;
		case '$':res = new VariableFunc(); break;
		case '?':res = new IfFunc(); break;
		case '{':res = new CompareFunc(); break;
		case 'r':res = new SqrtFunc(); break;
		case '-':res = new NegateFunc(); break;
		case 'i':res = new InterpFunc(); break;
		case 'c':res = new CastFunc(); break;
		case '[':res = new ColorFunc(); break;

		default:

					controller->getErrorLogger()->setParseError(text,pointer,"Unknown function");
		return 0;
	}
	pointer++;
	res->controller = controller;

	if (res->parse(pointer,text)){
		Func *simpler;
		if (res->simplify(simpler)){
			 delete res;
			 res = simpler;
			 res->controller = controller;
		}
		return res;
	}else{
		controller->getErrorLogger()->setParseError(text,pointer,"Failed to parse function");
		delete res;

		return 0;
	}
}
