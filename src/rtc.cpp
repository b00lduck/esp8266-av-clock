#include <Arduino.h>
#include "macros.h"
#include "config.h"

#include <NTPClient.h>
#include <WiFiUdp.h>

#include "rtc.h"

RTC::RTC() : timeClient(udp, NTP_SERVER), halfstep(0), time_gone_us(0) {
  timeClient.setTimeOffset(0);
}

bool RTC::update_by_ntp() {

  Serial.println(F("Trying NTP update")); 
  timeClient.update();
  utc = timeClient.getEpochTime();
  Serial.printf(FSTR("NTP UTC time updated: %d\n"), utc);

  last_update = utc;
  return true;
}

// create unix timestamp from given date (minimum is 1.1.2017)
unsigned long RTC::timestamp_from_date(byte day, byte month, unsigned int year, byte hour, byte minute, byte second) { 

  // 01.01.2020
  unsigned int ret = 1577836800; 
  
  // add years
  for (unsigned int i = 2020; i < year; i++) {
     ret += get_days_of_year(i) * 86400;
  }

  // add month
  for (byte i=1; i < month; i++) {
    ret += get_month_days(i, year) * 86400;
  }

  // add days
  ret += (day-1) * 86400;    

  // add time
  ret += hour * 3600;
  ret += minute * 60;
  ret += second;

  return ret;
}

bool RTC::tick(unsigned long delta_us) {

  bool changed = false;

  time_gone_us += delta_us;

  if (utc - last_update >= NTP_UPDATE_INTERVAL) {
    update_by_ntp();
  }

  while (time_gone_us >= 500000) {
    halfstep++;
    Serial.printf(FSTR("RTC ticked: %02d:%02d:%02d %02d.%02d.%04d TickTock:%d (DOW: %d)\n"), 
      datetime.hour, datetime.minute, datetime.second,
      datetime.day, datetime.month, datetime.year, halfstep, datetime.weekday);
    if (halfstep > 1) {
      halfstep = 0;
      utc++;
    }
    time_gone_us -= 500000;
    changed = true;
  }

  if (changed) {
    unsigned long localTime = utc + (TIME_ZONE * 3600);
    if (AUTO_DST && is_dst(localTime)) {
      localTime += 3600;
    }
    set_from_timestamp(&datetime, localTime);
  }

  return changed;
}

byte RTC::get_halfstep() {
  return halfstep;
}

byte RTC::get_second() {
  return datetime.second;
}

byte RTC::get_minute() {
  return datetime.minute;
}

byte RTC::get_hour() {
  return datetime.hour;
}

// true, if DST is active (last sunday of march to last sunday of october)
bool RTC::is_dst(unsigned long t) {
  rtc_datetime_t foo;  
  set_from_timestamp(&foo, t);
  return ((t >= last_sunday(3, foo.year)) && (t < last_sunday(10, foo.year)));
}

bool RTC::is_leap_year(unsigned int year) {
  return !(year%4) && ((year%100) || !(year%400));
}

unsigned int RTC::get_days_of_year(unsigned int year) {
  return is_leap_year(year) ? 366 : 365;
}

/**
 * 1  January
 * 12 December
 */
byte RTC::get_month_days(byte month, unsigned int year) {
  byte months[]= {31,28,31,30,31,30,31,31,30,31,30,31};
  if (is_leap_year(year)) {
    months[1] = 29;
  }
  return months[month-1];
}

bool RTC::is_sunday(unsigned long x) {  
  rtc_datetime_t foo;
  set_from_timestamp(&foo, x);  
  return foo.weekday == 7;
}

// return timestamp of the last sunday in march of the given year (02:00)
unsigned long RTC::last_sunday(byte month, unsigned int year) {  
  byte stop = get_month_days(month, year);
  byte start = stop - 6;
  for (byte i = start; i <= stop; i++) {
    unsigned long x = timestamp_from_date(i, month, year, 2, 0 ,0);
    if (is_sunday(x)) {
      return x;
    }
  }
  return 0;
}

// set rtc_datetime struct by given unix timestamp
void RTC::set_from_timestamp(rtc_datetime_t *dest, unsigned long t) {
  
  dest->second = t % 60;
  t /= 60;
  dest->minute = t % 60;
  t /= 60;
  dest->hour = t % 24;
  t /= 24;
  dest->weekday = ((t + 3) % 7) + 1;  // Monday = 1  
  
  unsigned long days = 0;
  unsigned int year = 1970;
  while((days += (is_leap_year(year) ? 366 : 365)) <= t) {
    year++;
  }

  days -= is_leap_year(year) ? 366 : 365;
  t -= days;

  byte monthLength = 0;
  byte month;
  for (month = 1; month <= 12; month++) {
    monthLength = get_month_days(month, year);
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