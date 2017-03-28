#include "dh_controller.h"
#include <stdint.h>
#include "Stream.h"
#include <Arduino.h>
//#define DEBUG ON

#define RESTART_ADDR       0xE000ED0C
#define READ_RESTART()     (*(volatile uint32_t *)RESTART_ADDR)
#define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))

	uint32_t Controller::lastProcessedMSTime = 0;
 char Controller::lastProcessedLine[LAST_LINE_SIZE];

void Controller::loadControlled(char id,Controlled* controlled){
	#ifdef DEBUG
	Serial.print("Loading Controlled Module:");
	Serial.println(id);
	#endif
	if (id>'Z' || id < 'A')
		return;
	library[id-'A'] = controlled;
	if (controlled){
		controlled->controller = this;
		controlled->id = id;
		controlled->begin();
	}
	#ifdef DEBUG
	Serial.print("Success loading Module:");
	Serial.println(id);
	#endif
}

Controller::Controlled* Controller::getControlled(char id){
	if (id<26)
		return library[(uint8_t)id];

	uint8_t val = id - 'A';
	if (val>25){
		error.print("Bad getControlled Lookup:");
		error.println(id);
		error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
		return 0;
	}
	return library[val];
}

void Controller::schedule(uint32_t id, uint16_t initialExecDelay,  uint16_t executeInterval,
bool additiveInterval, uint32_t runCount, char command[],char controlled,uint8_t style){


	if (timedSize == MAX_SCHED)
		return;

		uint8_t controlledIndex;

	if (controlled>='a' && controlled<='z')
		controlledIndex=controlled-'a';
	else if (controlled>='A' && controlled<='Z')
		controlledIndex= controlled-'A';
	else return;

	if (!library[controlledIndex]){
		return;
	}
	#ifdef DEBUG
	Serial.print(millis);
	Serial.print(":Scheduling id:");
	Serial.print(id);
	Serial.print(" initExecDelay:");
	Serial.print(initialExecDelay);
	Serial.print(" execInterval:");
	Serial.print(executeInterval);
	Serial.print(" runCount:");
	Serial.print(runCount);
	Serial.print(" command:");
	Serial.print(command);
	Serial.print(" controlled:");
	Serial.print(controlled);
	Serial.print(" style:");
	Serial.println(style);
	Serial.flush();
	#endif

	Entry* entry = new Entry();
	entry->id = id;
	entry->command = command;
	entry->style = style;

	entry->controlled = library[controlledIndex];
	entry->runCount = runCount;
	entry->executeInterval = executeInterval;
  //logger.sendLineSync(entry->controlled->id,entry->id);
	entry->controlled->startSchedule(command,id);

	entry->nextExecuteTime = (uint32_t)millis;
	entry->nextExecuteTime = entry->nextExecuteTime+initialExecDelay;

	entry->killed = false;

	addTimedEntry(entry);
	#ifdef DEBUG
	Serial.print("Successfully loaded timed entry:");
	Serial.println(id);
	#endif

}

void Controller::loadProgram(uint8_t id, char program[]){
	if (programs[id]){
		delete programs[id];
	}
	programs[id]=program;
}

void Controller::deleteProgram(uint8_t id){
	#ifdef DEBUG
	Serial.print("Deleting Program:");
	Serial.println(id);
	#endif
	if (programs[id]){
		delete programs[id];
	}
	programs[id] = 0;
	#ifdef DEBUG
	Serial.print("Successfully Deleted Program:");
	Serial.println(id);
	#endif
}

void Controller::runProgram(uint8_t id){
	#ifdef DEBUG
	Serial.print("Running Program:");
	Serial.println(id);
	#endif
	bufferCount = 0;
	if (programs[id]){
		uint16_t offset = 0;
		char* program = programs[id];
		while(true){
			char c = program[offset];
			offset++;
			if (c ==';' || c =='\0'){
				inputBuffer[bufferCount] = '\0';
				parseBuffer();
				bufferCount = 0;
				if (c == '\0'){
					return;
				}
			} else {
				inputBuffer[bufferCount] = c;
				bufferCount++;
			}
		}
	}
	#ifdef DEBUG
	Serial.print("Finished Running Program:");
	Serial.println(id);
	#endif
}

