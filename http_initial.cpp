#include <Arduino.h>

#include "http.h"
#include "config.h"


void http_initial_setup() {
  Serial.println("Setting up HTTP server for initial config");

  http_install_simple_get_handler("/", "/initial-index.html");

  http_server.on("/", HTTP_POST, []() {
    Serial.println("POST /init 200");       
    config_set_initial_config(http_server.arg("ssid").c_str(), http_server.arg("password").c_str());
    http_send_file("/initial-response.html");
    ESP.restart();
  });

  http_setup();
}



