#include <Arduino.h>
#include <Wire.h>

byte brightness_ring = 1;
byte brightness_digits = 1;
byte display_mode = 0;

void input_receive() {

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
  // Serial.printf("Brightness ring: %d   Brightness digits: %d    Display mode: %d\n", brightness_ring, brightness_digits, display_mode); 

}

byte input_get_brightness_ring() {
  return brightness_ring;
}

byte input_get_brightness_digits() {
  return brightness_digits;
}

byte input_get_mode() {
  return display_mode;
}

