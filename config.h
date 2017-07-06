#include <Arduino.h>
#include "app_config.h"

struct strConfig {

  // Generic settings
  char wifi_ssid[33];          
  char wifi_password[33];      
  boolean use_dhcp;        
  byte ip[4];             
  byte netmask[4];        
  byte gateway[4];        
  char device_name[33];
  char device_password[33];

  strAppConfig app_config;
    
}; 

void config_write();
boolean config_read();
void config_print();
void config_init();
void config_wipe();
void config_setup();

extern struct strConfig config;

void config_set_initial_config(const char* ssid, const char* wifipass, const char* devpass);
void config_set_regular_config(const char* wifi_ssid, const char* wifi_password, const char* device_name, const char* device_password, 
  const bool use_dhcp, 
  const byte ip1, const byte ip2, const byte ip3, const byte ip4,
  const byte gw1, const byte gw2, const byte gw3, const byte gw4,
  const byte mask1, const byte mask2, const byte mask3, const byte mask4);
