#ifndef DH_CONTROLLEDBN055_H__
#define DH_CONTROLLEDBN055_H__
#include "dh_controller.h"
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
#define BNO055_ADDRESS 0x28   //  Device address of BNO055 when ADO = 1
#define MS5637_ADDRESS   0x76   //  Address of MS5637 altimeter
#else
#define BNO055_ADDRESS 0x28   //  Device address of BNO055 when ADO = 0
#define MS5637_ADDRESS   0x76   //  Address of MS5637 altimeter
#endif  

#define SerialDebug true      // set to true to get Serial output for debugging



#define ADC_256  0x00 // define pressure and temperature conversion rates
#define ADC_512  0x02
#define ADC_1024 0x04
#define ADC_2048 0x06
#define ADC_4096 0x08
#define ADC_8192 0x0A
#define ADC_D1   0x40
#define ADC_D2   0x50


class ControlledBN055: public Controller::Controlled{
public:

	void begin(void){
		Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400, I2C_OP_MODE_DMA);
		 // Check software revision ID
		byte id = readByte(BNO055_ADDRESS, BNO055_CHIP_ID);
		Serial.print("BNO055 SW Revision ID: "); Serial.println(id, HEX);
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
	    }

	MS5637Reset();
  delay(100);
  Serial.println("MS5637 pressure sensor reset...");
  // Read PROM data from MS5637 pressure sensor
  MS5637PromRead(Pcal);
  unsigned char refCRC = Pcal[0] >> 12;
  
  nCRC = MS5637checkCRC(Pcal);  //calculate checksum to ensure integrity of MS5637 calibration data
  if (refCRC!=nCRC){
  	Serial.println("MS5637 pressure sensor failed CRC check");
  }
     
 // Select page 1 to configure sensors
   writeByte(BNO055_ADDRESS, BNO055_PAGE_ID, 0x01);
   // Configure ACC
   writeByte(BNO055_ADDRESS, BNO055_ACC_CONFIG, APwrMode << 5 | Abw << 3 | Ascale );
   // Configure GYR
   writeByte(BNO055_ADDRESS, BNO055_GYRO_CONFIG_0, Gbw << 3 | Gscale );
   writeByte(BNO055_ADDRESS, BNO055_GYRO_CONFIG_1, GPwrMode);
   // Configure MAG
   writeByte(BNO055_ADDRESS, BNO055_MAG_CONFIG, MPwrMode << 4 | MOpMode << 2 | Modr );
   
   // Select page 0 to read sensors
   writeByte(BNO055_ADDRESS, BNO055_PAGE_ID, 0x00);

   //Select external crystal
   writeByte(BNO055_ADDRESS, BNO055_SYS_TRIGGER, 0x80);
   // Select BNO055 gyro temperature source 
   writeByte(BNO055_ADDRESS, BNO055_TEMP_SOURCE, 0x01 );

   // Select BNO055 sensor units (temperature in degrees C, rate in dps, accel in mg)
   writeByte(BNO055_ADDRESS, BNO055_UNIT_SEL, 0x01 );
   
   // Select BNO055 system power mode
   writeByte(BNO055_ADDRESS, BNO055_PWR_MODE, PWRMode );
 
   // Select BNO055 system operation mode
   writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, OPRMode );

     // Select page 0 to read sensors
   writeByte(BNO055_ADDRESS, BNO055_PAGE_ID, 0x00);

	}


	void execute(uint32_t time,uint32_t id,char* command, bool serializeOnComplete){
		switch (command[0]) {
		case 'Z':// ZERO
		{
			uint16_t pointer = 4;
			Serial.println(command);
			if (command[pointer] == 'A') {

				if (command[pointer + 1] == '2') {
					//write biases to accelerometer offset registers ad 16 LSB/dps
					writeByte(BNO055_ADDRESS, BNO055_ACC_OFFSET_X_LSB, (int16_t)accel_bias[0] & 0xFF);
					writeByte(BNO055_ADDRESS, BNO055_ACC_OFFSET_X_MSB, ((int16_t)accel_bias[0] >> 8) & 0xFF);
					writeByte(BNO055_ADDRESS, BNO055_ACC_OFFSET_Y_LSB, (int16_t)accel_bias[1] & 0xFF);
					writeByte(BNO055_ADDRESS, BNO055_ACC_OFFSET_Y_MSB, ((int16_t)accel_bias[1] >> 8) & 0xFF);
					writeByte(BNO055_ADDRESS, BNO055_ACC_OFFSET_Z_LSB, (int16_t)accel_bias[2] & 0xFF);
					writeByte(BNO055_ADDRESS, BNO055_ACC_OFFSET_Z_MSB, ((int16_t)accel_bias[2] >> 8) & 0xFF);
					Serial.println("Updated2!");
					// Select BNO055 system operation mode
					writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, ControlledBN055::OPRMode);
				}
				else {
					Serial.println("Updated1!");
					if (command[pointer + 1] == '\0') { // this assumes some other class has set the bias directly
						writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, CONFIGMODE);
						controller->schedule(id + 1, 25, 0, false, 1, Controller::newString("ZEROA2"), 'G', false);
					}
					else {
						pointer += 2;
						if (!Controller::parse_int16(accel_bias[0], pointer, command)) {
							return;
						}

						pointer++;
						if (!Controller::parse_int16(accel_bias[1], pointer, command)) {
							return;
						}
						pointer++;
						if (!Controller::parse_int16(accel_bias[2], pointer, command)) {
							return;
						}
						writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, CONFIGMODE);
						controller->schedule(id + 1, 25, 0, false, 1, Controller::newString("ZEROA2"), 'G', false);
					}
				}
			}
			else if (command[pointer] == 'G') {
				if (command[pointer + 1] == '2') {
					//write biases to gyro offset registers
					writeByte(BNO055_ADDRESS, BNO055_GYR_OFFSET_X_LSB, (int16_t)gyro_bias[0] & 0xFF);
					writeByte(BNO055_ADDRESS, BNO055_GYR_OFFSET_X_MSB, ((int16_t)gyro_bias[0] >> 8) & 0xFF);
					writeByte(BNO055_ADDRESS, BNO055_GYR_OFFSET_Y_LSB, (int16_t)gyro_bias[1] & 0xFF);
					writeByte(BNO055_ADDRESS, BNO055_GYR_OFFSET_Y_MSB, ((int16_t)gyro_bias[1] >> 8) & 0xFF);
					writeByte(BNO055_ADDRESS, BNO055_GYR_OFFSET_Z_LSB, (int16_t)gyro_bias[2] & 0xFF);
					writeByte(BNO055_ADDRESS, BNO055_GYR_OFFSET_Z_MSB, ((int16_t)gyro_bias[2] >> 8) & 0xFF);

					// Select BNO055 system operation mode
					writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, ControlledBN055::OPRMode);
				}
				else {
					pointer += 2;
					if (!Controller::parse_int16(gyro_bias[0], pointer, command)) {
						return;
					}
					pointer++;
					if (!Controller::parse_int16(gyro_bias[1], pointer, command)) {
						return;
					}
					pointer++;
					if (!Controller::parse_int16(gyro_bias[2], pointer, command)) {
						return;
					}
					writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, CONFIGMODE);
					controller->schedule(id, 25, 0, false, 1, Controller::newString("ZEROG2"), 'G', false);
				}
			}

		}
		break;
		case 'C':// CALMAG
			magCalBNO055();
			break;
		case 'V':// ALL Values
		{
			if (command[1] == 'A') {
				if (!requestBytes(BNO055_ADDRESS, BNO055_GYR_DATA_X_LSB, 34)) {
					controller->schedule(id , 1,0,false,1, Controller::newString("VAL"),'G',false);
				} else {
					controller->schedule(id , 2, 0, false, 1, Controller::newString("V2"), 'G', false);
				}
			}
			else if (command[1] == '2') {
				uint8_t buffer[34];
				if (!getBytes(34, &buffer[0])) {
					controller->schedule(id, 1, 0, false, 1, Controller::newString("V2"), 'G', false);
					break;
				}
			
				uint8_t * rawData = &buffer[0];

				raw_rotVel[0] = ((int16_t)rawData[1] << 8) | rawData[0];       // Turn the MSB and LSB into a signed 16-bit value
				raw_rotVel[1] = ((int16_t)rawData[3] << 8) | rawData[2];
				raw_rotVel[2] = ((int16_t)rawData[5] << 8) | rawData[4];

				rawData += 6;

				heading[0] = ((double)(raw_heading[0] = (((int16_t)rawData[1] << 8) | rawData[0]))) / 16.f;       // Turn the MSB and LSB into a signed 16-bit value
				heading[1] = ((double)(raw_heading[1] = (((int16_t)rawData[3] << 8) | rawData[2]))) / 16.f;
				heading[2] = ((double)(raw_heading[2] = (((int16_t)rawData[5] << 8) | rawData[4]))) / 16.f;

				rawData += 6;

				raw_quat[0] = ((int16_t)rawData[1] << 8) | rawData[0];       // Turn the MSB and LSB into a signed 16-bit value
				raw_quat[1] = ((int16_t)rawData[3] << 8) | rawData[2];
				raw_quat[2] = ((int16_t)rawData[5] << 8) | rawData[4];
				raw_quat[3] = ((int16_t)rawData[7] << 8) | rawData[6];

				rawData += 8;

				raw_linAccel[0] = ((int16_t)rawData[1] << 8) | rawData[0];       // Turn the MSB and LSB into a signed 16-bit value
				raw_linAccel[1] = ((int16_t)rawData[3] << 8) | rawData[2];
				raw_linAccel[2] = ((int16_t)rawData[5] << 8) | rawData[4];

				rawData += 6;
				raw_grav[0] = ((int16_t)rawData[1] << 8) | rawData[0];       // Turn the MSB and LSB into a signed 16-bit value
				raw_grav[1] = ((int16_t)rawData[3] << 8) | rawData[2];
				raw_grav[2] = ((int16_t)rawData[5] << 8) | rawData[4];

				rawData++;

				raw_temp = rawData[0];

				rawData++;

				statusSys = (rawData[0] >> 6) & 0b00000011;
				statusGyr = (rawData[0] >> 4) & 0b00000011;
				statusAcc = (rawData[0] >> 2) & 0b00000011;
				statusMag = (rawData[0]) & 0b00000011;
				Serial.println(heading[0]);

			}



			
			break;
		}
		case 'H':// HEADING (euler)
		{
			Serial.print("Heading:");
			// if (liaReadMode == 0 && !i2cBusy){
			// 	requestBytes(BNO055_ADDRESS, BNO055_EUL_HEADING_LSB);
			// 	i2cBusy = true;
			// 	liaReadMode = 1;
			// }
			// if (liaReadMode == 1){
			// 	if (checkBytes(BNO055_ADDRESS , 6)){
			// 		liaReadMode = 2;
			// 	}
			// }
			// if (liaReadMode == 2){
			// 	uint8_t rawData[6];  // x/y/z gyro register data stored here
			// 	if (getBytes( 6, &rawData[0])){
			// 	//readBytes(BNO055_ADDRESS, BNO055_QUA_DATA_W_LSB, 8, &rawData[0]);  // Read the six raw data registers sequentially into data array
			// 		heading[0] =((double) (((int16_t)rawData[1] << 8) | rawData[0]))/16.f ;       // Turn the MSB and LSB into a signed 16-bit value
	// 				heading[1] =((double) (((int16_t)rawData[3] << 8) | rawData[2]))/16.f ;  
	// 				heading[2] =((double) (((int16_t)rawData[5] << 8) | rawData[4]))/16.f ;
			// 		liaReadMode = 0;
			// 	  	i2cBusy = false;
			// 	}
			// }
			// if (i2cBusy){
			// 	controller->schedule(id+1,1,0,false,1,Controller::newString("H"),'G',serializeOnComplete);
			// }
			uint8_t rawData[6];  // x/y/z gyro register data stored here
			readBytes(BNO055_ADDRESS, BNO055_EUL_HEADING_LSB, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
			heading[0] = ((double)(raw_heading[0] = (((int16_t)rawData[1] << 8) | rawData[0]))) / 16.f;       // Turn the MSB and LSB into a signed 16-bit value
			heading[1] = ((double)(raw_heading[1] = (((int16_t)rawData[3] << 8) | rawData[2]))) / 16.f;
			heading[2] = ((double)(raw_heading[2] = (((int16_t)rawData[5] << 8) | rawData[4]))) / 16.f;
			Serial.println(raw_heading[0]);
			break;
		}

		
		break;
		case 'A':// ALTITUDE
		{
			uint32_t D1 = 0, D2 = 0;
			double dT, OFFSET, SENS, T2 = 0, OFFSET2, SENS2;  // First order and second order corrections for raw S5637 temperature and pressure data

			D1 = MS5637Read(ADC_D1, OSR);  // get raw pressure value
			D2 = MS5637Read(ADC_D2, OSR);  // get raw temperature value

			dT = D2 - Pcal[5] * pow(2, 8);    // calculate temperature difference from reference
			OFFSET = Pcal[2] * pow(2, 17) + dT*Pcal[4] / pow(2, 6);
			SENS = Pcal[1] * pow(2, 16) + dT*Pcal[3] / pow(2, 7);

			Temperature = (2000 + (dT*Pcal[6]) / pow(2, 23)) / 100;           // First-order Temperature in degrees Centigrade
		//
		// Second order corrections
			if (Temperature > 20)
			{
				T2 = 5 * dT*dT / pow(2, 38); // correction for high temperatures
				OFFSET2 = 0;
				SENS2 = 0;
			}
			if (Temperature < 20)                   // correction for low temperature
			{
				T2 = 3 * dT*dT / pow(2, 33);
				OFFSET2 = 61 * (100 * Temperature - 2000)*(100 * Temperature - 2000) / 16;
				SENS2 = 29 * (100 * Temperature - 2000)*(100 * Temperature - 2000) / 16;
			}
			if (Temperature < -15)                      // correction for very low temperature
			{
				OFFSET2 = OFFSET2 + 17 * (100 * Temperature + 1500)*(100 * Temperature + 1500);
				SENS2 = SENS2 + 9 * (100 * Temperature + 1500)*(100 * Temperature + 1500);
			}
			// End of second order corrections
			//
			Temperature = Temperature - T2 / 100;
			OFFSET = OFFSET - OFFSET2;
			SENS = SENS - SENS2;

			Pressure = (((D1*SENS) / pow(2, 21) - OFFSET) / pow(2, 15)) / 100;  // Pressure in mbar or Pa/100

			altitude = 145366.45*(1. - pow((Pressure / 1013.25), 0.190284));
		}
		case 'T':// TEMP
		{
			raw_temp = readGyroTempData();

		}
		break;
		}
	}

	void serialize(Logger* logger, uint32_t id, char command[]) {
		switch (command[0]) {
			case 'H':
				logger->setTime(millis() - 5);
				logger->print(raw_heading[0]);
				logger->print(raw_heading[1]);
				logger->print(raw_heading[2]);
				logger->send();
			break;
		}
	}

	void startSchedule(char command[], uint32_t id){
		
	}
	void endSchedule(char command[], uint32_t id){
	
	}


 int16_t raw_linAccel[3] = { 0,0,0 }, raw_rotVel[3]={0,0,0}, raw_grav[3] = { 0,0,0 };
 int16_t raw_quat[4] = { 0,0,0,0 }, raw_heading[3] = { 0,0,0 };
 int8_t raw_temp=0;
 uint8_t statusSys, statusGyr, statusAcc, statusMag;
 bool updatedRawMag = false,updatedRawAccel = true,updatedRawGyro = true;
 float vel=0,velTimeInt = 0;
 double heading[3] = {0.,0.,0.}, altitude = 0;
 double Temperature, Pressure;   
 int16_t gyro_bias[3]  = {0, 0, 0}, accel_bias[3] = {0, 0, 0},mag_bias[3] = {0, 0, 0};


