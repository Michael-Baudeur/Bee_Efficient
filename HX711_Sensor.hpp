#ifndef HX711_SENSOR_H
#define HX711_SENSOR_H

#include <Arduino.h>
#include <string>
#include "Rules.hpp"
#include <HX711.h>

using namespace std;

#define SERIAL_INPUT 1
#define DEFAULT_CALIBRATION 2

class HX711_Sensor
{
  public:
    HX711_Sensor(const int LOADCELL_DOUT_PIN, const int LOADCELL_SCK_PIN);
    void module_setup();
    float get_mass();
    float get_prev_mass();
    void tare();
    void power_off();
    void power_on();
    bool calibrate(uint8_t mode = 0);
    bool calibrate_end(int mode = 0);
    void set_known_weight(float known_weight);
    uint8_t* get_data();
    uint8_t* get_data(uint8_t* packet, int* index);


  private:
    bool update_raw_data();
    bool update_tared_raw_data(uint32_t nb_samples);
    bool update_average_mass(uint32_t nb_samples);

    HX711 _module;
    const int _LOADCELL_DOUT_PIN;
    const int _LOADCELL_SCK_PIN;
    long _raw_data;
    double _tared_raw_data;
    float _mass;
    float _prev_mass;
    float _scale_factor;
    bool _calibrating;
    float _known_weight;
    uint8_t _data[4];
};

#endif //HX711_SENSOR_H
