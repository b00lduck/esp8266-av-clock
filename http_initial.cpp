#include <Arduino.h>
#include "http.h"
#include "config.h"

const char INITIAL_INDEX_HTML[] = R"=====(
<html>
  <body>
    <h1>ESP8266</h1>
    <h2>Initial configuration</h2>
    <form action="/" method="POST">
      SSID: <input name="ssid"> (max. 32 characters)<br>
      Password: <input name="password" type="password"> (max. 32 characters)<br>
      <input type="submit" value="save">
    </form>
  </body>
</html>
)=====";

const char INITIAL_RESPONSE_HTML[] = R"=====(
<html>
  <body>
    <h1>ESP8266</h1>
    <h2>Initial configuration was applied</h2>
    Please wait for the device to be rebooted.
  </body>
</html>
)=====";

void http_initial_setup() {
  Serial.println("Setting up HTTP server for initial config");

  http_install_simple_get_handler_cstr("/", INITIAL_INDEX_HTML);

  http_server.on("/", HTTP_POST, []() {
    Serial.println("POST /init 200");       
    config_set_initial_config(http_server.arg("ssid").c_str(), http_server.arg("password").c_str());
    http_server.send_P (200, "text/html", INITIAL_RESPONSE_HTML);
    ESP.restart();
  });

  http_setup();
}



