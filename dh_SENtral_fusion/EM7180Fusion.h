#ifndef DH_EM7180FUSION_H__
#define DH_EM7180FUSION_H__
#include "ControlledFusion.h"
#include <i2c_t3.h>

// See MS5637-02BA03 Low Voltage Barometric Pressure Sensor Data Sheet
#define MS5637_RESET      0x1E
#define MS5637_CONVERT_D1 0x40
#define MS5637_CONVERT_D2 0x50
#define MS5637_ADC_READ   0x00

// See also MPU-9250 Register Map and Descriptions, Revision 4.0, RM-MPU-9250A-00, Rev. 1.4, 9/9/2013 for registers not listed in 
// above document; the MPU9250 and MPU9150 are virtually identical but the latter has a different register map
//
//Magnetometer Registers
#define AK8963_ADDRESS   0x0C
#define WHO_AM_I_AK8963  0x00 // should return 0x48
#define INFO             0x01
#define AK8963_ST1       0x02  // data ready status bit 0
#define AK8963_XOUT_L   0x03  // data
#define AK8963_XOUT_H  0x04
#define AK8963_YOUT_L  0x05
#define AK8963_YOUT_H  0x06
#define AK8963_ZOUT_L  0x07
#define AK8963_ZOUT_H  0x08
#define AK8963_ST2       0x09  // Data overflow bit 3 and data read error status bit 2
#define AK8963_CNTL      0x0A  // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
#define AK8963_ASTC      0x0C  // Self test control
#define AK8963_I2CDIS    0x0F  // I2C disable
#define AK8963_ASAX      0x10  // Fuse ROM x-axis sensitivity adjustment value
#define AK8963_ASAY      0x11  // Fuse ROM y-axis sensitivity adjustment value
#define AK8963_ASAZ      0x12  // Fuse ROM z-axis sensitivity adjustment value

#define SELF_TEST_X_GYRO 0x00                  
#define SELF_TEST_Y_GYRO 0x01                                                                          
#define SELF_TEST_Z_GYRO 0x02
#define SELF_TEST_X_ACCEL 0x0D
#define SELF_TEST_Y_ACCEL 0x0E    
#define SELF_TEST_Z_ACCEL 0x0F
#define SELF_TEST_A      0x10

#define XG_OFFSET_H      0x13  // User-defined trim values for gyroscope
#define XG_OFFSET_L      0x14
#define YG_OFFSET_H      0x15
#define YG_OFFSET_L      0x16
#define ZG_OFFSET_H      0x17
#define ZG_OFFSET_L      0x18
#define SMPLRT_DIV       0x19
#define CONFIG           0x1A
#define GYRO_CONFIG      0x1B
#define ACCEL_CONFIG     0x1C
#define ACCEL_CONFIG2    0x1D
#define LP_ACCEL_ODR     0x1E   
#define WOM_THR          0x1F   

#define MOT_DUR          0x20  // Duration counter threshold for motion interrupt generation, 1 kHz rate, LSB = 1 ms
#define ZMOT_THR         0x21  // Zero-motion detection threshold bits [7:0]
#define ZRMOT_DUR        0x22  // Duration counter threshold for zero motion interrupt generation, 16 Hz rate, LSB = 64 ms

