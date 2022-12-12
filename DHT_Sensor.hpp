#ifndef DHT_Sensor_h
#define DHT_Sensor_h

#include "Arduino.h"
#include "Rules.hpp"

#include <DHT.h>
#include <DHT_U.h>

#define DHTTYPE DHT22

class DHT_Sensor
{
    public:
    DHT_Sensor();
    DHT_Sensor(int pin);
    void setup();
    bool update_data();
    float get_temperature();
    float get_humidity();
    uint8_t* get_data();
    uint8_t* get_data(uint8_t* packet, int* index);
    
  private:
    int _pin;
    float _temperature;
    float _humidity;
    uint8_t _data[4];
    DHT* _dht;
    

};

#endif
