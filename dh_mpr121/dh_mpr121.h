#ifndef DH_MPR121_H__
#define DH_MPR121_H__

#include <i2c_t3.h>

/*
    MPR121.h
	April 8, 2010
	by: Jim Lindblom
*/

// MPR121 Register Defines
#define MHD_R	0x2B
#define NHD_R	0x2C
#define	NCL_R 	0x2D
#define	FDL_R	0x2E
#define	MHD_F	0x2F
#define	NHD_F	0x30
#define	NCL_F	0x31
#define	FDL_F	0x32
#define	ELE0_T	0x41
#define	ELE0_R	0x42
#define	ELE1_T	0x43
#define	ELE1_R	0x44
#define	ELE2_T	0x45
#define	ELE2_R	0x46
#define	ELE3_T	0x47
#define	ELE3_R	0x48
#define	ELE4_T	0x49
#define	ELE4_R	0x4A
#define	ELE5_T	0x4B
#define	ELE5_R	0x4C
#define	ELE6_T	0x4D
#define	ELE6_R	0x4E
#define	ELE7_T	0x4F
#define	ELE7_R	0x50
#define	ELE8_T	0x51
#define	ELE8_R	0x52
#define	ELE9_T	0x53
#define	ELE9_R	0x54
#define	ELE10_T	0x55
#define	ELE10_R	0x56
#define	ELE11_T	0x57
#define	ELE11_R	0x58
#define	FIL_CFG	0x5D
#define	ELE_CFG	0x5E
#define GPIO_CTRL0	0x73
#define	GPIO_CTRL1	0x74
#define GPIO_DATA	0x75
#define	GPIO_DIR	0x76
#define	GPIO_EN		0x77
#define	GPIO_SET	0x78
#define	GPIO_CLEAR	0x79
#define	GPIO_TOGGLE	0x7A
#define	ATO_CFG0	0x7B
#define	ATO_CFGU	0x7D
#define	ATO_CFGL	0x7E
#define	ATO_CFGT	0x7F


// Global Constants
#define TOU_THRESH	0x06
#define	REL_THRESH	0x0A

class TouchPad{

public:
	
	boolean touchStates[12]; //to keep track of the previous touch states
	byte DEV_ADDR = 0x5B;

	TouchPad(uint8_t irqPin ){
		irqpin = irqPin;
	}

