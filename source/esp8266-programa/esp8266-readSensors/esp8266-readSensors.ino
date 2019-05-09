#include <DHT.h>   //
#include <DallasTemperature.h>
#include <TempSensor.h>

// ===============================================================================
// --- Mapeamento de Hardware ---
const uint8_t dht_pin = 5;
const uint8_t ds_pin = 10;
const uint8_t DHT_typeSensor = DHT11;

// ===============================================================================
// --- Declaração de Objetos ---
DHT dht = DHT(dht_pin, DHT_typeSensor);   //objeto para o sensor
TempSensor ds = TempSensor(ds_pin);

// ===============================================================================
// --- Global Variables ---
float    temperatura = 0x00,
         umidade     = 0x00;


void setup() {
   Serial.begin(9600);
} //end setup

void loop() {

   OneWire dsSensor(ds_pin);  //object OneWire (sensor ds plugged in the pin specified)
   OneWire oneWireData(ds_pin);
   DallasTemperature sensorsUsed(&oneWireData);


   temperatura = dht.readTemperature(false);
   umidade     = dht.readHumidity();

   Serial.println("------ DHT 11 ------");
   Serial.print(temperatura);
   Serial.print(" ");
   Serial.println(umidade);
   Serial.println("\n");

   Serial.println("------ DS18B20 ------");
   ds.printAddress(dsSensor);
   DeviceAddress sensor1 = { 0x28, 0xFF, 0x90, 0x26, 0x83, 0x16, 0x04, 0x27 }; // just to compile
   ds.initComponents(sensorsUsed, sensor1);
   ds.printTemperature(ds.getTemperature(sensorsUsed, sensor1));

   delay(1000);
 
} //end loop
