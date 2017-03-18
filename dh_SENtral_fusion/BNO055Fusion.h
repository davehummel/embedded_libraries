#ifndef DH_BNO055FUSION_H__
#define DH_BNO055FUSION_H__
#include "ControlledFusion.h"
#include <i2c_t3.h>

#define MS5637_RESET      0x1E
#define MS5637_CONVERT_D1 0x40
#define MS5637_CONVERT_D2 0x50
#define MS5637_ADC_READ   0x00

// BNO055 Register Map
// http://ae-bst.resource.bosch.com/media/products/dokumente/bno055/BST_BNO055_DS000_10_Release.pdf
//
// BNO055 Page 0
#define BNO055_CHIP_ID          0x00    // should be 0xA0
#define BNO055_ACC_ID           0x01    // should be 0xFB
#define BNO055_MAG_ID           0x02    // should be 0x32
#define BNO055_GYRO_ID          0x03    // should be 0x0F
#define BNO055_SW_REV_ID_LSB    0x04
#define BNO055_SW_REV_ID_MSB    0x05
#define BNO055_BL_REV_ID        0x06
#define BNO055_PAGE_ID          0x07
#define BNO055_ACC_DATA_X_LSB   0x08
#define BNO055_ACC_DATA_X_MSB   0x09
#define BNO055_ACC_DATA_Y_LSB   0x0A
#define BNO055_ACC_DATA_Y_MSB   0x0B
#define BNO055_ACC_DATA_Z_LSB   0x0C
#define BNO055_ACC_DATA_Z_MSB   0x0D
#define BNO055_MAG_DATA_X_LSB   0x0E
#define BNO055_MAG_DATA_X_MSB   0x0F
#define BNO055_MAG_DATA_Y_LSB   0x10
#define BNO055_MAG_DATA_Y_MSB   0x11
#define BNO055_MAG_DATA_Z_LSB   0x12
#define BNO055_MAG_DATA_Z_MSB   0x13
#define BNO055_GYR_DATA_X_LSB   0x14
#define BNO055_GYR_DATA_X_MSB   0x15
#define BNO055_GYR_DATA_Y_LSB   0x16
#define BNO055_GYR_DATA_Y_MSB   0x17
#define BNO055_GYR_DATA_Z_LSB   0x18
#define BNO055_GYR_DATA_Z_MSB   0x19
#define BNO055_EUL_HEADING_LSB  0x1A
#define BNO055_EUL_HEADING_MSB  0x1B
#define BNO055_EUL_ROLL_LSB     0x1C
#define BNO055_EUL_ROLL_MSB     0x1D
#define BNO055_EUL_PITCH_LSB    0x1E
#define BNO055_EUL_PITCH_MSB    0x1F
#define BNO055_QUA_DATA_W_LSB   0x20
#define BNO055_QUA_DATA_W_MSB   0x21
#define BNO055_QUA_DATA_X_LSB   0x22
#define BNO055_QUA_DATA_X_MSB   0x23
#define BNO055_QUA_DATA_Y_LSB   0x24
#define BNO055_QUA_DATA_Y_MSB   0x25
#define BNO055_QUA_DATA_Z_LSB   0x26
#define BNO055_QUA_DATA_Z_MSB   0x27
#define BNO055_LIA_DATA_X_LSB   0x28
#define BNO055_LIA_DATA_X_MSB   0x29
#define BNO055_LIA_DATA_Y_LSB   0x2A
#define BNO055_LIA_DATA_Y_MSB   0x2B
#define BNO055_LIA_DATA_Z_LSB   0x2C
#define BNO055_LIA_DATA_Z_MSB   0x2D
#define BNO055_GRV_DATA_X_LSB   0x2E
#define BNO055_GRV_DATA_X_MSB   0x2F
#define BNO055_GRV_DATA_Y_LSB   0x30
#define BNO055_GRV_DATA_Y_MSB   0x31
#define BNO055_GRV_DATA_Z_LSB   0x32
#define BNO055_GRV_DATA_Z_MSB   0x33
#define BNO055_TEMP             0x34
#define BNO055_CALIB_STAT       0x35
#define BNO055_ST_RESULT        0x36
#define BNO055_INT_STATUS       0x37
#define BNO055_SYS_CLK_STATUS   0x38
#define BNO055_SYS_STATUS       0x39
#define BNO055_SYS_ERR          0x3A
#define BNO055_UNIT_SEL         0x3B
#define BNO055_OPR_MODE         0x3D
#define BNO055_PWR_MODE         0x3E
#define BNO055_SYS_TRIGGER      0x3F
#define BNO055_TEMP_SOURCE      0x40
#define BNO055_AXIS_MAP_CONFIG  0x41
#define BNO055_AXIS_MAP_SIGN    0x42
#define BNO055_ACC_OFFSET_X_LSB 0x55
#define BNO055_ACC_OFFSET_X_MSB 0x56
#define BNO055_ACC_OFFSET_Y_LSB 0x57
#define BNO055_ACC_OFFSET_Y_MSB 0x58
#define BNO055_ACC_OFFSET_Z_LSB 0x59
#define BNO055_ACC_OFFSET_Z_MSB 0x5A
#define BNO055_MAG_OFFSET_X_LSB 0x5B
#define BNO055_MAG_OFFSET_X_MSB 0x5C
#define BNO055_MAG_OFFSET_Y_LSB 0x5D
#define BNO055_MAG_OFFSET_Y_MSB 0x5E
#define BNO055_MAG_OFFSET_Z_LSB 0x5F
#define BNO055_MAG_OFFSET_Z_MSB 0x60
#define BNO055_GYR_OFFSET_X_LSB 0x61
#define BNO055_GYR_OFFSET_X_MSB 0x62
#define BNO055_GYR_OFFSET_Y_LSB 0x63
#define BNO055_GYR_OFFSET_Y_MSB 0x64
#define BNO055_GYR_OFFSET_Z_LSB 0x65
#define BNO055_GYR_OFFSET_Z_MSB 0x66
#define BNO055_ACC_RADIUS_LSB   0x67
#define BNO055_ACC_RADIUS_MSB   0x68
#define BNO055_MAG_RADIUS_LSB   0x69
#define BNO055_MAG_RADIUS_MSB   0x6A
//
// BNO055 Page 1
#define BNO055_PAGE_ID          0x07
#define BNO055_ACC_CONFIG       0x08
#define BNO055_MAG_CONFIG       0x09
#define BNO055_GYRO_CONFIG_0    0x0A
#define BNO055_GYRO_CONFIG_1    0x0B
#define BNO055_ACC_SLEEP_CONFIG 0x0C
#define BNO055_GYR_SLEEP_CONFIG 0x0D
#define BNO055_INT_MSK          0x0F
#define BNO055_INT_EN           0x10
#define BNO055_ACC_AM_THRES     0x11
#define BNO055_ACC_INT_SETTINGS 0x12
#define BNO055_ACC_HG_DURATION  0x13
#define BNO055_ACC_HG_THRESH    0x14
#define BNO055_ACC_NM_THRESH    0x15
#define BNO055_ACC_NM_SET       0x16
#define BNO055_GYR_INT_SETTINGS 0x17
#define BNO055_GYR_HR_X_SET     0x18
#define BNO055_GYR_DUR_X        0x19
#define BNO055_GYR_HR_Y_SET     0x1A
#define BNO055_GYR_DUR_Y        0x1B
#define BNO055_GYR_HR_Z_SET     0x1C
#define BNO055_GYR_DUR_Z        0x1D
#define BNO055_GYR_AM_THRESH    0x1E
#define BNO055_GYR_AM_SET       0x1F


