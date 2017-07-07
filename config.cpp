#include <EEPROM.h>
#include "config.h"
#include "macros.h"

#define EEPROM_SIZE 512

struct strConfig config;

// Initialize EEPROM
void config_setup() {
    EEPROM.begin(EEPROM_SIZE); 
}

// Initialize EEPROM with default config data
void config_init() {
  Serial.println(F("Initializing new configuration..."));
  memset(&config, 0, sizeof(config));  
  config.use_dhcp = true;
  sprintf(config.device_name, FSTR("ESP8266-%s"), String(ESP.getChipId(), HEX).c_str());
  app_config_init();
  Serial.println(F("New configuration initialized!"));
}

// Erase EEPROM
void config_wipe() {
  Serial.println(F("Purging configuration..."));
  memset(&config, 0, sizeof(config));  
  EEPROM.put(0, FSTR("   "));
  EEPROM.put(4, config);
  EEPROM.commit();
}

// Write config to EEPROM
void config_write() {
  Serial.println(F("Writing configuration..."));  
  EEPROM.put(0, EEPROM_MAGIC);
  EEPROM.put(4, config);
  EEPROM.commit();
  Serial.println(F("Wrote configuration!"));  
}

// Read config from EEPROM
boolean config_read() {
  Serial.println(F("Reading configuration..."));
  char magic[4];
  EEPROM.get(0, magic);
  if (strncmp(magic, EEPROM_MAGIC, 3) != 0) {
    Serial.println(F("Configuration magic string not found, config not loaded!"));    
    Serial.printf(FSTR("Magic string was \"%s\"\n"), magic);    
    return false;
  }
  EEPROM.get(4,config);
  // TODO: CRC check
  config_print();  
  return true;
}

// Save initial config
void config_set_initial_config(const char* ssid, const char* wifipass, const char* devpass) {
  config_init();
  strncpy(config.wifi_ssid, ssid, 32);
  strncpy(config.wifi_password, wifipass, 32);
  strncpy(config.device_password, devpass, 32);
  config_print();  
  config_write();
}

// Save initial config
void config_set_regular_config(const char* wifi_ssid, const char* wifi_password, const char* device_name, const char* device_password, const bool use_dhcp,
  const byte ip1, const byte ip2, const byte ip3, const byte ip4,
  const byte gw1, const byte gw2, const byte gw3, const byte gw4,
  const byte mask1, const byte mask2, const byte mask3, const byte mask4) {    
  strncpy(config.wifi_ssid, wifi_ssid, 32);
  strncpy(config.wifi_password, wifi_password, 32);
  strncpy(config.device_name, device_name, 32);
  strncpy(config.device_password, device_password, 32);
  config.use_dhcp = use_dhcp;
  if (use_dhcp) {
    memset(&config.ip, 0, 4);  
    memset(&config.gateway, 0, 4);  
    memset(&config.netmask, 0, 4);  
  } else {
    config.ip[0] = ip1;
    config.ip[1] = ip2;
    config.ip[2] = ip3;
    config.ip[3] = ip4;
    config.gateway[0] = gw1;
    config.gateway[1] = gw2;
    config.gateway[2] = gw3;
    config.gateway[3] = gw4;
    config.netmask[0] = mask1;
    config.netmask[1] = mask2;
    config.netmask[2] = mask3;
    config.netmask[3] = mask4;  
  }
  config_print();
  config_write();
}

// Write current config to serial console
void config_print() {
  Serial.println(F("Current device configuration:"));
  Serial.printf(FSTR("SSID:               %s\n"), config.wifi_ssid);
  Serial.printf(FSTR("WiFi password:      %s\n"), config.wifi_password);
  Serial.printf(FSTR("DHCP:               %s\n"), config.use_dhcp ? "true" : "false");
  Serial.printf(FSTR("IP:                 %d.%d.%d.%d\n"), config.ip[0],config.ip[1],config.ip[2],config.ip[3]);
  Serial.printf(FSTR("Mask:               %d.%d.%d.%d\n"), config.netmask[0],config.netmask[1],config.netmask[2],config.netmask[3]);
  Serial.printf(FSTR("Gateway:            %d.%d.%d.%d\n"), config.gateway[0],config.gateway[1],config.gateway[2],config.gateway[3]);
  Serial.printf(FSTR("Device name:        %s\n"), config.device_name);
  Serial.printf(FSTR("Device password:    %s\n"), config.device_password);
  Serial.println();
  Serial.println(F("Current application configuration:"));
  app_config_print();
  Serial.println();
}


