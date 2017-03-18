#ifndef DH_CONTROLLEDFUSION_H__
#define DH_CONTROLLEDFUSION_H__
#include "dh_controller.h"
#include <i2c_t3.h>

#define ACCEL_CACHETIME 5
#define GYRO_CACHETIME 5
#define MAG_CACHETIME 10
#define HEADING_CACHETIME 10
#define QUAT_CACHETIME 10
#define ROTVEL_CACHETIME 10
#define TEMP_CACHETIME 20
#define PRESSURE_CACHETIME 20
#define ALT_CACHETIME 20


class ControlledFusion: public Controller::Controlled{
public:

	void setWire(i2c_t3* _wire){
		wire = _wire;
	}


	float readF(ADDR1 addr,uint8_t addr2){
		switch (addr.addr%26+'A'){
			case 'H' : // HED - heading
				updateHeading();
				if (addr2>2)
					return 0.0;
				return headingF[addr2];

			case 'G' : // GYR - rot vel
			 if ((addr.addr/26)%26+'A'=='R'){
				 updateGrav();
				 if (addr2>2)
					 return 0.0;
				 return gravF[addr2];
			 }else{
					updateGyro();
					if (addr2>2)
						return 0.0;
					return gyroF[addr2];
				}
			case 'A' : // ACL - linar acceleration
							updateAccel();
							if (addr2>2)
								return 0.0;
							return accelF[addr2];

			case 'Q' : // QAT - heading
				updateQuat();
				if (addr2>3)
					return 0.0;
				return quatF[addr2];
		}
		return 0.0;
	}

	double readD(ADDR1 addr,uint8_t addr2){
		return readF(addr,addr2);
	}

	uint32_t readT(ADDR1 addr,uint8_t addr2){
		return 0;
	}


	virtual void begin()=0;

	virtual void execute(uint32_t _time,uint32_t id,char command[]){

	}

	void startSchedule(char command[], uint32_t id){

	}

	void endSchedule(char command[], uint32_t id){

	}


protected:
	virtual void updateHeading()=0;
	virtual void updateGyro() = 0;
	virtual void updateMag() = 0;
	virtual void updateAccel() = 0;
	virtual void updateQuat() = 0;
		virtual void updateGrav() = 0;


	i2c_t3 *wire;

	bool started = false;

	uint8_t errorState = 0;
	float accelF[3]={};
	float gravF[3]={};
	float gyroF[3] = {};

	float magF[3] = {};
	float quatF[4] = {};
	float headingF[3] = {};

	float tempF;
	float barF;
	float  altF;

	elapsedMillis altElapsed = ALT_CACHETIME;
	elapsedMillis accelElapsed = ACCEL_CACHETIME;
	elapsedMillis gyroElapsed = GYRO_CACHETIME;
	elapsedMillis headingElapsed = HEADING_CACHETIME;
 elapsedMillis gravElapsed = HEADING_CACHETIME;
	elapsedMillis quatElapsed = QUAT_CACHETIME;
	elapsedMillis magElapsed = MAG_CACHETIME;



};




#endif
