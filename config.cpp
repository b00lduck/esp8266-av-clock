#include "config.h"
#include <EEPROM.h>

#define EEPROM_SIZE 512
#define EEPROM_MAGIC "B00"

struct strConfig config;

void config_init() {
  Serial.println("Initializing new configuration...");
  memset(&config, 0, sizeof(config));  
  config.use_dhcp = true;
  
  sprintf(config.ntp_server[0], "ntp1.ptb.de");
  sprintf(config.ntp_server[1], "ntp2.ptb.de");
  
  config.time_zone = 1;
  config.ntp_update_interval = 3600;
  config.auto_dst = true;
  
  config_print();
}

void config_write() {
  Serial.println("Writing configuration...");
  EEPROM.put(0, EEPROM_MAGIC);
  EEPROM.put(4, config);
  EEPROM.commit();
}

boolean config_read() {
  Serial.println("Reading configuration...");
  EEPROM.begin(EEPROM_SIZE);  
  char magic[4];
  EEPROM.get(0, magic);
  if (strncmp(magic, EEPROM_MAGIC, 3) != 0) {
    Serial.println("Configuration magic string not found, config not loaded.");    
    return false;
  }
  EEPROM.get(4,config);
  // TODO: CRC check
  config_print();
  return true;  
}

void config_set_wifi_ssid(const char* ssid) {
  strncpy(config.wifi_ssid, ssid, 32);
}

void config_set_wifi_password(const char* password) {
  strncpy(config.wifi_password, password, 32);
}

// Write current config to serial console
void config_print() {
  Serial.println("Current configuration:");
  Serial.printf("SSID:               %s\n", config.wifi_ssid);
  Serial.printf("PWD:                %s\n", config.wifi_password); 
  Serial.printf("DHCP:               %d\n", config.use_dhcp);
  Serial.printf("IP:                 %d.%d.%d.%d\n", config.ip[0],config.ip[1],config.ip[2],config.ip[3]);
  Serial.printf("Mask:               %d.%d.%d.%d\n", config.netmask[0],config.netmask[1],config.netmask[2],config.netmask[3]);
  Serial.printf("Gateway:            %d.%d.%d.%d\n", config.gateway[0],config.gateway[1],config.gateway[2],config.gateway[3]);
  Serial.printf("Device Name:        %s\n", config.device_name);
  Serial.printf("Auto DST:           %d\n", config.auto_dst);
  Serial.printf("NTP update interval %ld sec\n", config.ntp_update_interval);
  Serial.printf("Timezone:           %ld\n", config.time_zone);
  Serial.printf("ntp server 1:       %s\n", config.ntp_server[0]);
  Serial.printf("ntp server 2:       %s\n", config.ntp_server[1]);
  Serial.println("");
}