private:
uint8_t liaReadMode = 0;
bool i2cBusy = false;
     
     // stores MS5637 pressures sensor pressure and temperature

uint16_t Pcal[8];         // calibration constants from MS5637 PROM registers
unsigned char nCRC;       // calculated check sum to ensure PROM integrity

uint8_t PWRMode = Normal ;    // Select BNO055 power mode
uint8_t OPRMode = NDOF;        // specify operation mode for sensors
uint8_t OSR = ADC_8192;       // set pressure amd temperature oversample rate
//
uint8_t GPwrMode = Normal;    // Gyro power mode
uint8_t Gscale = GFS_250DPS;  // Gyro full scale
//uint8_t Godr = GODR_250Hz;    // Gyro sample rate
uint8_t Gbw = GBW_23Hz;       // Gyro bandwidth
uint8_t Ascale = AFS_2G;      // Accel full scale
//uint8_t Aodr = AODR_250Hz;    // Accel sample rate
uint8_t APwrMode = Normal;    // Accel power mode
uint8_t Abw = ABW_31_25Hz;    // Accel bandwidth, accel sample rate divided by ABW_divx
//uint8_t Mscale = MFS_4Gauss;  // Select magnetometer full-scale resolution
uint8_t MOpMode = HighAccuracy;    // Select magnetometer perfomance mode
uint8_t MPwrMode = Normal;    // Select magnetometer power mode
uint8_t Modr = MODR_10Hz;     // Select magnetometer ODR when in BNO055 bypass mode

	// Set initial input parameters
