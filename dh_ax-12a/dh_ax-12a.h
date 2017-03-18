#ifndef DH_AX12A_H__
#define DH_AX12A_H__
#include "Stream.h"
#include "Arduino.h"

//#define DEBUG 1

#define BUFFER_SIZE 128
#define READCYCLEDELAY 10
#define WRITEDELAY 5


class ax_12a {
public:


	ax_12a(Stream* source, uint8_t tristatePin, uint8_t deviceCount){
		stream = source;

		devCount = deviceCount;

		triPin = tristatePin;
		pinMode(triPin,OUTPUT);
		digitalWrite(triPin,LOW);
		stream->flush();
		lastKnownPos = new uint16_t[deviceCount];
	  for (uint8_t i = 0; i < deviceCount; i++)
			lastKnownPos[i] = 9999;

	}

	~ax_12a(){
		if (lastKnownPos!=0)
		   delete lastKnownPos;
	}

	bool setBaud(uint8_t deviceNum,uint8_t baud){
		return write8(deviceNum,4,baud);
	}

	bool setTorque(uint8_t deviceNum,bool enabled){
		return write8(deviceNum,24,enabled);
	}

	// Cant really process the response of the ping, so why bother
	bool ping(uint8_t deviceNum){

		if (!execute(deviceNum,1,0,0))
			return false;

		return true;
	}

	bool move(uint8_t deviceNum, uint16_t pos){
		 if (lastKnownPos[deviceNum-1]==pos){
		 	return true;
		}
	  lastKnownPos[deviceNum-1]=pos;
	   uint8_t parms[] = {30,(uint8_t)(pos%256),(uint8_t)(pos>>8)};
	   return execute(deviceNum,3,3, parms);

	}

	bool setCompliance(uint8_t deviceNum, uint8_t margin, uint8_t slope){

		if (!write8(deviceNum,26,margin)) return false;
		if (!write8(deviceNum,27,margin)) return false;
		if (!write8(deviceNum,28,slope)) return false;
		if (!write8(deviceNum,29,slope)) return false;

		return true;
	}

	bool setLED(uint8_t deviceNum,bool on){
		uint8_t val = on;
		return write8(deviceNum,25,val);
	}
	uint16_t askPos(uint8_t deviceNum){
		return read2Bytes(deviceNum,36);
	}

	uint16_t askGoal(uint8_t deviceNum){
		return read2Bytes(deviceNum,30);
	}

	float askSpeed(uint8_t deviceNum){
		uint16_t t = read2Bytes(deviceNum,38);
		if (t>1023){
			t-=1024;
			return ((float)t)*.111;
		}else{
			return ((float)t)*-.111;
		}
	}

	float askVoltage(uint8_t deviceNum){
		uint8_t t = readByte(deviceNum,38);

		return ((float)t)*.1;

	}

	bool askMoving(uint8_t devNum){
		return readByte(devNum,46);
	}

	uint16_t askAngleLimit(bool isCCW,uint16_t devNum){
		return read2Bytes(devNum,isCCW?8:6);
	}

	uint8_t readByte(uint8_t deviceNum, uint8_t addr){
		uint8_t parms[] = {addr,1};
		#ifdef DEBUG
			Serial.print("Sending read1 command:");
		#endif
				stream->flush();
		execute(deviceNum,2,2,parms);
		for (uint8_t i = 0; i < 40; i ++){

			delayMicroseconds(10);

			uint16_t bytes = stream->available();

			if (bytes<7)
				continue;

			uint8_t data[7];
			stream->readBytes(&data[0],7);

			#ifdef DEBUG
				Serial.print("Processing[");
				for (int i =0 ; i < 7 ; i++){
					Serial.print(data[i],HEX);
					Serial.print(' ');
				}
				Serial.println(']');
			#endif

			if (data[0]!=255 || data[1]!=255){
				Serial.print("Failed ParseBad Header :");
				Serial.print(data[0],HEX);
				Serial.print(data[2],HEX);
				Serial.print(" Syncing:");

				uint8_t offset = 1;

				while (data [offset] != 255 || data[offset+1] != 255){
					Serial.print('.');
					offset++;
					if (offset == 6){
										Serial.print("Failed!");
										return 0;
					}
				}

				for (uint8_t i = 0; i < 7- offset; i ++){
					data[i]= data[i+offset];
				}

					stream->readBytes(&data[offset+2],7-offset-1);
					Serial.print("Success!");
			}

			if (data[2] != deviceNum){
				Serial.println("Wrong device responded");
				return 0;
			}

			if ( data[3] != 3){
				Serial.println("Wrong length response");
				return 0;
			}

			if (data[4] > 0){
				newError = true;
				error = data[4];
				Serial.print("Error response:");
				Serial.println(data[4]);
			}

			if (data[6]!=(uint8_t)(255-((data[2]+data[3]+data[4]+data[5])%256))){
				Serial.println("Bad checksum");
				return 0;
			}

			return data[5];
		}

		return 0;
	}

