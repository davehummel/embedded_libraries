#include <i2c_t3.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

void setup(void) 
{
  uint32_t currentFrequency;
    
  Serial1.begin(460800);
  Serial1.println("Hello!");
  
  Serial1.println("Measuring voltage and current with INA219 ...");
  ina219.beginFullRange();
}

void loop(void) 
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  
  Serial1.print("Bus Voltage:   "); Serial1.print(busvoltage); Serial1.println(" V");
  Serial1.print("Shunt Voltage: "); Serial1.print(shuntvoltage); Serial1.println(" mV");
  Serial1.print("Load Voltage:  "); Serial1.print(loadvoltage); Serial1.println(" V");
  Serial1.print("Current:       "); Serial1.print(current_mA); Serial1.println(" mA");
  Serial1.println("");

  delay(5000);
}
