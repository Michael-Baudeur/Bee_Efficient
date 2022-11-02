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
  
  _data[0] = _temperature;
  _data[1] = _humidity;
  return true;
}

float DHT_Sensor::get_temperature()
{
  return _temperature;
}

float DHT_Sensor::get_humidity()
{
  return _humidity;
}

uint32_t* DHT_Sensor::get_data()
{
  return _data;
}
