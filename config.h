#include <Arduino.h>

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
  
}; 

void config_write();
boolean config_read();
void config_print();
void config_init();
void config_wipe();
void config_setup();

extern struct strConfig config;

void config_set_initial_config(const char* ssid, const char* password);
void config_set_regular_config(const char* wifi_ssid, const char* wifi_password, const char* device_name, const char* device_password);
