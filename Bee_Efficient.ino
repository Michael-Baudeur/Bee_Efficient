//External files includes******************************************************
#include "DHT_Sensor.hpp"
#include "HX711_Sensor.hpp"
#include "LoRa_E5.hpp"
#include "INA219_Sensor.hpp"
#include "Battery_Monitor.hpp"
#include "Microphone.hpp"
#include "Rules.hpp"

//*****************************************************************************

//PIN DEFINES******************************************************************
#define DHT_SENSOR_INT_PIN 6
#define DHT_SENSOR_EXT_PIN 7

#define DS18B20_SENSOR_1_PIN 5
#define DS18B20_SENSOR_2_PIN 5

#define HX711_SENSOR_DOUT_PIN 3
#define HX711_SENSOR_SCK_PIN 2

//#define PWR_LED_PIN 10

#define BATTERY_MONITOR_PIN A0 //Analog

#define MICROPHONE_PIN A3 //Analog

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



//Function prototypes
float ds1_temperature();
uint16_t ds1_temp_data[2];

//*****************************************************************************



//MODULE DECLARATIONS**********************************************************
DHT_Sensor dht_ext(DHT_SENSOR_EXT_PIN);
DHT_Sensor dht_int(DHT_SENSOR_INT_PIN);
HX711_Sensor Scale(HX711_SENSOR_DOUT_PIN, HX711_SENSOR_SCK_PIN);
LoRa_E5 E5;
INA219_Sensor SP_Current_Sensor;
Battery_Monitor batt(BATTERY_MONITOR_PIN, BATTERY_FULL_CHARGE_VOLTAGE_V, BATTERY_CUTOFF_VOLTAGE_V, BATTERY_VOLTAGE_BRIDGE_RATIO_DIVIDER);
Microphone microphone(MICROPHONE_PIN, 586);
uint8_t data_packet[43];


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
  //pinMode(PWR_LED_PIN, OUTPUT);
  digitalWrite(LED_PWR, HIGH);  
  //digitalWrite(PWR_LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PWR, LOW);
  //digitalWrite(PWR_LED_PIN, LOW);
  //setup
  //Serial.println("SETUP BEGIN");
  pinMode(SWITCH_LOAD, OUTPUT);
  digitalWrite(SWITCH_LOAD, HIGH);
  delay(3000);
  SP_Current_Sensor.module_setup(5);
  dht_ext.setup();
  dht_int.setup();
  uint8_t init_temp;
  ds1_temperature(&init_temp, 0);
  ds1_temp_data[0] = (65534/2);
  ds1_temp_data[1] = (65534/2);
  batt.setup();
  microphone.setup();
  Scale.module_setup();
  
  //E5.module_setup();
  //E5.factory_reset();
  E5.module_setup();
  E5.connect(5);

  

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
  if(batt.get_voltage() > 3.31)
  {
    digitalWrite(SWITCH_LOAD, HIGH);
    go_high();
    //delay(5000);
    dht_ext.setup();
    dht_int.setup();
    int index = 0;
    delay(1000);
    SP_Current_Sensor.module_setup(1);
    dht_ext.get_data(data_packet, &index);
    dht_int.get_data(data_packet, &index);
    //delay(1000);
    ds1_temperature(data_packet, &index);
    //delay(2000);
    Scale.power_on();
    delay(500);
    Scale.module_setup();
    Scale.get_data(data_packet, &index);
    Scale.power_off();

    SP_Current_Sensor.get_data(data_packet, &index);
    batt.get_data(data_packet, &index);
    microphone.get_data(data_packet, &index);

    go_low();
    E5.low_power_OFF();
    delay(1000);
    digitalWrite(SWITCH_LOAD, LOW);
    E5.connect(10, LORA_JOIN_FORCE);
    //E5.positioning();
    E5.module_send_8(data_packet, 43);
    E5.low_power_ON();
  } 
  delay(1000); //wait 10 minutes
} 
//*****************************************************************************




//LOW POWER FUNCTIONS**********************************************************
void go_low()
{
  digitalWrite(LED_PWR, LOW);
  digitalWrite(PIN_ENABLE_SENSORS_3V3, LOW);
  digitalWrite(PIN_ENABLE_I2C_PULLUP, LOW);
}

void go_high()
{
  digitalWrite(LED_PWR, HIGH);
  digitalWrite(PIN_ENABLE_SENSORS_3V3, HIGH);
  digitalWrite(PIN_ENABLE_I2C_PULLUP, HIGH);  
}
//*****************************************************************************









//MaximWire DS18B20 Functions**************************************************
float ds1_temperature(uint8_t* packet, int* index)
{
  float temperature = 0;
  int array_index = 0;
  MaximWire::Bus bus_ds1(PIN_BUS_DS1);
  MaximWire::DS18B20 device;

  MaximWire::Discovery discovery = bus_ds1.Discover();
  do
  {
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
        temperature += temp;
        #ifdef PRINT_ENABLE
        Serial.print(" temp=");
        Serial.print(temp);
        Serial.println();
        #endif
        device.Update(bus_ds1);
        if(!isnan(temp))
        {
          uint16_t temp_format = (temp*100)+(65534/2);
          ds1_temp_data[array_index] = temp_format;
        }
        array_index++;
        
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
    
  }while(discovery.HaveMore());

  uint8_t* temp_formatter = (uint8_t*)ds1_temp_data;
  for(int i = 0; i < 4; i++)
  {
    packet[*index] = temp_formatter[i];
    (*index)++;
  }
  return temperature/2;
}
//*****************************************************************************