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
    <h2>Device configuration successfully updated</h2>
    Please wait for the device to be rebooted.
  </body>
</html>
)=====";      
 
const char REGULAR_CONFIG_HTML[] PROGMEM = R"=====(
<html>
  <head>
    <meta charset="utf-8"/>
    <script src="/assets/jquery-3.2.1.min.js"></script>
    <script>
      $(function() {

        $.getJSON("config.json", function( data ) {
          if (data.use_dhcp) {  
            $("#dhcpbox").prop("checked", true);
            $("#iprow").hide();
            $("#maskrow").hide();
            $("#gwrow").hide();
          } else {
            $("#dhcpbox").prop("checked", false);
            $("#iprow").show();
            $("#maskrow").show();
            $("#gwrow").show();            
          }

          $("input[name='ssid']").val(data.wifi_ssid);
          $("input[name='devname']").val(data.device_name);

          var ipsplit = data.ip.split(".");
          var gwsplit = data.gw.split(".");
          var masksplit = data.mask.split(".");

          $("input[name='ip1']").val(ipsplit[0]);
          $("input[name='ip2']").val(ipsplit[1]);
          $("input[name='ip3']").val(ipsplit[2]);
          $("input[name='ip4']").val(ipsplit[3]); 
                   
          $("input[name='gw1']").val(gwsplit[0]);
          $("input[name='gw2']").val(gwsplit[1]);
          $("input[name='gw3']").val(gwsplit[2]);
          $("input[name='gw4']").val(gwsplit[3]);          

          $("input[name='mask1']").val(masksplit[0]);          
          $("input[name='mask2']").val(masksplit[1]);
          $("input[name='mask3']").val(masksplit[2]);
          $("input[name='mask4']").val(masksplit[3]);          
          
        });
      
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
        <tr><td>Device name:</td><td><input name="devname" maxlength="32" required></td></tr>
        <tr><td>Device password:</td><td>
          <input id="devpass" name="devpass" type="password" value="" maxlength="32">
          <input type="button" id="devpassbox" name="changedevpass" value="click to change device password"></td></tr>
        <tr><td colspan="2"><br><b>WiFi config:</b><td></tr>
        <tr><td>SSID:</td><td><input name="ssid" maxlength="32" required></td></tr>      
        <tr><td>WiFi password:</td><td>
          <input id="wifipass" name="wifipass" type="password" value="" maxlength="32"> 
          <input type="button" id="wifipassbox" value="click to change WiFi password"></td></tr>        
        <tr><td colspan="2"><br><b>Network config</b><td></tr>
        <tr><td>DHCP:</td><td><input id="dhcpbox" name="dhcp" type="checkbox" value="1"></td></tr>        
        <tr id="iprow"><td>IP:</td><td>
          <input name="ip1" type="number" min="0" max="255" width="50">.
          <input name="ip2" type="number" min="0" max="255" width="50">.
          <input name="ip3" type="number" min="0" max="255" width="50">.
          <input name="ip4" type="number" min="0" max="255" width="50"></td></tr>
        <tr id="maskrow"><td>Mask:</td><td>
          <input name="mask1" type="number" min="0" max="255" width="50">.
          <input name="mask2" type="number" min="0" max="255" width="50">.
          <input name="mask3" type="number" min="0" max="255" width="50">.
          <input name="mask4" type="number" min="0" max="255" width="50"></td></tr>
        <tr id="gwrow"><td>Gateway:</td><td>          
          <input name="gw1" type="number" min="0" max="255" width="50">.
          <input name="gw2" type="number" min="0" max="255" width="50">.
          <input name="gw3" type="number" min="0" max="255" width="50">.
          <input name="gw4" type="number" min="0" max="255" width="50"></td></tr>      
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

  // "/" endpoint
  http_server.on ("/", []() {
    HTTP_AUTH
    http_server.send_P (200, TEXT_HTML, REGULAR_INDEX_HTML);
  });

  // "/config.json" GET endpoint
  http_server.on (FSTR("/config.json"), HTTP_GET, []() {    
    HTTP_AUTH
    char json[300];
    sprintf((char*)&json, 
      FSTR("{\"wifi_ssid\": \"%s\", \"use_dhcp\": %s, \"device_name\": \"%s\", \"ip\": \"%s\", \"mask\": \"%s\", \"gw\": \"%s\"}"),
      config.wifi_ssid,
      config.use_dhcp ? FSTR("true") : FSTR("false"),
      config.device_name,
      WiFi.localIP().toString().c_str(),
      WiFi.subnetMask().toString().c_str(),
      WiFi.gatewayIP().toString().c_str());

    http_server.send(200, APPLICATION_JSON, json);

  });

  // "/config" GET endpoint
  http_server.on (FSTR("/config"), HTTP_GET, []() {          
    HTTP_AUTH
    http_server.send_P(200, TEXT_HTML, REGULAR_CONFIG_HTML);
  });

  // "/config" POST endpoint
  http_server.on(FSTR("/config"), HTTP_POST, []() {
    HTTP_AUTH
    http_server.send_P (200, TEXT_HTML, REGULAR_CONFIG_RESPONSE_HTML);       
    config_set_regular_config(
      http_server.arg(FSTR("ssid")).c_str(), 
      http_server.arg(FSTR("changewifipass")).length() ? http_server.arg(FSTR("wifipass")).c_str() : config.wifi_password, 
      http_server.arg(FSTR("devname")).c_str(), 
      http_server.arg(FSTR("changedevpass")).length() ? http_server.arg(FSTR("devpass")).c_str() : config.device_password,
      http_server.arg(FSTR("dhcp")).length(),
      http_server.arg(FSTR("ip1")).toInt(),
      http_server.arg(FSTR("ip2")).toInt(),
      http_server.arg(FSTR("ip3")).toInt(),
      http_server.arg(FSTR("ip4")).toInt(),
      http_server.arg(FSTR("gw1")).toInt(),
      http_server.arg(FSTR("gw2")).toInt(),
      http_server.arg(FSTR("gw3")).toInt(),
      http_server.arg(FSTR("gw4")).toInt(),
      http_server.arg(FSTR("mask1")).toInt(),
      http_server.arg(FSTR("mask2")).toInt(),
      http_server.arg(FSTR("mask3")).toInt(),
      http_server.arg(FSTR("mask4")).toInt());
    Serial.println("Now rebooting");
    ESP.restart();
  });

  // "/app_config" GET endpoint
  http_server.on (FSTR("/app_config"), HTTP_GET, []() {    
    HTTP_AUTH
    http_server.send_P(200, TEXT_HTML, APP_CONFIG_HTML);
  });

  // "/app_config" POST endpoint
  http_server.on (FSTR("/app_config"), HTTP_POST, []() {
    HTTP_AUTH
    app_config_persist();   
    http_server.send_P(200, TEXT_HTML, APP_CONFIG_RESPONSE_HTML);
    Serial.println("Now rebooting");
    ESP.restart();
  });  

  // "/app_config.json" GET endpoint
  http_server.on (FSTR("/app_config.json"), HTTP_GET, []() {
    HTTP_AUTH
    app_config_send_json();  
  });

  http_server.on(FSTR("/update"), HTTP_GET, []() {
    HTTP_AUTH
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
      HTTP_AUTH
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




