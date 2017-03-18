#ifndef DH_CONTROLLEDCALC_H__
#define DH_CONTROLLEDCALC_H__
#include "dh_controller.h"

#define FUNC_COUNT 20
#define TIME_VARCONST 8339
#define MAX_INTERPS 20
#define MAX_STRING_LEN 64;

//#define DEBUG ON

class Func{
	public:
	virtual bool parse(uint16_t &pointer,char* text)=0;
	virtual ADDRTYPE getType()=0;
	virtual uint8_t readB()=0;
	virtual uint16_t readU()=0;
	virtual int16_t readI()=0;
	virtual int32_t readL()=0;
	virtual float readF()=0;
	virtual double readD()=0;
	virtual uint32_t readT()=0;
	virtual bool simplify(Func* (&better) ){
		return false;
	}
	virtual const char* getName(){
		return "No Name";
	}
	virtual ~Func(){};
	Controller* controller=0;
};

class CommandListFunc:public Func{
public:
	CommandListFunc(Func* inc ){
					func = inc;
	}
	bool parse(uint16_t &pointer,char* text){
		return false;
	}

	const char* getName(){
		return "Command List(,)";
	}

	~CommandListFunc(){
		if (func != 0)
			delete func;
		if (next != 0)
			delete next;
	};

	ADDRTYPE getType(){
		return func->getType();
	}
	uint8_t readB(){
		uint8_t temp = func->readB();
		if (next!=0)
			next->readB();
		return temp;
	}
	uint16_t readU(){
		uint16_t temp = func->readU();
		if (next!=0)
			next->readU();
		return  temp;
	}
	int16_t readI(){
		int16_t temp = func->readI();
		if (next!=0)
			next->readI();
		return  temp;
	}
	int32_t readL(){
		int32_t temp = func->readL();
		if (next!=0)
			next->readL();
		return  temp;
	}
	float readF(){
		float temp = func->readF();
		if (next!=0)
			next->readF();
		return  temp;
	}
	double readD(){
		double temp = func->readD();
		if (next!=0)
			next->readD();
		return  temp;
	}
	uint32_t readT(){
		uint32_t temp = func->readT();
		if (next!=0)
			next->readT();
		return  temp;
	}

	Func* func=0;
	Func* next=0;
};

Func* createFunc(uint16_t &pointer,char* text, Controller* controller);

class ISeg{
	public:

	static ISeg* interpRoots[MAX_INTERPS];

	static ISeg* parse(uint16_t &pointer,char* command, Controller* controller);

	static ISeg* getNext(ISeg* prev,uint16_t &pointer,char* command, Controller* controller);

	static double eval(ISeg* startSeg,double inX,uint32_t time);

	static void del(ISeg* start);

	virtual void updateSeg(){
		 // nothing to do here since we return the immediate value.
	};

	virtual double calculate(double xTarget){
		return getY1();
	};

	double interpolate(double xTarget){
		if (expiredCurve){
			#ifdef DEBUG
			Serial.println("Updating Segment");
			#endif
			updateSeg();
			expiredCurve = false;
		}
			#ifdef DEBUG
			double y = calculate(xTarget);
			Serial.print("intepolated x=");
			Serial.print(xTarget);
			Serial.print(" y=");
			Serial.println(y);
			return y;
			#else
			return calculate(xTarget);
			#endif
	}

	double getX0(){
		if (expiredX0){
			x0 = fx0->readD();
			expiredX0 = false;
		}
		return x0;
	}

	double getX1(){
		if (expiredX1){
			x1 = fx1->readD();
			expiredX1 = false;
		}
		return x1;
	}

	double getY0(){
		if (expiredY0){
			y0 = fy0->readD();
			expiredY0 = false;
		}
		return y0;
	}

	double getY1(){
		if (expiredY1){
			y1 = fy1->readD();
			expiredY1 = false;
		}
		return y1;
	}

	void validate(uint32_t time){
		if (time < expTime)
			return;
		expTime = time + expDur;

		expiredCurve = true;
		expiredX0 = true;
		expiredX1 = true;
		expiredY0 = true;
		expiredY1 = true;
	}