enum Ascale {  // ACC Full Scale
  AFS_2G = 0,
  AFS_4G,
  AFS_8G,
  AFS_18G
};

enum Abw { // ACC Bandwidth
  ABW_7_81Hz = 0,
  ABW_15_63Hz,
  ABW_31_25Hz,
  ABW_62_5Hz,
  ABW_125Hz,    
  ABW_250Hz,
  ABW_500Hz,     
  ABW_1000Hz,    //0x07
};

enum APwrMode { // ACC Pwr Mode
  NormalA = 0,  
  SuspendA,
  LowPower1A,
  StandbyA,        
  LowPower2A,
  DeepSuspendA
};

enum Gscale {  // gyro full scale
  GFS_2000DPS = 0,
  GFS_1000DPS,
  GFS_500DPS,
  GFS_250DPS,
  GFS_125DPS    // 0x04
};

enum GPwrMode { // GYR Pwr Mode
  NormalG = 0,
  FastPowerUpG,
  DeepSuspendedG,
  SuspendG,
  AdvancedPowerSaveG
};

enum Gbw { // gyro bandwidth
  GBW_523Hz = 0,
  GBW_230Hz,
  GBW_116Hz,
  GBW_47Hz,
  GBW_23Hz,
  GBW_12Hz,
  GBW_64Hz,
  GBW_32Hz
};

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