#define FIFO_EN          0x23
#define I2C_MST_CTRL     0x24   
#define I2C_SLV0_ADDR    0x25
#define I2C_SLV0_REG     0x26
#define I2C_SLV0_CTRL    0x27
#define I2C_SLV1_ADDR    0x28
#define I2C_SLV1_REG     0x29
#define I2C_SLV1_CTRL    0x2A
#define I2C_SLV2_ADDR    0x2B
#define I2C_SLV2_REG     0x2C
#define I2C_SLV2_CTRL    0x2D
#define I2C_SLV3_ADDR    0x2E
#define I2C_SLV3_REG     0x2F
#define I2C_SLV3_CTRL    0x30
#define I2C_SLV4_ADDR    0x31
#define I2C_SLV4_REG     0x32
#define I2C_SLV4_DO      0x33
#define I2C_SLV4_CTRL    0x34
#define I2C_SLV4_DI      0x35
#define I2C_MST_STATUS   0x36
#define INT_PIN_CFG      0x37
#define INT_ENABLE       0x38
#define DMP_INT_STATUS   0x39  // Check DMP interrupt
#define INT_STATUS       0x3A
#define ACCEL_XOUT_H     0x3B
#define ACCEL_XOUT_L     0x3C
#define ACCEL_YOUT_H     0x3D
#define ACCEL_YOUT_L     0x3E
#define ACCEL_ZOUT_H     0x3F
#define ACCEL_ZOUT_L     0x40
#define TEMP_OUT_H       0x41
#define TEMP_OUT_L       0x42
#define GYRO_XOUT_H      0x43
#define GYRO_XOUT_L      0x44
#define GYRO_YOUT_H      0x45
#define GYRO_YOUT_L      0x46
#define GYRO_ZOUT_H      0x47
#define GYRO_ZOUT_L      0x48
#define EXT_SENS_DATA_00 0x49
#define EXT_SENS_DATA_01 0x4A
#define EXT_SENS_DATA_02 0x4B
#define EXT_SENS_DATA_03 0x4C
#define EXT_SENS_DATA_04 0x4D
#define EXT_SENS_DATA_05 0x4E
#define EXT_SENS_DATA_06 0x4F
#define EXT_SENS_DATA_07 0x50
#define EXT_SENS_DATA_08 0x51
#define EXT_SENS_DATA_09 0x52
#define EXT_SENS_DATA_10 0x53
#define EXT_SENS_DATA_11 0x54
#define EXT_SENS_DATA_12 0x55
#define EXT_SENS_DATA_13 0x56
#define EXT_SENS_DATA_14 0x57
#define EXT_SENS_DATA_15 0x58
#define EXT_SENS_DATA_16 0x59
#define EXT_SENS_DATA_17 0x5A
#define EXT_SENS_DATA_18 0x5B
#define EXT_SENS_DATA_19 0x5C
#define EXT_SENS_DATA_20 0x5D
#define EXT_SENS_DATA_21 0x5E
#define EXT_SENS_DATA_22 0x5F
#define EXT_SENS_DATA_23 0x60
#define MOT_DETECT_STATUS 0x61
#define I2C_SLV0_DO      0x63
#define I2C_SLV1_DO      0x64
#define I2C_SLV2_DO      0x65
#define I2C_SLV3_DO      0x66
#define I2C_MST_DELAY_CTRL 0x67
#define SIGNAL_PATH_RESET  0x68
#define MOT_DETECT_CTRL  0x69
#define USER_CTRL        0x6A  // Bit 7 enable DMP, bit 3 reset DMP
#define PWR_MGMT_1       0x6B // Device defaults to the SLEEP mode
#define PWR_MGMT_2       0x6C
#define DMP_BANK         0x6D  // Activates a specific bank in the DMP
#define DMP_RW_PNT       0x6E  // Set read/write pointer to a specific start address in specified DMP bank
#define DMP_REG          0x6F  // Register in DMP from which to read or to which to write
#define DMP_REG_1        0x70
#define DMP_REG_2        0x71 
#define FIFO_COUNTH      0x72
#define FIFO_COUNTL      0x73
#define FIFO_R_W         0x74
#define WHO_AM_I_MPU9250 0x75 // Should return 0x71
#define XA_OFFSET_H      0x77
#define XA_OFFSET_L      0x78
#define YA_OFFSET_H      0x7A
#define YA_OFFSET_L      0x7B
#define ZA_OFFSET_H      0x7D
#define ZA_OFFSET_L      0x7E

