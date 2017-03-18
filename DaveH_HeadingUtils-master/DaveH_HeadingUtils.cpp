
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <Adafruit_LSM9DS0.h>
#include <limits.h>
#include <Math.h>
#include "DaveH_HeadingUtils.h"



/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/
void Heading_Util::processTemp(void){
	temperature = lsm.temperature*tempMultiplier;
}

void Heading_Util::processMag(void){
	magData.x = lsm.magData.x*magMultiplier;
	magData.y = lsm.magData.y*magMultiplier;
	magData.z = lsm.magData.z*magMultiplier;
}

void Heading_Util::processGyro(bool accumulate){
	gyroData.x = (lsm.gyroData.x*gyroMultiplier)/-1000.0;
	gyroData.y = (lsm.gyroData.y*gyroMultiplier)/-1000.0;
	gyroData.z = (lsm.gyroData.z*gyroMultiplier)/-1000.0;
	if (!isCalibrating){
		gyroStats.correct(&gyroData,gyroDeadZoneFactor);
		if (accumulate){
			int delta = gyroData.z*gyroInterval;
			if (delta < 0){
				if (delta*-1 > heading){
					delta +=heading;
					heading = MAX_ANGLE_RANGE;
				}
			}else if (delta > 0){
				unsigned int dif = MAX_ANGLE_RANGE - heading ;
				if (dif < delta){
					delta -= dif;
					heading = 0;
				}
			}
			heading += delta;
		}
	}else{
		gyroStats.add(gyroData);
	}
}

void Heading_Util::processAccel(bool accumulate){
	accelData.x = (lsm.accelData.x*accelMultiplier)/1000.0;
	accelData.y = (lsm.accelData.y*accelMultiplier)/1000.0;
	accelData.z = (lsm.accelData.z*accelMultiplier)/1000.0;
	if (!isCalibrating){
		accelStats.correct(&accelData,accelDeadZoneFactor);
		if (accumulate){
			velocity.x+=accelData.x*accelInterval;
			velocity.y+=accelData.y*accelInterval;
			velocity.z+=accelData.z*accelInterval;
		}
	}else{
		accelData.z-=GRAV_ACCEL_CONST;
		accelStats.add(accelData);
	}
}

/***************************************************************************
 CONSTRUCTOR
 ***************************************************************************/
 

Heading_Util::Heading_Util(Adafruit_LSM9DS0::lsm9ds0AccelRange_t accelRange,Adafruit_LSM9DS0::lsm9ds0MagGain_t magGain, Adafruit_LSM9DS0::lsm9ds0GyroScale_t gyroScale) 
{

  lsm.setupAccel(accelRange);
  
   switch (accelRange)
  {
    case Adafruit_LSM9DS0::LSM9DS0_ACCELRANGE_2G:
      accelMultiplier = LSM9DS0_ACCEL_MG_LSB_2G;
      break;
    case Adafruit_LSM9DS0::LSM9DS0_ACCELRANGE_4G:
      accelMultiplier = LSM9DS0_ACCEL_MG_LSB_4G;
      break;
    case Adafruit_LSM9DS0::LSM9DS0_ACCELRANGE_6G:
      accelMultiplier = LSM9DS0_ACCEL_MG_LSB_6G;
      break;
    case Adafruit_LSM9DS0::LSM9DS0_ACCELRANGE_8G:
      accelMultiplier = LSM9DS0_ACCEL_MG_LSB_8G;
      break;    
    case Adafruit_LSM9DS0::LSM9DS0_ACCELRANGE_16G:
      accelMultiplier = LSM9DS0_ACCEL_MG_LSB_16G;
      break;
  }
 
  lsm.setupMag(magGain);
  switch(magGain)
  {
    case Adafruit_LSM9DS0::LSM9DS0_MAGGAIN_2GAUSS:
      magMultiplier = LSM9DS0_MAG_MGAUSS_2GAUSS;
      break;
    case Adafruit_LSM9DS0::LSM9DS0_MAGGAIN_4GAUSS:
      magMultiplier = LSM9DS0_MAG_MGAUSS_4GAUSS;
      break;
    case Adafruit_LSM9DS0::LSM9DS0_MAGGAIN_8GAUSS:
      magMultiplier = LSM9DS0_MAG_MGAUSS_8GAUSS;
      break;
    case Adafruit_LSM9DS0::LSM9DS0_MAGGAIN_12GAUSS:
      magMultiplier = LSM9DS0_MAG_MGAUSS_12GAUSS;
      break;
  }
  
  lsm.setupGyro(gyroScale);
   switch(gyroScale)
  {
    case Adafruit_LSM9DS0::LSM9DS0_GYROSCALE_245DPS:
      gyroMultiplier = LSM9DS0_GYRO_DPS_DIGIT_245DPS*(65535.0/720.0);
      break;
    case Adafruit_LSM9DS0::LSM9DS0_GYROSCALE_500DPS:
      gyroMultiplier = LSM9DS0_GYRO_DPS_DIGIT_500DPS*(65535.0/720.0);
      break;
    case Adafruit_LSM9DS0::LSM9DS0_GYROSCALE_2000DPS:
      gyroMultiplier = LSM9DS0_GYRO_DPS_DIGIT_2000DPS*(65535.0/720.0);
      break;
  }
  
  tempMultiplier = 1;
  
  gyroDeadZoneFactor = 1;
  accelDeadZoneFactor = 1;
}

