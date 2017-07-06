#pragma once
#include <Arduino.h>

#define EEPROM_MAGIC FSTR("B00")
#define INITIAL_AP_PASSWORD FSTR("1337")

struct strAppConfig {
  // add application settings here
  bool isCoolApp;    
}; 

void app_config_init(strAppConfig *appConfig);
void app_config_print(strAppConfig *appConfig);

void app_config_send_config_page(strAppConfig *appConfig);
void app_config_send_response_page(strAppConfig *appConfig);