// EM7180 SENtral register map
// see http://www.emdeveloper.com/downloads/7180/EMSentral_EM7180_Register_Map_v1_3.pdf
//
#define EM7180_QX                 0x00  // this is a 32-bit normalized floating point number read from registers 0x00-03
#define EM7180_QY                 0x04  // this is a 32-bit normalized floating point number read from registers 0x04-07
#define EM7180_QZ                 0x08  // this is a 32-bit normalized floating point number read from registers 0x08-0B
#define EM7180_QW                 0x0C  // this is a 32-bit normalized floating point number read from registers 0x0C-0F
#define EM7180_HED				  0x00  // this is a 32-bit normalized floating point number read from registers 0x00-03
#define EM7180_QTIME              0x10  // this is a 16-bit unsigned integer read from registers 0x10-11
#define EM7180_MX                 0x12  // int16_t from registers 0x12-13
#define EM7180_MY                 0x14  // int16_t from registers 0x14-15
#define EM7180_MZ                 0x16  // int16_t from registers 0x16-17
#define EM7180_MTIME              0x18  // uint16_t from registers 0x18-19
#define EM7180_AX                 0x1A  // int16_t from registers 0x1A-1B
#define EM7180_AY                 0x1C  // int16_t from registers 0x1C-1D
#define EM7180_AZ                 0x1E  // int16_t from registers 0x1E-1F
#define EM7180_ATIME              0x20  // uint16_t from registers 0x20-21
#define EM7180_GX                 0x22  // int16_t from registers 0x22-23
#define EM7180_GY                 0x24  // int16_t from registers 0x24-25
#define EM7180_GZ                 0x26  // int16_t from registers 0x26-27
#define EM7180_GTIME              0x28  // uint16_t from registers 0x28-29
#define EM7180_Baro               0x2A  // start of two-byte MS5637 pressure data, 16-bit signed interger
#define EM7180_BaroTIME           0x2C  // start of two-byte MS5637 pressure timestamp, 16-bit unsigned
#define EM7180_Temp               0x2E  // start of two-byte MS5637 temperature data, 16-bit signed interger
#define EM7180_TempTIME           0x30  // start of two-byte MS5637 temperature timestamp, 16-bit unsigned
#define EM7180_QRateDivisor       0x32  // uint8_t 
#define EM7180_EnableEvents       0x33
#define EM7180_HostControl        0x34
#define EM7180_EventStatus        0x35
#define EM7180_SensorStatus       0x36
#define EM7180_SentralStatus      0x37
#define EM7180_AlgorithmStatus    0x38
#define EM7180_FeatureFlags       0x39
#define EM7180_ParamAcknowledge   0x3A
#define EM7180_SavedParamByte0    0x3B
#define EM7180_SavedParamByte1    0x3C
#define EM7180_SavedParamByte2    0x3D
#define EM7180_SavedParamByte3    0x3E
#define EM7180_ActualMagRate      0x45
#define EM7180_ActualAccelRate    0x46
#define EM7180_ActualGyroRate     0x47
#define EM7180_ActualBaroRate     0x48
#define EM7180_ActualTempRate     0x49
#define EM7180_ErrorRegister      0x50
#define EM7180_AlgorithmControl   0x54
#define EM7180_MagRate            0x55
#define EM7180_AccelRate          0x56
#define EM7180_GyroRate           0x57
#define EM7180_BaroRate           0x58
#define EM7180_TempRate           0x59
#define EM7180_LoadParamByte0     0x60
#define EM7180_LoadParamByte1     0x61
#define EM7180_LoadParamByte2     0x62
#define EM7180_LoadParamByte3     0x63
#define EM7180_ParamRequest       0x64
#define EM7180_ROMVersion1        0x70
#define EM7180_ROMVersion2        0x71
#define EM7180_RAMVersion1        0x72
#define EM7180_RAMVersion2        0x73
#define EM7180_ProductID          0x90
#define EM7180_RevisionID         0x91
#define EM7180_RunStatus          0x92
#define EM7180_UploadAddress      0x94 // uint16_t registers 0x94 (MSB)-5(LSB)
#define EM7180_UploadData         0x96  
#define EM7180_CRCHost            0x97  // uint32_t from registers 0x97-9A
#define EM7180_ResetRequest       0x9B   
#define EM7180_PassThruStatus     0x9E   
#define EM7180_PassThruControl    0xA0
#define EM7180_ACC_LPF_BW         0x5B  //Register GP36
#define EM7180_GYRO_LPF_BW        0x5C  //Register GP37
#define EM7180_BARO_LPF_BW        0x5D  //Register GP38

#define EM7180_ADDRESS      0x28   // Address of the EM7180 SENtral sensor hub
#define M24512DFM_DATA_ADDRESS   0x50   // Address of the 500 page M24512DFM EEPROM data buffer, 1024 bits (128 8-bit bytes) per page
#define M24512DFM_IDPAGE_ADDRESS 0x58   // Address of the single M24512DFM lockable EEPROM ID page
#define MPU9250_ADDRESS 0x68  // Device address when ADO = 0
#define AK8963_ADDRESS 0x0C   //  Address of magnetometer
#define MS5637_ADDRESS 0x76   // Address of altimeter

