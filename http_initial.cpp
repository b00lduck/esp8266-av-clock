#include <Arduino.h>

#include "http.h"
#include "config.h"


void http_initial_setup() {
  Serial.println("Setting up HTTP server for initial config");

  http_install_simple_get_handler("/", "/initial-index.html");

  /*
  http_server.on("/", HTTP_POST, []() {
    Serial.println("POST /init 200");       
    config_set_wifi_ssid(http_server.arg("ssid").c_str());
    config_set_wifi_password(http_server.arg("password").c_str());
    config_write();
    http_server.send_P (200, "text/html", http_initial_reboot_html); 
    ESP.restart();
  });
  */  

  http_setup();
}