enum PWRMode {
  Normalpwr = 0,   
  Lowpower,       
  Suspendpwr       
};

enum Modr {         // magnetometer output data rate  
  MODR_2Hz = 0,     
  MODR_6Hz,
  MODR_8Hz,
  MODR_10Hz,  
  MODR_15Hz,
  MODR_20Hz,
  MODR_25Hz, 
  MODR_30Hz 
};

enum MOpMode { // MAG Op Mode
  LowPower = 0,
  Regular,
  EnhancedRegular,
  HighAccuracy
};

enum MPwrMode { // MAG power mode
  Normal = 0,   
  Sleep,     
  Suspend,
  ForceMode  
};


void accelgyroCalBNO055() 
{
  uint8_t data[6]; // data array to hold accelerometer and gyro x, y, z, data
  uint16_t ii = 0, sample_count = 0;
 	gyro_bias[0]=0;
 	gyro_bias[1]=0;
 	gyro_bias[2]=0;
 	accel_bias[0]=0;
 	accel_bias[1]=0;
 	accel_bias[2]=0;
 
  // Select page 0 to read sensors
   writeByte(BNO055_ADDRESS, BNO055_PAGE_ID, 0x00);
   // Select BNO055 system operation mode as NDOF for calibration
   writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, CONFIGMODE );
   delay(25);
   writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, NDOF );
 // In NDF fusion mode, accel full scale is at +/- 4g, ODR is 62.5 Hz
   sample_count = 50;
   for(ii = 0; ii < sample_count; ii++) {
    int16_t accel_temp[3] = {0, 0, 0};
    readBytes(BNO055_ADDRESS, BNO055_ACC_DATA_X_LSB, 6, &data[0]);  // Read the six raw data registers into data array
    accel_temp[0] = (int16_t) (((int16_t)data[1] << 8) | data[0]) ; // Form signed 16-bit integer for each sample in FIFO
    accel_temp[1] = (int16_t) (((int16_t)data[3] << 8) | data[2]) ;
    accel_temp[2] = (int16_t) (((int16_t)data[5] << 8) | data[4]) ;
    accel_bias[0]  += (int32_t) accel_temp[0];
    accel_bias[1]  += (int32_t) accel_temp[1];
    accel_bias[2]  += (int32_t) accel_temp[2];
    delay(20);  // at 62.5 Hz ODR, new accel data is available every 16 ms
    Serial.print(accel_temp[1]);
    Serial.print(".");
   }

    accel_bias[0]  /= (int32_t) sample_count;  // get average accel bias in mg
    accel_bias[1]  /= (int32_t) sample_count;
    accel_bias[2]  /= (int32_t) sample_count;
    
  if(accel_bias[2] > 0L) {accel_bias[2] -= (int32_t) 1000;}  // Remove gravity from the z-axis accelerometer bias calculation
  else {accel_bias[2] += (int32_t) 1000;}    

 // In NDF fusion mode, gyro full scale is at +/- 2000 dps, ODR is 32 Hz
   for(ii = 0; ii < sample_count; ii++) {
    int16_t gyro_temp[3] = {0, 0, 0};
    readBytes(BNO055_ADDRESS, BNO055_GYR_DATA_X_LSB, 6, &data[0]);  // Read the six raw data registers into data array
    gyro_temp[0] = (int16_t) (((int16_t)data[1] << 8) | data[0]) ;  // Form signed 16-bit integer for each sample in FIFO
    gyro_temp[1] = (int16_t) (((int16_t)data[3] << 8) | data[2]) ;
    gyro_temp[2] = (int16_t) (((int16_t)data[5] << 8) | data[4]) ;
    gyro_bias[0]  += (int32_t) gyro_temp[0];
    gyro_bias[1]  += (int32_t) gyro_temp[1];
    gyro_bias[2]  += (int32_t) gyro_temp[2];
    delay(35);  // at 32 Hz ODR, new gyro data available every 31 ms
    Serial.print(".");
   }
    gyro_bias[0]  /= (int32_t) sample_count;  // get average gyro bias in counts
    gyro_bias[1]  /= (int32_t) sample_count;
    gyro_bias[2]  /= (int32_t) sample_count;

  // Return to config mode to write accelerometer biases in offset register
  // This offset register is only used while in fusion mode when accelerometer full-scale is +/- 4g
  writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, CONFIGMODE );
  delay(25);
  
  //write biases to accelerometer offset registers ad 16 LSB/dps
  writeByte(BNO055_ADDRESS, BNO055_ACC_OFFSET_X_LSB, (int16_t)accel_bias[0] & 0xFF);
  writeByte(BNO055_ADDRESS, BNO055_ACC_OFFSET_X_MSB, ((int16_t)accel_bias[0] >> 8) & 0xFF);
  writeByte(BNO055_ADDRESS, BNO055_ACC_OFFSET_Y_LSB, (int16_t)accel_bias[1] & 0xFF);
  writeByte(BNO055_ADDRESS, BNO055_ACC_OFFSET_Y_MSB, ((int16_t)accel_bias[1] >> 8) & 0xFF);
  writeByte(BNO055_ADDRESS, BNO055_ACC_OFFSET_Z_LSB, (int16_t)accel_bias[2] & 0xFF);
  writeByte(BNO055_ADDRESS, BNO055_ACC_OFFSET_Z_MSB, ((int16_t)accel_bias[2] >> 8) & 0xFF);
 
 
   //write biases to gyro offset registers
  writeByte(BNO055_ADDRESS, BNO055_GYR_OFFSET_X_LSB, (int16_t)gyro_bias[0] & 0xFF);
  writeByte(BNO055_ADDRESS, BNO055_GYR_OFFSET_X_MSB, ((int16_t)gyro_bias[0] >> 8) & 0xFF);
  writeByte(BNO055_ADDRESS, BNO055_GYR_OFFSET_Y_LSB, (int16_t)gyro_bias[1] & 0xFF);
  writeByte(BNO055_ADDRESS, BNO055_GYR_OFFSET_Y_MSB, ((int16_t)gyro_bias[1] >> 8) & 0xFF);
  writeByte(BNO055_ADDRESS, BNO055_GYR_OFFSET_Z_LSB, (int16_t)gyro_bias[2] & 0xFF);
  writeByte(BNO055_ADDRESS, BNO055_GYR_OFFSET_Z_MSB, ((int16_t)gyro_bias[2] >> 8) & 0xFF);
  
  // Select BNO055 system operation mode
  writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, ControlledBN055::OPRMode );
}