#define PIFACTOR 114.5915590261

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

class EM7180Fusion:public ControlledFusion{
public:
	EM7180Fusion(){

	}
	void begin(){
		uint8_t PID = readByte(EM7180_ADDRESS, EM7180_ProductID);
		if (PID != 0x80){
			Serial.print("Failed to read EM7180 product ID (0x80).. got:");
			Serial.println(PID, HEX);
			errorState = 1;
			return;
		}
		Serial.println("PID found");
		Serial.flush();
		delay(1000);
		  // Enter EM7180 initialized state
	  writeByte(EM7180_ADDRESS, EM7180_HostControl, 0x00); // set SENtral in initialized state to configure registers
	  writeByte(EM7180_ADDRESS, EM7180_PassThruControl, 0x00); // make sure pass through mode is off
	  writeByte(EM7180_ADDRESS, EM7180_HostControl, 0x01); // Force initialize
	  writeByte(EM7180_ADDRESS, EM7180_HostControl, 0x00); // set SENtral in initialized state to configure registers
	  
	  //Setup LPF bandwidth (BEFORE setting ODR's)
	  writeByte(EM7180_ADDRESS, EM7180_ACC_LPF_BW, 0x03); // 41Hz
	  writeByte(EM7180_ADDRESS, EM7180_GYRO_LPF_BW, 0x03); // 41Hz
	 // Set accel/gyro/mage desired ODR rates
	  writeByte(EM7180_ADDRESS, EM7180_QRateDivisor, 0x02); // 100 Hz
	  writeByte(EM7180_ADDRESS, EM7180_MagRate, 0x64); // 100 Hz
	  writeByte(EM7180_ADDRESS, EM7180_AccelRate, 0x14); // 200/10 Hz
	  writeByte(EM7180_ADDRESS, EM7180_GyroRate, 0x14); // 200/10 Hz
	  writeByte(EM7180_ADDRESS, EM7180_BaroRate, 0x80 | 0x32);  // set enable bit and set Baro rate to 25 Hz
	  // writeByte(EM7180_ADDRESS, EM7180_TempRate, 0x19);  // set enable bit and set rate to 25 Hz
	  // Configure operating mode
	  writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0); 
	    // Enable interrupt to host upon certain events
  // choose host interrupts when any sensor updated (0x40), new gyro data (0x20), new accel data (0x10),
  // new mag data (0x08), quaternions updated (0x04), an error occurs (0x02), or the SENtral needs to be reset(0x01)
	  writeByte(EM7180_ADDRESS, EM7180_EnableEvents, 0x03); // Errors and reset needed
		
	   writeByte(EM7180_ADDRESS, EM7180_HostControl, 0x01); // set SENtral in normal run mode

