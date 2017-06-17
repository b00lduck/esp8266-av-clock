#include "config.h"
#include <EEPROM.h>

#define EEPROM_SIZE 512
#define EEPROM_MAGIC "B00"

struct strConfig config;

// Initialize EEPROM
void config_setup() {
    EEPROM.begin(EEPROM_SIZE); 
}

// Initialize EEPROM with default config data
void config_init() {
  Serial.println("Initializing new configuration...");
  memset(&config, 0, sizeof(config));  
  config.use_dhcp = true;
  sprintf(config.device_name, "ESP8266-%s", String(ESP.getChipId(), HEX).c_str());
  config_print();
}

// Erase EEPROM
void config_wipe() {
  Serial.println("Purging configuration...");
  memset(&config, 0, sizeof(config));  
  EEPROM.put(0, "   ");
  EEPROM.put(4, config);
  EEPROM.commit();
}

// Write config to EEPROM
void config_write() {
  Serial.println("Writing configuration...");  
  EEPROM.put(0, EEPROM_MAGIC);
  EEPROM.put(4, config);
  EEPROM.commit();
}

// Read config from EEPROM
boolean config_read() {
  Serial.println("Reading configuration...");
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

// Save initial config
void config_set_initial_config(const char* ssid, const char* password) {
  strncpy(config.wifi_ssid, ssid, 32);
  strncpy(config.wifi_password, password, 32);
  config_write();
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
  Serial.println("");
}


