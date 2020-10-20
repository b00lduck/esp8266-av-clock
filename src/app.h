#pragma once
#include "Arduino.h"
#include <Ticker.h>
#include "display.h"
#include "rtc.h"
#include "input.h"

class App {

  private:
    RTC rtc;
    Display display;
    Input input;
    Ticker ticker;

    int led = 13;
    byte numRetries = 0;


  public:
    App();
    void loop();
    void init();

};
