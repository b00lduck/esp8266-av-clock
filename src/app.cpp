#include "app.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "config.h"
#include "macros.h"

bool tickOccured = false;

void handler() {
  tickOccured = true;
}

App::App() : led(13), numRetries(0) {
  ticker.attach(0.5, handler);
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  input.receive();
}

void App::init() {
  
  //if (WiFi.status() != WL_CONNECTED) {
    Serial.printf(FSTR("Connecting to SSID \"%s\" with given password...\n"), WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
      numRetries++;
      if (numRetries > 60) {
        numRetries = 0;
      }
      display.loading_ring_cw(numRetries, input.get_brightness_ring(), input.get_brightness_digits());
      delay(500);
    }
  //} else {
  //  Serial.printf(FSTR("Already connected to SSID \"%s\""), WIFI_SSID);
  //}  
 
  Serial.printf(FSTR("\nConnected to SSID \"%s\" with IPv4 address "), WIFI_SSID);
  Serial.println(WiFi.localIP());

  numRetries = 0;
  bool rc = false;
  while(!rc) {      
    numRetries++;
    if (numRetries > 60) {
      numRetries = 0;
    }
    display.loading_ring_ccw(numRetries, input.get_brightness_ring(), input.get_brightness_digits());
    rc = rtc.update_by_ntp();
  }

}

unsigned long last_timestamp = micros();

void App::loop() {
  input.receive();

  unsigned long now = micros();
  unsigned long time_gone_us = now - last_timestamp;
  last_timestamp = now;

  if (rtc.tick(time_gone_us)) {
    display.update(rtc.get_hour(), rtc.get_minute(), rtc.get_second(), rtc.get_halfstep());
    display.render();
    display.send(input.get_brightness_ring(), input.get_brightness_digits());
  }
}
