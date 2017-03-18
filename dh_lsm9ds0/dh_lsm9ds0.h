
#ifndef DH_LSM9DS0_H__
#define DH_LSM9DS0_H__

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
#endif
#include <dh_logger.h>



#define	ACC_G_MAX_POS		1495040	/** max positive value acc [ug] */
#define	ACC_G_MAX_NEG		1495770	/** max negative value acc [ug] */
#define	MAG_G_MAX_POS		983520	/** max positive value mag [ugauss] */
#define	MAG_G_MAX_NEG		983040	/** max negative value mag [ugauss] */
#define FS_MAX		32768


#define LSM9DS0_SAD0L_ACC_MAG		(0x02)
#define LSM9DS0_SAD0H_ACC_MAG		(0x01)
#define LSM9DS0_SAD0L_GYR		(0x00)
#define LSM9DS0_SAD0H_GYR		(0x01)


#define LSM9DS0_GYR_I2C_SADROOT	(0x35)
#define LSM9DS0_ACC_MAG_I2C_SADROOT	(0x07)

/* I2C address if gyr SA0 pin to GND */  
#define LSM9DS0_GYR_I2C_SAD_L		((LSM9DS0_GYR_I2C_SADROOT<<1)| \
							LSM9DS0_SAD0L_GYR)
/* I2C address if gyr SA0 pin to Vdd */
#define LSM9DS0_GYR_I2C_SAD_H		((LSM9DS0_GYR_I2C_SADROOT<<1)| \
							LSM9DS0_SAD0H_GYR)

/* I2C address if gyr SA0 pin to GND */
#define LSM9DS0_ACC_MAG_I2C_SAD_L	((LSM9DS0_ACC_MAG_I2C_SADROOT<<2)| \
							LSM9DS0_SAD0L_ACC_MAG)
/* I2C address if gyr SA0 pin to Vdd */
#define LSM9DS0_ACC_MAG_I2C_SAD_H	((LSM9DS0_ACC_MAG_I2C_SADROOT<<2)| \
							LSM9DS0_SAD0H_ACC_MAG)


#define LSM9DS0_G_ID                (0xD4)
#define LSM9DS0_XM_ID                (0x49)

#define G_CTRL_REG1	(0x20)    /* CTRL REG1 */
#define G_CTRL_REG2	(0x21)    /* CTRL REG2 */
#define G_CTRL_REG3	(0x22)    /* CTRL_REG3 */
#define G_CTRL_REG4	(0x23)    /* CTRL_REG4 */
#define G_CTRL_REG5	(0x24)    /* CTRL_REG5 */
#define	G_REFERENCE	(0x25)    /* REFERENCE REG */
#define	G_OUT_X_L		(0x28)    /* 1st AXIS OUT REG of 6 */
#define M_OUT_X_L		(0x08)
#define A_OUT_X_L		(0x28)
#define T_OUT_L		0x05
#define	G_FIFO_CTRL_REG	(0x2E)    /* FIFO CONTROL REGISTER */
#define G_FIFO_SRC_REG	(0x2F)    /* FIFO SOURCE REGISTER */

#define M_INT_CTRL_REG	(0x12)	/** CTRL_REG0_XM address register */

#define XM_CTRL_REG0	(0x1F)	/** INT_CTRL_REG_M address register */
#define XM_CTRL_REG1	(0x20)	/** INT_CTRL_REG1_M address register */
#define XM_CTRL_REG2	(0x21)	/** INT_CTRL_REG2_M address register */
#define XM_CTRL_REG3	(0x22)	/** INT_CTRL_REG3_M address register */
#define XM_CTRL_REG4	(0x23)	/** INT_CTRL_REG4_M address register */
#define XM_CTRL_REG5	(0x24)	/** INT_CTRL_REG5_M address register */
#define XM_CTRL_REG6	(0x25)	/** INT_CTRL_REG6_M address register */
#define XM_CTRL_REG7	(0x26)	/** INT_CTRL_REG7_M address register */

#define WHO_AM_I	(0x0F)


class LSM9DS0{
public:

