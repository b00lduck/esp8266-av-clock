#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "config.h"
#include "macros.h"

// create AP for initial configuration
void wifi_create_ap() {
  char ssid[32];
  sprintf(ssid, FSTR("ESP8266-%s"), String(ESP.getChipId(), HEX).c_str());
 
  Serial.printf(FSTR("Creating AP with SSID \"%s\"\n"), ssid);

  WiFi.mode(WIFI_AP);  
  WiFi.softAPConfig(
    IPAddress(192, 168, 100, 1),      
    IPAddress(192, 168, 100, 254), 
    IPAddress(255, 255, 255, 0));  
  WiFi.softAP(ssid, INITIAL_AP_PASSWORD);

  Serial.printf(FSTR("Created AP with SSID \"%s\", password \"%s\" and IPv4 address "), ssid, INITIAL_AP_PASSWORD);
  Serial.println(WiFi.softAPIP());
}

// connect to AP for regular operation
void wifi_connect() {
  Serial.printf(FSTR("Connecting to SSID \"%s\"...\n"), config.wifi_ssid);
  
  WiFi.begin(config.wifi_ssid, config.wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  if (!config.use_dhcp) {
    WiFi.config(
      IPAddress(config.ip[0], config.ip[1], config.ip[2], config.ip[3] ),  
      IPAddress(config.gateway[0], config.gateway[1], config.gateway[2], config.gateway[3]), 
      IPAddress(config.netmask[0], config.netmask[1], config.netmask[2], config.netmask[3]));
  }
  
  Serial.printf(FSTR("\nConnected to SSID \"%s\" with IPv4 address "), config.wifi_ssid);
  Serial.println(WiFi.localIP());
}

