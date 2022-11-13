#include "HX711_Sensor.hpp"

HX711_Sensor::HX711_Sensor(const int LOADCELL_DOUT_PIN, const int LOADCELL_SCK_PIN)
  : _LOADCELL_DOUT_PIN(LOADCELL_DOUT_PIN), _LOADCELL_SCK_PIN(LOADCELL_SCK_PIN)
{

}

void HX711_Sensor::module_setup()
{
  _module.begin(_LOADCELL_DOUT_PIN, _LOADCELL_SCK_PIN);
  #ifdef PRINT_ENABLE
  Serial.println("HX711 module setup.");
  #endif
}

float HX711_Sensor::get_mass()
{
  this->update_average_mass(10);
  return _mass;
}

float HX711_Sensor::get_prev_mass()
{
  return _prev_mass;
}

void HX711_Sensor::tare()
{
  _module.tare();
}

bool HX711_Sensor::calibrate(uint8_t mode)
{
  if(!_calibrating)
  {
    _module.set_scale();
    _module.tare();
    _calibrating = true;
    #ifdef PRINT_ENABLE
    Serial.println("Put a known weight on the scale.");
    if(mode == SERIAL_INPUT)
    {
      Serial.println("And enter its value.");
      Serial.print(">");
      string data;
      int i = 0;
      while(!Serial.available());
      while(Serial.available() > 0)
      {
        data[i] = Serial.read();
        i++;
      }
      float weight = stof(data);
      this->set_known_weight(weight);
      Serial.println(weight);
      this->calibrate_end();
    }
    #endif  
    if(mode == DEFAULT_CALIBRATION)
    {
      this->calibrate_end(DEFAULT_CALIBRATION);
    }
    return true;
  }
  #ifdef PRINT_ENABLE
  Serial.println("Scale already tared for calibration.");
  #endif
  return false;
}

bool HX711_Sensor::calibrate_end(int mode)
{
  if(_calibrating)
  {
    if(mode == DEFAULT_CALIBRATION)
    {
      _scale_factor = 1791474/69.1; //shitty approximation
    }
    else if(mode == SERIAL_INPUT)
    {
      _scale_factor = _module.get_units(10)/_known_weight;
    }
    _module.set_scale(_scale_factor);
    _calibrating = false;
    #ifdef PRINT_ENABLE
    Serial.println("Scale calibrated");
    #endif
    return true;
  }
  #ifdef PRINT_ENABLE
  Serial.println("Scale must be tared first");
  #endif
  return false;
}

void HX711_Sensor::power_off()
{
  _module.power_down();
  #ifdef PRINT_ENABLE
  Serial.println("HX711 ADC OFF.");
  #endif
}

void HX711_Sensor::power_on()
{
  _module.power_up();
  #ifdef PRINT_ENABLE
  Serial.println("HX711 ADC ON.");
  #endif
}

void HX711_Sensor::set_known_weight(float known_weight)
{
  _known_weight = known_weight;
}

bool HX711_Sensor::update_raw_data()
{
  if(_module.is_ready())
  {
    _raw_data = _module.read();
    #ifdef PRINT_ENABLE
    Serial.print("HX711 raw data : ");
    Serial.println(_raw_data);
    #endif
    return true;
  }
  else
  {
    #ifdef PRINT_ENABLE
    Serial.println("HX711 not found.");
    #endif
    return false;
  }
}

bool HX711_Sensor::update_tared_raw_data(uint32_t nb_samples)
{
  if(_module.is_ready())
  {
    _tared_raw_data = _module.get_value();
    #ifdef PRINT_ENABLE
    Serial.print("HX711 tared raw data : ");
    Serial.println(_tared_raw_data);
    #endif
    return true;
  }
  else
  {
    #ifdef PRINT_ENABLE
    Serial.println("HX711 not found.");
    #endif
    return false;
  }
}

bool HX711_Sensor::update_average_mass(uint32_t nb_samples)
{
  if(_module.is_ready())
  {
    _prev_mass = _mass;
    _mass = _module.get_units(nb_samples);
    #ifdef PRINT_ENABLE
    Serial.print("HX711 mass = ");
    Serial.println(_mass);
    #endif

    uint32_t mass_format = _mass*100;
    uint8_t* mass_formatter = (uint8_t*)&mass_format;
    _data[0] = mass_formatter[0];
    _data[1] = mass_formatter[1];
    _data[2] = mass_formatter[2];
    _data[3] = mass_formatter[3];

    return true;
  }
  else
  {
    #ifdef PRINT_ENABLE
    Serial.println("HX711 not found.");
    #endif
    return false;
  }
}

uint8_t* HX711_Sensor::get_data()
{
  this->update_average_mass(10);
  return _data;
}

uint8_t* HX711_Sensor::get_data(uint8_t* packet, int* index)
{
  this->update_average_mass(10);
  for(int i = 0; i < 4; i++)
  {
    packet[*index] = _data[i];
    (*index)++;
  }
  return _data;
}