void Controller::run(uint32_t id, char command[],uint8_t controlled,uint8_t style){
	#ifdef DEBUG
		Serial.println("Running  command");
	#endif
	if (immediateSize == MAX_IMMED){
		error.println("Too many immediate tasks, exceeded MAX_IMMED");
		error.finished(lastProcessedMSTime,ErrorLogger::OS_MISC);
		return;
	}


	if (controlled>='a' && controlled<='z')
		controlled-='a';
	else if (controlled>='A' && controlled<='Z')
		controlled-='A';
	else{
		error.println("Illegal controllable id, must be A-Z value");
		error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
		return;
	}

	Entry* entry = new Entry();
	entry->id = id;
	entry->command = command;
	entry->controlled = library[controlled];
	if (!entry->controlled){
		error.print("Error, unknown controllable referenced as ");
		error.println((char)controlled);
		error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
		delete entry;
		return;
	}

	entry->style = style;

	immediate[immediateSize] = entry;
	immediateSize++;
	#ifdef DEBUG
		Serial.print("Succesfully loaded command, immediate list at size:");
		Serial.println(immediateSize);
	#endif
}

void Controller::setOutputStream(Stream* output){
	// First check immediate run queue
	logger.setStream(output);
}

void Controller::setErrorStream(Stream* output){
	// First check immediate run queue
	error.stream=(output);
}

void Controller::execute(){

	for (int i = 0; i < immediateSize; i ++){
		#ifdef DEBUG
			Serial.print("Running immediate command :");
			Serial.println(i);
		#endif

		switch(immediate[i]->style){
			case COMMAND:
				immediate[i]->controlled->execute((uint32_t)millis,immediate[i]->id,immediate[i]->command);
				break;
			case READ:
				//logger.sendLineSync(immediate[i]->controlled->id,immediate[i]->id);
				immediate[i]->controlled->transmit(&logger,(uint32_t)millis,immediate[i]->id,immediate[i]->command);
				break;
			case WRITE:
				immediate[i]->controlled->write(immediate[i]->command);
				break;
		}

		#ifdef DEBUG
			Serial.print("deleting immediate command :");
			Serial.println(i);
		#endif

		delete immediate[i]->command;

		delete immediate[i];
	}



		immediateSize = 0;


	uint32_t offset = (uint32_t)millis - lastProcessedMSTime;

	if (offset == 0)
		return;

	// if (offset > 1){
	// 	Serial.print('#');
	// 	Serial.print(offset);
	// 	Serial.print('#');
	// }

	lastProcessedMSTime +=offset;

	if (transmitTimeOnTick){
	  		logger.sendTimeSync(lastProcessedMSTime);
	}


uint16_t targetCount = 0; // THis is used to track killed count  and copy back when something is killed
	for (uint16_t i =0 ; i < timedSize; i ++){

		Entry* iter =timed[i];
		currentlyRunning = iter;


		if (iter->killed){

	// Serial.print(iter->command);
			iter->controlled->endSchedule(iter->command, iter->id);

			delete iter->command;

			delete iter;

			continue;

		}

		if ((uint32_t)millis >= iter->nextExecuteTime){

			if (iter->executeInterval == 0 ){
				iter->runCount = 1;
			}
			switch (iter->style){
				case COMMAND:
					iter->controlled->execute((uint32_t)millis,iter->id,iter->command);
				break;
				case READ:
					iter->controlled->transmit(&logger,(uint32_t)millis,iter->id,iter->command);
				break;
				case WRITE:
					iter->controlled->write(iter->command);
				break;
			}

			if (iter->runCount>0){
				iter->runCount--;
				if (iter->runCount == 0){

					iter->controlled->endSchedule(iter->command, iter->id);

					delete iter->command;

					delete iter;

					continue;
				}
			}
			if (iter->additiveInterval)
				iter->nextExecuteTime =millis + iter->executeInterval;
			else
				iter->nextExecuteTime += iter->executeInterval;
		}

		timed[targetCount]=iter;
		targetCount++;
	}
 timedSize = targetCount;
	currentlyRunning = 0;

}

