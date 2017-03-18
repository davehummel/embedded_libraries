#include "dh_lsm9ds0.h"
#include <Wire.h> // Wire library is used for I2C

#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#include <dh_logger.h>

bool LSM9DS0::initAndVerify(bool gyroHigh,bool xmHigh){

	initI2C();

	gyroAddr = gyroHigh?LSM9DS0_GYR_I2C_SAD_H:LSM9DS0_GYR_I2C_SAD_L;
	xmAddr = xmHigh?LSM9DS0_ACC_MAG_I2C_SAD_H:LSM9DS0_ACC_MAG_I2C_SAD_L;

	if (logger){
		logger->print("gyroAddr=");
		logger->print(gyroAddr,HEX);
	}

	uint8_t test = I2CreadByte(gyroAddr,WHO_AM_I);		// Read the gyro WHO_AM_I

	if (logger){
		logger->print("gyroID=");
		logger->print(test,HEX);
		logger->println();
	}


	if (test != LSM9DS0_G_ID){
		return false;
	}

	test = I2CreadByte(xmAddr,WHO_AM_I);		// Read the gyro WHO_AM_I

	if (logger){
		logger->print("xmID=");
		logger->print(test,HEX);
		logger->println();
	}

	if (test != LSM9DS0_XM_ID){
		return false;
	}



	return true;
}

