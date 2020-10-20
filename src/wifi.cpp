#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "config.h"
#include "macros.h"

// connect to AP for regular operation
void wifi_connect(void (*callback)(void)) {
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.printf(FSTR("Connecting to SSID \"%s\" with given password...\n"), WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
      callback();
      delay(500);
    }
  } else {
    Serial.printf(FSTR("Already connected to SSID \"%s\""), WIFI_SSID);
  }  
 
  Serial.printf(FSTR("\nConnected to SSID \"%s\" with IPv4 address "), WIFI_SSID);
  Serial.println(WiFi.localIP());
}