void Controller::processInput(Stream* stream){
	while (stream->available()){
		char next = stream->read();
		if (next == '\n'||next == '\r'){
			parseBuffer();
				memcpy (lastProcessedLine,inputBuffer,(bufferCount<254?bufferCount:254));
				if (bufferCount>=254)
						lastProcessedLine[254] = '\0';

			bufferCount = 0;
			inputBuffer[0] = '#';
			inputBuffer[1] = '\0';
		}else{
			inputBuffer[bufferCount] = next;
			inputBuffer[bufferCount+1] = '\0';
			if (bufferCount == INP_BUFF_SIZE-1){
				bufferCount = 0;
			}else{
				bufferCount++;
			}
		}
	}

}

void Controller::parseBuffer(){

	if (inputBuffer[0]=='K'){
			if (inputBuffer[1]=='R'){
				reboot();
				return;
			}
		if (bufferCount<3){
			error.setParseError(inputBuffer,1,"Missing data after (K)ill command");
			error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
			return;
		}
		uint16_t offset = 2;
		uint32_t id=0;
		if (!parse_uint32(id,offset,inputBuffer)){
			error.setParseError(inputBuffer,offset,"Missing instruction id");
			error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
			return;
		}
 	#ifdef DEBUG
	Serial.print(">K>");
	Serial.print(id);
	Serial.print('@');
	Serial.println(millis);
	#endif

		kill(id);
		return;
	}

	if (inputBuffer[0]=='P'){
		if (bufferCount<3){
			error.setParseError(inputBuffer,1,"Missing data after (P)rogram command");
			error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
			return;
		}
		uint8_t id=0;
		uint16_t offset = 2;

		if (!parse_uint8(id,offset,inputBuffer)){
			error.setParseError(inputBuffer,offset,"Missing program id");
			error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
			return;
		}

		offset++;

		if (bufferCount<=offset){
			#ifdef DEBUG
			Serial.print("Deleting program: ");
			Serial.println(id);
			#endif
			deleteProgram(id);
			return;
		}

		char* program = new char[bufferCount-offset];

		for (uint16_t i = 0; i < bufferCount-offset; i++){
			program[i] = inputBuffer[i+offset];
		}

		#ifdef DEBUG
			Serial.print("Loading program: ");
			Serial.println(id);
		#endif
		loadProgram(id,program);
		#ifdef DEBUG
			Serial.println("Finished Loading Program.");
		#endif
		return;
	}

	if (inputBuffer[0]=='R'){
		if (bufferCount<3){
			error.setParseError(inputBuffer,1,"Missing data after (R)un program command");
			error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
			return;
		}
		uint8_t id=0;
		uint16_t offset = 2;

		if (!parse_uint8(id,offset,inputBuffer)){
			error.setParseError(inputBuffer,offset,"Missing program run id");
			error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
			return;
		}

		#ifdef DEBUG
			Serial.print("Running program: ");
			Serial.println(id);
		#endif
		runProgram(id);

		return;
	}
	if (inputBuffer[0] == 'S' ) {

		char modID;



		uint32_t id = 0;
		uint16_t timeDelay;
		uint16_t timeInterval;
		uint16_t offset = 3;
		uint8_t style;
		if (inputBuffer[1] == 'C'){
			style = COMMAND;
		} else if (inputBuffer[1] == 'R'){
				style = READ;
			} else if (inputBuffer[1] == 'W'){
				style = WRITE;
			} else {
				error.setParseError(inputBuffer,1,"(S)chedule command must be 'SC','SR ','SW '");
				error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
				return;
			}

		uint32_t runCount;
		if (!parse_uint32(id, offset, inputBuffer)){
			error.setParseError(inputBuffer,offset,"Bad instruction id");
			error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
			return;
		}

		offset++;


		modID = inputBuffer[offset];

		offset += 2;

		if (!parse_uint16(timeDelay, offset, inputBuffer)){
			error.setParseError(inputBuffer,offset,"Bad start delay (ms)");
			error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
			return;
		}

		offset++;

		if (!parse_uint16(timeInterval, offset, inputBuffer)){
			error.setParseError(inputBuffer,offset,"Bad interval time (ms)");
			error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
			return;
		}


		offset++;


		if (!parse_uint32(runCount, offset, inputBuffer)){
			error.setParseError(inputBuffer,offset,"Bad run count");
			error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
			return;
		}

		offset++;

		char* command = new char[bufferCount - offset + 1];

		for (uint16_t i = 0; i <= bufferCount - offset; i++) {
			command[i] = inputBuffer[i + offset];
		}
		if (command[bufferCount - offset] != '\0') {
			error.setParseError(inputBuffer,offset,"Incomplete command");
			error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
			return;
		}
		#ifdef DEBUG
		Serial.print("Scheduling");
		if (style == READ){
			Serial.print(" R:");
		} else if (style == WRITE){
			Serial.print(" W:");
		} else {
			Serial.print(":");
		}
		Serial.print(modID);
		Serial.print(" ");
		Serial.print(id);
		Serial.print(" ");
		Serial.println(command);
		#endif

			schedule(id, timeDelay, timeInterval, false, runCount, command, modID, style);

		#ifdef DEBUG
			Serial.println("Scheduling complete");
		#endif

		return;
	}

	if (inputBuffer[0]!='I'){
		error.print("Command must start with IC,SC,SW,SR,IW,IR,K,R,P");
		error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
		return;
	}

	uint32_t id = 0;
	uint16_t offset = 3;
	uint8_t style = COMMAND;
		if (inputBuffer[1] == 'C'){
			style = COMMAND;
		} else if (inputBuffer[1] == 'R'){
				style = READ;

			} else if (inputBuffer[1] == 'W'){
				style = WRITE;

			} else {
				error.println("Immediate commands can only be IC,IR,IW");
				error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
				return;
			}

	if (!parse_uint32(id, offset, inputBuffer)){
		error.setParseError(inputBuffer,offset,"Bad instruction id");
		error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
		return;
	}

	offset++;

	char commandID;
	commandID = inputBuffer[offset];

	offset += 2;

	char* command =new char[bufferCount-offset+1];

	for (uint16_t i = 0; i <= bufferCount-offset; i++){
		command[i] = inputBuffer[i+offset];
	}
	if (command[bufferCount-offset]!='\0'){
		error.setParseError(inputBuffer,offset,"Incomplete command");
		error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
		return;
	}

	#ifdef DEBUG
	 Serial.print("Command processed:");
	 Serial.print(" id =");
	 Serial.print(id);
	 Serial.print(" command ");
	 Serial.print(command);
	 Serial.print(" Controlled ID:");
	 Serial.println((char)commandID);
	#endif

	run(id,command,commandID,style);

}