	// accel_scale defines all possible FSR's of the accelerometer:
	enum accel_scale
	{
		A_SCALE_2G,	// 000:  2g
		A_SCALE_4G,	// 001:  4g
		A_SCALE_6G,	// 010:  6g
		A_SCALE_8G,	// 011:  8g
		A_SCALE_16G	// 100:  16g
	};
		// accel_odr defines all possible output data rates of the accelerometer:
	enum accel_odr
	{
		A_POWER_DOWN, 	// Power-down mode (0x0)
		A_ODR_3125,		// 3.125 Hz	(0x1)
		A_ODR_625,		// 6.25 Hz (0x2)
		A_ODR_125,		// 12.5 Hz (0x3)
		A_ODR_25,		// 25 Hz (0x4)
		A_ODR_50,		// 50 Hz (0x5)
		A_ODR_100,		// 100 Hz (0x6)
		A_ODR_200,		// 200 Hz (0x7)
		A_ODR_400,		// 400 Hz (0x8)
		A_ODR_800,		// 800 Hz (9)
		A_ODR_1600		// 1600 Hz (0xA)
	};

      // accel_abw defines all possible anti-aliasing filter rates of the accelerometer:
	enum accel_abw
	{
		A_ABW_773,		// 773 Hz (0x0)
		A_ABW_194,		// 194 Hz (0x1)
		A_ABW_362,		// 362 Hz (0x2)
		A_ABW_50,		//  50 Hz (0x3)
	};

	enum gyro_scale
	{
		G_SCALE_245DPS,		// 00:  245 degrees per second
		G_SCALE_500DPS,		// 01:  500 dps
		G_SCALE_2000DPS,	// 10:  2000 dps
	};

		enum gyro_dr_bw   // Data Rate and Cut off (possibly for low pass filter???)
	{
		G_DR_95_BW_125,//  95         12.5
		G_DR_95_BW_25, //  95          25
		G_DR_95_BW_25ALT, // 95          25 same as other?
		G_DR_95_BW_25ALT2, // 95          25 same as other?
		G_DR_190_BW_125 , //   190        12.5
		G_DR_190_BW_25 ,//   190         25
		G_DR_190_BW_50 , //   190         50
		G_DR_190_BW_70 , //   190         70
		G_DR_380_BW_20 , //   380         20
		G_DR_380_BW_25 , //   380         25
		G_DR_380_BW_50 , //   380         50
		G_DR_380_BW_100 , //   380         100
		G_DR_760_BW_30 , //   760         30
		G_DR_760_BW_35 , //   760         35
		G_DR_760_BW_50 , //   760         50
		G_DR_760_BW_100 , //   760         100
	};
	enum gyro_hpmode
	{
		G_HP_NORMAL_RESET,		
		G_HP_REFSIGNAL,		
		G_HP_NORMAL,		
		G_HP_AUTORESET,
	};

		// mag_scale defines all possible FSR's of the magnetometer:
	enum mag_scale
	{
		M_SCALE_2GS,	// 00:  2Gs
		M_SCALE_4GS, 	// 01:  4Gs
		M_SCALE_8GS,	// 10:  8Gs
		M_SCALE_12GS,	// 11:  12Gs
	};

		// mag_odr defines all possible output data rates of the magnetometer:
	enum mag_odr
	{
		M_ODR_3125,	// 3.125 Hz (0x00)
		M_ODR_625,	// 6.25 Hz (0x01)
		M_ODR_125,	// 12.5 Hz (0x02)
		M_ODR_25,	// 25 Hz (0x03)
		M_ODR_50,	// 50 (0x04)
		M_ODR_100,	// 100 Hz (0x05)
	};

	enum accel_HPMode
	{
		A_HPM_NORMAL_RESET,
		A_HPM_REFSIGNAL,
		A_HPM_NORMAL,
		A_HPM_AUTORESET
	};

	enum mag_sensormode
	{
		M_S_CONT,
		M_S_SINGLE,
		M_S_POWERDOWN,
		M_S_POWERDOWN2
	};

	bool initAndVerify(bool gyroHigh,bool xmHigh);

	void setGyroDrBw(gyro_dr_bw dr_bw){
			gyroDrBW = dr_bw;
			changedGyroRegisterMap |= 1;
	}