	void begin(void (*function)()){

      Wire.begin();

	  set_register(DEV_ADDR, ELE_CFG, 0x00); 

	  // Section A - Controls filtering when data is > baseline.
	  set_register(DEV_ADDR, MHD_R, 0x01);
	  set_register(DEV_ADDR, NHD_R, 0x01);
	  set_register(DEV_ADDR, NCL_R, 0x00);
	  set_register(DEV_ADDR, FDL_R, 0x00);

	  // Section B - Controls filtering when data is < baseline.
	  set_register(DEV_ADDR, MHD_F, 0x01);
	  set_register(DEV_ADDR, NHD_F, 0x01);
	  set_register(DEV_ADDR, NCL_F, 0xFF);
	  set_register(DEV_ADDR, FDL_F, 0x02);

	  // Section C - Sets touch and release thresholds for each electrode
	  set_register(DEV_ADDR, ELE0_T, TOU_THRESH);
	  set_register(DEV_ADDR, ELE0_R, REL_THRESH);

	  set_register(DEV_ADDR, ELE1_T, TOU_THRESH);
	  set_register(DEV_ADDR, ELE1_R, REL_THRESH);

	  set_register(DEV_ADDR, ELE2_T, TOU_THRESH);
	  set_register(DEV_ADDR, ELE2_R, REL_THRESH);

	  set_register(DEV_ADDR, ELE3_T, TOU_THRESH);
	  set_register(DEV_ADDR, ELE3_R, REL_THRESH);

	  set_register(DEV_ADDR, ELE4_T, TOU_THRESH);
	  set_register(DEV_ADDR, ELE4_R, REL_THRESH);

	  set_register(DEV_ADDR, ELE5_T, TOU_THRESH);
	  set_register(DEV_ADDR, ELE5_R, REL_THRESH);

	  set_register(DEV_ADDR, ELE6_T, TOU_THRESH);
	  set_register(DEV_ADDR, ELE6_R, REL_THRESH);

	  set_register(DEV_ADDR, ELE7_T, TOU_THRESH);
	  set_register(DEV_ADDR, ELE7_R, REL_THRESH);

	  set_register(DEV_ADDR, ELE8_T, TOU_THRESH);
	  set_register(DEV_ADDR, ELE8_R, REL_THRESH);

	  set_register(DEV_ADDR, ELE9_T, TOU_THRESH);
	  set_register(DEV_ADDR, ELE9_R, REL_THRESH);

	  set_register(DEV_ADDR, ELE10_T, TOU_THRESH);
	  set_register(DEV_ADDR, ELE10_R, REL_THRESH);

	  set_register(DEV_ADDR, ELE11_T, TOU_THRESH);
	  set_register(DEV_ADDR, ELE11_R, REL_THRESH);

	  // Section D
	  // Set the Filter Configuration
	  // Set ESI2
	  set_register(DEV_ADDR, FIL_CFG, 0x04);

	  // Section E
	  // Electrode Configuration
	  // Set ELE_CFG to 0x00 to return to standby mode
	  set_register(DEV_ADDR, ELE_CFG, 0x0C);  // Enables all 12 Electrodes


	  // Section F
	  // Enable Auto Config and auto Reconfig
	  /*set_register(DEV_ADDR, ATO_CFG0, 0x0B);
	  set_register(DEV_ADDR, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(DEV_ADDR, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
	  set_register(DEV_ADDR, ATO_CFGT, 0xB5);*/  // Target = 0.9*USL = 0xB5 @3.3V

	  set_register(DEV_ADDR, ELE_CFG, 0x0C);
	   

	  pinMode(irqpin,INPUT_PULLUP);
		attachInterrupt(irqpin,function,FALLING  );
	
	}


	void process(void (*onPress)(uint8_t), void (*onRelease)(uint8_t) = 0){

		 if(!digitalRead(irqpin)){
	
			  //read the touch state from the MPR121
			  Wire.requestFrom(DEV_ADDR,2); 
			  
			  byte LSB = Wire.read();
			  byte MSB = Wire.read();
			  
			  uint16_t touched = ((MSB << 8) | LSB); //16bits that make up the touch states

			  
			  for (int i=0; i < 12; i++){ // Check what electrodes were pressed
			   if(touched & (1<<i)){
			   
			    if(touchStates[i] == 0){
			     //pin i was just touched
			     if (onPress)
			     	onPress(i);		
			    
			    }else if(touchStates[i] == 1){
			     //pin i is still being touched
			    } 
			   
			    touchStates[i] = 1;   
			   }else{
			    if(touchStates[i] == 1){
			       if (onRelease)
			     	onRelease(i);		
			     
			     //pin i is no longer being touched
			   }
			    
			    touchStates[i] = 0;
			   }
			  
			  }
			  
			 }
	}

	uint16_t test(){
		  Wire.requestFrom(DEV_ADDR,2); 
			  
			  byte LSB = Wire.read();
			  byte MSB = Wire.read();
			  
			  return ((MSB << 8) | LSB); //16bits that make up the touch states
	}

	static char numToKey(uint16_t num){
		switch (num){
			case 0: return 'L';
			case 1: return '7';
			case 2: return '4';
			case 3: return '1';
			case 4: return '0';
			case 5: return '8';
			case 6: return '5';
			case 7: return '2';
			case 8: return 'R';
			case 9: return '9';
			case 10: return '6';
			case 11: return '3';
		}
		return 'X'; 
	}



private:

	uint8_t irqpin =2 ;



	void set_register(int address, unsigned char r, unsigned char v){
	    Wire.beginTransmission(address);
	    Wire.write(r);
	    Wire.write(v);
	    Wire.endTransmission();
	}

};




#endif