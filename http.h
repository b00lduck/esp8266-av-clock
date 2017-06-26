#include <ESP8266WebServer.h>
extern ESP8266WebServer http_server;

void http_setup();
void http_handle();

void http_install_simple_get_handler_cstr(const char* path, const char* cstr);

