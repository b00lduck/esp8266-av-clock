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

  // Application settings
  boolean auto_dst;          
  long ntp_update_interval;
  long time_zone;         
  char ntp_server[2][33];  
  
}; 

void config_write();
boolean config_read();
void config_print();
void config_init();

extern struct strConfig config;

void config_set_wifi_ssid(const char*);
void config_set_wifi_password(const char*);

