#ifndef DH_CONTROLLER_H__
#define DH_CONTROLLER_H__

#include <stdint.h>
#include "Stream.h"
#include <Arduino.h>
#include <dh_logger.h>


#define MAX_IMMED 100
#define MAX_PROG 100
#define MAX_SCHED 512
#define INP_BUFF_SIZE 1024
#define LAST_LINE_SIZE 255

#define COMMAND 0
#define READ 1
#define WRITE 2

//#define DEBUG 1

class Controller{
public:
		class Controlled{

		public:

			virtual void begin(void)=0;
			virtual void execute(uint32_t time,uint32_t id,char command[])=0;
			virtual void startSchedule(char command[],uint32_t id)=0;
			virtual void endSchedule(char command[], uint32_t id)=0;
			virtual uint8_t readB(ADDR1 addr,uint8_t addr2){
				return 0;
			}
			virtual uint16_t readU(ADDR1 addr,uint8_t addr2){
				return 0;
			}
			virtual int16_t readI(ADDR1 addr,uint8_t addr2){
				return 0;
			}
			virtual int32_t readL(ADDR1 addr,uint8_t addr2){
				return 0;
			}
			virtual float readF(ADDR1 addr,uint8_t addr2){
				return 0;
			}
			virtual double readD(ADDR1 addr,uint8_t addr2){
				return 0;
			}
			virtual uint32_t readT(ADDR1 addr,uint8_t addr2){
				return 0;
			}
			virtual char* readS(ADDR1 addr,uint8_t addr2){
				return 0;
			}


			void write(char command[]){
				ADDRTYPE type;

				if (!ADDR1::parseType(type,command[0]))
					return;


				char temp[3];

				temp[0] = command[2];
				temp[1] = command[3];
				temp[2] = command[4];

				ADDR1 addr = ADDR1(temp,type);

				uint16_t offset = 6;

// TODO Read a value from VARIABLE IN including the ADDR1 and ADDR2 parts
				// bool isWriteFromVar = false;
				// if (command[offset] == '$'){
				// 	isWriteFromVar = true;
				//
				// }

				switch(type){
					case A_BYTE:{
						uint8_t val;
						parse_uint8(val,offset,command);
						write(addr,val);
						return;
					}
					case A_UINT:{
						uint16_t val;
						parse_uint16(val,offset,command);
						write(addr,val);
						return;
					}
					case A_INT:{
						int16_t val;
						parse_int16(val,offset,command);
						write(addr,val);
						return;
					}
					case A_LONG:{
						int32_t val;
						parse_int32(val,offset,command);
						write(addr,val);
						return;
					}
					case A_FLOAT:{
						double val;
						parse_double(val,offset,command);
						write(addr,(float)val);
						return;
					}
					case A_DOUBLE:{
						double val;
						parse_double(val,offset,command);
						write(addr,val);
						return;
					}
					case A_TIME:{
						uint32_t val;
						parse_uint32(val,offset,command);
						write(addr,val);
						return;
					}
					case A_STRING:{
						char temp[64];
						for (int i = 0; i < 64 ; i++){
							temp[i] = command[offset+i];
							if (temp[i] == '\0')
								break;
						}
						write(addr,temp);
						return;
					}
				}

			}

