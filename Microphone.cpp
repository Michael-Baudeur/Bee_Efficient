#include "Microphone.hpp"

Microphone::Microphone(int MICROPHONE_ADC_PIN, float max_frequency_Hz)
{
  _FFT = arduinoFFT();
  _ADC_PIN = MICROPHONE_ADC_PIN;
  _samples_N = 1024;
  _sampling_frequency = 2*max_frequency_Hz;
}

void Microphone::setup()
{
  pinMode(_ADC_PIN, INPUT);
  _sampling_period_us = round(1000000*(1.0/_sampling_frequency));
}

void Microphone::compute_averages()
{
  #ifdef PRINT_ENABLE
  Serial.println("Microphone amps : ");
  #endif
  int freq = 96; //Hz
  for(int byte_number = 0; byte_number < 10; byte_number++)
  {
    int sum = 0;
    for(int i = this->freq_to_index(freq); i < this->freq_to_index(freq+49); i++)
    {
      sum += _vReal[i];
    }
    float amp = sum/49;
    #ifdef PRINT_ENABLE
    Serial.println(amp);
    #endif
    _data[byte_number] = (amp/15000.0)*255;
    freq += 49;
  }
  #ifdef PRINT_ENABLE
  Serial.println("Microphone bytes : ");
  for(int k = 0; k < 10; k++)
  {
    Serial.println(_data[k]);
  }
  #endif
}

void Microphone::calc_FFT()
{
  _microseconds = micros();
  for(int i = 0; i < _samples_N; i++)
  {
      _vReal[i] = analogRead(_ADC_PIN);
      _vImag[i] = 0;
      while(micros() - _microseconds < _sampling_period_us){} //empty loop
      _microseconds += _sampling_period_us;
  }
  /* Print the results of the sampling according to time */
  _FFT.Windowing(_vReal, _samples_N, FFT_WIN_TYP_BLACKMAN_HARRIS, FFT_FORWARD);	/* Weigh data */
  _FFT.Compute(_vReal, _vImag, _samples_N, FFT_FORWARD); /* Compute FFT */
  _FFT.ComplexToMagnitude(_vReal, _vImag, _samples_N); /* Compute magnitudes */

  this->compute_averages();
}

uint8_t* Microphone::get_data()
{
  this->calc_FFT();
  return _data;
}

uint8_t* Microphone::get_data(uint8_t* packet, int* index)
{
  this->calc_FFT();
  for(int i = 0; i < 10; i++)
  {
    packet[*index] = _data[i];
    (*index)++;
  }
  return _data;
}

int Microphone::freq_to_index(int freq)
{
  return ((freq/(_sampling_frequency/2))*(_samples_N/2));
}