// Using the BNO055_MS5637 breakout board/Teensy 3.1 Add-On Shield, ADO is set to 1 by default
#define ADO 1
#if ADO
#define BNO055_ADDRESS 0x29   //  Device address of BNO055 when ADO = 1
#define MS5637_ADDRESS   0x76   //  Address of MS5637 altimeter
#else
#define BNO055_ADDRESS 0x28   //  Device address of BNO055 when ADO = 0
#define MS5637_ADDRESS   0x76   //  Address of MS5637 altimeter
#endif

#define ADC_256  0x00 // define pressure and temperature conversion rates
#define ADC_512  0x02
#define ADC_1024 0x04
#define ADC_2048 0x06
#define ADC_4096 0x08
#define ADC_8192 0x0A
#define ADC_D1   0x40
#define ADC_D2   0x50
// Set initial input parameters
enum Ascale {
  AFS_2G = 0,
  AFS_4G,
  AFS_8G,
  AFS_16G
};

enum Gscale {
  GFS_250DPS = 0,
  GFS_500DPS,
  GFS_1000DPS,
  GFS_2000DPS
};

enum Mscale {
  MFS_14BITS = 0, // 0.6 mG per LSB
  MFS_16BITS      // 0.15 mG per LSB
};

#define ADC_256  0x00 // define pressure and temperature conversion rates
#define ADC_512  0x02
#define ADC_1024 0x04
#define ADC_2048 0x06
#define ADC_4096 0x08
#define ADC_8192 0x0A
#define ADC_D1   0x40
#define ADC_D2   0x50