			virtual void write(ADDR1 addr,uint8_t val){

			}
			virtual void write(ADDR1 addr,uint16_t val){

			}
			virtual void write(ADDR1 addr,int16_t val){

			}
			virtual void write(ADDR1 addr,int32_t val){

			}
			virtual void write(ADDR1 addr,float val){

			}
			virtual void write(ADDR1 addr,double val){

			}
			virtual void write(ADDR1 addr,uint32_t val){

			}
			virtual void write(ADDR1 addr,const char* val){

			}
			virtual bool transmit(Logger* logger,uint32_t time,uint32_t id,char command[]){
				#ifdef DEBUG
					Serial.print("Outer Transmit :\n ID = ");
					Serial.print(id);
					Serial.print("\n command = ");
					Serial.println(command);
				#endif
				uint16_t c = 0;
					ADDRTYPE type;
				if (!ADDR1::parseType(type,command[c]))
					return false;
				c+=2;
				uint8_t width;
				for (width = 0;width<255;width++){
					if (width>253 || command[width+c] == '\0')
						return false;
					if (command[width+c] == ' ')
						break;
					if (command[width+c] == ','){
						if (width%4 !=3)
							return false;
						continue;
					}
					if (command[width+c] >='A' && command[width+c] <='Z')
						continue;
					if (command[width+c] >='a' && command[width+c] <='z')
						continue;
					return false;
				}

				width = (width + 1)/4;

				if (width == 0)
					return false;

				ADDR1* addr1Array[width];
				char temp[3];
				for (uint8_t i = 0; i < width ; i++){
					temp[0] = command[2+i*4];
					temp[1] = command[3+i*4];
					temp[2] = command[4+i*4];
					#ifdef DEBUG
						Serial.print("Creating addr");
						Serial.print(temp);
						Serial.print("\n n = ");
						Serial.print(i);
						Serial.print(" type = ");
						Serial.println((int)type);
					#endif
					addr1Array[i] = new ADDR1(temp,type);
				}



				c+=width*4;

				uint8_t length,offset;

				if (!parse_uint8(length,c,command)){
					for (uint8_t i = 0; i < width; i++){
						delete addr1Array[i];
					}
					return false;
				}

				c++;
				if (!parse_uint8(offset,c,command)){
					for (uint8_t i = 0; i < width; i++){
						delete addr1Array[i];
					}
					return false;
				}

				bool finally = transmit(logger,time,id,width,length,addr1Array,offset);

					for (uint8_t i = 0; i < width; i++){
						delete addr1Array[i];
					}
				return finally;
			}
			virtual bool transmit(Logger* logger,uint32_t time,uint32_t instID, uint8_t width, uint8_t length,ADDR1** addr1Array,uint8_t addr2Offset){
				#ifdef DEBUG
					Serial.print("Starting Transmit :\n ID = ");
					Serial.print(instID);
					Serial.print("\n width = ");
					Serial.print(width);
					Serial.print(" length = ");
					Serial.print(length);
					Serial.print(" offset = ");
					Serial.println(addr2Offset);
				#endif
				if (length == 0)
					return false;
				ADDRTYPE type = addr1Array[0]->type;
				if (type == A_STRING){
						length = 1;
						width = 1;
				}
				for (uint8_t i=0 ; i< width ; i ++){
					#ifdef DEBUG
						Serial.print(".");
						Serial.flush();
					#endif
					if (type!=addr1Array[i]->type)
						return false;
				}
				#ifdef DEBUG
					Serial.print(".");
					Serial.flush();
				#endif
				uint32_t size = width*length;
				switch (type){
					case A_BYTE: //do nothing *1
					break;
					case A_UINT:
					case A_INT:
					 size *=2;
					break;
					case A_LONG:
					case A_FLOAT:
					case A_TIME:
					 size *=4;
					break;
					case A_DOUBLE:
					 size*=8;
					break;
					case A_STRING:
					  char* temp = readS(*addr1Array[0],addr2Offset);
							for(size=0; size<1024;size++){
								 if (temp[size] == '\0'){
									 break;
								 }
							}
							logger->startStreamSend(size,id,instID);
							logger->print(temp,size);
						 uint16_t remainder = logger->streamSend();
						 return remainder == 0 ;
					break;
				}
				//
				// size+=width*2; // bytes to write the names of each ADDR1
				// size+=4; // bytes to hold Type,width,length,offset
				if (size > 65532)  // might need to save space for the length encoding bytes
					return false;
					#ifdef DEBUG
						Serial.print("Starting Stream send. Size=");
						Serial.println(size);
					#endif
				logger->startStreamSend(size,id,instID);
				// logger->print((uint8_t) type);
				// logger->print(width);
				// logger->print(length);
				// for(uint8_t i = 0 ; i < width ; i++){
				// 	logger->print(addr1Array[i]->addr);
				// }
				// logger->print(addr2Offset);

				for (uint8_t x = 0; x < width; x++){
					for (uint8_t y = 0; y < length ; y++){
						switch (type){
							case A_BYTE: logger->print(readB(*addr1Array[x],y+addr2Offset));
							break;
							case A_UINT: logger->print(readU(*addr1Array[x],y+addr2Offset));
							break;
							case A_INT: logger->print(readI(*addr1Array[x],y+addr2Offset));
							break;
							case A_FLOAT: logger->print(readF(*addr1Array[x],y+addr2Offset));
							break;
							case A_LONG: logger->print(readL(*addr1Array[x],y+addr2Offset));
							break;
							case A_DOUBLE: logger->print(readD(*addr1Array[x],y+addr2Offset));
							break;
							case A_TIME: logger->print(readT(*addr1Array[x],y+addr2Offset));
							break;
							case A_STRING: break;
						}
					}
				}




				uint16_t remainder = logger->streamSend();
				if (remainder>0){
					Serial.print('X');
					Serial.print(remainder);
					Serial.print('X');
				}
				return remainder==0;
			}


