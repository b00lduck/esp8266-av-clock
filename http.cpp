#include <ESP8266WebServer.h>

ESP8266WebServer http_server(80);

void http_handle() {
  http_server.handleClient();
}

void http_setup() {
  http_server.onNotFound ([]() {
    Serial.println("Page not found");
    http_server.send (404, "text/html", "not found");
  });
  http_server.begin();  
}