	virtual ~ISeg(){
		if (fx0 != 0)
			delete fx0;
		if (fx1 != 0)
			delete fx1;
		if (fy0 != 0)
			delete fy0;
		if (fy1 != 0)
			delete fy1;
	}

	void decoupleFront(){
		fx1 = 0;
		fy1 = 0;
		next = 0;
	}

	protected:
		uint32_t expDur = 1000;
		uint32_t expTime = 0;
		ISeg* prev;
		ISeg* next;

		bool expiredCurve = true;
		bool expiredX0 = true;
		bool expiredY0 = true;
		bool expiredX1 = true;
		bool expiredY1 = true;
		double x0;
		double x1;
		double y0;
		double y1;
		Func* fx0;
		Func* fx1;
		Func* fy0;
		Func* fy1;

};




class ControlledCalc: public Controller::Controlled{
public:

	uint8_t readB(ADDR1 addr,uint8_t addr2){
		Func* func = 0;
		if (getFunc(func,addr,addr2)){
			if (func == 0)
				return 0;
			else
				return func->readB();
		}
		uint8_t num;
		if (getVar(num,addr)){
			return bVars[num];
		}else{
			if (addr.addr == TIME_VARCONST)
				return Controller::lastProcessedMSTime;
			return 0;
		}
	}
	uint16_t readU(ADDR1 addr,uint8_t addr2){
		Func* func = 0;
		if (getFunc(func,addr,addr2)){
			if (func == 0){
							return 0;
			}else
				return func->readU();
		}
		uint8_t num;
		if (getVar(num,addr)){
			return uVars[num];
		}else{
			if (addr.addr == TIME_VARCONST)
				return Controller::lastProcessedMSTime;
			return 0;
		}
	}
	int16_t readI(ADDR1 addr,uint8_t addr2){
		Func* func = 0;
		if (getFunc(func,addr,addr2)){
			if (func == 0)
				return 0;
			else
				return func->readI();
		}
		uint8_t num;
		if (getVar(num,addr)){
			return iVars[num];
		}else{
			if (addr.addr == TIME_VARCONST)
				return Controller::lastProcessedMSTime;
			return 0;
		}
	}
	int32_t readL(ADDR1 addr,uint8_t addr2){
		Func* func = 0;
		if (getFunc(func,addr,addr2)){
			if (func == 0)
				return 0;
			else
				return func->readL();
		}
		uint8_t num;
		if (getVar(num,addr)){
			return lVars[num];
		}else{
			if (addr.addr == TIME_VARCONST)
				return Controller::lastProcessedMSTime;
			return 0;
		}
	}
	float readF(ADDR1 addr,uint8_t addr2){
		Func* func = 0;
		if (getFunc(func,addr,addr2)){
			if (func == 0)
				return 0;
			else
				return func->readF();
		}
		uint8_t num;
		if (getVar(num,addr)){
			return fVars[num];
		}else{
			if (addr.addr == TIME_VARCONST)
				return Controller::lastProcessedMSTime;
			return 0;
		}
	}
	double readD(ADDR1 addr,uint8_t addr2){
		Func* func = 0;
		if (getFunc(func,addr,addr2)){
			if (func == 0)
				return 0;
			else
				return func->readD();
		}
		uint8_t num;
		if (getVar(num,addr)){
			return dVars[num];
		}else{
			if (addr.addr == TIME_VARCONST)
				return Controller::lastProcessedMSTime;
			return 0;
		}
	}
	uint32_t readT(ADDR1 addr,uint8_t addr2){
		Func* func = 0;
		if (getFunc(func,addr,addr2)){
			if (func == 0)
				return 0;
			else
				return func->readT();
		}
		uint8_t num;
		if (getVar(num,addr)){
			return tVars[num];
		}else{
			if (addr.addr == TIME_VARCONST)
				return Controller::lastProcessedMSTime;
			return 0;
		}
	}

	void write(ADDR1 addr,uint8_t val){
		uint8_t num;
		if (getVar(num,addr)){
			bVars[num] = val;
			return;
		}
	}

	void write(ADDR1 addr,uint16_t val){
		uint8_t num;
		if (getVar(num,addr)){
			uVars[num] = val;
			return;
		}
	}

	void write(ADDR1 addr,int16_t val){
		uint8_t num;
		if (getVar(num,addr)){
			iVars[num] = val;
			return;
		}
	}

