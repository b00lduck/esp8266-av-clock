#pragma once
#include "Arduino.h"
#include "rtc.h"

class Display {

  private:
    unsigned char display_ram_second_dots[60];
    unsigned char display_ram_hour_dots[12];
    unsigned char display_ram_quarter_dots[4];
    unsigned char display_ram_digits[6];
    unsigned char display_ram_digit_dots[2];
    unsigned char display_ram_digit_colon;
    unsigned char display_ram_raw[20];

  public:
    Display();
    void init();
    void update_decimal(unsigned char *digit, unsigned char value);
    void update(unsigned char hour, unsigned char minute, unsigned char second, unsigned char halfstep);
    void render();
    void send(unsigned char brightness_ring, unsigned char brightness_digits);
    void sendData(byte sda, byte address, byte brightness, byte *display_data);

    void reset_loading_ring();
    void loading_ring_cw(unsigned char n, unsigned char brightness_ring, unsigned char brightness_digits);
    void loading_ring_ccw(unsigned char n, unsigned char brightness_ring, unsigned char brightness_digits);

};
