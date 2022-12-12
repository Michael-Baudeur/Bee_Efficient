#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "Arduino.h"
#include "arduinoFFT.h"
#include "Rules.hpp"

#define SAMPLES_N 1024

class Microphone
{
  public:
    Microphone(int MICROPHONE_ADC_PIN, float max_frequency_Hz);
    void setup();
    void compute_averages();
    void calc_FFT();
    uint16_t* get_data();
    uint8_t* get_data(uint8_t* packet, int* index);
    

  private:
    int freq_to_index(int freq);

    int _ADC_PIN;
    arduinoFFT _FFT;
    uint16_t _samples_N;
    double _sampling_frequency;
    uint _sampling_period_us;
    uint16_t _data[10];
    int16_t _signal_sample[SAMPLES_N];
    double _vReal[SAMPLES_N];
    double _vImag[SAMPLES_N];
    unsigned long _microseconds;
    double _top_frequency;

};

#endif //MICROPHONE_H