			// EM7180 parameter adjustments
		 #ifdef DEBUG
		  Serial.println("Beginning Parameter Adjustments");
		 #endif
		  // Read sensor default FS values from parameter space
		  writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x4A); // Request to read parameter 74
		  writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x80); // Request parameter transfer process
		  byte param_xfer = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
		  while(!(param_xfer==0x4A)) {
		    param_xfer = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
		  }

		  uint8_t param[4];                         // used for param transfer
		  param[0] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte0);
		  param[1] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte1);
		  param[2] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte2);
		  param[3] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte3);

		  uint16_t EM7180_mag_fs, EM7180_acc_fs, EM7180_gyro_fs; // EM7180 sensor full scale ranges

		  EM7180_mag_fs = ((int16_t)(param[1]<<8) | param[0]);
		  EM7180_acc_fs = ((int16_t)(param[3]<<8) | param[2]);
		  
		  writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x4B); // Request to read  parameter 75
		  param_xfer = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
		  while(!(param_xfer==0x4B)) {
		    param_xfer = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
		  }
		  param[0] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte0);
		  param[1] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte1);
		  param[2] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte2);
		  param[3] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte3);
		  EM7180_gyro_fs = ((int16_t)(param[1]<<8) | param[0]);
		
		  writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x00); //End parameter transfer
		  writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0); //Set the HPROutput to 1 
		  
		  //Disable stillness mode
		  EM7180_set_integer_param (0x49, 0x00);
		  
		  //Write desired sensor full scale ranges to the EM7180
		  EM7180_set_mag_acc_FS (0x3E8, 0x08); // 1000 uT, 8 g
		  EM7180_set_gyro_FS (0x7D0); // 2000 dps
		  
		  // Read sensor new FS values from parameter space
		  writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x4A); // Request to read  parameter 74
		  writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x80); // Request parameter transfer process
		  param_xfer = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
		  while(!(param_xfer==0x4A)) {
		    param_xfer = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
		  }
		  param[0] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte0);
		  param[1] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte1);
		  param[2] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte2);
		  param[3] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte3);
		  EM7180_mag_fs = ((int16_t)(param[1]<<8) | param[0]);
		  EM7180_acc_fs = ((int16_t)(param[3]<<8) | param[2]);
		  writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x4B); // Request to read  parameter 75
		  param_xfer = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
		  while(!(param_xfer==0x4B)) {
		    param_xfer = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
		  }
		  param[0] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte0);
		  param[1] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte1);
		  param[2] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte2);
		  param[3] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte3);
		  EM7180_gyro_fs = ((int16_t)(param[1]<<8) | param[0]);
		 
		  writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x00); //End parameter transfer
		  writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0); 

		  #ifdef DEBUG
		   Serial.print("Gyroscope New Full Scale Range: +/-"); Serial.print(EM7180_gyro_fs); Serial.println("dps");
 		   Serial.print("Magnetometer New Full Scale Range: +/-"); Serial.print(EM7180_mag_fs); Serial.println("uT");
		   Serial.print("Accelerometer New Full Scale Range: +/-"); Serial.print(EM7180_acc_fs); Serial.println("g");
		  #endif
		  
		// Read EM7180 status
		uint8_t runStatus = readByte(EM7180_ADDRESS, EM7180_RunStatus);
		if(runStatus & 0x01) Serial.println(" EM7180 run status = normal mode");
		uint8_t algoStatus = readByte(EM7180_ADDRESS, EM7180_AlgorithmStatus);
		if(algoStatus & 0x01) Serial.println(" EM7180 standby status");
		if(algoStatus & 0x02) Serial.println(" EM7180 algorithm slow");
		if(algoStatus & 0x04) Serial.println(" EM7180 in stillness mode");
		if(algoStatus & 0x08) Serial.println(" EM7180 mag calibration completed");
		if(algoStatus & 0x10) Serial.println(" EM7180 magnetic anomaly detected");
		if(algoStatus & 0x20) Serial.println(" EM7180 unreliable sensor data");
		uint8_t passthruStatus = readByte(EM7180_ADDRESS, EM7180_PassThruStatus);
		if(passthruStatus & 0x01) Serial.print(" EM7180 in passthru mode!");
		uint8_t eventStatus = readByte(EM7180_ADDRESS, EM7180_EventStatus);
		if(eventStatus & 0x01){ Serial.println(" EM7180 CPU reset needed?");}
		if(eventStatus & 0x02){

		 Serial.println(" EM7180 Error");
		 return;
		}


		  
		  // Check sensor status
		  uint8_t sensorStatus = readByte(EM7180_ADDRESS, EM7180_SensorStatus);
		  Serial.print(" EM7180 sensor status = "); Serial.println(sensorStatus);
		  if(sensorStatus & 0x01) Serial.print("Magnetometer not acknowledging!");
		  if(sensorStatus & 0x02) Serial.print("Accelerometer not acknowledging!");
		  if(sensorStatus & 0x04) Serial.print("Gyro not acknowledging!");
		  if(sensorStatus & 0x10) Serial.print("Magnetometer ID not recognized!");
		  if(sensorStatus & 0x20) Serial.print("Accelerometer ID not recognized!");
		  if(sensorStatus & 0x40) Serial.print("Gyro ID not recognized!");
		  #ifdef DEBUG
		  Serial.print("Actual MagRate = "); Serial.print(readByte(EM7180_ADDRESS, EM7180_ActualMagRate)); Serial.println(" Hz"); 
		  Serial.print("Actual AccelRate = "); Serial.print(10*readByte(EM7180_ADDRESS, EM7180_ActualAccelRate)); Serial.println(" Hz"); 
		  Serial.print("Actual GyroRate = "); Serial.print(10*readByte(EM7180_ADDRESS, EM7180_ActualGyroRate)); Serial.println(" Hz"); 
		  Serial.print("Actual BaroRate = "); Serial.print(readByte(EM7180_ADDRESS, EM7180_ActualBaroRate)); Serial.println(" Hz"); 
		  Serial.print("Actual TempRate = "); Serial.print(readByte(EM7180_ADDRESS, EM7180_ActualTempRate)); Serial.println(" Hz"); 
		  #endif
		  started = true;
	}