void LSM9DS0::updateSettings(){
	if (changedGyroRegisterMap&0b1 == 0b1){
		uint8_t regVal = 0;
		if (gyroXEnabled)
			regVal += 1;
		if (gyroYEnabled)
			regVal += 0b10;
		if (gyroZEnabled)
			regVal += 0b100;
		if (gyroPowered)
			regVal += 0b1000;
		regVal += gyroDrBW << 4;
		if (logger){
			logger->print("Updating Gyro Register 1:");
			logger->print(regVal,BIN);
			logger->println();
		}
		I2CwriteByte(gyroAddr, G_CTRL_REG1,regVal );
	}
	if (changedGyroRegisterMap&0b10 == 0b10){
		uint8_t regVal = 0;
	
		regVal += gyroHPmode << 4;
		regVal += gyroHPCutoff;
		if (logger){
			logger->print("Updating Gyro Register 2:");
			logger->print(regVal,BIN);
			logger->println();
		}
		I2CwriteByte(gyroAddr, G_CTRL_REG2,regVal );
	}
	if (changedGyroRegisterMap&0b100 == 0b100){
		uint8_t regVal = 0;
		if (gyroEnableFIFOEmpty_DRDY)
			regVal += 1;
		if (gyroEnableFIFOOverrun_DRDY)
			regVal += 0b10;
		if (gyroEnableWaterMark_DRDY)
			regVal += 0b100;
		if (gyroEnableDataInterup_DRDY)
			regVal += 0b1000;
		if (gyroPushPull_OpenDrain)
			regVal += 0b10000;
		if (gyroEnableActive_Int_G)
			regVal += 0b100000;
		if (gyroEnableBoot_Int_G)
			regVal += 0b1000000;
		if (gyroEnableInterput_Int_G)
			regVal += 0b10000000;
		if (logger){ 
			logger->print("Updating Gyro Register 3:");
			logger->print(regVal,BIN);
			logger->println();
		}
		I2CwriteByte(gyroAddr, G_CTRL_REG3,regVal );
	}
	if (changedGyroRegisterMap&0b1000 == 0b1000){
		uint8_t regVal = 0;
	
		regVal += gyroBlockDataUntilRead << 7;
		regVal += gyroBLE << 6;
		regVal += gyroScale << 4;
		regVal += gyroTestMode << 1;
		regVal += gyroSIM;

		if (logger){
			logger->print("Updating Gyro Register 4:");
			logger->print(regVal,BIN);
			logger->println();
		}
		I2CwriteByte(gyroAddr, G_CTRL_REG4,regVal );
		// to calculate DPS/(ADC tick) based on that 2-bit value:
	}
	if (changedGyroRegisterMap&0b10000 == 0b10000){
		uint8_t regVal = 0;
	
		regVal += gyroBoot << 7;

		regVal += gyroFIFOEnabled << 6;
		regVal += gyroHPEnabled << 4;
		regVal += gyroINT1Select << 2;
		regVal += gyroOUTSelect;

		if (logger){
			logger->print("Updating Gyro Register 5:");
			logger->print(regVal,BIN);
			logger->println();
		}
		I2CwriteByte(gyroAddr, G_CTRL_REG5,regVal );
	}

	if (changedMagRegister){
		uint8_t regVal = 0;
	
		regVal +=magEnableXInt << 7;
		regVal += magEnableYInt << 6;
		regVal += magEnableZInt << 5;
		regVal += magPushPull_OpenDrain << 4;
		regVal += magXMEnableActive_Int << 3;
		regVal += magXMLatch<<2;
		regVal += magXM4dEnabled<<1;
		regVal += magInteruptEnabled;

		if (logger){
			logger->print("Updating Mag Register:");
			logger->print((uint8_t)M_INT_CTRL_REG);
			logger->print(":");
			logger->print(regVal,HEX);
			logger->println();
		}
		I2CwriteByte(xmAddr, M_INT_CTRL_REG,regVal );
	}
	if (changedXMRegisterMap&0b1 == 0b1){
		uint8_t regVal = 0;
	
		regVal += xmBoot << 7;
		regVal += xmFIFOEnabled << 6;
		regVal += xmWTMEnabled << 5;

		regVal += xmHPClickEnabled << 2;
		regVal += xmHPInteruptGen1<<1;
		regVal += xmHPInteruptGen2;

		if (logger){
			logger->print("Updating XM Register0:");
			logger->print((uint8_t)XM_CTRL_REG0);
			logger->print(":");
			logger->print(regVal,HEX);
			logger->println();
		}
		I2CwriteByte(xmAddr, XM_CTRL_REG0,regVal );
	}
	if (changedXMRegisterMap&0b10 == 0b10){
		uint8_t regVal = 0;
	
		regVal += accelRate << 4;
		regVal += xmBlockDataUntilRead << 3;
		regVal += xmAccZEnabled << 2;
		regVal += xmAccYEnabled << 1;
		regVal += xmAccXEnabled;
		
		if (logger){
			logger->print("Updating XM Register1:");
			logger->print((uint8_t)XM_CTRL_REG1);
			logger->print(":");
			logger->print(regVal,HEX);
			logger->println();
		}
		I2CwriteByte(xmAddr, XM_CTRL_REG1,regVal );
	}
	if (changedXMRegisterMap&0b100 == 0b100){
		uint8_t regVal = 0;
	
		regVal += accelAAFilterBW << 6;
		regVal += accelScale << 3;
		regVal += xmTestMode << 1;
		regVal += xmSIM;
		
		if (logger){
			logger->print("Updating XM Register2:");
			logger->print((uint8_t)XM_CTRL_REG2);
			logger->print(":");
			logger->print(regVal,HEX);
			logger->println();
		}
		I2CwriteByte(xmAddr, XM_CTRL_REG2,regVal );
	}
	if (changedXMRegisterMap&0b1000 == 0b1000){
		uint8_t regVal = 0;
	
		regVal += xmP1Boot << 7;
		regVal += xmP1Tap << 6;
		regVal += xmP1Int1 << 5;
		regVal += xmP1Int2 << 4;
		regVal += magP1IntM << 3;
		regVal += xmP1DRInt << 2;
		regVal += magP1DRInt << 1; 
		regVal += xmFIFOEmptyInt1;
		
		if (logger){
			logger->print("Updating XM Register3:");
			logger->print((uint8_t)XM_CTRL_REG3);
			logger->print(":");
			logger->print(regVal,HEX);
			logger->println();
		}
		I2CwriteByte(xmAddr, XM_CTRL_REG3,regVal );
	}

	if (changedXMRegisterMap&0b10000 == 0b10000){
		uint8_t regVal = 0;
	
		regVal += xmP2Tap << 7;
		regVal += xmP2Int1 << 6;
		regVal += xmP2Int2 << 5;
		regVal += magP2IntM << 4;
		regVal += xmP2DRInt << 3;
		regVal += magP2DRInt << 2;
		regVal += xmFIFOOverrunInt2 << 1;
		regVal += xmFIFOWMInt2;
		
		if (logger){
			logger->print("Updating XM Register4:");
			logger->print((uint8_t)XM_CTRL_REG4);
			logger->print(":");
			logger->print(regVal,HEX);
			logger->println();
		}
		I2CwriteByte(xmAddr, XM_CTRL_REG4,regVal );
	}

	if (changedXMRegisterMap&0b100000 == 0b100000){
		uint8_t regVal = 0;
	
		regVal += tempEnabled << 7;
		regVal += magHighResMode << 6;
		regVal += magHighResMode << 5;
		regVal += magRate << 2;
		regVal += latchIntOnINT2_SRC << 1;
		regVal += latchIntOnINT1_SRC;
		
		if (logger){
			logger->print("Updating XM Register5:");
			logger->print((uint8_t)XM_CTRL_REG5);
			logger->print(":");
			logger->print(regVal,BIN);
			logger->println();
		}
		I2CwriteByte(xmAddr, XM_CTRL_REG5,regVal );
	}

	if (changedXMRegisterMap&0b1000000 == 0b1000000){
		uint8_t regVal = 0;
	
		regVal += magScale << 5;
		
		if (logger){
			logger->print("Updating XM Register6:");
			logger->print((uint8_t)XM_CTRL_REG6);
			logger->print(":");
			logger->print(regVal,HEX);
			logger->println();
		}
		I2CwriteByte(xmAddr, XM_CTRL_REG6,regVal );
	}

	if (changedXMRegisterMap&0b10000000 == 0b10000000){
		uint8_t regVal = 0;
	
		regVal += accelHPMode << 6;
		regVal += xmFilterDS << 5;
		regVal += magLowPower << 2;
		regVal += magSensorMode ;
		
		if (logger){
			logger->print("Updating XM Register7:");
			logger->print((uint8_t)XM_CTRL_REG7);
			logger->print(":");
			logger->print(regVal,BIN);
			logger->println();
		}
		I2CwriteByte(xmAddr, XM_CTRL_REG7,regVal );
	}


	changedGyroRegisterMap = 0;
	changedXMRegisterMap = 0;
	changedMagRegister = false;
}

