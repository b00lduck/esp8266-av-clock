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
    <h2>Application configuration successfully updated<h2>
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
      });
    </script>
  </head>
  <body>
    <h1>ESP8266</h1>
    <h2>Application configuration</h2>
    <form action="/app_config" method="POST">
      <table>
        <tr><td colspan="2"><b>Generic application settings:</b><td></tr>
        <tr><td>Application is cool:</td><td><input id="iscoolbox" name="iscoolbox" type="checkbox" value="1" %ISCOOL%></td></tr>
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

void app_config_init(strAppConfig *appConfig) {
  // set default settings here
  appConfig->isCoolApp = true;
}

void app_config_print(strAppConfig *appConfig) {
  // print settings for debugging purposes
  Serial.printf(FSTR("isCoolApp:      %s\n"), appConfig->isCoolApp ? FSTR("true") : FSTR("false"));  
}

void app_config_send_config_page(strAppConfig *appConfig) {
  String s = String(APP_CONFIG_HTML);
  bool isCool = appConfig->isCoolApp;
  s.replace(FSTR("%ISCOOL%"), isCool ? FSTR("checked=\"checked\"") : FSTR(""));      
  http_server.send_P (200, TEXT_HTML, s.c_str());
}

void app_config_send_response_page(strAppConfig *appConfig) {
  http_server.send_P (200, TEXT_HTML, APP_CONFIG_RESPONSE_HTML);
  appConfig->isCoolApp = http_server.arg(FSTR("iscoolbox")) == FSTR("1");
  config_write();
}

