#include "DHT_Sensor.hpp"

DHT_Sensor::DHT_Sensor() : _pin(2), _temperature(0), _humidity(0)
{
  _dht = new DHT(2, DHTTYPE);
  _data[0] = 0;
  _data[1] = 0;
}

DHT_Sensor::DHT_Sensor(int pin) : _pin(pin), _temperature(0), _humidity(0)
{
  _dht = new DHT(_pin, DHTTYPE);
  _data[0] = 0;
  _data[1] = 0;
}

void DHT_Sensor::setup()
{
  _dht->begin();
}

bool DHT_Sensor::update_data()
{
  _temperature = _dht->readTemperature();
  _humidity = _dht->readHumidity();

  if (isnan(_humidity) || isnan(_temperature)) {
    #ifdef PRINT_ENABLE
    Serial.println(F("Failed to read from DHT sensor!"));
    #endif
    return false;
  }

  #ifdef PRINT_ENABLE
  Serial.print("Température : ");
  Serial.println(_temperature);
  Serial.print("Humidité : ");
  Serial.println(_humidity);
  #endif
  
  /*_data[0] = _temperature;
  _data[1] = _humidity;*/

  uint16_t temperature_format = ((_temperature*100)+(65534/2));
  uint16_t humidity_format = ((_humidity*100)+(65534/2));
  uint8_t* temperature_formatter = (uint8_t*)&temperature_format;
  uint8_t* humidity_formatter = (uint8_t*)&humidity_format;
  _data[0] = temperature_formatter[0];
  _data[1] = temperature_formatter[1];
  //_data[2] = temperature_formatter[2];
  //_data[3] = temperature_formatter[3];
  _data[2] = humidity_formatter[0];
  _data[3] = humidity_formatter[1];
  //_data[6] = humidity_formatter[2];
  //_data[7] = humidity_formatter[3];
  return true;
}

float DHT_Sensor::get_temperature()
{
  this->update_data();
  return _temperature;
}

float DHT_Sensor::get_humidity()
{
  this->update_data();
  return _humidity;
}

uint8_t* DHT_Sensor::get_data()
{
  this->update_data();
  return _data;
}

uint8_t* DHT_Sensor::get_data(uint8_t* packet, int* index)
{
  this->update_data();
  for(int i = 0; i < 4; i++)
  {
    packet[*index] = _data[i];
    (*index)++;
  }

  return _data;
}
