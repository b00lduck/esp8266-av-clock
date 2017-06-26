#include <Arduino.h>
#include "http.h"
#include "config.h"

const char REGULAR_INDEX_HTML[] = R"=====(
<html>
  <body>
    <h1>ESP8266</h1>
    <ul>
      <li><a href="/config">Configuration</a></li>      
    </ul>
  </body>
</html>
)=====";    


const char REGULAR_CONFIG_RESPONSE_HTML[] = R"=====(
<html>
  <body>
    CONFIG RESPONSE STUB
  </body>
</html>
)=====";      
  
const char REGULAR_CONFIG_HTML[] = R"=====(
<html>
  <head>
    <script src="https://code.jquery.com/jquery-3.2.1.slim.min.js"
            integrity="sha256-k2WSCIexGzOj3Euiig+TlR8gA0EmPjuc79OEeY5L45g="
            crossorigin="anonymous"></script>
    <script>
      $(function() {
        if ($('#dhcpbox').is(":checked")) {  
          $("#iprow").hide();
          $("#maskrow").hide();
          $("#gwrow").hide();
        }
      
        $("#dhcpbox").on("change", function(e) {
          if (e.target.checked !== true) {
            $("#iprow").show();
            $("#maskrow").show();
            $("#gwrow").show();
          } else {
            $("#iprow").hide();
            $("#maskrow").hide();
            $("#gwrow").hide();  
          }
        });
      
        $("#devpass").hide();
        $("#wifipass").hide();        
        $("#devpassbox").on("click", function(e) {
          $("#devpass").show();
          $("#devpassbox").hide();
        });
        
        $("#wifipassbox").on("click", function(e) {        
          $("#wifipass").show();
          $("#wifipassbox").hide();
        });
        
      });
    </script>
  </head>
  <body>
    <h1>ESP8266</h1>
    <h2>Configuration</h2>
    <form action="/config" method="POST">
      <table>
        <tr><td colspan="2"><b>Device config:</b><td></tr>
        <tr><td>Device name:</td><td><input name="devname" value="%DEVNAME%" maxlength="32" required></td></tr>
        <tr><td>Device password:</td><td>
          <input id="devpass" name="devpass" type="password" value="" maxlength="32">
          <input type="button" id="devpassbox" name="changedevpass" value="click to change device password"></td></tr>
        <tr><td colspan="2"><br><b>WiFi config:</b><td></tr>
        <tr><td>SSID:</td><td><input name="ssid" value="%SSID%" maxlength="32" required></td></tr>      
        <tr><td>WiFi password:</td><td>
          <input id="wifipass" name="wifipass" type="password" value="" maxlength="32"> 
          <input type="button" id="wifipassbox" value="click to change WiFi password"></td></tr>        
        <tr><td colspan="2"><br><b>Network config</b><td></tr>
        <tr><td>DHCP:</td><td><input id="dhcpbox" name="dhcp" type="checkbox" value="1" %DHCP%></td></tr>        
        <tr id="iprow"><td>IP:</td><td>
          <input name="ip1" type="number" value="%MASK1%" min="0" max="255" width="50">.
          <input name="ip2" type="number" value="%MASK2%" min="0" max="255" width="50">.
          <input name="ip3" type="number" value="%MASK3%" min="0" max="255" width="50">.
          <input name="ip4" type="number" value="%MASK4%" min="0" max="255" width="50"></td></tr>
        <tr id="maskrow"><td>Mask:</td><td>
          <input name="mask1" type="number" value="%MASK1%" min="0" max="255" width="50">.
          <input name="mask2" type="number" value="%MASK2%" min="0" max="255" width="50">.
          <input name="mask3" type="number" value="%MASK3%" min="0" max="255" width="50">.
          <input name="mask4" type="number" value="%MASK4%" min="0" max="255" width="50"></td></tr>
        <tr id="gwrow"><td>Gateway:</td><td>          
          <input name="gw1" type="number" value="%GW1%" min="0" max="255" width="50">.
          <input name="gw2" type="number" value="%GW2%" min="0" max="255" width="50">.
          <input name="gw3" type="number" value="%GW3%" min="0" max="255" width="50">.
          <input name="gw4" type="number" value="%GW4%" min="0" max="255" width="50"></td></tr>
      </table>
      <br>
      <input type="submit" value="save configuration and reboot">
    </form>
    <form action="/" method="GET">
        <input type="submit" value="cancel">
    </form>
  </body>
</html>
)=====";    

void http_regular_setup() { 
  Serial.println("Setting up HTTP server for regular config");

  http_server.on ("/", []() {
    http_server.send_P (200, "text/html", REGULAR_INDEX_HTML);
  });
  
  http_server.on ("/config", HTTP_GET, []() {
    
    String s = String(REGULAR_CONFIG_HTML);   
    s.replace("%SSID%", config.wifi_ssid);
    s.replace("%PWD%", config.wifi_password);
    s.replace("%DHCP%", config.use_dhcp ? "checked=\"checked\"" : "");      
    s.replace("%DEVNAME%", config.device_name); 
    s.replace("%IP1%", String(WiFi.localIP()[0]));
    s.replace("%IP2%", String(WiFi.localIP()[1]));
    s.replace("%IP3%", String(WiFi.localIP()[2]));
    s.replace("%IP4%", String(WiFi.localIP()[3]));
    s.replace("%MASK1%", String(WiFi.subnetMask()[0]));
    s.replace("%MASK2%", String(WiFi.subnetMask()[1]));
    s.replace("%MASK3%", String(WiFi.subnetMask()[2]));
    s.replace("%MASK4%", String(WiFi.subnetMask()[3]));
    s.replace("%GW1%", String(WiFi.gatewayIP()[0]));
    s.replace("%GW2%", String(WiFi.gatewayIP()[1]));
    s.replace("%GW3%", String(WiFi.gatewayIP()[2]));
    s.replace("%GW4%", String(WiFi.gatewayIP()[3]));
    http_server.send_P (200, "text/html", s.c_str());
  });

  http_server.on ("/config", HTTP_POST, []() {
    http_server.send_P (200, "text/html", REGULAR_CONFIG_RESPONSE_HTML);
    
    config_set_regular_config(
      http_server.arg("ssid").c_str(), 
      http_server.arg("changewifipass") == "" ? http_server.arg("wifipass").c_str() : config.wifi_password, 
      http_server.arg("devname").c_str(), 
      http_server.arg("changedevpass") == "" ? http_server.arg("devpass").c_str() : config.device_password);  
  });
  
  http_setup();
}


