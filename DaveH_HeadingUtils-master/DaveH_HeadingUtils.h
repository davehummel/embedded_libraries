#include <Adafruit_LSM9DS0.h>
#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#define MAX_ANGLE_RANGE 65535
#define GRAV_ACCEL_CONST (.9807F) 

class VectorSummaryStats {
	public:
      Adafruit_LSM9DS0::lsm9ds0Vector_t  error;
	  Adafruit_LSM9DS0::lsm9ds0Vector_t  min;
	  Adafruit_LSM9DS0::lsm9ds0Vector_t  max;
   
	  unsigned long samples;
	  
	  void add(Adafruit_LSM9DS0::lsm9ds0Vector_t val);
	  void correct(Adafruit_LSM9DS0::lsm9ds0Vector_t *val,float deadZoneFactor);
	  void reset(void);
	  
	  VectorSummaryStats(void);
};

class Heading_Util
{
  public:
  
    Heading_Util ( Adafruit_LSM9DS0::lsm9ds0AccelRange_t accelRange , Adafruit_LSM9DS0::lsm9ds0MagGain_t magGain , Adafruit_LSM9DS0::lsm9ds0GyroScale_t gyroScale );
	
	bool begin(void);
	
	void readAll(bool accumulate);
	void readAccel(bool accumulate);
	void readMag(void);
	void readGyro(bool accumulate);
	void readTemp(void);

	void clearCalibration(void);
	
	void setCalibrating(bool calibrating);

	
	Adafruit_LSM9DS0::lsm9ds0Vector_t accelData;    // Processed accelerometer data will be available here
    Adafruit_LSM9DS0::lsm9ds0Vector_t magData;      // Processed magnetometer data will be available here
    Adafruit_LSM9DS0::lsm9ds0Vector_t gyroData;     // Processed gyroscope data will be available here
    int16_t  temperature;  // Processed temperature data will be available here
	
	long readTimeAccel;
	long readTimeMag;
	long readTimeGyro;
	long readTimeTemp;
	
	Adafruit_LSM9DS0::lsm9ds0Vector_t velocity;
	unsigned int heading;
	float gyroDeadZoneFactor;
	float accelDeadZoneFactor;
	
	VectorSummaryStats gyroStats;
	VectorSummaryStats accelStats;
	
	bool isCalibrating;
	
		
	Adafruit_LSM9DS0 lsm;

  private:
	
	void accelAccumulate(Adafruit_LSM9DS0::lsm9ds0Vector_t accelVals);
	void gyroAccumulate(Adafruit_LSM9DS0::lsm9ds0Vector_t gyroVals);
	
	void processAccel(bool accumulate);
	void processMag(void);
	void processGyro(bool accumulate);
	void processTemp(void);
  
	float tempMultiplier;
	float accelMultiplier;
	float magMultiplier;
	float gyroMultiplier;
	
	int gyroInterval;
	int accelInterval;
	

};

