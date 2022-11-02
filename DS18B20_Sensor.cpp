#include "DS18B20.h"
#include "DS18B20_Sensor.hpp"

DS18B20_Sensor::DS18B20_Sensor() 
  : _temperature(0), _connected(false)
{
  _bus = new MaximWire::Bus(2);
}

DS18B20_Sensor::DS18B20_Sensor(int PIN_BUS) 
  : _temperature(0), _connected(false)
{
  _bus = new MaximWire::Bus(PIN_BUS);
}

bool DS18B20_Sensor::connect()
{
  *_discovery = _bus->Discover();
  if(_discovery->FindNextDevice(_address))
  {
    #ifdef PRINT_ENABLE
    Serial.print("FOUND: ");
    Serial.print(_address.ToString());
    #endif
    if(_address.IsValid())
    {
      #ifdef PRINT_ENABLE
      Serial.print("(VALID)");
      #endif
      if(1)//_address.GetModelCode() == MaximWire::DS18B20::MODEL_CODE)
      {
        #ifdef PRINT_ENABLE
        Serial.print("(DS18B20)");
        #endif
        _device = new MaximWire::DS18B20(_address);
        if(_device->IsParasitePowered(*_bus))
        {
          #ifdef PRINT_ENABLE
          Serial.print("(PARASITE POWER)");
          #endif
          return false;
        }
        _connected = true;
        return true;
      }
      
    }
    else
    {
      #ifdef PRINT_ENABLE
      Serial.print("(INVALID)");
      #endif
    }
  }
  else
  {
    #ifdef PRINT_ENABLE
    Serial.println("NOTHING FOUND");
    #endif
    
  }
  return false;
  
}

bool DS18B20_Sensor::update_data()
{
  if(_connected)
  {
    _temperature = _device->GetTemperature<float>(*_bus);
    #ifdef PRINT_ENABLE
    Serial.print(" temperature=");
    Serial.print(_temperature);
    Serial.println();
    #endif
    _device->Update(*_bus);
    return true;
  }
  return false;
}

float DS18B20_Sensor::get_temperature()
{
  return _temperature;
}