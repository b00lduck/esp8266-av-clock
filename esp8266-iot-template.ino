#include <ArduinoOTA.h>
#include "config.h"
#include "wifi.h"
#include "http.h"
#include "http_initial.h"
#include "http_regular.h"

void setup(void){

  pinMode(16, INPUT_PULLUP);  
  
  Serial.begin(115200);
  Serial.println("\nStarted");

  config_setup();

  boolean config_reset = (digitalRead(16) == 0);

  if (config_reset) {
    config_wipe();
  }
  
  boolean config_ok = config_read();

  if (!config_ok) {
    Serial.println("No valid config found");
    wifi_create_ap();     
    http_initial_setup();
  } else {
    wifi_connect();  
    http_regular_setup();

    ArduinoOTA.setHostname("esptest");
    ArduinoOTA.setPassword("");
    ArduinoOTA.onStart([]() {
      Serial.println("OTA start");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nOTA End");
      ESP.restart();
    });
    
    ArduinoOTA.onError([](ota_error_t error) { 
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("OTA Auth failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("OTA Begin failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("OTA Connect failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("OTA Receive failed");
        else if (error == OTA_END_ERROR) Serial.println("OTA End failed");
      });
    
    ArduinoOTA.begin();  
  } 
}

void loop(void){ 
  ArduinoOTA.handle();      
  http_handle();
}