	void setGyroPowered(bool isPowered){
			gyroPowered = isPowered;
			changedGyroRegisterMap |= 1;
	}

	void setGyroAxisEnabled(bool x, bool y , bool z){
			gyroXEnabled = x;
			gyroYEnabled =y;
			gyroZEnabled = z;
			changedGyroRegisterMap |= 1;
	}

	void setGyroHPMode(gyro_hpmode mode){
		gyroHPmode = mode;
		changedGyroRegisterMap |= 2;
	}

	void setGyroHPCutoff(uint8_t cutoff){  // the actual cutoff depends on ODR, but 0 = highest and 9 = lowest
		gyroHPCutoff = cutoff;
		if (gyroHPCutoff>9)
			gyroHPCutoff = 9;
		changedGyroRegisterMap |= 0b10;
	}

	void setGyroPins(bool _enableInterput_Int_G ,bool _enableBoot_Int_G ,bool _enableActive_Int_G, bool _pushPull_OpenDrain, bool _enableDataInterup_DRDY,bool _enableWaterMark_DRDY,bool _enableFIFOOverrun_DRDY,bool _enableFIFOEmpty_DRDY){
		gyroEnableInterput_Int_G =_enableInterput_Int_G;
		gyroEnableBoot_Int_G = _enableBoot_Int_G;
		gyroEnableActive_Int_G = _enableActive_Int_G; 
		gyroPushPull_OpenDrain = _pushPull_OpenDrain; 
		gyroEnableDataInterup_DRDY = _enableDataInterup_DRDY;
		gyroEnableWaterMark_DRDY = _enableWaterMark_DRDY; 
		gyroEnableFIFOOverrun_DRDY = _enableFIFOOverrun_DRDY; 
		gyroEnableFIFOEmpty_DRDY = _enableFIFOEmpty_DRDY;
		changedGyroRegisterMap |= 0b100;
	}

	void setGyroScale(gyro_scale scale){
		gyroScale = scale;
		changedGyroRegisterMap |= 0b1000;
	}

	void setGyroBlockMode(bool blockUntilRead){
		gyroBlockDataUntilRead = blockUntilRead;
		changedGyroRegisterMap |= 0b1000;
	}

	void setGyroBoot(bool boot){
		gyroBoot = boot;
		changedGyroRegisterMap |= 0b10000;
	}

	void setGyroFIFOEnabled(bool enabled){
		gyroFIFOEnabled = enabled;
		changedGyroRegisterMap |= 0b10000;
	}

	void setGyroHPEnabled(bool enabled){
		gyroHPEnabled = enabled;
		changedGyroRegisterMap |= 0b10000;
	}

	void setGyroSelectionVals(uint8_t interupt, uint8_t output){
		gyroINT1Select = interupt;

		if (gyroINT1Select > 3)
			gyroINT1Select = 3;

		gyroOUTSelect=output;

		if (gyroOUTSelect > 3)
			gyroOUTSelect = 3;

		changedGyroRegisterMap |= 0b10000;
	}

	void setMagXMPins(bool xEnabled, bool yEnabled, bool zEnabled,
	 bool _pushPull_OpenDrain, bool interuptActiveHigh, bool latchMagXM,
	  bool _4dEnable, bool intEnabled){
		magEnableXInt = xEnabled;
		magEnableYInt = yEnabled;
		magEnableZInt = zEnabled;

		magPushPull_OpenDrain = _pushPull_OpenDrain;
		magXMEnableActive_Int = interuptActiveHigh;
		magXMLatch = latchMagXM;
		magInteruptEnabled = intEnabled;
		magXM4dEnabled = _4dEnable;

		changedMagRegister = true;
	}

	void setXMBoot(bool boot){
		xmBoot = boot;
		changedXMRegisterMap |= 0b1;
	}

	void setXMFIFOEnabled(bool fifoEnabled, bool fifoWMEnabled){
		xmFIFOEnabled = fifoEnabled;
		xmWTMEnabled = fifoWMEnabled;
		changedXMRegisterMap |= 0b1;
	}

	void setXMHPFeatures(bool click, bool int1, bool int2){
		xmHPClickEnabled = click;
		xmHPInteruptGen1 = int1;
		xmHPInteruptGen2 = int2;
	}

