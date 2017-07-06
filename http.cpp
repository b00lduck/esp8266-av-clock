#include <ESP8266WebServer.h>
#include "assets/jquery-3.2.1.slim.min.js.gz.h"
#include "http.h"
#include "macros.h"

ESP8266WebServer http_server(80);

void http_install_simple_get_handler_cstr(const char* path, const char* cstr) {
  http_server.on(path, HTTP_GET, [cstr]() {
    http_server.send_P (200, TEXT_HTML, cstr);
  });
}

void http_handle() {
  http_server.handleClient();
}

void http_setup() {
  http_server.onNotFound ([]() {
    Serial.println(F("Page not found"));
    http_server.send(404, TEXT_HTML, F("<h1>404</h1>not found"));
  });

  http_server.on(FSTR("/assets/jquery-3.2.1.slim.min.js"), HTTP_GET, []() {    
    http_server.sendHeader(F("Content-Encoding"), F("gzip"));
    http_server.send_P(200, APPLICATION_JAVASCRIPT, asset_jquery, ASSET_JQUERY_LEN);
  });
  
  http_server.begin();  
}




