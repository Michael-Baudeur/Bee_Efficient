#include "Battery_Monitor.hpp"

Battery_Monitor::Battery_Monitor(int ADC_pin, float battery_full_charge_voltage, float battery_cutoff_voltage, float voltage_bridge_ratio)
{
  _ADC_pin = ADC_pin;
  _battery_full_charge_voltage = battery_full_charge_voltage;
  _battery_cutoff_voltage = battery_cutoff_voltage;
  _voltage_bridge_ratio = voltage_bridge_ratio;
}

void Battery_Monitor::setup()
{
  pinMode(_ADC_pin, INPUT);
}

float Battery_Monitor::get_charge()
{
  this->update();
  #ifdef PRINT_ENABLE
  Serial.print("Charge : ");
  Serial.print(_charge_percent);
  Serial.println("%");
  #endif
  return _charge_percent;
}

float Battery_Monitor::get_voltage()
{
  this->update();
  /*#ifdef PRINT_ENABLE
  Serial.print("Voltage : ");
  Serial.print(_voltage);
  Serial.println("V");
  #endif*/
  return _voltage;
}

void Battery_Monitor::update()
{
  _voltage = ((float)analogRead(_ADC_pin)/1023.0)*3.3*_voltage_bridge_ratio;
  _charge_percent = (_voltage - _battery_cutoff_voltage)/(_battery_full_charge_voltage - _battery_cutoff_voltage);
  #ifdef PRINT_ENABLE
  Serial.print("Voltage : ");
  Serial.print(_voltage);
  Serial.println("V");
  #endif

  uint16_t voltage_format = ((_voltage*100)+(65534/2));
  uint8_t* voltage_formatter = (uint8_t*)&voltage_format;
  _data[0] = voltage_formatter[0];
  _data[1] = voltage_formatter[1];
  //_data[2] = voltage_formatter[2];
  //_data[3] = voltage_formatter[3];
}

uint8_t* Battery_Monitor::get_data()
{
  this->update();
  return _data;
}

uint8_t* Battery_Monitor::get_data(uint8_t* packet, int* index)
{
  this->update();
  for(int i = 0; i < 2; i++)
  {
    packet[*index] = _data[i];
    (*index)++;
  }
  return _data;
}