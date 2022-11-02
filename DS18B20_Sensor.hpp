#ifndef DS18B20_Sensor_h
#define DS18B20_Sensor_h

#include <Arduino.h>
#include "Rules.hpp"

#include <MaximWire.h>

class DS18B20_Sensor
{
  public:
    DS18B20_Sensor();
    DS18B20_Sensor(int PIN_BUS);
    bool connect();
    bool update_data();
    float get_temperature();

  private:
    MaximWire::Bus* _bus;
    MaximWire::DS18B20* _device;
    MaximWire::Address _address;
    MaximWire::Discovery* _discovery;
    float _temperature;
    bool _connected;

};

#endif