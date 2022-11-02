#include "DHT_Sensor.hpp"
#include "LoRa_E5.hpp"
#include "DS18B20_Sensor.hpp"
#include "Rules.hpp"

DHT_Sensor dht1(2);
DS18B20_Sensor ds1(5);

LoRa_E5 E5;

void setup() {
  // put your setup code here, to run once:
  E5.module_setup();
  while(!E5.join());
  
  #ifdef PRINT_ENABLE
  Serial.begin(9600);
  #endif
  if(!ds1.connect())
  {
    Serial.println("Failed to connect thermometer!");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  dht1.update_data();
  ds1.update_data();
  E5.module_send(0x00, (uint8_t*)dht1.get_data(), 8);
}
