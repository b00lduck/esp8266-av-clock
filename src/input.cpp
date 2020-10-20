#include <Arduino.h>
#include <Wire.h>
#include "input.h"

Input::Input() : brightness_ring(1), brightness_digits(1), display_mode(0) {
}

void Input::receive() {
  Wire.begin(2, 5); 
  Wire.requestFrom(0x20, 1);  
  while (Wire.available()) { // slave may send less than requested
    byte c = Wire.read(); // receive a byte as character
    brightness_digits = 15 - (c >> 4);
    display_mode = 15 - (c & 0xf);
  }

  Wire.requestFrom(0x25, 1);  
  while (Wire.available()) {
    byte c = Wire.read(); // receive a byte as character
    brightness_ring = 15 - (c >> 4);    
  }
}

byte Input::get_brightness_ring() {
  return brightness_ring;
}

byte Input::get_brightness_digits() {
  return brightness_digits;
}

byte Input::get_mode() {
  return display_mode;
}



