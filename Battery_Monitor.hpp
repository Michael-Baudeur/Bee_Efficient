#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <Arduino.h>
#include "Rules.hpp"

class Battery_Monitor
{
  public:
    Battery_Monitor(int ADC_pin, float battery_full_charge_voltage, float battery_cutoff_voltage, float voltage_bridge_ratio);
    void setup();
    float get_charge();
    float get_voltage();
    uint8_t* get_data();
    uint8_t* get_data(uint8_t* packet, int* index);
    

  private:
    void update();
    int _ADC_pin;
    float _voltage;
    float _charge_percent;
    float _battery_full_charge_voltage;
    float _battery_cutoff_voltage;
    float _voltage_bridge_ratio;
    uint8_t _data[4];
};

#endif //BATTERY_MONITOR_H