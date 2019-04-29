/* ========================================================================================================
 
   Curso de Arduino e AVR 149
   
   WR Kits Channel

   Sensor de Temperatura DS18B20 Dallas
    
   Autor: Eng. Wagner Rambo  Data: Dezembro de 2017
   
   www.wrkits.com.br | facebook.com/wrkits | youtube.com/user/canalwrkits

   HARDWARE Termômetro olhando-o de frente:

   Terminal da direita   -> 5V do Arduino
   Terminal da esquerda  -> GND do Arduino
   Terminal central      -> digital 10 do Arduino (ligar um resistor de 4,7k entre esta entrada e os 5V)
   
   
======================================================================================================== */


// ========================================================================================================

#include <OneWire.h>
#include <DallasTemperature.h>

// ========================================================================================================
// --- Define the pin tha will be used ---
#define ONE_WIRE_BUS 10


// ========================================================================================================
// --- Objects ---
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// if there are another sensors, include more DevideAddress objects

// needs to use another file to find the sensor's address
DeviceAddress sensor1 = { 0x28, 0xFF, 0x90, 0x26, 0x83, 0x16, 0x04, 0x27 };

void setup() {
 
  Serial.begin(9600);
 
  sensors.begin(); //initialize sensors
 
  sensors.setResolution(sensor1, 10); //set resolution to 10 bits
 
}

void loop(void) {
  
 Serial.println("Sensor DS18B20");
  sensors.requestTemperatures();
  printTemperature(sensor1);
  
  delay(1000);
  
}

// ========================================================================================================
// --- Functions ---
void printTemperature(DeviceAddress deviceAddress) {

  float tempC = sensors.getTempC(deviceAddress);

  if (tempC >= -127.00) {
  
    Serial.print("Erro de leitura");

  } else {
    Serial.print(tempC);
    Serial.print(" graus C ");
    Serial.print("  |  ");
    Serial.print(DallasTemperature::toFahrenheit(tempC));
    Serial.print(" graus F ");
  }

  Serial.print("\n\r");
  
}
