#include "INA219_Sensor.hpp"

INA219_Sensor::INA219_Sensor()
{
  _module = new DFRobot_INA219_IIC(&Wire, INA219_I2C_ADDRESS4);
  _ina219Reading_mA = 1000;
  _extMeterReading_mA = 1000;
  _full_sun_current = FULL_SUN_CURRENT_mA;
}

bool INA219_Sensor::module_setup(uint32_t max_attempts)
{
  for(int i = 0; i < max_attempts; i++)
  {
    if(_module->begin())
    {
      #ifdef PRINT_ENABLE
      Serial.println("INA219 Ready.");
      #endif
      return true;
    }
  }
  #ifdef PRINT_ENABLE
  Serial.println("INA219 begin failed.");
  #endif
}

void INA219_Sensor::calibrate()
{
  _module->linearCalibrate(_ina219Reading_mA, _extMeterReading_mA);
  #ifdef PRINT_ENABLE
  Serial.println("INA219 Calibrated.");
  #endif
}

double INA219_Sensor::bus_voltage()
{
  _bus_voltage = _module->getBusVoltage_V();
  
  #ifdef PRINT_ENABLE
  Serial.print("INA219 Bus voltage = ");
  Serial.print(_bus_voltage);
  Serial.println("V");
  #endif

  return _bus_voltage;
}

double INA219_Sensor::shunt_voltage()
{
  _shunt_voltage = _module->getShuntVoltage_mV();

  #ifdef PRINT_ENABLE
  Serial.print("INA219 Shunt voltage = ");
  Serial.print(_shunt_voltage);
  Serial.println("mV");
  #endif

  return _shunt_voltage;
}

double INA219_Sensor::current()
{
  _current = _module->getCurrent_mA();

  #ifdef PRINT_ENABLE
  Serial.print("INA219 Current = ");
  Serial.print(_current);
  Serial.println("mA");
  #endif

  return _current;
}

double INA219_Sensor::power()
{
  _power = _module->getPower_mW();

  #ifdef PRINT_ENABLE
  Serial.print("INA219 Power = ");
  Serial.print(_power);
  Serial.println("mW");
  #endif

  return _power;
}

double INA219_Sensor::light_percent()
{
  double current = this->current();
  _light = current*(0.9/_full_sun_current)*100;

  #ifdef PRINT_ENABLE
  Serial.print("Light percent : ");
  Serial.print(_light);
  Serial.println("%");
  #endif

  uint32_t light_format = _light*100;
  uint8_t* light_formatter = (uint8_t*)&light_format;
  _data[0] = light_formatter[0];
  _data[1] = light_formatter[1];
  _data[2] = light_formatter[2];
  _data[3] = light_formatter[3];

  return _light;
}

uint8_t* INA219_Sensor::get_data()
{
  this->light_percent();
  return _data;
}

uint8_t* INA219_Sensor::get_data(uint8_t* packet, int* index)
{
  this->light_percent();
  for(int i = 0; i < 4; i++)
  {
    packet[*index] = _data[i];
    (*index)++;
  }
  return _data;
}