void LSM9DS0::readRawGyro(){
	uint8_t temp[6]; // We'll read six bytes from the gyro into temp
	I2CreadBytes(gyroAddr,G_OUT_X_L, temp, 6); // Read 6 bytes, beginning at OUT_X_L_G
	gx = (temp[1] << 8) | temp[0]; // Store x-axis values into gx
	gy = (temp[3] << 8) | temp[2]; // Store y-axis values into gy
	gz = (temp[5] << 8) | temp[4]; // Store z-axis values into gz
}

void LSM9DS0::readRawTemp(){
	uint8_t temp[2]; // We'll read six bytes from the gyro into temp
	I2CreadBytes(xmAddr,T_OUT_L, temp, 2); // Read 6 bytes, beginning at OUT_X_L_G
	temperature = (((int16_t) temp[1] << 12) | temp[0] << 4 ) >> 4; // Temperature is a 12-bit signed integer
}

void LSM9DS0::readRawMag(){
	uint8_t temp[6]; // We'll read six bytes from the mag into temp	
	I2CreadBytes(xmAddr,M_OUT_X_L, temp, 6); // Read 6 bytes, beginning at OUT_X_L_M
	mx = (temp[1] << 8) | temp[0]; // Store x-axis values into mx
	my = (temp[3] << 8) | temp[2]; // Store y-axis values into my
	mz = (temp[5] << 8) | temp[4]; // Store z-axis values into mz
}

void LSM9DS0::readRawAccel(){
	uint8_t temp[6]; // We'll read six bytes from the accelerometer into temp	
	I2CreadBytes(xmAddr,A_OUT_X_L, temp, 6); // Read 6 bytes, beginning at OUT_X_L_A
	ax = (temp[1] << 8) | temp[0]; // Store x-axis values into ax
	ay = (temp[3] << 8) | temp[2]; // Store y-axis values into ay
	az = (temp[5] << 8) | temp[4]; // Store z-axis values into az
}


// PRIVATE


void LSM9DS0::initI2C()
{
	Wire.begin();	// Initialize I2C library
}

// Wire.h read and write protocols
void LSM9DS0::I2CwriteByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
	Wire.beginTransmission(address);  // Initialize the Tx buffer
	Wire.write(subAddress);           // Put slave register address in Tx buffer
	Wire.write(data);                 // Put data in Tx buffer
	Wire.endTransmission();           // Send the Tx buffer
}

uint8_t LSM9DS0::I2CreadByte(uint8_t address, uint8_t subAddress)
{
	uint8_t data; // `data` will store the register data	 
	Wire.beginTransmission(address);         // Initialize the Tx buffer
	Wire.write(subAddress);	                 // Put slave register address in Tx buffer
	Wire.endTransmission(false);             // Send the Tx buffer, but send a restart to keep connection alive
	Wire.requestFrom(address, (uint8_t) 1);  // Read one byte from slave register address 
	data = Wire.read();                      // Fill Rx buffer with result
	return data;                             // Return data read from slave register
}

void LSM9DS0::I2CreadBytes(uint8_t address, uint8_t subAddress, uint8_t * dest, uint8_t count)
{  
	Wire.beginTransmission(address);   // Initialize the Tx buffer
	// Next send the register to be read. OR with 0x80 to indicate multi-read.
	Wire.write(subAddress | 0x80);     // Put slave register address in Tx buffer
	Wire.endTransmission(false);       // Send the Tx buffer, but send a restart to keep connection alive
	uint8_t i = 0;
	Wire.requestFrom(address, count);  // Read bytes from slave register address 
	while (Wire.available()) 
	{
		dest[i++] = Wire.read(); // Put read results in the Rx buffer
	}
}