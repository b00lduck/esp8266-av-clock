#include <ESP8266WebServer.h>
#include "FS.h"

ESP8266WebServer http_server(80);

void http_handle() {
  http_server.handleClient();
}

void http_setup() {
  SPIFFS.begin();
  http_server.onNotFound ([]() {
    Serial.println("Page not found");
    http_server.send (404, "text/html", "not found");
  });
  http_server.begin();  
}

void http_install_simple_get_handler_cstr(const char* path, const char* cstr) {
  http_server.on(path, HTTP_GET, [cstr]() {
    Serial.printf("GET %s -> 200");
    http_server.send_P (200, "text/html", cstr);
  });
}


