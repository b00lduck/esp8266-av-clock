#pragma once
#include <Arduino.h>

#define EEPROM_MAGIC "B00"
#define INITIAL_AP_PASSWORD "1337"
#define ADMIN_USERNAME "admin"

extern const char APP_CONFIG_RESPONSE_HTML[];
extern const char APP_CONFIG_HTML[];

void app_config_init();
void app_config_print();
void app_config_persist();
void app_config_send_json();

// application settings
struct strAppConfig {  
  bool is_cool_app;    
}; 



