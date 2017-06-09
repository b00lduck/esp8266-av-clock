#include <Arduino.h>
#include "http.h"
#include "config.h"

const char http_initial_html[] PROGMEM = R"=====(
  <h1>ESP8266</h1>
  <h2>Initial configuration</h2>
  <form action="/" method="POST">
    SSID: <input name="ssid"> (max. 32 characters)<br>
    Password: <input name="password" type="password"> (max. 32 characters)<br>
    <input type="submit" value="save">
  </form>
)=====";

const char http_initial_reboot_html[] PROGMEM = R"=====(
  <h1>ESP8266</h1>
  <h2>Initial configuration was applied</h2>
  Please wait for the device to be rebooted.
)=====";

void http_initial_setup() {
  Serial.println("Setting up HTTP server for initial config");
  http_server.on("/", HTTP_GET, []() {
    Serial.println("GET / 200");
    http_server.send_P (200, "text/html", http_initial_html); 
  });
  http_server.on("/", HTTP_POST, []() {
    Serial.println("POST /init 200");       
    config_set_wifi_ssid(http_server.arg("ssid").c_str());
    config_set_wifi_password(http_server.arg("password").c_str());
    config_write();
    http_server.send_P (200, "text/html", http_initial_reboot_html); 
    ESP.restart();
  });  

  http_setup();
}



