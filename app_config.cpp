#include <Arduino.h>
#include "config.h"
#include "http.h"
#include "macros.h"

const char APP_CONFIG_RESPONSE_HTML[] PROGMEM = R"=====(
<html>
  <head>
    <meta charset="utf-8"/>
  </head>
  <body>
    <h1>ESP8266</h1>
    <h2>Application configuration successfully updated</h2>
    Please wait for the device to be rebooted.
  </body>
</html>
)=====";

const char APP_CONFIG_HTML[] PROGMEM = R"=====(
<html>
  <head>
    <meta charset="utf-8"/>
    <script src="/assets/jquery-3.2.1.min.js"></script>  
    <script>
      $(function() {
        $.getJSON("app_config.json", function( data ) {
          $("input[name='autodstbox']").prop("checked", data.auto_dst);         
          $("input[name='ntpint']").val(data.ntp_update_interval);         
          $("input[name='tz']").val(data.time_zone);                  
          $("input[name='ntp']").val(data.ntp_server_name);         
        });        
      });
    </script>
  </head>
  <body>
    <h1>ESP8266</h1>
    <h2>Application configuration</h2>
    <form action="/app_config" method="POST">
      <table>
        <tr><td colspan="2"><b>Clock settings:</b><td></tr>
        <tr><td>Auto DST:</td><td><input name="autodstbox" type="checkbox" value="1"></td></tr>
        <tr><td>NTP update interval:</td><td><input name="ntpint" type="number" required></td></tr>
        <tr><td>Timezone:</td><td><input name="tz" type="number" required></td></tr>
        <tr><td>NTP server:</td><td><input name="ntp" maxlength="32" required></td></tr>
      </table>
      <br>
      <input type="submit" value="save configuration and reboot">
    </form>
    <form action="/" method="GET">
        <input type="submit" value="cancel">
    </form>
  </body>
</html>
)=====";    

// set application default settings
void app_config_init() {  
  config.app_config.auto_dst = true;
  config.app_config.ntp_update_interval = 3600;
  config.app_config.time_zone = 1;
  strncpy(config.app_config.ntp_server, FSTR("ntp1.ptb.de"), 32);  
}

// print application settings for debugging purposes
void app_config_print() {  
  Serial.printf(FSTR("Auto DST:            %d\n"), config.app_config.auto_dst ? FSTR("true") : FSTR("false"));
  Serial.printf(FSTR("NTP update interval: %ld sec\n"), config.app_config.ntp_update_interval);
  Serial.printf(FSTR("Timezone:            %ld\n"), config.app_config.time_zone);
  Serial.printf(FSTR("ntp server:          %s\n"), config.app_config.ntp_server);
}

// send application config as JSON
void app_config_send_json() {
  char json[150];
  sprintf((char*)&json, 
    FSTR("{\"auto_dst\": %s,\"ntp_update_interval\": %d, \"time_zone\": %d, \"ntp_server_name\": \"%s\"}"),
    config.app_config.auto_dst ? FSTR("true") : FSTR("false"),
    config.app_config.ntp_update_interval,
    config.app_config.time_zone,
    config.app_config.ntp_server);
  http_server.send(200, APPLICATION_JSON, json);
}

// persist application settings
void app_config_persist() {
  config.app_config.auto_dst = http_server.arg(FSTR("autodstbox")) == FSTR("1");
  config.app_config.ntp_update_interval = http_server.arg(FSTR("ntpint")).toInt();
  config.app_config.time_zone = http_server.arg(FSTR("tz")).toInt();
  strncpy(config.app_config.ntp_server, http_server.arg(FSTR("ntp")).c_str(), 32);
  config_write();
}



