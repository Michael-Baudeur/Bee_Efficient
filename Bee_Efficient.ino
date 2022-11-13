//External files includes******************************************************
#include "DHT_Sensor.hpp"
#include "HX711_Sensor.hpp"
#include "LoRa_E5.hpp"
#include "INA219_Sensor.hpp"
#include "Battery_Monitor.hpp"
#include "Rules.hpp"

//*****************************************************************************

//PIN DEFINES******************************************************************
#define DHT_SENSOR_1_PIN 2
#define DHT_SENSOR_2_PIN 4

#define DS18B20_SENSOR_1_PIN 3
#define DS18B20_SENSOR_2_PIN 5

#define HX711_SENSOR_DOUT_PIN 7
#define HX711_SENSOR_SCK_PIN 6

#define PWR_LED_PIN 10

#define BATTERY_MONITOR_PIN A0 //Analog

#define MICROPHONE_PIN 2 //Analog

#define SWITCH_LOAD 9
//*****************************************************************************

//ELECTRICAL CONSTANTS*********************************************************
#define BATTERY_FULL_CHARGE_VOLTAGE_V 4.2
#define BATTERY_CUTOFF_VOLTAGE_V 3.0
#define BATTERY_VOLTAGE_BRIDGE_RATIO_DIVIDER 2
//*****************************************************************************


//MaximWire DS18B20 dependencies***********************************************
#define MAXIMWIRE_EXTERNAL_PULLUP

#include <MaximWire.h>

//PINOUT
#define PIN_BUS_DS1 DS18B20_SENSOR_1_PIN
#define PIN_BUS_DS2 DS18B20_SENSOR_2_PIN


//Objects declaration
MaximWire::Bus bus_ds1(PIN_BUS_DS1);
MaximWire::Bus bus_ds2(PIN_BUS_DS2);
MaximWire::DS18B20 device;

//Function prototypes
float ds1_temperature();
float ds2_temperature();

//*****************************************************************************



//MODULE DECLARATIONS**********************************************************
DHT_Sensor dht_ext(DHT_SENSOR_1_PIN);
DHT_Sensor dht_int(DHT_SENSOR_2_PIN);
HX711_Sensor Scale(HX711_SENSOR_DOUT_PIN, HX711_SENSOR_SCK_PIN);
LoRa_E5 E5;
INA219_Sensor SP_Current_Sensor;
Battery_Monitor batt(BATTERY_MONITOR_PIN, BATTERY_FULL_CHARGE_VOLTAGE_V, BATTERY_CUTOFF_VOLTAGE_V, BATTERY_VOLTAGE_BRIDGE_RATIO_DIVIDER);
uint8_t data_packet[45];


//*****************************************************************************

//EXECUTE**********************************************************************
void setup() {
  // put your setup code here, to run once:
  #ifdef PRINT_ENABLE
  //delay(5000);
  Serial.begin(9600);
  //delay(1000);
  Serial.println("Launching Hive...");
  //delay(5000);
  #endif
  pinMode(MICROPHONE_PIN, INPUT);
  pinMode(PWR_LED_PIN, OUTPUT);
  digitalWrite(PWR_LED_PIN, HIGH);
  delay(1000);
  digitalWrite(PWR_LED_PIN, LOW);
  //setup
  //Serial.println("SETUP BEGIN");
  pinMode(SWITCH_LOAD, OUTPUT);
  digitalWrite(SWITCH_LOAD, HIGH);
  delay(1000);
  SP_Current_Sensor.module_setup(5);
  dht_ext.setup();
  dht_int.setup();
  batt.setup();
  Scale.module_setup();
  E5.module_setup();
  E5.connect(1);

  

  //Serial.println("SETUP END");
  //Serial.println("CALIBRATE BEGIN");
  SP_Current_Sensor.calibrate();
  Scale.calibrate(DEFAULT_CALIBRATION);
  //Scale.calibrate_end();
  //Serial.println("CALIBRATE END");
  //Serial.println("RUN");
}


void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(SWITCH_LOAD, HIGH);
  int index = 0;
  dht_ext.get_data(data_packet, &index);
  Serial.println(index);
  dht_int.get_data(data_packet, &index);
