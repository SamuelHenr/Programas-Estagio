/*
    Exibe o endereço físico do sensor Dallas DS18B20
    e mostra no Serial Monitor 

*/


#include <OneWire.h> //biblioteca protocolo One Wire

OneWire  ds(10);  //declaração de objeto (sensor ds no pino digital 10)


// Configurações Iniciais
void setup() 
{
  Serial.begin(9600);        //inicia Serial com 9600 baud rate
  discoverOneWireDevices();  //chama função que irá buscar por sensores presentes no sistema
  
} //end setup

void discoverOneWireDevices() 
{
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];

  Serial.print("Buscando dispositivos DS18B20...\n\r");

  while(ds.search(addr)) 
  {
    Serial.print("\n\rEncontrado sensor \'DS18B20\' com endereco:\n\r");
    for( i = 0; i < 8; i++) 
    {
      Serial.print("0x");
       if (addr[i] < 16)
       {
        Serial.print('0');

       }
      Serial.print(addr[i], HEX);

       if (i < 7) 
       {
        Serial.print(", ");
       }

    }

    if ( OneWire::crc8( addr, 7) != addr[7]) 
    {
        Serial.print("CRC nao e valido!\n");
        return;
    }
  }
  Serial.print("\n\r\n\rFinal da verificacao.\r\n");
  ds.reset_search();
  return;
}



void loop() 
{
  // Loop Vazio
} 
