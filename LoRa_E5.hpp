#ifndef LoRa_E5_h
#define LoRa_E5_h

#include "Arduino.h"
#include "Rules.hpp"
#include <vector>
#include <string>
#include <regex>
#include <iostream>

#define LORA_JOIN_FORCE 1

using namespace std;

typedef struct coordinates
{
  double longitude;
  double latitude;
}coordinates_t;

class LoRa_E5
{
  public:
    LoRa_E5();
    
    bool module_setup();
    bool join(int mode = 0);
    bool connect(uint32_t max_join_attempts, int mode = 0);
    void module_send(uint8_t ID, uint8_t* data, uint32_t data_length);
    void module_send_8(uint8_t* data, uint32_t data_length);
    void low_power_ON();
    void low_power_OFF();
    void factory_reset();
    void positioning();
    int module_receive(uint8_t* data);
    vector<string> split(const string &s, const regex &sep_regex);

  private:
    char _recv_buf[512];
    int _downlink_data;
    bool _is_exist;
    bool _is_join;
    int _ret;
    coordinates_t _position;

    int at_send_check_response(char *p_ack, int timeout_ms, char *p_cmd, ...);
    
};

#endif