bool Controller::addTimedEntry(Entry* entry){

if (timedSize>=MAX_SCHED)
  return false;

		timed[timedSize] = entry;

	timedSize++;
	return true;
}

// ID = 0 kills all running controllables
bool Controller::kill(uint32_t id){
	bool success = false;

	// for (int i = remainingTimedSize  ; i < remainingTimedSize + timedSize ; i++){
	for (int i = 0  ; i <  timedSize ; i++){

		Entry* entry = timed[i];
		if (!entry){
			if (success == false) {
				error.print("Could not kill - Failed to find process id:");
				error.println(id);
				error.finished(lastProcessedMSTime,ErrorLogger::OS_PARSER);
			}
			break;
		}
		if (entry->id == id || id == 0){
			entry->killed = true;
			success = true;
		}
	}

	return success;
}

void Controller::reboot(){
	WRITE_RESTART(0x5FA0004);
	delay(10000);
	return;
}

void Controller::kill(){
	if (currentlyRunning!=0)
		currentlyRunning->killed =true;
}


bool Controller::parse_uint8(uint8_t &val, uint16_t &pointer,char* text){
	val = 0;

	uint8_t valChars;

	for (valChars=0;valChars<=3;valChars++){
		char c = text[pointer+valChars];
		if (c<'0' || c>'9')
			break;
	}

	if (valChars == 0 || valChars == 4)
		return false;

	uint8_t multiplier = 1;

	for (int i = valChars-1;i>=0;i--){
		val+= multiplier*(text[pointer+i]-'0');
		multiplier*=10;
	}

	pointer+=valChars;

	return true;

}

bool Controller::parse_uint16(uint16_t &val, uint16_t &pointer,char* text){
	val = 0;

	uint8_t valChars;

	for (valChars=0;valChars<=5;valChars++){
		char c = text[pointer+valChars];
		if (c<'0' || c>'9')
			break;
	}

	if (valChars == 0 || valChars == 6)
		return false;

	uint16_t multiplier = 1;

	for (int i = valChars-1;i>=0;i--){
		val+= multiplier*(text[pointer+i]-'0');
		multiplier*=10;
	}

	pointer+=valChars;

	return true;

}

