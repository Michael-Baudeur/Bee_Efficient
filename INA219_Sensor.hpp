#ifndef INA219_SENSOR_H
#define INA219_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include "DFRobot_INA219.h"
#include "Rules.hpp"

#define FULL_SUN_CURRENT_mA 320

class INA219_Sensor
{
  public:
    INA219_Sensor();
    bool module_setup(uint32_t max_attempts);
    void calibrate();
    double bus_voltage();
    double shunt_voltage();
    double current();
    double power();
    double light_percent();
    uint8_t* get_data();
    uint8_t* get_data(uint8_t* packet, int* index);

  private:
    DFRobot_INA219_IIC* _module;
    float _ina219Reading_mA;
    float _extMeterReading_mA;
    double _bus_voltage;
    double _shunt_voltage;
    double _current;
    double _power;
    float _light;
    double _full_sun_current;
    uint8_t _data[4];

};

#endif //INA219_SENSOR_H