Serial.println(index);
  ds1_temperature(data_packet, &index);
  Serial.println(index);
  ds2_temperature(data_packet, &index);
  Serial.println(index);
  Scale.power_on();
  delay(500);
  Scale.get_data(data_packet, &index);
  Serial.println(index);
  Scale.power_off();
  SP_Current_Sensor.get_data(data_packet, &index);
  Serial.println(index);
  batt.get_data(data_packet, &index);
  Serial.println(index);
  Serial.println("data packet : ");
  for(int i = 0; i < 45; i++)
  {
    Serial.println(data_packet[i]);
  }
  
  digitalWrite(SWITCH_LOAD, LOW);
  //E5.join(LORA_JOIN_FORCE);
  E5.module_send_8(data_packet, 45);
  delay(1000*5); //wait 10 minutes
}
//*****************************************************************************















//MaximWire DS18B20 Functions**************************************************
float ds1_temperature(uint8_t* packet, int* index)
{
  MaximWire::Discovery discovery = bus_ds1.Discover();
  MaximWire::Address address;
  if (discovery.FindNextDevice(address))
  {
    #ifdef PRINT_ENABLE
    Serial.print("FOUND: ");
    Serial.print(address.ToString());
    
    if (address.IsValid())
    {
      Serial.print(" (VALID)");
    } 
    else
    {
      Serial.print(" (INVALID)");
    }
    #endif

    if (address.GetModelCode() == MaximWire::DS18B20::MODEL_CODE)
    {
      #ifdef PRINT_ENABLE
      Serial.print(" (DS18B20)");
      #endif
      MaximWire::DS18B20 device(address);
      #ifdef PRINT_ENABLE
      if (device.IsParasitePowered(bus_ds1))
      {
        Serial.print(" (PARASITE POWER)");
      }
      #endif
      float temp = device.GetTemperature<float>(bus_ds1);
      #ifdef PRINT_ENABLE
      Serial.print(" temp=");
      Serial.print(temp);
      Serial.println();
      #endif
      device.Update(bus_ds1);

      uint32_t temp_format = temp*100;
      uint8_t* temp_formatter = (uint8_t*)&temp_format;
      for(int i = 0; i < 4; i++)
      {
        packet[*index] = temp_formatter[i];
        (*index)++;
      }
      return temp;
    }
    #ifdef PRINT_ENABLE
    else 
    {
      Serial.println();
    }
    #endif
  }
  #ifdef PRINT_ENABLE
  else
  {
    Serial.println("NOTHING FOUND");
  }
  #endif
}

float ds2_temperature(uint8_t* packet, int* index)
{
  MaximWire::Discovery discovery = bus_ds2.Discover();
  MaximWire::Address address;
  if (discovery.FindNextDevice(address))
  {
    #ifdef PRINT_ENABLE
    Serial.print("FOUND: ");
    Serial.print(address.ToString());
    
    if (address.IsValid())
    {
      Serial.print(" (VALID)");
    } 
    else
    {
      Serial.print(" (INVALID)");
    }
    #endif

    if (address.GetModelCode() == MaximWire::DS18B20::MODEL_CODE)
    {
      #ifdef PRINT_ENABLE
      Serial.print(" (DS18B20)");
      #endif
      MaximWire::DS18B20 device(address);
      #ifdef PRINT_ENABLE
      if (device.IsParasitePowered(bus_ds2))
      {
        Serial.print(" (PARASITE POWER)");
      }
      #endif
      float temp = device.GetTemperature<float>(bus_ds2);
      #ifdef PRINT_ENABLE
      Serial.print(" temp=");
      Serial.print(temp);
      Serial.println();
      #endif
      device.Update(bus_ds2);

      uint32_t temp_format = temp*100;
      uint8_t* temp_formatter = (uint8_t*)&temp_format;
      for(int i = 0; i < 4; i++)
      {
        packet[*index] = temp_formatter[i];
        (*index)++;
      }
      return temp;
    }
    #ifdef PRINT_ENABLE
    else 
    {
      Serial.println();
    }
    #endif
  }
  #ifdef PRINT_ENABLE
  else
  {
    Serial.println("NOTHING FOUND");
  }
  #endif 
}
//*****************************************************************************