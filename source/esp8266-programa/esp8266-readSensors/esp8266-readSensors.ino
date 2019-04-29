#include <DHT.h>   //biblioteca do sensor de umidade e temperatura


// ===============================================================================
// --- Mapeamento de Hardware ---
const uint8_t dht_pin = 5;   //pino de sinal do dht11 ligado no digital 5
const uint8_t DHT_typeSensor = DHT11;

// ===============================================================================
// --- Declaração de Objetos ---
DHT my_dht = DHT(dht_pin, DHT_typeSensor);   //objeto para o sensor


// ===============================================================================
// --- Variáveis Globais ---
float    temperatura = 0x00,   //armazena a temperatura
         umidade     = 0x00;   //armazena a umidade


void setup() {
   Serial.begin(9600);   //serial em 9600 baud rate
   
} //end setup

void loop() {

   temperatura = my_dht.readTemperature(false);
   umidade     = my_dht.readHumidity();


   Serial.print(temperatura);
   Serial.print(" ");
   Serial.println(umidade);
   


   delay(100);
 


} //end loop