	void setAccelRate( accel_odr rate){
		accelRate = rate;
		changedXMRegisterMap |= 0b10;
	}

	void setXMBlockMode(bool blockUntilRead){
		xmBlockDataUntilRead = blockUntilRead;
		changedXMRegisterMap |= 0b10;
	}

	void setAccelAxisEnabled(bool x,bool y,bool z){
		xmAccXEnabled = x;
		xmAccYEnabled = y;
		xmAccZEnabled = z;
		changedXMRegisterMap |= 0b10;
	}

	void setAccelAAFilterBW (	accel_abw abw){
		 accelAAFilterBW = abw;
		changedXMRegisterMap |= 0b100;
	}

	void setAccelScale (	accel_scale scale){
		 accelScale=scale;
		changedXMRegisterMap |= 0b100;
	}

	void setXMTestMode (uint8_t mode){
		xmTestMode = mode;
		changedXMRegisterMap |= 0b100;
	}

	void setXMIntPin1( bool _xmP1Boot, bool _xmP1Tap ,	bool _xmP1Int1 ,bool _xmP1Int2 ,
		bool _magP1IntM, bool _xmP1DRInt1, bool _magP1DRInt1 , bool _xmFIFOEmptyInt1 ){
		xmP1Boot = _xmP1Boot;
		xmP1Tap = _xmP1Tap;
		xmP1Int1 = _xmP1Int1;
		xmP1Int2 = _xmP1Int2;
		magP1IntM = _magP1IntM;
		xmP1DRInt = _xmP1DRInt1;
		magP1DRInt = _magP1DRInt1;
		xmFIFOEmptyInt1 = _xmFIFOEmptyInt1;

		changedXMRegisterMap |= 0b1000;
	}

	void setXMIntPin2( bool _xmP2Tap ,	bool _xmP2Int1 ,bool _xmP2Int2 ,
		bool _magP2IntM, bool _xmP2DRInt1, bool _magP2DRInt1 , bool _xmFIFOOverrunInt2, bool _xmFIFOWMInt2 ){
		xmP2Tap = _xmP2Tap;
		xmP2Int1 = _xmP2Int1;
		xmP2Int2 = _xmP2Int2;
		magP2IntM = _magP2IntM;
		xmP2DRInt = _xmP2DRInt1;
		magP2DRInt = _magP2DRInt1;
		xmFIFOOverrunInt2 = _xmFIFOOverrunInt2;
		xmFIFOWMInt2 = _xmFIFOWMInt2;
		changedXMRegisterMap |= 0b10000;
	}

	void setLIR(bool int1Latched,bool int2Latched){
		latchIntOnINT1_SRC = int1Latched;
		latchIntOnINT2_SRC = int2Latched;
		changedXMRegisterMap |= 0b100000;
	}

	void setTemperatureEnabled(bool enabled){
		tempEnabled = enabled;
		changedXMRegisterMap |= 0b100000;
	}

	void setMagHighResMode (bool enabled){
		magHighResMode = enabled;
		changedXMRegisterMap |= 0b100000;
	}

	void setMagRate(mag_odr rate){
		magRate = rate;
		changedXMRegisterMap |= 0b100000;
	}

	void setMagScale(mag_scale scale){
		magScale = scale;
		changedXMRegisterMap |= 0b1000000;
	}


	void setAccelHPMode (accel_HPMode mode){
		accelHPMode = mode;
		changedXMRegisterMap |= 0b10000000;
	}

	void setXMFilterDS (bool enableXMFilterData){
		xmFilterDS = enableXMFilterData;
		changedXMRegisterMap |= 0b10000000;
	}

	void setMagLowPower (bool enableLowPower){
		magLowPower = enableLowPower;
		changedXMRegisterMap |= 0b10000000;
	}

	void setMagSensorMode (mag_sensormode sensorMode){
		magSensorMode = sensorMode;
		changedXMRegisterMap |= 0b10000000;
	}

	void updateSettings();

	Logger *logger = NULL;

	void readRawGyro();
	void readRawAccel();
	void readRawMag();
	void readRawTemp();

