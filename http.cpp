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

const char* http_read_file(char* filename) {
  File f = SPIFFS.open(filename, "r");
  if (!f) {
    return NULL;
  }
  return f.readString().c_str();
}

boolean http_send_file(char* filename) {
  const char* c = http_read_file(filename);
  if (!c) {
    http_server.send_P (500, "text/html", "Internal server error");
    return false;
  }
  http_server.send_P (200, "text/html", c);
  return true;
}

void http_install_simple_get_handler(char* path, char* filename) {
  http_server.on(path, HTTP_GET, [filename]() {
    Serial.printf("GET %s -> ");
    if (http_send_file(filename)) {
      Serial.printf("200\n");
    } else {
      Serial.printf("500\n");
    }
  });
}