	 uint16_t read2Bytes(uint8_t deviceNum, uint8_t addr){
	 	uint8_t parms[] = {addr,2};

		#ifdef DEBUG
			Serial.print("Sending read2 command:");
		#endif
		stream->flush();
	 	execute(deviceNum,2,2,parms);
   uint16_t bytes = 0;
		for (uint8_t i = 0; i < 40; i++){

			delayMicroseconds(READCYCLEDELAY);

			 bytes = stream->available();

			if (bytes<8)
				continue;

			uint8_t data[8];
			stream->readBytes(&data[0],8);

			#ifdef DEBUG
				Serial.print("Processing[");
				for (int i =0 ; i < 8 ; i++){
					Serial.print(data[i],HEX);
					Serial.print(' ');
				}
				Serial.println(']');
			#endif

			if (data[0]!=255 || data[1]!=255){
				Serial.print("Failed ParseBad Header :");
				Serial.print(data[0],HEX);
				Serial.print(data[2],HEX);
				Serial.print(" Syncing:");

				uint8_t offset = 1;

				while (data [offset] != 255 || data[offset+1] != 255){
					Serial.print('.');
					offset++;
					if (offset == 7){
										Serial.print("Failed!");
										return 0;
					}
				}

				for (uint8_t i = 0; i < 8- offset; i ++){
					data[i]= data[i+offset];
				}

					stream->readBytes(&data[offset+2],8-offset-1);
					Serial.print("Success!");
			}


			if (data[2] != deviceNum){
				Serial.println("Wrong device responded");
				return 0;
			}

			if ( data[3] != 4){
				Serial.println("Wrong length response");
				return 0;
			}

			if (data[4] > 0){
				newError = true;
				error = data[4];
				Serial.print("Error response:");
				Serial.println(data[4]);
			}

			if (data[7]!=(uint8_t)(255-((data[2]+data[3]+data[4]+data[5]+data[6])%256))){
				Serial.println("Bad checksum");
				return 0;
			}


			return ((uint16_t)data[5]) | ((uint16_t)data[6])<<8;

		}
	   Serial.print("Failed to recieve enough data:");
		 Serial.print(bytes);
		 Serial.println(" Bytes.");
		return 0;
	}


	bool write8(uint8_t deviceNum, uint8_t addr, uint8_t val ){
		uint8_t parms[] = {addr,val};
   		return execute(deviceNum,3,2, parms );
	}

	bool write16(uint8_t deviceNum, uint8_t addr, uint16_t val ){
 		uint8_t parms[] = {addr,(uint8_t)(val%256),(uint8_t)(val>>8)};
   		return execute(deviceNum,3,3, parms );
	}


	// uint16_t listen(void){

	// 	uint16_t bytes = stream->available();

	// 	#ifdef DEBUG
	// 	 Serial.print("<");
	// 	 Serial.print(bytes);
	// 	 Serial.print("..");
	// 	#endif

	// 	if (bytes == 0){
	// 		return 0;
	// 	}

	// 	// Just to limit the size we read per interval
	// 	if (bytes>24)
	// 		bytes = 24;

	// 	if (bytes+currentSize>=BUFFER_SIZE){
	// 		bytes = BUFFER_SIZE-currentSize;
	// 	}


	// 	stream->readBytes(data+currentSize,bytes);
	// 	currentSize+=bytes;

	// 	#ifdef DEBUG

	// 	Serial.print("Processing[");

	// 	for (int i =0 ; i < currentSize ; i++){
	// 		Serial.print(data[i],HEX);
	// 		Serial.print(' ');
	// 	}
	// 	Serial.println(']');
	// 	#endif