#define UPDATEID 55
class BNO055Fusion:public ControlledFusion{
public:
	BNO055Fusion(){

	}
	void begin(){

		if (wire->status()!=I2C_WAITING){
			wire->finish(1000);
		}
		byte lid = readByte(BNO055_ADDRESS, BNO055_CHIP_ID);
		if (lid != 0xA0){
			Serial.print("BAD BNO055 SW Revision ID: "); Serial.println(lid, HEX);
			return;
		}
	    byte swlsb = readByte(BNO055_ADDRESS, BNO055_SW_REV_ID_LSB);
	    byte swmsb = readByte(BNO055_ADDRESS, BNO055_SW_REV_ID_MSB);
	    Serial.print("BNO055 SW Revision ID: "); Serial.print(swmsb, HEX); Serial.print("."); Serial.println(swlsb, HEX);

	    // Check bootloader version
	    byte blid = readByte(BNO055_ADDRESS, BNO055_BL_REV_ID);
	    Serial.print("BNO055 bootloader Version: "); Serial.println(blid);

	    // Check self-test results
	    byte selftest = readByte(BNO055_ADDRESS, BNO055_ST_RESULT);

	    if(selftest & 0x01) {
	      Serial.println("accelerometer passed selftest");
	    } else {
	      Serial.println("accelerometer failed selftest");
	    }
	    if(selftest & 0x02) {
	      Serial.println("magnetometer passed selftest");
	    } else {
	      Serial.println("magnetometer failed selftest");
	    }
	    if(selftest & 0x04) {
	      Serial.println("gyroscope passed selftest");
	    } else {
	      Serial.println("gyroscope failed selftest");
	    }
	    if(selftest & 0x08) {
	      Serial.println("MCU passed selftest");
	    } else {
	      Serial.println("MCU failed selftest");
	      return;
	    }

		MS5637Reset();
	  delay(100);
	  Serial.println("MS5637 pressure sensor reset...");
	  // Read PROM data from MS5637 pressure sensor


	 // // Select page 1 to configure sensors
	 //   writeByte(BNO055_ADDRESS, BNO055_PAGE_ID, 0x01);
	 //   // Configure ACC
	 //   writeByte(BNO055_ADDRESS, BNO055_ACC_CONFIG, APwrMode << 5 | Abw << 3 | Ascale );
	 //   // Configure GYR
	 //   writeByte(BNO055_ADDRESS, BNO055_GYRO_CONFIG_0, Gbw << 3 | Gscale );
	 //   writeByte(BNO055_ADDRESS, BNO055_GYRO_CONFIG_1, GPwrMode);
	 //   // Configure MAG
	 //   writeByte(BNO055_ADDRESS, BNO055_MAG_CONFIG, MPwrMode << 4 | MOpMode << 2 | Modr );

	   // Select page 0 to read sensors
	   writeByte(BNO055_ADDRESS, BNO055_PAGE_ID, 0x00);

	   //Select external crystal
	   writeByte(BNO055_ADDRESS, BNO055_SYS_TRIGGER, 0x00);
	   // Select BNO055 gyro temperature source
	   writeByte(BNO055_ADDRESS, BNO055_TEMP_SOURCE, 0x01 );

	   // Select BNO055 sensor units (temperature in degrees C, rate in dps, accel in mg)
	   writeByte(BNO055_ADDRESS, BNO055_UNIT_SEL, 0b00010000 );

	   // Select BNO055 system power mode
	   writeByte(BNO055_ADDRESS, BNO055_PWR_MODE, 0x00 );

	   // Select BNO055 system operation mode
	   writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, OPRMode );

