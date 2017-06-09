#include <Arduino.h>
#include "http.h"
#include "config.h"

const char* http_regular_index_html PROGMEM = R"=====(
  <h1>ESP8266</h1>
  <h2>Configuration</h2>
  
  <table>
    <tr><td>SSID:</td><td>%s</td></tr>
    <tr><td>PWD:</td><td>%s</td></tr>
    <tr><td>DHCP:</td><td>%s</td></tr>
    <tr><td>IP:</td><td>%s</td></tr>
    <tr><td>Mask:</td><td>%s</td></tr>
    <tr><td>Gateway:</td><td>%s</td></tr>
    <tr><td>Device Name:</td><td>%s</td></tr>
    <tr><td>Auto DST:</td><td>%s</td></tr>
    <tr><td>NTP update interval:</td><td>%s</td></tr>
    <tr><td>Timezone:</td><td>%s</td></tr>
    <tr><td>ntp server 1:</td><td>%s</td></tr>
    <tr><td>ntp server 2:</td><td>%s</td></tr>
  </table>
  
)=====";

void http_regular_setup() { 
  Serial.println("Setting up HTTP server for regular config");
  http_server.on ("/", []() {
    Serial.println("admin.html");
   http_server.send_P (200, "text/html", http_regular_index_html); 
  });

  http_setup();
}



