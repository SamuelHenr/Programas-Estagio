#include "TempSensor.h"
#include "Arduino.h"

#include <OneWire.h> //biblioteca protocolo One Wire
#include <DallasTemperature.h>

TempSensor::TempSensor(uint8_t pin) {
	_pin = pin;
	// setup
	Serial.begin(9600);
	OneWire  ds(10);  //declaração de objeto (sensor ds no pino digital 10)
	OneWire oneWire(_pin);
	DallasTemperature sensors(&oneWire);
	DeviceAddress sensor1 = NULL;
	sensors.begin(); //initialize sensors
	sensors.setResolution(sensor1, 10); //set resolution to 10 bits
}

// Write funtions here

void TempSensor::Address(void) {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];

  Serial.print("Buscando dispositivos DS18B20...\n\r");

  while(ds.search(addr)){
    Serial.print("\n\rEncontrado sensor \'DS18B20\' com endereco:\n\r");
    for( i = 0; i < 8; i++) {
      Serial.print("0x");

      if (addr[i] < 16) {
       Serial.print('0');
      }
       
      Serial.print(addr[i], HEX);

       if (i < 7) {
        Serial.print(", ");
       }

    }

    if (OneWire::crc8( addr, 7) != addr[7]) {
        Serial.print("CRC nao e valido!\n");
        return;
    }
  }
  
  Serial.print("\n\r\n\rFinal da verificacao.\r\n");
  ds.reset_search();
  return;
}

void TempSensor::setDeviceAddress(DeviceAddress deviceAddress) {
	sensor1 = deviceAddress;
}

float TempSensor::getTemperature() {

	if(sensor1 == NULL) return -2;

	float temperature = sensors.getTempC(sensor1);
	if(temperature >= -127.00) return -1;
	return temperature;
}

void TempSensor::printTemperature(float temp) {
	Serial.println(temp);
	Serial.println(" graus Cº");
}