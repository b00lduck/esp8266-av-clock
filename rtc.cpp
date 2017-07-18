#include <Arduino.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include "config.h"
#include "macros.h"
#include "rtc.h"

static WiFiUDP udp;

unsigned long rtc_utc;
unsigned long rtc_last_update;
byte rtc_halfstep = 0;

rtc_datetime_t rtc_datetime;

bool rtc_update_by_ntp() {

  Serial.println(F("Trying NTP update"));

  const long ntp_prequel = 0xEC0600E3;
  const byte ntp_packet_length = 48;
  const unsigned long ntp_utc_offset = 2208988800ul;

  const unsigned int ntp_port = 123;

  if (!udp.begin(ntp_port)) {
    Serial.println(F("UDP error: begin failed"));
    Serial.printf(FSTR("WiFi status: %d\n"), WiFi.status());
    return false;
  }

  udp.flush();

  // Send an NTP request
  if (! (udp.beginPacket(config.app_config.ntp_server, ntp_port) && udp.write((byte *)&ntp_prequel, ntp_packet_length) == ntp_packet_length && udp.endPacket())) {
    Serial.println(F("UDP error: beginPacket failed"));
    Serial.printf(FSTR("WiFi status: %d\n"), WiFi.status());    
    return false;    
  }
  
  // Wait for response; check every pollIntv ms up to maxPoll times
  const int pollIntv = 10;   // poll every this many ms
  const byte maxPoll = 5;    // poll up to this many times
  int pktLen;
  for (byte i=0; i<maxPoll; i++) {
    if ((pktLen = udp.parsePacket()) == ntp_packet_length)
      break;
    delay(pollIntv);
  }
  if (pktLen != ntp_packet_length) {
    Serial.println(F("UDP error: ntp_packet_length mismatch"));
    Serial.printf(FSTR("WiFi status: %d\n"), WiFi.status());    
    return false;        
  }

  // Read and discard the first useless bytes
  // Set useless to 32 for speed; set to 40 for accuracy.
  const byte useless = 40;
  for (byte i = 0; i < useless; ++i)
    udp.read();

  // Read the integer part of sending time
  unsigned long time = udp.read();  // NTP time
  for (byte i = 1; i < 4; i++)
    time = time << 8 | udp.read();

  // Round to the nearest second if we want accuracy
  // The fractionary part is the next byte divided by 256: if it is
  // greater than 500ms we round to the next second; we also account
  // for an assumed network delay of 50ms, and (0.5-0.05)*256=115;
  // additionally, we account for how much we delayed reading the packet
  // since its arrival, which we assume on average to be pollIntv/2.
  time += (udp.read() > 115 - pollIntv/8);

  udp.flush(); 
  
  rtc_utc = time - ntp_utc_offset;

  Serial.printf(FSTR("NTP UTC time updated: %d\n"), rtc_utc);
  Serial.printf(FSTR("WiFi status: %d\n"), WiFi.status());  

  rtc_last_update = rtc_utc;

  return true;
}

bool rtc_is_leap_year(unsigned int year) {
  return !(year%4) && ((year%100) || !(year%400));
}

unsigned int rtc_get_days_of_year(unsigned int year) {
  return rtc_is_leap_year(year) ? 366 : 365;
}

/**
 * 1  January
 * 12 December
 */
byte rtc_get_month_days(byte month, unsigned int year) {
  byte months[]= {31,28,31,30,31,30,31,31,30,31,30,31};
  if (rtc_is_leap_year(year)) {
    months[1] = 29;
  }
  return months[month-1];
}

// create unix timestamp from given date (minimum is 1.1.2017)
unsigned long rtc_timestamp_from_date(byte day, byte month, unsigned int year, byte hour, byte minute, byte second) { 

  // 01.01.2017
  unsigned int ret = 1483228800; 
  
  // add years
  for (unsigned int i = 2017; i < year; i++) {
     ret += rtc_get_days_of_year(i) * 86400;
  }    //Serial.printf(FSTR("RTC ticked: %d\n\n"), rtc_timestamp_from_date(rtc_day, rtc_month, rtc_year, rtc_hour, rtc_minute, rtc_second));


  // add month
  for (byte i=1; i < month; i++) {
    ret += rtc_get_month_days(i, year) * 86400;
  }

  // add days
  ret += (day-1) * 86400;    

  // add time
  ret += hour * 3600;
  ret += minute * 60;
  ret += second;

  return ret;
}

// set rtc_datetime struct by given unix timestamp
void rtc_set_from_timestamp(rtc_datetime_t *dest, unsigned long t) {
  
  dest->second = t % 60;
  t /= 60;
  dest->minute = t % 60;
  t /= 60;
  dest->hour = t % 24;
  t /= 24;
  dest->weekday = ((t + 3) % 7) + 1;  // Monday = 1  
  
  unsigned long days = 0;
  unsigned int year = 1970;
  while((days += (rtc_is_leap_year(year) ? 366 : 365)) <= t) {
    year++;
  }

  days -= rtc_is_leap_year(year) ? 366 : 365;
  t -= days;

  byte monthLength = 0;
  byte month;
  for (month = 1; month <= 12; month++) {
    monthLength = rtc_get_month_days(month, year);
    if (t >= monthLength) {
      t -= monthLength;
    } else {
       break;
    }
  }
  
  dest->year = year;
  dest->month = month;
  dest->day = t + 1;
}

bool rtc_is_sunday(unsigned long x) {  
  rtc_datetime_t foo;
  rtc_set_from_timestamp(&foo, x);  
  return foo.weekday == 7;
}

// return timestamp of the last sunday in march of the given year (02:00)
unsigned long rtc_last_sunday(byte month, unsigned int year) {  
  byte stop = rtc_get_month_days(month, year);
  byte start = stop - 6;
  for (byte i = start; i <= stop; i++) {
    unsigned long x = rtc_timestamp_from_date(i, month, year, 2, 0 ,0);
    if (rtc_is_sunday(x)) {
      return x;
    }
  }
  return 0;
}

// true, if DST is active (last sunday of march to last sunday of october)
bool rtc_is_dst(unsigned long t) {
  rtc_datetime_t foo;  
  rtc_set_from_timestamp(&foo, t);
  return ((t >= rtc_last_sunday(3, foo.year)) && (t < rtc_last_sunday(10, foo.year)));
}

// tick rtc 0.5 seconds
void rtc_tick() {
  Serial.printf(FSTR("WiFi status: %d\n"), WiFi.status());
  rtc_halfstep++;
  if (rtc_halfstep > 1) {
    rtc_halfstep = 0;
    rtc_utc++;
    unsigned long localTime = rtc_utc + (config.app_config.time_zone * 3600);
    if (config.app_config.auto_dst && rtc_is_dst(localTime)) {
      localTime += 3600;
    }

    rtc_set_from_timestamp(&rtc_datetime, localTime);
    Serial.printf(FSTR("RTC ticked: %02d:%02d:%02d %02d.%02d.%04d (DOW: %d)\n"), 
      rtc_datetime.hour, rtc_datetime.minute, rtc_datetime.second,
      rtc_datetime.day, rtc_datetime.month, rtc_datetime.year, rtc_datetime.weekday);    

    if (rtc_utc - rtc_last_update >= config.app_config.ntp_update_interval) {
      rtc_update_by_ntp();
    }
  }    
}

byte rtc_get_halfstep() {
  return rtc_halfstep;
}

byte rtc_get_second() {
  return rtc_datetime.second;
}

byte rtc_get_minute() {
  return rtc_datetime.minute;
}

byte rtc_get_hour() {
  return rtc_datetime.hour;
}


