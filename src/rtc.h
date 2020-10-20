#pragma once
#include "Arduino.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

typedef struct {
  unsigned char second;
  unsigned char minute;
  unsigned char hour;
  unsigned char weekday;
  unsigned char day;
  unsigned char month;
  unsigned int year;
} rtc_datetime_t;

class RTC {

  private:
    WiFiUDP udp;
    NTPClient timeClient;

    unsigned long utc;
    unsigned long last_update;
    byte halfstep;
    rtc_datetime_t datetime;
    unsigned long time_gone_us;


    bool is_leap_year(unsigned int year);
    unsigned int get_days_of_year(unsigned int year);
    byte get_month_days(byte month, unsigned int year);
    bool is_dst(unsigned long t);
    bool is_sunday(unsigned long x);
    unsigned long last_sunday(byte month, unsigned int year);
    void set_from_timestamp(rtc_datetime_t *dest, unsigned long t);
    unsigned long timestamp_from_date(byte day, byte month, unsigned int year, byte hour, byte minute, byte second);

  public:
    RTC();  
    bool tick(unsigned long time_gone_ms);
    unsigned char get_halfstep();
    unsigned char get_second();
    unsigned char get_hour();
    unsigned char get_minute();
    bool update_by_ntp();

};