void magCalBNO055() 
{
  uint8_t data[6]; // data array to hold accelerometer and gyro x, y, z, data
  uint16_t ii = 0, sample_count = 0;
  mag_bias[0] = 0;
  mag_bias[1] = 0;
  mag_bias[2] = 0;
  int16_t mag_max[3] = {0, 0, 0}, mag_min[3] = {0, 0, 0};
 
  // Select page 0 to read sensors
   writeByte(BNO055_ADDRESS, BNO055_PAGE_ID, 0x00);
   // Select BNO055 system operation mode as NDOF for calibration
   writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, CONFIGMODE );
   delay(25);
   writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, NDOF );

 // In NDF fusion mode, mag data is in 16 LSB/microTesla, ODR is 20 Hz in forced mode
   sample_count = 256;
   for(ii = 0; ii < sample_count; ii++) {
    int16_t mag_temp[3] = {0, 0, 0};
    readBytes(BNO055_ADDRESS, BNO055_MAG_DATA_X_LSB, 6, &data[0]);  // Read the six raw data registers into data array
    mag_temp[0] = (int16_t) (((int16_t)data[1] << 8) | data[0]) ;   // Form signed 16-bit integer for each sample in FIFO
    mag_temp[1] = (int16_t) (((int16_t)data[3] << 8) | data[2]) ;
    mag_temp[2] = (int16_t) (((int16_t)data[5] << 8) | data[4]) ;
    for (int jj = 0; jj < 3; jj++) {
      if(mag_temp[jj] > mag_max[jj]) mag_max[jj] = mag_temp[jj];
      if(mag_temp[jj] < mag_min[jj]) mag_min[jj] = mag_temp[jj];
    }
    Serial.print('.');
    delay(55);  // at 20 Hz ODR, new mag data is available every 50 ms
   }

 //   Serial.println("mag x min/max:"); Serial.println(mag_max[0]); Serial.println(mag_min[0]);
 //   Serial.println("mag y min/max:"); Serial.println(mag_max[1]); Serial.println(mag_min[1]);
 //   Serial.println("mag z min/max:"); Serial.println(mag_max[2]); Serial.println(mag_min[2]);

    mag_bias[0]  = (mag_max[0] + mag_min[0])/2;  // get average x mag bias in counts
    mag_bias[1]  = (mag_max[1] + mag_min[1])/2;  // get average y mag bias in counts
    mag_bias[2]  = (mag_max[2] + mag_min[2])/2;  // get average z mag bias in counts
  

  // Return to config mode to write mag biases in offset register
  // This offset register is only used while in fusion mode when magnetometer sensitivity is 16 LSB/microTesla
  writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, CONFIGMODE );
  delay(25);
  
  //write biases to accelerometer offset registers as 16 LSB/microTesla
  writeByte(BNO055_ADDRESS, BNO055_MAG_OFFSET_X_LSB, (int16_t)mag_bias[0] & 0xFF);
  writeByte(BNO055_ADDRESS, BNO055_MAG_OFFSET_X_MSB, ((int16_t)mag_bias[0] >> 8) & 0xFF);
  writeByte(BNO055_ADDRESS, BNO055_MAG_OFFSET_Y_LSB, (int16_t)mag_bias[1] & 0xFF);
  writeByte(BNO055_ADDRESS, BNO055_MAG_OFFSET_Y_MSB, ((int16_t)mag_bias[1] >> 8) & 0xFF);
  writeByte(BNO055_ADDRESS, BNO055_MAG_OFFSET_Z_LSB, (int16_t)mag_bias[2] & 0xFF);
  writeByte(BNO055_ADDRESS, BNO055_MAG_OFFSET_Z_MSB, ((int16_t)mag_bias[2] >> 8) & 0xFF);
 
  // Check that offsets were properly written to offset registers
