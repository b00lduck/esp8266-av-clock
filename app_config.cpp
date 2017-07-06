#include <Arduino.h>
#include "app_config.h"
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
          $("input[name='iscoolbox']").prop("checked", data.is_cool_app);         
        });        
      });
    </script>
  </head>
  <body>
    <h1>ESP8266</h1>
    <h2>Application configuration</h2>
    <form action="/app_config" method="POST">
      <table>
        <tr><td colspan="2"><b>Generic application settings:</b><td></tr>
        <tr><td>Application is cool:</td><td><input name="iscoolbox" type="checkbox" value="1"></td></tr>
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
  config.app_config.is_cool_app = true;
}

// print application settings for debugging purposes
void app_config_print() {  
  Serial.printf(FSTR("isCoolApp:      %s\n"), config.app_config.is_cool_app ? FSTR("true") : FSTR("false"));  
}

// send application config as JSON
void app_config_send_json() {
  char json[50];
  sprintf((char*)&json, 
    FSTR("{\"is_cool_app\": %s}"),
    config.app_config.is_cool_app ? FSTR("true") : FSTR("false"));
  http_server.send(200, APPLICATION_JSON, json);
}

// persist application settings
void app_config_persist() {
  config.app_config.is_cool_app = http_server.arg(FSTR("iscoolbox")) == FSTR("1");
  config_write();
}