			char* error;
			Controller* controller;
			char id;

		private:

		};

	void loadControlled(char id,Controlled* controlled);

	Controlled* getControlled(char id);

	void schedule(uint32_t id, uint16_t initialExecDelay, uint16_t executeInterval,bool additiveInterval, uint32_t runCount,char command[],char controlled,uint8_t style = COMMAND);

	void run(uint32_t id,char command[],uint8_t controlled,uint8_t style = COMMAND);

	void loadProgram(uint8_t id, char command[]);

	void runProgram(uint8_t id);

	void deleteProgram(uint8_t id);

	void setOutputStream(Stream* stream);

		void setErrorStream(Stream* stream);

	void execute();

	bool kill(uint32_t id);

	void kill(void);

	void reboot(void);

	void processInput(Stream* serial);

	void parserError(uint16_t offset,const char* errorTitle);

	char* getInputBuffer();

	inline ErrorLogger* getErrorLogger(){
		return &error;
	}

	inline Logger* getLogger(){
		return &logger;
	}

	static bool parse_uint8(uint8_t &val, uint16_t &pointer,char* text);

	static bool parse_uint16(uint16_t &val,uint16_t &pointer,char* text);

	static bool parse_int16(int16_t &val,uint16_t &pointer,char* text);

	static bool parse_uint32(uint32_t &val,uint16_t &pointer,char* text);

	static bool parse_int32(int32_t &val,uint16_t &pointer,char* text);

	static bool parse_double(double &val, uint16_t &pointer, char* text);

	static char* newString(const char original[]);

	static char lastProcessedLine[LAST_LINE_SIZE];

	static uint32_t lastProcessedMSTime;

	bool transmitTimeOnTick = false;

private:

	struct Entry {
		uint32_t id;
		char* command;
		uint8_t style;
		Controlled *controlled;
		uint16_t executeInterval;
		uint32_t runCount;
		uint32_t nextExecuteTime;
		uint32_t additiveInterval;
		bool killed;
	};

	bool addTimedEntry(Entry* entry);

	Controller::Controlled* library[26];

	Logger logger;

	Entry* currentlyRunning;
	Entry* timed[MAX_SCHED];
	Entry* immediate[MAX_IMMED];

	char* programs[MAX_PROG]={};

	uint8_t immediateSize = 0;
	uint8_t timedSize = 0;

	char inputBuffer[INP_BUFF_SIZE];
	uint16_t bufferCount=0;
	ErrorLogger error;

	elapsedMillis millis;

	void parseBuffer();



};



#endif
