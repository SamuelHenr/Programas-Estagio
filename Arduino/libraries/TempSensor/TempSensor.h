#ifndef TempSensor_h
#define TempSensor_h

#include "Arduino.h"

class TempSensor {
public:
	TempSensor(uint8_t pin);
	void printAddress(void);
	float getTemperature();
	void printTemperature(float temp);
	void setDeviceAddress(DeviceAddress deviceAddress);
};

#endif