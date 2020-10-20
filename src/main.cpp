#include <Arduino.h>
#include "wifi.h"
#include "app.h"

App app;

void setup(void) {
  Serial.begin(115200);
  Serial.println(F("\nStarted"));
  app.init();
}

void loop(void) {
  app.loop();
}