protected:
	void updateHeading(){
		updateQuat();
	}

	void updateQuat(){
	if (quatElapsed>=QUAT_CACHETIME && started){
				quatElapsed = 0;
				uint8_t rawData[16];
				readBytes(EM7180_ADDRESS, EM7180_HED, 16, &rawData[0]);  
				uint32_reg_to_float(&rawData[0],quatF[0]);
				uint32_reg_to_float(&rawData[4],quatF[1]);
				uint32_reg_to_float(&rawData[8],quatF[2]);
				uint32_reg_to_float(&rawData[12],quatF[3]);

				headingF[0]   = atan2(2.0f * (quatF[0] * quatF[1] + quatF[3] * quatF[2]), quatF[3] * quatF[3] + quatF[0] * quatF[0] - quatF[1] * quatF[1] - quatF[2] * quatF[2]);   
    			headingF[1] = -asin(2.0f * (quatF[0] * quatF[2] - quatF[3] * quatF[1]));
    			headingF[2]  = atan2(2.0f * (quatF[3] * quatF[0] + quatF[1] * quatF[2]), quatF[3] * quatF[3] - quatF[0] * quatF[0] - quatF[1] * quatF[1] + quatF[2] * quatF[2]);
    			headingF[1] *= 180.0f / PI;
    			headingF[0]   *= 180.0f / PI; 
    			headingF[0]   -= 3; 
    			if(headingF[0] < 0) headingF[0]   += 360.0f ; // Ensure yaw stays between 0 and 360
    			headingF[2]  *= 180.0f / PI;
	
		}
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

	void uint32_reg_to_float  (uint8_t *buf, float &fltVal)
	{
	  u.ui32 =     (((uint32_t)buf[0]) +
	               (((uint32_t)buf[1]) <<  8) +
	               (((uint32_t)buf[2]) << 16) +
	               (((uint32_t)buf[3]) << 24));
	  fltVal = u.f;
	}

	void float_to_bytes (float param_val, uint8_t *buf) {
	  union {
	    float f;
	    uint8_t comp[sizeof(float)];
	  } u;
	  u.f = param_val;
	  for (uint8_t i=0; i < sizeof(float); i++) {
	    buf[i] = u.comp[i];
	  }
	  //Convert to LITTLE ENDIAN
	  for (uint8_t i=0; i < sizeof(float); i++) {
	    buf[i] = buf[(sizeof(float)-1) - i];
	  }
	}

	void EM7180_set_float_param (uint8_t param, float param_val) {
	  uint8_t bytes[4], STAT;
	  float_to_bytes (param_val, &bytes[0]);
	  param = param | 0x80; //Parameter is the decimal value with the MSB set high to indicate a paramter write processs
	  writeByte(EM7180_ADDRESS, EM7180_LoadParamByte0, bytes[0]); //Param LSB
	  writeByte(EM7180_ADDRESS, EM7180_LoadParamByte1, bytes[1]);
	  writeByte(EM7180_ADDRESS, EM7180_LoadParamByte2, bytes[2]);
	  writeByte(EM7180_ADDRESS, EM7180_LoadParamByte3, bytes[3]); //Param MSB
	  writeByte(EM7180_ADDRESS, EM7180_ParamRequest, param);
	  writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x80); //Request parameter transfer procedure
	  STAT = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge); //Check the parameter acknowledge register and loop until the result matches parameter request byte
	  while(!(STAT==param)) {
	    STAT = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
	  }
	  writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x00); //Parameter request = 0 to end parameter transfer process
	  writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x00); // Re-start algorithm
	}

	void EM7180_set_integer_param (uint8_t param, uint32_t param_val) {
	  uint8_t bytes[4], STAT;
	  bytes[0] = param_val & (0xFF);
	  bytes[1] = (param_val >> 8) & (0xFF);
	  bytes[2] = (param_val >> 16) & (0xFF);
	  bytes[3] = (param_val >> 24) & (0xFF);
	  param = param | 0x80; //Parameter is the decimal value with the MSB set high to indicate a paramter write processs
	  writeByte(EM7180_ADDRESS, EM7180_LoadParamByte0, bytes[0]); //Param LSB
	  writeByte(EM7180_ADDRESS, EM7180_LoadParamByte1, bytes[1]);
	  writeByte(EM7180_ADDRESS, EM7180_LoadParamByte2, bytes[2]);
	  writeByte(EM7180_ADDRESS, EM7180_LoadParamByte3, bytes[3]); //Param MSB
	  writeByte(EM7180_ADDRESS, EM7180_ParamRequest, param);
	  writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x80); //Request parameter transfer procedure
	  STAT = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge); //Check the parameter acknowledge register and loop until the result matches parameter request byte
	  while(!(STAT==param)) {
	    STAT = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
	  }
	  writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x00); //Parameter request = 0 to end parameter transfer process
	  writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x00); // Re-start algorithm
	}

	void EM7180_set_gyro_FS (uint16_t gyro_fs) {
	  uint8_t bytes[4], STAT;
	  bytes[0] = gyro_fs & (0xFF);
	  bytes[1] = (gyro_fs >> 8) & (0xFF);
	  bytes[2] = 0x00;
	  bytes[3] = 0x00;
	  writeByte(EM7180_ADDRESS, EM7180_LoadParamByte0, bytes[0]); //Gyro LSB
	  writeByte(EM7180_ADDRESS, EM7180_LoadParamByte1, bytes[1]); //Gyro MSB
	  writeByte(EM7180_ADDRESS, EM7180_LoadParamByte2, bytes[2]); //Unused
	  writeByte(EM7180_ADDRESS, EM7180_LoadParamByte3, bytes[3]); //Unused
	  writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0xCB); //Parameter 75; 0xCB is 75 decimal with the MSB set high to indicate a paramter write processs
	  writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x80); //Request parameter transfer procedure
	  STAT = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge); //Check the parameter acknowledge register and loop until the result matches parameter request byte
	  while(!(STAT==0xCB)) {
	    STAT = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
	  }
	  writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x00); //Parameter request = 0 to end parameter transfer process
	  writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x00); // Re-start algorithm
	}

	void EM7180_set_mag_acc_FS  (uint16_t mag_fs, uint16_t acc_fs) {
	  uint8_t bytes[4], STAT;
	  bytes[0] = mag_fs & (0xFF);
	  bytes[1] = (mag_fs >> 8) & (0xFF);
	  bytes[2] = acc_fs & (0xFF);
	  bytes[3] = (acc_fs >> 8) & (0xFF);
	  writeByte(EM7180_ADDRESS, EM7180_LoadParamByte0, bytes[0]); //Mag LSB
	  writeByte(EM7180_ADDRESS, EM7180_LoadParamByte1, bytes[1]); //Mag MSB
	  writeByte(EM7180_ADDRESS, EM7180_LoadParamByte2, bytes[2]); //Acc LSB
	  writeByte(EM7180_ADDRESS, EM7180_LoadParamByte3, bytes[3]); //Acc MSB
	  writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0xCA); //Parameter 74; 0xCA is 74 decimal with the MSB set high to indicate a paramter write processs
	  writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x80); //Request parameter transfer procedure
	  STAT = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge); //Check the parameter acknowledge register and loop until the result matches parameter request byte
	  while(!(STAT==0xCA)) {
	    STAT = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
	  }
	  writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x00); //Parameter request = 0 to end parameter transfer process
	  writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x00); // Re-start algorithm
	}

	union {
	    uint32_t ui32;
	    float f;
	 } u;


};

#endif