VectorSummaryStats::VectorSummaryStats(void){
	reset();
}
  
  /***************************************************************************
  PUBLIC FUNCTIONS
  
  ***************************************************************************/
	bool Heading_Util::begin(){
		clearCalibration();
		setCalibrating(false);
		heading = 0;
		velocity.x = 0;
		velocity.y = 0;
		velocity.z = 0;
		return lsm.begin();
	}
	
	void Heading_Util::readAll(bool accumulate){
		readTimeMag = readTimeTemp = millis();
		
		if (readTimeAccel <= 0){
			accelInterval = 0;
		}else{
			accelInterval = readTimeMag - readTimeAccel;
		}
		if (readTimeGyro <= 0){
			gyroInterval = 0;
		}else{
			gyroInterval = readTimeMag - readTimeGyro;
		}
		
		readTimeGyro = readTimeAccel = readTimeMag;
		
		lsm.read();

		processAccel( accumulate);
		processMag();
		processGyro( accumulate);
		processTemp();

	}
	
	void Heading_Util::readAccel(bool accumulate){
		long temp = millis();
		if (readTimeAccel <= 0){
			accelInterval = 0;
		}else{
			accelInterval = temp - readTimeAccel;
		}
		readTimeAccel = temp;
		lsm.readAccel();
		processAccel(accumulate);
	}
	
	void Heading_Util::readGyro(bool accumulate){
		long temp = millis();
		if (readTimeGyro <= 0){
			gyroInterval = 0;
		}else{
			gyroInterval = temp - readTimeGyro;
		}
		readTimeGyro = temp;
		lsm.readGyro();
		processGyro(accumulate);
	}
	
	void Heading_Util::readMag(){
		readTimeMag = millis();
		lsm.readMag();
		processMag();
	}
	
	void Heading_Util::readTemp(){
		readTimeTemp = millis();
		lsm.readTemp();
		processTemp();
	}
	
	void Heading_Util::setCalibrating(bool calibrating){
		isCalibrating = calibrating;
		if (isCalibrating==false){
			readTimeAccel = -1;
			readTimeMag = -1;
			readTimeGyro = -1;
			readTimeTemp = -1;
		}
	}
	
	void Heading_Util::clearCalibration(void){
		gyroStats.reset();
		accelStats.reset();
	}

	void VectorSummaryStats::reset(){
	Serial.print("Minz=");
	Serial.print(min.z);
	Serial.print(">");
		error.x=error.y=error.z=0;
		min.x=min.y=min.z=1000000;
		max.x=max.y=max.z=-1000000;
		samples = 0;
		Serial.println(min.z);
	}
	
	void VectorSummaryStats::add(Adafruit_LSM9DS0::lsm9ds0Vector_t val){
		samples++;
		error.x = error.x+((val.x - error.x)/samples);
		error.y = error.y+((val.y - error.y)/samples);
		error.z = error.z+((val.z - error.z)/samples);
		
		if (val.x>max.x)
			max.x = val.x;
		else if(val.x<min.x)
			min.x = val.x;
			
		if (val.y>max.y)
			max.y = val.y;
		else if(val.y<min.y)
			min.y = val.y;
			
		if (val.z>max.z)
			max.z = val.z;
		else if(val.z<min.z)
			min.z = val.z;
	}
	
	void VectorSummaryStats::correct(Adafruit_LSM9DS0::lsm9ds0Vector_t *val,float deadZoneFactor){
		if (samples == 0)
			return;
		val->x-=error.x;
		val->y-=error.y;
		val->z-=error.z;

		if (val->x < 0){
			if(val->x > (min.x - error.x )*deadZoneFactor)
				val->x = 0;
		}else{
			if(val->x < (max.x - error.x )*deadZoneFactor)
				val->x = 0;
		}
		if (val->y < 0){
			if(val->y > (min.y - error.y )*deadZoneFactor)
				val->y = 0;
		}else{
			if(val->y < (max.y - error.y )*deadZoneFactor)
				val->y = 0;
		}
		if (val->z < 0){
			if(val->z > (min.z - error.z )*deadZoneFactor)
				val->z = 0;
		}else{
			if(val->z < (max.z - error.z )*deadZoneFactor)
				val->z = 0;
		}
	}