	void write(ADDR1 addr,int32_t val){
		uint8_t num;
		if (getVar(num,addr)){
			lVars[num] = val;
			return;
		}
	}

	void write(ADDR1 addr,float val){
		uint8_t num;
		if (getVar(num,addr)){
			fVars[num] = val;
			return;
		}
	}

	void write(ADDR1 addr,double val){
		uint8_t num;
		if (getVar(num,addr)){
			dVars[num] = val;
			return;
		}
	}

	void write(ADDR1 addr,uint32_t val){
		uint8_t num;
		if (getVar(num,addr)){
			tVars[num] = val;
			return;
		}
	}




	void begin(void){

	}

	bool transmit(Logger* logger,uint32_t _time,uint32_t instID, uint8_t width, uint8_t length,ADDR1** addr1Array,uint8_t addr2Offset){

		return Controlled::transmit(logger,_time,instID,width,length,addr1Array,addr2Offset);
	}
	void execute(uint32_t _time,uint32_t id,char command[]){

		if (command[0] == 'S' && command[1] == 'E' && command[2] == 'T'){
			set(command);
		} else if (command[0] == 'F' && command[1] == 'U' && command[2] == 'N'){
			fun(command);
		} else if (command[0] == 'E' && command[1] == 'X' && command[2] == 'E'){
			exe(command);
		} else if (command[0] == 'I' && command[1] == 'N' && command[2] == 'T'){
			interp(command);
		}
	}

	void startSchedule(char command[], uint32_t id){

	}

	void endSchedule(char command[], uint32_t id){

	}

private:
 double process();

	bool getFunc(Func* (&match),ADDR1 addr1, uint8_t addr2){
		char temp[3];
		addr1.getChars(temp);
		if (temp[0] == 'F' && temp[1] == 'N'){
			uint8_t modID = temp[2] - 'A';
			if (functions[modID] == 0){
				match = 0;
				return true;
			} else {
				Func** mid = functions[modID];

				match = mid[addr2];
				return true;
			}
		}
		return false;
	}

	bool getVar(uint8_t &num,ADDR1 addr1){
		char temp[3];
		addr1.getChars(temp);
		if (temp[0] == 'V' && temp[1] == 'R'){
			num = temp[2]-'A';
			return true;
		}
		return false;
	}

	void fun(char command[]){

		uint16_t offset = 4;
		uint8_t modID = command[offset]-'A';

		if (modID > 25)
			return;
		offset++;
		uint8_t addr2;
		Controller::parse_uint8(addr2,offset,command);

		offset++;
		if (addr2 >= FUNC_COUNT)
			return;

		Func* func = createFunc(offset,command,controller);
		if (command[offset]== ','){

			CommandListFunc* clf = new CommandListFunc(func);
			func = clf;

			while (true){
				offset++;
				Func* func2 = createFunc(offset,command,controller);
				if (command[offset]!= ','){
					clf->next = func2;
					break;
				}else{
					CommandListFunc* clf2 = new CommandListFunc(func2);
					clf->next = clf2;
					clf = clf2;
				}
			}
		}

		if (command[offset]!='\0'){
			controller->getErrorLogger()->setParseError(command,offset,"Failed to completely parse function");
			controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
			if (func!=0)
				delete func;
			return;
		}

		if (functions[modID] == 0){
			functions[modID] = new Func*[FUNC_COUNT];
			for (uint8_t i = 0; i < FUNC_COUNT; i++)
				functions[modID][i] = 0;
		}else{
			if (functions[modID][addr2] != 0)
				delete functions[modID][addr2];
		}
		functions[modID][addr2] = func;
	}

