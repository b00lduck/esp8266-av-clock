#pragma once
#include <Arduino.h>

class Input {

  private:
    byte brightness_ring;
    byte brightness_digits;
    byte display_mode;

  public:
    Input();
    void receive();
    byte get_brightness_ring();
    byte get_brightness_digits();
    byte get_mode();

};
