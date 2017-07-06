#include <Arduino.h>
#include "http.h"
#include "config.h"
#include "macros.h"

const char INITIAL_INDEX_HTML[] PROGMEM = R"=====(
<html>
  <body>    
    <h1>ESP8266</h1>
    <h2>Initial configuration</h2>
    <form action="/" method="POST">
      SSID: <input name="ssid" maxlength="32" required> (max. 32 characters)<br>
      WiFi password: <input name="wifipass" type="password" required maxlength="32"> (max. 32 characters)<br>
      Device password: <input name="devpass" type="password" required maxlength="32"> (max. 32 characters)<br>
      <input type="submit" value="save">
    </form>
  </body>
</html>
)=====";

const char INITIAL_RESPONSE_HTML[] PROGMEM = R"=====(
<html>
  <body>
    <h1>ESP8266</h1>
    <h2>Initial configuration was applied</h2>
    Please wait for the device to be rebooted.
  </body>
</html>
)=====";

void http_initial_setup() {
  Serial.println(F("Setting up HTTP server for initial config"));

  http_install_simple_get_handler_cstr(FSTR("/"), INITIAL_INDEX_HTML);
  
  http_server.on(FSTR("/"), HTTP_POST, []() {    
    config_set_initial_config(http_server.arg(FSTR("ssid")).c_str(), http_server.arg(FSTR("wifipass")).c_str(), http_server.arg(FSTR("devpass")).c_str());
    http_server.send_P(200, TEXT_HTML, INITIAL_RESPONSE_HTML);
    Serial.println("Now rebooting");
    ESP.restart();
  });

  http_setup();
}



