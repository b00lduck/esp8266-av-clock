#include <Arduino.h>
#include <WiFiUdp.h>
#include "Time.h"
#include "config.h"

time_t rtc_time;

static WiFiUDP udp;

byte rtc_halfstep = 0;
byte rtc_seconds = 0;
byte rtc_minutes = 0;
byte rtc_hours = 0;
byte rtc_day = 0;
byte rtc_month = 0;
unsigned int rtc_year = 0;

time_t rtc_updated_at = 0;

void rtc_update_by_ntp() {

  const long ntp_prequel = 0xEC0600E3;
  const byte ntp_packet_length = 48;
  const unsigned long ntp_utc_offset = 2208988800ul;

  const unsigned int ntp_port = 123;

  if (!udp.begin(ntp_port)) {
    return;
  }

  udp.flush();

  // Send an NTP request
  if (! (udp.beginPacket(config.app_config.ntp_server, ntp_port) && udp.write((byte *)&ntp_prequel, ntp_packet_length) == ntp_packet_length && udp.endPacket())) {
    return;
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
  if (pktLen != ntp_packet_length)
    return;

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
  
  rtc_time = time - ntp_utc_offset + (config.app_config.time_zone * 3600);   // convert NTP time to Unix time

  rtc_hours = hour(rtc_time);
  rtc_minutes = minute(rtc_time);
  rtc_seconds = second(rtc_time);

  rtc_day = day(rtc_time);
  rtc_month = month(rtc_time);
  rtc_year = year(rtc_time);

  rtc_updated_at = rtc_time;  
}

void rtc_tick() {
  rtc_halfstep++;
  if (rtc_halfstep > 1) {
    rtc_seconds++;
    rtc_halfstep = 0;
    if (rtc_seconds > 59) {
      rtc_minutes++;
      rtc_seconds = 0;
      if (rtc_minutes > 59) {
        rtc_hours++;
        rtc_minutes = 0;
        if (rtc_hours > 23) {
          rtc_hours = 0;            
        }          
        rtc_update_by_ntp();
      }
    }
  }    
}

byte rtc_get_halfstep() {
  return rtc_halfstep;
}

byte rtc_get_seconds() {
  return rtc_seconds;
}

byte rtc_get_minutes() {
  return rtc_minutes;
}

byte rtc_get_hours() {
  return rtc_hours;
}





