#include <Arduino.h>
#include <Wire.h>
#include "rtc.h"
#include "input.h"

byte display_ram_second_dots[60];
byte display_ram_hour_dots[12];
byte display_ram_quarter_dots[4];
byte display_ram_digits[6];
byte display_ram_digit_dots[2];
byte display_ram_digit_colon;

byte display_ram_raw[20];

byte seven[16] = {
  B00111111,  // 0
  B00000110,  // 1
  B01011011,  // 2
  B01001111,  // 3
  B01100110,  // 4
  B01101101,  // 5
  B01111101,  // 6
  B00000111,  // 7
  B01111111,  // 8
  B01101111,  // 9
  B00000000,  // a
  B00000000,  // b
  B00000000,  // c
  B00000000,  // d
  B00000000,  // e
  B00000000,  // f
};

void sendData(byte sda, byte address, byte brightness, byte *display_data) {

  if (brightness > 7) {
    brightness = 7;
  }

  byte x = ((brightness & 0xf) << 4) + 7;
  byte data[6] = {
    0, x, display_data[0], display_data[1], display_data[2], display_data[3]
  };

  Wire.begin(sda, 5);

  Wire.beginTransmission(address);

  Wire.write(data, 6);
  byte ret = Wire.endTransmission();
  if (ret != 0) {
    Serial.printf("I2C Error code: %d\n", ret);
    delay(250);
  }

}

// Initialize the display
void display_init() {
  memset(display_ram_second_dots, 0, sizeof(display_ram_second_dots));
  memset(display_ram_hour_dots, 0, sizeof(display_ram_hour_dots));
  memset(display_ram_quarter_dots, 0, sizeof(display_ram_quarter_dots));
  memset(display_ram_digits, 0, sizeof(display_ram_digits));
  memset(display_ram_digit_dots, 0, sizeof(display_ram_digit_dots));
  display_ram_digit_colon = 0;

  memset(display_ram_hour_dots, 1, 12);
  memset(display_ram_quarter_dots, 1, 4);
}

// Transform the logical representation of the display memory to the raw display memory
void display_render() {

  memset(display_ram_raw, 0, sizeof(display_ram_raw));

  // Second dots
  display_ram_raw[3] |= display_ram_second_dots[0] << 3;
  for (byte i = 0; i < 8; i++) {
    display_ram_raw[8] |= display_ram_second_dots[1 + i] << i;  // 1..8
    display_ram_raw[9] |= display_ram_second_dots[9 + i] << i;  // 9..16
    display_ram_raw[10] |= display_ram_second_dots[17 + i] << i; // 17..24
    display_ram_raw[11] |= display_ram_second_dots[25 + i] << i; // 25..32
    display_ram_raw[0] |= display_ram_second_dots[33 + i] << i; // 33..40
    display_ram_raw[1] |= display_ram_second_dots[41 + i] << i; // 41..48
    display_ram_raw[2] |= display_ram_second_dots[49 + i] << i; // 49..56
    if (i < 3) {
      display_ram_raw[3] |= display_ram_second_dots[57 + i] << i; // 57..59
    }
  }

  // Hour dots
  display_ram_raw[3] |= display_ram_hour_dots[0] << 7;
  display_ram_raw[6] |= display_ram_hour_dots[1];
  display_ram_raw[6] |= display_ram_hour_dots[2] << 1;
  display_ram_raw[3] |= display_ram_hour_dots[3] << 4;
  display_ram_raw[6] |= display_ram_hour_dots[4] << 2;
  display_ram_raw[6] |= display_ram_hour_dots[5] << 3;
  display_ram_raw[3] |= display_ram_hour_dots[6] << 5;
  display_ram_raw[6] |= display_ram_hour_dots[7] << 4;
  display_ram_raw[6] |= display_ram_hour_dots[8] << 5;
  display_ram_raw[3] |= display_ram_hour_dots[9] << 6;
  display_ram_raw[6] |= display_ram_hour_dots[10] << 6;
  display_ram_raw[7] |= display_ram_hour_dots[11] << 4;

  // Quarter dots
  display_ram_raw[7] |= display_ram_quarter_dots[0] << 3;
  display_ram_raw[7] |= display_ram_quarter_dots[1];
  display_ram_raw[7] |= display_ram_quarter_dots[2] << 1;
  display_ram_raw[7] |= display_ram_quarter_dots[3] << 2;

  // Digits
  display_ram_raw[12] = seven[display_ram_digits[4]];
  display_ram_raw[13] = seven[display_ram_digits[5]];

  display_ram_raw[16] = seven[display_ram_digits[0]];
  display_ram_raw[17] = seven[display_ram_digits[1]];
  display_ram_raw[18] = seven[display_ram_digits[2]];
  display_ram_raw[19] = seven[display_ram_digits[3]];

  display_ram_raw[16] |= display_ram_digit_colon << 7;

}

void display_update_decimal(byte *digit, byte value) {
  byte tens = value / 10;
  byte ones = value - (tens * 10);
  digit[0] = tens;
  digit[1] = ones;  
}

void display_update() {

  display_update_decimal(display_ram_digits, rtc_get_hours());
  display_update_decimal(display_ram_digits + 2, rtc_get_minutes());
  display_update_decimal(display_ram_digits + 4, rtc_get_seconds());

  display_ram_digit_colon = !rtc_get_halfstep();

  memset(display_ram_second_dots, 0, 60);
  memset(display_ram_second_dots, 1, rtc_get_seconds() + 1);

}

void display_send() {
  sendData(4, 0x38, input_get_brightness_ring(), display_ram_raw);        // IC7 (second dots + some special dots)
  sendData(4, 0x3A, input_get_brightness_ring(), display_ram_raw + 4);    // IC6 (special dots only)
  sendData(4, 0x3B, input_get_brightness_ring(), display_ram_raw + 8);    // IC8 (second dots only)

  sendData(2, 0x39, input_get_brightness_digits(), display_ram_raw + 12);
  sendData(2, 0x3A, input_get_brightness_digits(), display_ram_raw + 16);
}



