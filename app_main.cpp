#include <Ticker.h> //timer
#include "app_main.h"
#include "display.h"
#include "rtc.h"
#include "input.h"
#include "macros.h"

Ticker ticker;

const int led = 13;

bool tickOccured;

void ticker_handler() {
  tickOccured = true;
}

byte numRetries = 0;

void app_setup() {  
  tickOccured = false;
  ticker.attach(0.5, ticker_handler);
  display_init();
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  input_receive();
}

void app_loading_ring_reset() {
    display_update_decimal(display_ram_digits, 0);
    display_update_decimal(display_ram_digits + 2, 0);
    display_update_decimal(display_ram_digits + 4, 0);
    display_ram_digit_colon = 0;
    memset(display_ram_second_dots, 0, 60);  
}

void app_loading_ring_cw() {
    app_loading_ring_reset();
    memset(display_ram_second_dots, 1, numRetries);         
    display_render();
    display_send();
}

void app_loading_ring_ccw() {
    app_loading_ring_reset();
    memset(display_ram_second_dots + (60-numRetries), 1, numRetries);         
    display_render();  
    display_send();
}

void app_connecting() {  
    numRetries++;
    if (numRetries > 60) {
      numRetries = 0;
    }
    app_loading_ring_cw();
}

void app_connected() {  
    Serial.println(F("Connected."));   
    numRetries = 0;
    bool rc = false;
    while(!rc) {      
      numRetries++;
      if (numRetries > 60) {
        numRetries = 0;
      }
      app_loading_ring_ccw();
      rc = rtc_update_by_ntp();
    }
}

void app_loop() {
  input_receive();  
  if (tickOccured == true) {
    rtc_tick();
    display_update();
    display_render();
    tickOccured = false;
  }
  display_send();
}