		// Gyro raw values
	int16_t gx=0,gy=0,gz=0;
	int16_t ax=0,ay=0,az=0;
	int16_t mx=0, my=0, mz=0; // x, y, and z axis readings of the magnetometer
    int16_t temperature=0;

private:
	uint8_t xmAddr = 0; // I2C address for accelerometer 
	uint8_t gyroAddr = 0; // I2C address for Gyro .. either \


	uint8_t changedGyroRegisterMap=0b00011111;
	uint8_t changedXMRegisterMap=0b11111111;
	bool changedMagRegister = true;

	// Gyro Register 1
	gyro_dr_bw gyroDrBW=G_DR_95_BW_125;

	bool gyroPowered=false;

	bool gyroZEnabled=true,gyroXEnabled=true,gyroYEnabled=true;
	
	// Gyro Register 2
	gyro_hpmode gyroHPmode=G_HP_NORMAL_RESET;
	uint8_t gyroHPCutoff=0000;

	// Gyro Register 3
	bool gyroEnableInterput_Int_G = false; //Interrupt enable on INT_G pin. Default value 0. (0: disable; 1: enable)
	bool gyroEnableBoot_Int_G = false; //Boot status available on INT_G. Default value 0. (0: disable; 1: enable)
	bool gyroEnableActive_Int_G = false; //Interrupt active configuration on INT_G. Default value 0. (0: high; 1:low)
	bool gyroPushPull_OpenDrain = false; //Push-pull / Open drain. Default value: 0. (0: push- pull; 1: open drain)
	bool gyroEnableDataInterup_DRDY = true; //Data-ready on DRDY_G. Default value 1. (0: disable; 1: enable)
	bool gyroEnableWaterMark_DRDY = false; // FIFO watermark interrupt on DRDY_G. Default value: 0. (0: disable; 1: enable)
	bool gyroEnableFIFOOverrun_DRDY = false; // FIFO overrun interrupt on DRDY_G. Default value: 0. (0: disable; 1: enable)
	bool gyroEnableFIFOEmpty_DRDY = false; //FIFO empty interrupt on DRDY_G. Default value: 0. (0: disable; 1: enable)
	//Gyro Register 4
	bool gyroBlockDataUntilRead = false; // continuous mode
	bool gyroBLE = false;
	gyro_scale gyroScale = G_SCALE_245DPS;
	uint8_t gyroTestMode = 0,xmTestMode = 0;
	bool gyroSIM = 0,xmSIM = 0;
	//Gyro Register 5
	bool gyroBoot = false, xmBoot = false; //  Reboot memory content. Default value: 0 (0: normal mode; 1: reboot memory content)
	bool gyroFIFOEnabled = false;
	bool gyroHPEnabled = false;
	uint8_t gyroINT1Select = 0; //0-3
	uint8_t gyroOUTSelect = 0; //0-3

	bool magEnableXInt = false;
	bool magEnableYInt = false;
	bool magEnableZInt = false;

	bool magPushPull_OpenDrain = false;	 // - Push-pull/open-drain interrupt configuration (0=push-pull, 1=od)
	bool magXMEnableActive_Int = false;  // Interrupt polarity for both accelerometer and magnetometer. Default value: 0. (0: interrupt active-low; 1: interrupt active-high)
	bool magXMLatch = false;  // Latch interrupt request on accelerometer INT_GEN_1_SRC (31h) and INT_GEN_2_SRC (35h) registers, and magnetometer INT_SRC_REG_M (13h) register. Default value: 0. (0: interrupt request not latched; 1: interrupt request latched)
	bool magXM4dEnabled = false; // 4D enable: 4D detection on acceleration data is enabled when 6D bit in INT_GEN_1_REG (30h) is set to 1.
	bool magInteruptEnabled = false; 

	bool xmFIFOEnabled = false; 
	bool xmWTMEnabled = false;
	bool xmHPClickEnabled = false;
	bool xmHPInteruptGen1 = false;
	bool xmHPInteruptGen2 = false;



	accel_odr accelRate = A_POWER_DOWN;
	bool xmBlockDataUntilRead =false;
	bool xmAccZEnabled =false;
	bool xmAccYEnabled =false;
	bool xmAccXEnabled =false;