//  Serial.println("Average magnetometer bias = "); 
//  Serial.println((int16_t)((int16_t)readByte(BNO055_ADDRESS, BNO055_MAG_OFFSET_X_MSB) << 8 | readByte(BNO055_ADDRESS, BNO055_MAG_OFFSET_X_LSB))); 
//  Serial.println((int16_t)((int16_t)readByte(BNO055_ADDRESS, BNO055_MAG_OFFSET_Y_MSB) << 8 | readByte(BNO055_ADDRESS, BNO055_MAG_OFFSET_Y_LSB))); 
//  Serial.println((int16_t)((int16_t)readByte(BNO055_ADDRESS, BNO055_MAG_OFFSET_Z_MSB) << 8 | readByte(BNO055_ADDRESS, BNO055_MAG_OFFSET_Z_LSB)));
  // Select BNO055 system operation mode
  writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, OPRMode );

}


// I2C read/write functions for the BNO055 sensor

void writeByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
  Wire.beginTransmission(address);  // Initialize the Tx buffer
  Wire.write(subAddress);           // Put slave register address in Tx buffer
  Wire.write(data);                 // Put data in Tx buffer
  Wire.endTransmission();           // Send the Tx buffer
}

uint8_t readByte(uint8_t address, uint8_t subAddress)
{
  uint8_t data; // `data` will store the register data   
  Wire.beginTransmission(address);         // Initialize the Tx buffer
  Wire.write(subAddress);                  // Put slave register address in Tx buffer
  Wire.endTransmission(I2C_NOSTOP);        // Send the Tx buffer, but send a restart to keep connection alive
//  Wire.endTransmission(false);             // Send the Tx buffer, but send a restart to keep connection alive
//  Wire.requestFrom(address, 1);  // Read one byte from slave register address 
  Wire.requestFrom(address, (size_t) 1,I2C_STOP);   // Read one byte from slave register address 
  data = Wire.read();                      // Fill Rx buffer with result
  return data;                             // Return data read from slave register
}

