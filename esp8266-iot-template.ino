#include "config.h"
#include "wifi.h"
#include "http.h"
#include "http_initial.h"
#include "http_regular.h"
#include "app_main.h"

void setup(void){

  pinMode(16, INPUT_PULLUP);  
  
  Serial.begin(115200);
  Serial.println(F("\nStarted"));

  config_setup();

  // if GPIO 16 is low, wipe EEPROM
  boolean config_reset = (digitalRead(16) == 0);
  if (config_reset) {
    config_wipe();
  }

  app_setup();

  // try to read config
  boolean config_ok = config_read();
  if (!config_ok) {
    Serial.println(F("No valid config found"));
    wifi_create_ap();     
    http_initial_setup();
  } else {    
    app_connecting();
    wifi_connect();  
    app_connected();
    http_regular_setup();
  } 
}

void loop(void){ 
  http_handle();
  app_loop();
}