	accel_abw accelAAFilterBW = A_ABW_362;
	accel_scale accelScale=A_SCALE_2G;

	bool xmP1Boot = false; //Boot on INT1_XM pin enable. Default value: 0
	bool xmP1Tap = false; //Tap generator interrupt on INT1_XM pin. Default value: 0
	bool xmP1Int1 = false; // Inertial interrupt generator 1 on INT1_XM pin. Default value: 0
	bool xmP1Int2 = false; //Inertial interrupt generator 2 on INT1_XM pin. Default value: 0
	bool magP1IntM = false; // Magnetic interrupt generator on INT1_XM pin. Default value: 0
	bool xmP1DRInt = false; // Accelerometer data-ready signal on INT1_XM pin. Default value: 0
	bool magP1DRInt = false; // A Magnetometer data-ready signal on INT1_XM pin. Default value: 0
	bool xmFIFOEmptyInt1 = false; //FIFO empty indication on INT1_XM pin. Default value: 0

	bool xmP2Tap = false; //Tap generator interrupt on INT2_XM pin. Default value: 0
	bool xmP2Int1 = false; // Inertial interrupt generator 1 on INT2_XM pin. Default value: 0
	bool xmP2Int2 = false; //Inertial interrupt generator 2 on INT2_XM pin. Default value: 0
	bool magP2IntM = false; // Magnetic interrupt generator on INT2_XM pin. Default value: 0
	bool xmP2DRInt = false; // Accelerometer data-ready signal on INT2_XM pin. Default value: 0
	bool magP2DRInt = false; // A Magnetometer data-ready signal on INT2_XM pin. Default value: 0
	bool xmFIFOOverrunInt2 = false; //FIFO overrun interrupt on INT2_XM pin. Default value: 0
	bool xmFIFOWMInt2 = false; //FIFO watermark interrupt on INT2_XM pin. Default value: 0

	bool tempEnabled = false;
	mag_scale magScale = M_SCALE_2GS;
	mag_odr magRate = M_ODR_3125;
	bool magHighResMode = false; //Magnetic resolution selection. 
	bool latchIntOnINT2_SRC = false;
	bool latchIntOnINT1_SRC = false;

	accel_HPMode accelHPMode = A_HPM_NORMAL_RESET; //.High-pass filter mode selection for acceleration data. Default value: 00
	bool xmFilterDS = false; //Filtered acceleration data selection. Default value: 0 (0: internal filter bypassed; 1: data from internal filter sent to output register and FIFO)
	bool magLowPower = false; //  Magnetic data low-power mode. Default value: 0. If this bit is ‘1’ the MODR is set to 3.125 Hz independently from the MODR settings. 
    mag_sensormode magSensorMode = M_S_POWERDOWN;


	///////////////////
	// I2C Functions //
	///////////////////
	// initI2C() -- Initialize the I2C hardware.
	// This function will setup all I2C pins and related hardware.
	static void initI2C();
	
	// I2CwriteByte() -- Write a byte out of I2C to a register in the device
	// Input:
	//	- address = The 7-bit I2C address of the slave device.
	//	- subAddress = The register to be written to.
	//	- data = Byte to be written to the register.
	static void I2CwriteByte(uint8_t address, uint8_t subAddress, uint8_t data);
	
	// I2CreadByte() -- Read a single byte from a register over I2C.
	// Input:
	//	- address = The 7-bit I2C address of the slave device.
	//	- subAddress = The register to be read from.
	// Output:
	//	- The byte read from the requested address.
	static uint8_t I2CreadByte(uint8_t address, uint8_t subAddress);
	
	// I2CreadBytes() -- Read a series of bytes, starting at a register via SPI
	// Input:
	//	- address = The 7-bit I2C address of the slave device.
	//	- subAddress = The register to begin reading.
	// 	- * dest = Pointer to an array where we'll store the readings.
	//	- count = Number of registers to be read.
	// Output: No value is returned by the function, but the registers read are
	// 		all stored in the *dest array given.
	static void I2CreadBytes(uint8_t address, uint8_t subAddress, uint8_t * dest, uint8_t count);

};

#endif