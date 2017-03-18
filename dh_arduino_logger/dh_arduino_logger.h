#ifndef DH_ARDUINO_LOGGER_H__
#define DH_ARDUINO_LOGGER_H__


#include <dh_logger.h>

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif




class ArduinoLogger :public Logger{
public:
	ArduinoLogger(uint8_t sNum);

	void print(uint8_t val, uint8_t base = 10);
	void print(int8_t val);
	void print(uint16_t val);
	void print(int16_t val);
	void print(uint32_t val);
	void print(int32_t val);
	void print(bool val);

	void print(float val, uint8_t prec=4);
	void print(double val, uint8_t prec=4);
	void print(const char text[]);

	void println (void);

private:
	Stream *serial;
	uint8_t serialNumber = 0;
};

#endif