	     // Select page 0 to read sensors
	   writeByte(BNO055_ADDRESS, BNO055_PAGE_ID, 0x00);

	   started = true;
	    controller->schedule(UPDATEID , 10,10,false,0, Controller::newString("S1"),id,false);
	}

	void execute(uint32_t _time,uint32_t instid,char command[]){
		if (step2Read){
			uint8_t buffer[34];
			if (!getBytes(34, &buffer[0])) {
				controller->schedule(instid+1, 1, 0, false, 1, Controller::newString("S2"), id, false);
				return;
			}
			step2Read = false;
			uint8_t * rawData = &buffer[0];
 // int16_t val = ((int16_t)rawData[1] << 8) | rawData[0];
			gyroF[0] =((float) ((int16_t)((int16_t)rawData[1] << 8) | rawData[0]))/16.f;       // Turn the MSB and LSB into a signed 16-bit value
			gyroF[1] =((float) ((int16_t)((int16_t)rawData[3] << 8) | rawData[2]))/16.f;
			gyroF[2] =((float) ((int16_t)((int16_t)rawData[5] << 8) | rawData[4]))/16.f;

			rawData += 6;

			headingF[0] = ((float) ((int16_t)((int16_t)rawData[1] << 8) | rawData[0]))/ 16.f;       // Turn the MSB and LSB into a signed 16-bit value
			headingF[1] = ((float) ((int16_t)((int16_t)rawData[3] << 8) | rawData[2])) / 16.f;
			headingF[2] = ((float) ((int16_t)((int16_t)rawData[5] << 8) | rawData[4])) / 16.f;

			rawData += 6;

			quatF[0] =((float) ((int16_t)((int16_t)rawData[1] << 8) | rawData[0]))/16384.f;       // Turn the MSB and LSB into a signed 16-bit value
			quatF[1] =((float)  ((int16_t)((int16_t)rawData[3] << 8) | rawData[2]))/16384.f;
			quatF[2] =((float) ((int16_t)((int16_t)rawData[5] << 8) | rawData[4]))/16384.f;
			quatF[3] =((float) ((int16_t)((int16_t)rawData[7] << 8) | rawData[6]))/16384.f;

			rawData += 8;

			accelF[0] = ((float) ((int16_t)((int16_t)rawData[1] << 8) | rawData[0]))/100.f;       // Turn the MSB and LSB into a signed 16-bit value
			accelF[1] =((float) ((int16_t)((int16_t)rawData[3] << 8) | rawData[2]))/100.f;
			accelF[2] = ((float) ((int16_t)((int16_t)rawData[5] << 8) | rawData[4]))/100.f;

			rawData += 6;
      gravF[0] = ((float) ((int16_t)((int16_t)rawData[1] << 8) | rawData[0]))/100.f;       // Turn the MSB and LSB into a signed 16-bit value
			gravF[1] =((float) ((int16_t)((int16_t)rawData[3] << 8) | rawData[2]))/100.f;
			gravF[2] = ((float) ((int16_t)((int16_t)rawData[5] << 8) | rawData[4]))/100.f;

			rawData++;

			tempF = rawData[0];

			// rawData++;
      //
			// statusSys = (rawData[0] >> 6) & 0b00000011;
			// statusGyr = (rawData[0] >> 4) & 0b00000011;
			// statusAcc = (rawData[0] >> 2) & 0b00000011;
			// statusMag = (rawData[0]) & 0b00000011;


		}else{

			if (!requestBytes(BNO055_ADDRESS, BNO055_GYR_DATA_X_LSB, 34)) {
					controller->schedule(instid+1 , 1,0,false,1, Controller::newString("S2"),id,false);
			} else {
					step2Read = true;
					controller->schedule(instid+1 , 1, 0, false, 1, Controller::newString("S2"), id, false);
			}
		}

	}
