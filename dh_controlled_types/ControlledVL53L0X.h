#ifndef DH_CONTROLLEDVL53L0X_h
#define DH_CONTROLLEDVL53L0X_h
#include "dh_controller.h"
#include <i2c_t3.h>

// The Arduino two-wire interface uses a 7-bit number for the address,
// and sets the last bit correctly based on reads and writes
#define ADDRESS_DEFAULT 0b0101001
#define SCAN_RATE_DEFAULT 100
#define SCAN_ID 6751


class ControlledVL53L0X: public Controller::Controlled{
public:

	ControlledVL53L0X(int devCount, uint8_t _pins[]){

		deviceCount = devCount;
		pins = _pins;

		devices = new VL53L0X[deviceCount]{};
		values = new uint16_t[deviceCount];
		goodRead = 	 new uint32_t[deviceCount];
		retries = new uint8_t[deviceCount];
	}


	void begin(void){
		scanning = false;


		for (int i = 0 ; i < deviceCount ; i++){
			pinMode(pins[i],OUTPUT);
			digitalWrite(pins[i],LOW);
		}
		delay(500);

	}

	void setWire(i2c_t3 *_wire){
		if (started==true)
			return;

wire = _wire;

	}


	void writeCon(ADDR1 addr,uint8_t val){
					if (val>0){
						startScan();
					} else {
						stopScan();
					}
	}

	void writeCon(ADDR1 addr,uint16_t val){
					if (val <=20){
						val = 20;
					}


						scanRate = val;

						startScan();


	}


	uint8_t readConB(ADDR1 addr,uint8_t addr2){
		return scanning;
	}

	uint16_t readConU(ADDR1 addr,uint8_t addr2){
		if (!scanning)
			return 0;

			uint8_t id = addr.addr%26;

			if (id>= deviceCount)
				return 0;

		if (retries[id]>2)
			return 0;

			return values[id];

	}

	void startScan(){

		if (scanning){
			stopScan();
			delay(10);
			controller->kill(SCAN_ID);
		}
		for (int i = 0 ; i < deviceCount ; i++){
			initDevice(i);
		}
		started = true;
		scanning = true;
		controller->schedule(SCAN_ID,scanRate,scanRate,false,0,Controller::newString("SCAN"),id,false);
	}

	void stopScan(){
		if (scanning){
			for (int i = 0 ; i < deviceCount ; i++){
				digitalWrite(pins[i],LOW);
			}

			controller->kill(SCAN_ID);
			scanning = false;
			started = false;
		}
	}

	void execute(uint32_t time,uint32_t id,char* command){
    if (command[0] == 'E'){
			startScan();
			return;
		}

		if (!scanning)
			return;

	 for (uint8_t i = 0; i < deviceCount;i++){
		uint16_t result = devices[i].readRangeContinuousMillimeters(true);
		if (result < 1000 && result >10){
			values[i] =( result+values[i]*2 )/3;
			// Serial.print(values[i] );
			// 			Serial.print(" , ");
			goodRead[i] = time;
			retries[i] = 0;
		} else{
			controller->getErrorLogger()->print("Error - VL53L0X device #");
			controller->getErrorLogger()->print(i);
			if ( time > 2*scanRate + goodRead[i]){
				if (retries[i] > 64){
					controller->getErrorLogger()->print("VL53L0X device #");
					controller->getErrorLogger()->print(i);
					controller->getErrorLogger()->println("No longer trying to restart");
					controller->getErrorLogger()->finished(time,ErrorLogger::OS_MISC);
					return;
				}

				controller->getErrorLogger()->println("Restarting");

				initDevice(i);
				goodRead[i] = time+5000l;
				retries[i]++;
			}
		controller->getErrorLogger()->finished(time,ErrorLogger::OS_MISC);
		}
	 }
	//  Serial.println();
	}


	void startSchedule(char command[], uint32_t id){

	}
	void endSchedule(char command[], uint32_t id){

	}




private:
void initDevice(uint8_t i){
	digitalWrite(pins[i],LOW);
	delay(100);
	digitalWrite(pins[i],HIGH);
	delay(100);
	devices[i]= VL53L0X();

	devices[i].setWire(wire);

	devices[i].setAddress(0b0101010+i+1);

	devices[i].setMeasurementTimingBudget(scanRate<1000?scanRate*1000l:100000l);

	devices[i].init();

	devices[i].startContinuous(scanRate);

	goodRead[i] = millis()+scanRate*4;
}

 bool scanning = false;

	VL53L0X* devices;

	i2c_t3 *wire = &Wire;

	uint8_t deviceCount;

	uint8_t* pins;

	uint16_t* values;
	uint32_t* goodRead;
	uint8_t* retries;

	uint16_t scanRate = SCAN_RATE_DEFAULT;

	bool started = false;

};

#endif
