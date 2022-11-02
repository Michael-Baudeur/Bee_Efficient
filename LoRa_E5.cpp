#include "LoRa_E5.hpp"

LoRa_E5::LoRa_E5()
{
  _is_exist = false;
  _is_join = false;
  _ret = 0;
}

int LoRa_E5::at_send_check_response(char *p_ack, int timeout_ms, char *p_cmd, ...) 
{ 
  int ch; 
  int num = 0; 
  int index = 0; 
  int startMillis = 0; 
  memset(_recv_buf, 0, sizeof(_recv_buf)); 
  Serial1.write(p_cmd); 
  #ifdef PRINT_ENABLE
  Serial.write(p_cmd); 
  #endif
  delay(200); 
  startMillis = millis(); 
  do 
  { 
    while (Serial1.available() > 0) 
    { 
      ch = Serial1.read(); 
      _recv_buf[index++] = ch;
      #ifdef PRINT_ENABLE 
      Serial.write(ch); 
      #endif
      delay(2); 
    } 
  } while (millis() - startMillis < timeout_ms); 
  if (strstr(_recv_buf, p_ack) != NULL) 
  { 
    return 1; 
  } 
  else return 0; 
}

bool LoRa_E5::module_setup()
{
  Serial1.begin(9600);
  #ifdef PRINT_ENABLE 
  Serial.print("E5 LORAWAN TEST\r\n"); 
  #endif
  if(at_send_check_response("+AT: OK", 100, "AT\r\n")) 
  { 
    _is_exist = true; 
    at_send_check_response("+ID: AppEui", 1000, "AT+ID\r\n"); 
    at_send_check_response("+MODE: LWOTAA", 1000, "AT+MODE=LWOTAA\r\n"); 
    at_send_check_response("+DR: EU868", 1000, "AT+DR=EU868\r\n"); 
    at_send_check_response("+CH: NUM", 1000, "AT+CH=NUM,0-2\r\n"); 
    at_send_check_response("+KEY: APPKEY", 1000, 
    "AT+KEY=APPKEY,\"CF6615B23A1D62DAF409D4FD072EA6F1\"\r\n"); 
    at_send_check_response("+KEY: DEVEUI", 1000, "AT+KEY=DEVEUI,\"2CF7F1203230288A\"\r\n"); 
    at_send_check_response("+KEY: APPEUI", 1000, "AT+KEY=APPEUI,\"0000000000000000\"\r\n"); 
    at_send_check_response("+CLASS: C", 1000, "AT+CLASS=A\r\n"); 
    _ret=at_send_check_response("+PORT: 8", 1000, "AT+PORT=8\r\n"); 
    delay(200); 
    _is_join = true;
    return true; 
  } 
  _is_exist = false; 
  #ifdef PRINT_ENABLE
  Serial.print("No E5 module found.\r\n");
  #endif
  return false; 
}

bool LoRa_E5::join()
{
  if (_is_exist) 
  { 
    int ret = 0; 
    if (_is_join) 
    { 
      ret = at_send_check_response("+JOIN: Network joined", 12000, "AT+JOIN\r\n"); 
      if (ret) 
      { 
        _is_join = false; 
        #ifdef PRINT_ENABLE 
        Serial.println();
        Serial.print("Network JOIN !\r\n\r\n"); 
        #endif
        return true;
      } 
      else 
      { 
        at_send_check_response("+ID: AppEui", 1000, "AT+ID\r\n");
        #ifdef PRINT_ENABLE 
        Serial.println(); 
        Serial.print("JOIN failed!\r\n\r\n"); 
        #endif
        return false;
        delay(5000); 
      } 
    } 
  } 
  else 
  { 
    return false;
  } 
}

void LoRa_E5::module_send(uint8_t ID, uint8_t* data, uint32_t data_length)
{
  char cmd[128];
  sprintf(cmd, "AT+MSGHEX=%02X", ID);
  for(int i = 0; i < data_length + 1; i++)
  {
    char byteCat[2];
    sprintf(byteCat, "%02X", data[i]); 
    strcat(cmd, byteCat);
  }
  strcat(cmd, "\r\n"); 
  this->at_send_check_response("ACK Received", 5000, cmd); 
  delay(20000);
}

void LoRa_E5::module_send_8(uint8_t* data, uint32_t data_length)
{
  char cmd[128];
  sprintf(cmd, "AT+MSGHEX=");
  for(int i = 0; i < data_length + 1; i++)
  {
    char byteCat[2];
    sprintf(byteCat, "%02X", data[i]); 
    strcat(cmd, byteCat);
  }
  strcat(cmd, "\r\n"); 
  this->at_send_check_response("ACK Received", 5000, cmd); 
  delay(20000);
}