protected:
	void updateHeading(){
	}

	void updateQuat(){
	}

  void updateGrav(){
	}

	void updateGyro() {}
	void updateMag() {}
	void updateAccel() {}

private:
	uint8_t readByte(uint8_t address, uint8_t subAddress)
	{
		uint8_t data; // `data` will store the register data
		wire->beginTransmission(address);         // Initialize the Tx buffer
		wire->write(subAddress);	                 // Put slave register address in Tx buffer
		wire->endTransmission(I2C_NOSTOP);        // Send the Tx buffer, but send a restart to keep connection alive
	//	wire->endTransmission(false);             // Send the Tx buffer, but send a restart to keep connection alive
	//	wire->requestFrom(address, 1);  // Read one byte from slave register address
		wire->requestFrom(address, (size_t) 1);   // Read one byte from slave register address
		data = wire->read();                      // Fill Rx buffer with result
		return data;                             // Return data read from slave register
	}

	void readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest)
	{
		wire->beginTransmission(address);   // Initialize the Tx buffer
		wire->write(subAddress);            // Put slave register address in Tx buffer
		wire->endTransmission(I2C_NOSTOP);  // Send the Tx buffer, but send a restart to keep connection alive
	//	wire->endTransmission(false);       // Send the Tx buffer, but send a restart to keep connection alive
		uint8_t i = 0;
	//        wire->requestFrom(address, count);  // Read bytes from slave register address
	        wire->requestFrom(address, (size_t) count);  // Read bytes from slave register address
		while (wire->available()) {
	        dest[i++] = wire->read(); }         // Put read results in the Rx buffer
	}

	void writeByte(uint8_t address, uint8_t subAddress, uint8_t data)
	{
		wire->beginTransmission(address);  // Initialize the Tx buffer
		wire->write(subAddress);           // Put slave register address in Tx buffer
		wire->write(data);                 // Put data in Tx buffer
		wire->endTransmission();           // Send the Tx buffer
	}

	// 1st Call this
	bool requestBytes(uint8_t address, uint8_t subAddress , uint8_t count){
		if (!wire->done()){
			return false;
		}
		wire->beginTransmission(address);   // Initialize the Tx buffer
		wire->write(subAddress);            // Put slave register address in Tx buffer
		wire->endTransmission(I2C_NOSTOP);  // Send the Tx buffer, but send a restart to keep connection alive
		wire->sendRequest(address, (size_t)count, I2C_STOP);  // Read bytes from slave register address
	  	return true;
	}

	// 2nd Call this, recall later if return false
	bool getBytes( uint8_t count, uint8_t * dest ){
		if (!wire->done())
		  return false;

		for (uint8_t i = 0; i < count; i ++){
			dest[i] = wire->read();
		}         // Put read results in the Rx buffer

	    return true;
	}


	void MS5637Reset()
    {
        wire->beginTransmission(MS5637_ADDRESS);  // Initialize the Tx buffer
		wire->write(MS5637_RESET);                // Put reset command in Tx buffer
		wire->endTransmission();                  // Send the Tx buffer
    }


	enum OPRMode {  // BNO-55 operation modes
	  CONFIGMODE = 0x00,
	// Sensor Mode
	  ACCONLY,
	  MAGONLY,
	  GYROONLY,
	  ACCMAG,
	  ACCGYRO,
	  MAGGYRO,
	  AMG,            // 0x07
	// Fusion Mode
	  IMU,
	  COMPASS,
	  M4G,
	  NDOF_FMC_OFF,
	  NDOF            // 0x0C
	};

	uint8_t OPRMode = NDOF;        // specify operation mode for sensors
	bool step2Read = false;

};

#endif
