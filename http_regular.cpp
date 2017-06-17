#include <Arduino.h>
#include "http.h"
#include "config.h"

String ipToString(byte ip[4]) {
  char buf[16];
  snprintf(buf, sizeof(buf), "%3d.%3d.%3d.%3d", ip[0], ip[1], ip[2], ip[3]);
  return String(buf);  
}

void http_regular_setup() { 
  Serial.println("Setting up HTTP server for regular config");

  http_server.on ("/", []() {
    String s = http_read_file("/regular-config.html");
    if (!s) {
      http_server.send_P (500, "text/html", "Internal server error");
      return;
    }

    s.replace("%SSID%", config.wifi_ssid);
    s.replace("%PWD%", config.wifi_password);
    if (config.use_dhcp) {
      s.replace("%DHCP%", "enabled");      
      s.replace("%IP%", config.wifi_ssid);
      s.replace("%MASK%", config.wifi_ssid);
      s.replace("%GW%", config.wifi_ssid);
    } else {
      s.replace("%DHCP%", "disabled");      
      s.replace("%IP%", ipToString(config.ip));
      s.replace("%MASK%", ipToString(config.netmask));
      s.replace("%GW%", ipToString(config.gateway));      
    }
    
    s.replace("%DEVNAME%", config.device_name);
    
    http_server.send_P (200, "text/html", s.c_str());
  });
  
  http_setup();
}