	void set(char command[]){
		uint16_t offset = 4;
		if (command[offset]=='$')
			offset++;
		ADDR1 dest(offset,command,controller->getErrorLogger());
		if (dest.type == BAD_TYPE){
			controller->getErrorLogger()->setParseError(command,offset,"Failed to parse address to set");
			controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
			return;
		}
		Controlled* targetMod = controller->getControlled(dest.modID);
		if (targetMod == 0){
			controller->getErrorLogger()->setParseError(command,offset,"Failed to find modID to set");
			controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
			return;
		}
		offset++;

		Func* func = createFunc(offset,command,controller);

		if (func == 0){

			controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
			return;
		}

		if (command[offset]!='\0'){
			controller->getErrorLogger()->setParseError(command,offset,"Unexpect content left over");
			controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
			if (func!=0)
				delete func;
			return;
		}

		switch(dest.type){
			case A_BYTE: targetMod->write(dest,func->readB());break;
			case A_UINT: targetMod->write(dest,func->readU());break;
			case A_INT: targetMod->write(dest,func->readI());break;
			case A_TIME: targetMod->write(dest,func->readT());break;
			case A_LONG: targetMod->write(dest,func->readL());break;
			case A_FLOAT: targetMod->write(dest,func->readF());break;
			case A_DOUBLE: targetMod->write(dest,func->readD());break;
			case A_STRING:break;
			case BAD_TYPE:
			controller->getErrorLogger()->setParseError(command,offset,"Bad type in set");
			controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
			break;
		}

		delete func;
	}

	void exe(char command[]){
		uint16_t offset = 4;
		while (command[offset]!='\0'){
			char modID = command[offset];

			modID-='A';
			if (modID > 25 ){
				controller->getErrorLogger()->setParseError(command,offset,"Can only exec a function A-Z");
				controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
				return;
			}

			offset++;
			uint8_t num ;
			if (!Controller::parse_uint8(num,offset,command)){
				controller->getErrorLogger()->setParseError(command,offset,"Failed to parse function number");
				controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
				return;
			}

			if (num>=FUNC_COUNT){
				controller->getErrorLogger()->setParseError(command,offset,"Function number too high");
				controller->getErrorLogger()->print("Can only exec a function num less than ");
				controller->getErrorLogger()->println(FUNC_COUNT);
				controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
			return;
			}
			Func** temp = functions[(uint8_t)modID];

			if (temp == 0){
				controller->getErrorLogger()->print("No functions exist for mod ");
				controller->getErrorLogger()->println((char)('A'+modID));
				controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
				return;
			}

			Func* func = temp[num];

			if (func == 0){
					controller->getErrorLogger()->print("No functions exist for number ");
					controller->getErrorLogger()->print((char)('A'+modID));
					controller->getErrorLogger()->print(num);
					controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
					return;
				}

			ADDRTYPE type =  func->getType();

			switch (type){
				case A_BYTE:func->readB();break;
				case A_UINT:func->readU();break;
				case A_INT:func->readI();break;
				case A_LONG:func->readL();break;
				case A_FLOAT:func->readF();break;
				case A_DOUBLE:func->readD();break;
				case A_TIME:func->readT();break;
				case A_STRING:break;
				case BAD_TYPE:
				controller->getErrorLogger()->setParseError(command,offset,"Bad type to read from function");
				controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
				 return;
			}
			if (command[offset]=='\0')
				return;
			offset++;
		}

	}

	void interp(char command[]){
		#ifdef DEBUG
			Serial.println("Parsing interp function");
		#endif
		uint16_t offset = 4;
		uint8_t interpID = 0;
		if (!Controller::parse_uint8(interpID,offset,command)){
			controller->getErrorLogger()->setParseError(command,offset,"Failed to parse interpolation number");
			controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
			return;
		}
		if (interpID >= MAX_INTERPS){
			controller->getErrorLogger()->setParseError(command,offset,"Interpolation number is to high");
			controller->getErrorLogger()->print("Can only exec an interpolation num less than ");
			controller->getErrorLogger()->println(MAX_INTERPS);
			controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
			return;
		}

		offset++;

		if (ISeg::interpRoots[interpID] != 0){
			#ifdef DEBUG
				Serial.println("Found existing interp function, deleting...");
			#endif
			ISeg::del(ISeg::interpRoots[interpID]);
		}

		ISeg::interpRoots[interpID] = ISeg::parse(offset, command,controller);

	}


	Func** functions[26]={};


	uint8_t bVars[156]={};
	uint16_t uVars[156]={};
	int16_t iVars[156]={};
	int32_t lVars[156]={};
	float fVars[156]={};
	double dVars[156]={};
	uint32_t tVars[156]={};

};




#endif
