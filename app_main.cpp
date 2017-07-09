#include <Ticker.h> //timer
#include "app_main.h"
#include "display.h"
#include "rtc.h"
#include "input.h"

Ticker ticker;

const int led = 13;

bool tickOccured;

void ticker_handler() {
  tickOccured = true;
}

void app_setup() {  
  tickOccured = false;
  ticker.attach(0.5, ticker_handler);
  display_init();
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.printf("%d\n", rtc_update_by_ntp());
}

void app_loop() {
  if (tickOccured == true) {
    rtc_tick();
    display_update();
    display_render();
    tickOccured = false;
  }
  display_send();
  input_receive();  
}



