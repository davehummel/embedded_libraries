#ifndef DH_CONTROLLEDLIDAR_H__
#define DH_CONTROLLEDLIDAR_H__
#include "dh_controller.h"
#include <i2c_t3.h>

#define LIDAR_ADDR 0x62
#define LIDAR_INST_ID 199

class ControlledLidar: public Controller::Controlled{
public:
	ControlledLidar(uint8_t lidarEnPin){
		lidarPin = lidarEnPin;
	}

	void setWire(i2c_t3* _wire){
		wire = _wire;
	}

	uint8_t readB(ADDR1 addr,uint8_t addr2){
		return powerOn;
	}

	uint16_t readU(ADDR1 addr,uint8_t addr2){
		if (!powerOn || delayedOn)
			return 0;


		 uint8_t distanceArray[2];
		 readBytes(LIDAR_ADDR,0x8f,2,distanceArray);
		 uint16_t distance = (distanceArray[0] << 8) + distanceArray[1];
		//  Serial.print("Dist:");
		//  Serial.println(distance);
		 return distance;
	}

	void write(ADDR1 addr,uint8_t val){
					if (powerOn == (val>0))
						return;

					powerOn = val > 0;

					digitalWrite(lidarPin,powerOn);

					if (powerOn == false)
						return;

				 delayedOn = true;

				 controller->schedule(LIDAR_INST_ID,100,0,false,1,Controller::newString("A"),'L',false);

	}


	void begin(void){
		pinMode(lidarPin, OUTPUT);
		digitalWrite(lidarPin, LOW);
		powerOn = false;

	}


	void execute(uint32_t time,uint32_t id,char command[]){
		if (command[0]=='A' && delayedOn == true){
			delayedOn = false;
			writeByte(LIDAR_ADDR,0x45,0x02); // Set minimum delay between reads
			writeByte(LIDAR_ADDR,0x04,0x20);
			writeByte(LIDAR_ADDR,0x11,0xff); // infinite reads
			writeByte(LIDAR_ADDR,0x00,0x04); // Start Reads
		}
	}

	void startSchedule(char command[], uint32_t id){

	}
	void endSchedule(char command[], uint32_t id){


	}


private:
	i2c_t3* wire;
	uint8_t lidarPin;

	bool powerOn = false;
	bool delayedOn = false;

	void correct(uint16_t &data){
		if (data>50)
			data-=4;
		else if (data>40)
			data-=5;
		else if (data>30)
			data-=6;
		else if (data>20)
			data-=7;
		else
			data-=8;
	}

		void writeByte(uint8_t address, uint8_t subAddress, uint8_t data)
	{
		wire->beginTransmission(address);  // Initialize the Tx buffer
		wire->write(subAddress);           // Put slave register address in Tx buffer
		wire->write(data);                 // Put data in Tx buffer
		wire->endTransmission();           // Send the Tx buffer
	}

	uint8_t readByte(uint8_t address, uint8_t subAddress)
	{
		uint8_t data; // `data` will store the register data
		wire->beginTransmission(address);         // Initialize the Tx buffer
		wire->write(subAddress);	                 // Put slave register address in Tx buffer
		wire->endTransmission(I2C_STOP);        // Send the Tx buffer, but send a restart to keep connection alive
	//	wire->endTransmission(false);             // Send the Tx buffer, but send a restart to keep connection alive
	//	wire->requestFrom(address, 1);  // Read one byte from slave register address
		wire->requestFrom(address, (size_t) 1);   // Read one byte from slave register address
		data = wire->read();                      // Fill Rx buffer with result
		return data;                             // Return data read from slave register
	}

	void readBytes(uint8_t address, uint8_t subAddress, uint8_t count, byte * dest)
	{
		wire->beginTransmission(address);   // Initialize the Tx buffer
		wire->write(subAddress);            // Put slave register address in Tx buffer
		wire->endTransmission();  // Send the Tx buffer, but send a restart to keep connection alive

	        wire->requestFrom(address, (size_t) count);  // Read bytes from slave register address
		if (wire->available()>=count){
			for (uint8_t i=0; i < count ; i++){
	        	dest[i] = wire->read();
	    	}
	    }
	}


};



#endif
