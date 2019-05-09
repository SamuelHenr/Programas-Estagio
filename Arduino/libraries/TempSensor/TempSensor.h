#ifndef TempSensor_h
#define TempSensor_h

#include "Arduino.h"
#include "DallasTemperature.h"

class TempSensor {
public:

	// Constructor
	TempSensor(uint8_t pin);

	// Show the address of the sensor ds18b20
	void printAddress(OneWire ds);

	// Return the current temperature
	float getTemperature(DallasTemperature sensors, DeviceAddress sensor1);

	// Show in the serial monitor the temperature
	void printTemperature(float temp);

	// Set the address of the device
	void initComponents(DallasTemperature sensors, DeviceAddress sensor1);

private:
	uint8_t pin;
};

#endif