// Traditional Blocking
void readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest)
{  
		Wire.beginTransmission(address);   // Initialize the Tx buffer
		Wire.write(subAddress);            // Put slave register address in Tx buffer
		Wire.endTransmission(I2C_NOSTOP);  // Send the Tx buffer, but send a restart to keep connection alive
		

		uint8_t i=0;
        Wire.requestFrom(address, (size_t) count, I2C_STOP);  // Read bytes from slave register address 

		for (uint8_t i = 0; i < count; i++) {
			dest[i] = Wire.read();
		}
	
}

// 1st Call this
bool requestBytes(uint8_t address, uint8_t subAddress , uint8_t count){
	if (!Wire.done())
		return false;
	Wire.beginTransmission(address);   // Initialize the Tx buffer
	Wire.write(subAddress);            // Put slave register address in Tx buffer
	Wire.endTransmission(I2C_NOSTOP);  // Send the Tx buffer, but send a restart to keep connection alive
  Wire.sendRequest(address, (size_t)count, I2C_STOP);  // Read bytes from slave register address 
  return true;
}


// 2nd Call this, recall later if return false
bool getBytes( uint8_t count, uint8_t * dest ){
	if (!Wire.done())
	  return false;

	for (uint8_t i = 0; i < count; i ++){
		dest[i] = Wire.read();
	}         // Put read results in the Rx buffer

    return true;
}