bool Controller::parse_int16(int16_t &val, uint16_t &pointer,char* text){
	val = 0;

	bool negative = false;
	if (text[pointer]=='-'){
		negative = true;
		pointer++;
	}

	uint8_t valChars;

	for (valChars=0;valChars<5;valChars++){
		char c = text[pointer+valChars];
		if (c<'0' || c>'9')
			break;
	}

	if (valChars == 0 || valChars == 5)
		return false;

	int16_t multiplier = negative?-1:1;

	for (int i = valChars-1;i>=0;i--){
		val+= multiplier*(text[pointer+i]-'0');
		multiplier*=10;
	}

	pointer+=valChars;

	return true;

}

bool Controller::parse_int32(int32_t &val, uint16_t &pointer,char* text){
	val = 0;

	bool negative = false;
	if (text[pointer]=='-'){
		negative = true;
		pointer++;
	}

	uint8_t valChars;

	for (valChars=0;valChars<10;valChars++){
		char c = text[pointer+valChars];
		if (c<'0' || c>'9')
			break;
	}

	if (valChars == 0 || valChars == 10)
		return false;

	int32_t multiplier = negative?-1:1;

	for (int i = valChars-1;i>=0;i--){
		val+= multiplier*(text[pointer+i]-'0');
		multiplier*=10;
	}

	pointer+=valChars;

	return true;

}

bool Controller::parse_double(double &val, uint16_t &pointer, char* text){
	val = 0;
	bool negative = false;
	if (text[pointer] == '-'){
		negative = true;
		pointer++;
	}

	uint8_t valChars;
	uint8_t decimalLoc=255;

	for (valChars = 0; valChars <= 20; valChars++){
		char c = text[pointer + valChars];
		if (c == '.'){
			if (decimalLoc != 255)
				return false;
			decimalLoc = valChars;
			continue;
		}
		if (c<'0' || c>'9')
			break;
	}

	if (valChars == 0 || valChars == 20 || (valChars == 1 && decimalLoc == 0))
		return false;

	if (decimalLoc == 255)
		decimalLoc = valChars;

	double multiplier = negative ? -1 : 1;

	for (int i = decimalLoc - 1; i >= 0; i--){
		val += multiplier*(text[pointer + i] - '0');
		multiplier *= 10;
	}



	multiplier = negative ? -.1 : .1;

	for (int i = decimalLoc + 1; i < valChars; i++){
		val += multiplier*(text[pointer + i] - '0');
		multiplier *= .1;
	}

	pointer += valChars;

	return true;

}

bool Controller::parse_uint32(uint32_t &val, uint16_t &pointer,char* text){
	val = 0;

	uint8_t valChars;

	for (valChars=0;valChars<=10;valChars++){
		char c = text[pointer+valChars];
		if (c<'0' || c>'9')
			break;
	}

	if (valChars == 0 || valChars == 11)
		return false;

	uint32_t multiplier = 1;

	for (int i = valChars-1;i>=0;i--){
		val+= multiplier*(text[pointer+i]-'0');
		multiplier*=10;
	}

	pointer+=valChars;

	return true;

}

char* Controller::getInputBuffer(){
	return inputBuffer;
}

char* Controller::newString(const char original[]){
	uint16_t size;
	for (size = 0 ; size < INP_BUFF_SIZE-1 ; size++){
		if (original[size]=='\0')
			break;
	}

	char* data = new char[size+1];
	data[size] = '\0';
	for (int i =0 ; i < size ; i++)
		data[i] = original[i];

	return data;
}
//
// void Controller::parserError(char modID, uint16_t pointer, const char* errorTitle){
// 	if (parserAttempt == lastParseErrorAttempt){
// 		return;
// 	}else{
// 		lastParseErrorAttempt = parserAttempt;
// 	}
// 	errorTime = millis;
// 	Serial.print("Parser Failed[");
// 	Serial.print(errorTitle);
// 	Serial.print("] @ ");
// 	Serial.println(inputBuffer[pointer]);
// 	Serial.println(inputBuffer);
// 	for (uint16_t i = 0 ; i < pointer; i++)
// 		Serial.print(" ");
// 	Serial.println('^');
// }