	// 	uint16_t index=0;

	// 	while(currentSize-index>=6){
	// 		waitingForRead = false;
	// 		if (data[index++]!=255){
	// 			Serial.println("Failed ParseBad Header 1");
	// 			continue;
	// 		}

	// 		if (data[index++]!=255){
	// 			Serial.println("Failed Parse:Bad Header 2");
	// 			continue;
	// 		}

	// 		uint8_t deviceNum = data[index++];


	// 		uint8_t plength = data[index++];

	// 		#ifdef DEBUG
	// 		 Serial.print("Device = ");
	// 		 Serial.print(deviceNum);
	// 		 Serial.print(" ");
	// 		 Serial.print(" plength = ");
	// 		 Serial.println(plength);
	// 		#endif

	// 		if (plength+index>bytes){
	// 			Serial1.println("Failed Parse:Incomplete message");
	// 			break;
	// 		}

	// 		if (plength==2){
	// 			uint8_t error = data[index++];
	// 			if (error!=0){
	// 				errors[deviceNum] = error;
	// 				Serial.print("!!Servo #");
	// 				Serial.print(deviceNum);
	// 				Serial.print(" reported error:");
	// 				Serial.print(error);
	// 				Serial.println("!!");
	// 				index++;
	// 				newError = true;
	// 				continue;
	// 			}
	// 			Serial.print("Ping back:");
	// 			Serial.println(deviceNum);
	// 			index++;
	// 			continue;
	// 		}

	// 		if (plength == 3){
	// 			uint8_t inst = data[index++];
	// 			if (inst!=0){
	// 				Serial.println("Failed Parse:Unknown 3 length message or error");
	// 			}

	// 			readData[deviceNum]  = data[index++];
	// 			readAddr[deviceNum] = -readAddr[deviceNum];

	// 			#ifdef DEBUG
	// 				Serial.print("Read ");
	// 				Serial.print(deviceNum);
	// 				Serial.print(" addr = ");
	// 				Serial.print(readAddr[deviceNum]);
	// 				Serial.print(" val = ");
	// 				Serial.println(readData[deviceNum]);
	// 			#endif


	// 			index++;
	// 			continue;
	// 		}

	// 		if (plength == 4){
	// 			uint8_t inst = data[index++];

	// 			if (inst!=0 ){
	// 				Serial.println("Failed Parse:Unknown 4 length message or errors");
	// 			}

	// 			readData[deviceNum]  =  data[index++];
	// 			readData[deviceNum]  |= data[index++]<<8;

	// 			readAddr[deviceNum] = -readAddr[deviceNum];

	// 			#ifdef DEBUG
	// 			Serial.print("Read ");
	// 			Serial.print(deviceNum);
	// 			Serial.print(" addr = ");
	// 			Serial.print(readAddr[deviceNum]);
	// 			Serial.print(" val = ");
	// 			Serial.println(readData[deviceNum]);
	// 			#endif

	// 			waitingForRead = false;
	// 			index++;
	// 			continue;
	// 		}

	// 	}

	// 	if (index>0){
	// 		memmove(data,data+index,currentSize-index);
	// 		currentSize-=index;
	// 		return true;
	// 	}

	// 	return false;
	// }

	bool newError = false;
	uint8_t error = 0;



private:

	bool execute(uint8_t deviceNum,uint8_t command,uint16_t parmCount, uint8_t parms[]){
		if(deviceNum == 0 || deviceNum > devCount)
			return false;

		  uint8_t data[parmCount+6];
		  uint16_t checksum=0;

		  data[0]=data[1]=0xff;
		  data[2]=deviceNum;
		  checksum+=data[2];
		  data[3]=parmCount+2;
		  checksum+=data[3];
		  data[4]=command;
		  checksum+=data[4];
		  for (int i = 0; i < parmCount ; i++){
		    data[5+i] = parms[i];
		    checksum+=parms[i];
		  }

		  data[parmCount+5]=(uint8_t)(255-(checksum%256));

		  digitalWrite(triPin,HIGH);
		  stream->write(data,parmCount+6);
		  stream->flush();
	 	  digitalWrite(triPin,LOW);

		 return true;
	}

	uint8_t devCount;
	uint16_t* lastKnownPos;
	Stream* stream;
	uint8_t triPin ;
};

#endif