// I2C communication with the MS5637 is a little different from that with the BNO055 and most other sensors
// For the MS5637, we write commands, and the MS5637 sends data in response, rather than directly reading
// MS5637 registers

 void MS5637Reset()
        {
        Wire.beginTransmission(MS5637_ADDRESS);  // Initialize the Tx buffer
  Wire.write(MS5637_RESET);                // Put reset command in Tx buffer
  Wire.endTransmission();                  // Send the Tx buffer
        }
        
 void MS5637PromRead(uint16_t * destination)
        {
        uint8_t data[2] = {0,0};
        for (uint8_t ii = 0; ii < 7; ii++) {
          Wire.beginTransmission(MS5637_ADDRESS);  // Initialize the Tx buffer
          Wire.write(0xA0 | ii << 1);              // Put PROM address in Tx buffer
          Wire.endTransmission(I2C_NOSTOP);        // Send the Tx buffer, but send a restart to keep connection alive
//          Wire.endTransmission(false);        // Send the Tx buffer, but send a restart to keep connection alive
    uint8_t i = 0;
          Wire.requestFrom(MS5637_ADDRESS, 2);   // Read two bytes from slave PROM address 
    while (Wire.available()) {
          data[i++] = Wire.read(); }               // Put read results in the Rx buffer
          destination[ii] = (uint16_t) (((uint16_t) data[0] << 8) | data[1]); // construct PROM data for return to main program
        }
        }

        uint32_t MS5637Read(uint8_t CMD, uint8_t OSR)  // temperature data read
        {
        uint8_t data[3] = {0,0,0};
        Wire.beginTransmission(MS5637_ADDRESS);  // Initialize the Tx buffer
        Wire.write(CMD | OSR);                  // Put pressure conversion command in Tx buffer
        Wire.endTransmission(I2C_NOSTOP);        // Send the Tx buffer, but send a restart to keep connection alive
//        Wire.endTransmission(false);        // Send the Tx buffer, but send a restart to keep connection alive
        
        switch (OSR)
        {
          case ADC_256: delay(1); break;  // delay for conversion to complete
          case ADC_512: delay(3); break;
          case ADC_1024: delay(4); break;
          case ADC_2048: delay(6); break;
          case ADC_4096: delay(10); break;
          case ADC_8192: delay(20); break;
        }
       
        Wire.beginTransmission(MS5637_ADDRESS);  // Initialize the Tx buffer
        Wire.write(0x00);                        // Put ADC read command in Tx buffer
//        Wire.endTransmission(false);        // Send the Tx buffer, but send a restart to keep connection alive
        Wire.endTransmission(I2C_NOSTOP);        // Send the Tx buffer, but send a restart to keep connection alive
  		uint8_t i = 0;
        Wire.requestFrom(MS5637_ADDRESS, 3);     // Read three bytes from slave PROM address 
  		while (Wire.available()) {
	    	data[i++] = Wire.read(); 
	    }               // Put read results in the Rx buffer
	    return (uint32_t) (((uint32_t) data[0] << 16) | (uint32_t) data[1] << 8 | data[2]); // construct PROM data for return to main program
    }



unsigned char MS5637checkCRC(uint16_t * n_prom)  // calculate checksum from PROM register contents
{
  int cnt;
  unsigned int n_rem = 0;
  unsigned char n_bit;
  
  n_prom[0] = ((n_prom[0]) & 0x0FFF);  // replace CRC byte by 0 for checksum calculation
  n_prom[7] = 0;
  for(cnt = 0; cnt < 16; cnt++)
  {
    if(cnt%2==1) n_rem ^= (unsigned short) ((n_prom[cnt>>1]) & 0x00FF);
    else         n_rem ^= (unsigned short)  (n_prom[cnt>>1]>>8);
    for(n_bit = 8; n_bit > 0; n_bit--)
    {
        if(n_rem & 0x8000)    n_rem = (n_rem<<1) ^ 0x3000;
        else                  n_rem = (n_rem<<1);
    }
  }
  n_rem = ((n_rem>>12) & 0x000F);
  return (n_rem ^ 0x00);
}


int8_t readGyroTempData()
{
  return readByte(BNO055_ADDRESS, BNO055_TEMP);  // Read the two raw data registers sequentially into data array 
}


};

	

#endif