#include <Arduino.h>
#include "http.h"
#include "config.h"
#include "macros.h"

const char REGULAR_INDEX_HTML[] PROGMEM = R"=====(
<html>
  <head>
    <meta charset="utf-8"/>
  </head>
  <body>
    <h1>ESP8266</h1>
    <h2>Main menu</h2>
    <ul>
      <li><a href="/config">Device Configuration</a></li>
      <li><a href="/app_config">Application configuration</a></li>
      <li><a href="/update">Firmware update</a></li>      
    </ul>
  </body>
</html>
)=====";    

const char REGULAR_UPDATE_HTML[] PROGMEM = R"=====(
<html>
  <head>
    <meta charset="utf-8"/>
  </head>
  <body>
    <h1>ESP8266</h1>
    <h2>Firmware update</h2>  
    <form method="POST" action="/update" enctype="multipart/form-data">
      <input type="file" name="update"><br><br>
      <input type="submit" value="start firmware update">
    </form>
    <form action="/" method="GET">
        <input type="submit" value="cancel">
    </form>    
  </body>
</html>
)=====";

const char REGULAR_UPDATE_OK_HTML[] PROGMEM = R"=====(
<html>
  <head>
    <meta charset="utf-8"/>
  </head>
  <body>
    <h1>ESP8266</h1>
    <h2>Firmware update successfully uploaded</h2>
    Please wait for the device to be rebooted.
  </body>
</html>
)=====";

const char REGULAR_UPDATE_FAIL_HTML[] PROGMEM = R"=====(
<html>
  <head>
    <meta charset="utf-8"/>
  </head>
  <body>
    <h1>ESP8266</h1>
    <h2>Firmware update failed</h2>
    <a href="/">back to main menu</a>
  </body>
</html>
)=====";

const char REGULAR_CONFIG_RESPONSE_HTML[] PROGMEM = R"=====(
<html>
  <head>
    <meta charset="utf-8"/>
  </head>
  <body>
    <h1>ESP8266</h1>
    <h2>Device configuration successfully updated<h2>
  </body>
</html>
)=====";      
 
const char REGULAR_CONFIG_HTML[] PROGMEM = R"=====(
<html>
  <head>
    <meta charset="utf-8"/>
    <script src="/assets/jquery-3.2.1.slim.min.js" integrity="sha256-k2WSCIexGzOj3Euiig+TlR8gA0EmPjuc79OEeY5L45g=" crossorigin="anonymous"></script>
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
    <h2>Device configuration</h2>
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
  Serial.println(F("Setting up HTTP server for regular config"));

  http_server.on ("/", []() {
    http_server.send_P (200, TEXT_HTML, REGULAR_INDEX_HTML);
  });
  
  http_server.on (FSTR("/config.json"), HTTP_GET, []() {    
    char json[300];
    sprintf((char*)&json, 
      FSTR("{\"wifi_ssid\": \"%32s\", \"use_dhcp\": \"%4s\", \"device_name\": \"%32s\", \"ip\": \"%15s\", \"mask\": \"%15s\", \"gw\": \"%15s\"}"),
      config.wifi_ssid,
      config.use_dhcp ? FSTR("true") : FSTR("false"),
      config.device_name,
      String(WiFi.localIP()).c_str(),
      String(WiFi.subnetMask()).c_str(),
      String(WiFi.gatewayIP()).c_str());

    http_server.send(200, APPLICATION_JSON, json);

  });
  
  http_server.on (FSTR("/config"), HTTP_GET, []() {          
    http_server.send_P(200, TEXT_HTML, REGULAR_CONFIG_HTML);
  });

  http_server.on(FSTR("/config"), HTTP_POST, []() {
    http_server.send_P (200, TEXT_HTML, REGULAR_CONFIG_RESPONSE_HTML);    
    config_set_regular_config(
      http_server.arg(FSTR("ssid")).c_str(), 
      http_server.arg(FSTR("changewifipass")) == "" ? http_server.arg(FSTR("wifipass")).c_str() : config.wifi_password, 
      http_server.arg(FSTR("devname")).c_str(), 
      http_server.arg(FSTR("changedevpass")) == "" ? http_server.arg(FSTR("devpass")).c_str() : config.device_password);  
  });

  http_server.on (FSTR("/app_config"), HTTP_GET, []() {    
    app_config_send_config_page(&config.app_config);   
  });

  http_server.on (FSTR("/app_config"), HTTP_POST, []() {
    app_config_send_response_page(&config.app_config);   
  });  

  http_server.on(FSTR("/update"), HTTP_GET, []() {
    http_server.send_P (200, TEXT_HTML, REGULAR_UPDATE_HTML);
  });  

  http_server.on(FSTR("/update"), HTTP_POST, []() {
      http_server.sendHeader(FSTR("Connection"), FSTR("close"));
      if (Update.hasError()) {
        http_server.send_P(500, TEXT_HTML, REGULAR_UPDATE_FAIL_HTML);  
      } else {
        http_server.send_P(200, TEXT_HTML, REGULAR_UPDATE_OK_HTML);
        ESP.restart();  
      }
    },[]() {
      HTTPUpload& upload = http_server.upload();    
      if(upload.status == UPLOAD_FILE_START) {
        Serial.printf(FSTR("Update: %s\n"), upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if(!Update.begin(maxSketchSpace)) {
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_WRITE){
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_END){
        if(Update.end(true)) { 
          Serial.printf(FSTR("Update Success: %u\nRebooting...\n"), upload.totalSize);
        } else {
          Update.printError(Serial);
        }
      }      
  });
  
  http_setup();
}


