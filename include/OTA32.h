#pragma once

#include <WiFi.h> 
#include <ArduinoOTA.h>
#include "OLED.h"

const char* ssidAP     = "ESP32-Access-Point";
const char* passwordAP = "vikings!";

inline const char* ssid = "CenturyLink9329";
inline const char* password = "xufxbhsa7nc9ct";

void setupAccessPoint() {
 
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssidAP, passwordAP);

  // Set static IP
  IPAddress AP_LOCAL_IP(10, 0, 0, 2);
  IPAddress AP_GATEWAY_IP(0, 0, 0, 0);
  IPAddress AP_NETWORK_MASK(255, 255, 255, 0);
  if (!WiFi.softAPConfig(AP_LOCAL_IP, AP_GATEWAY_IP, AP_NETWORK_MASK)) {
    Serial.println("AP Config Failed");
    return;
  }

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
}

////////////////////////////////////////////////
// Connect to WiFi. STA(0), AP(1), AP_STA(2)  //
////////////////////////////////////////////////
inline void connectWiFi(uint8_t wifi_mode=0)
{
  // LOGLN("Connecting Wifi");
  drawText(1, 0, "Connecting...");

  WiFi.disconnect(); // Just in case
  delay(1000); 
    
  if (wifi_mode == 0) {
    Serial.println("WIFI_STA mode");
    drawText(2, 0, String(ssid));
    WiFi.mode(WIFI_STA); // Mode 0 (default)
    WiFi.setAutoConnect(true);      
    WiFi.setAutoReconnect(true);    
    WiFi.begin(ssid, password);
    Serial.println(String(WiFi.localIP()));

  } else if(wifi_mode == 1) {
    Serial.println("WIFI_AP mode");
    drawText(2, 0, String(ssidAP));
    WiFi.mode(WIFI_AP); // Mode 1 
    
    WiFi.softAP(ssidAP, passwordAP);

    // Set static IP
    IPAddress AP_LOCAL_IP(10, 0, 0, 2);
    IPAddress AP_GATEWAY_IP(0, 0, 0, 0);
    IPAddress AP_NETWORK_MASK(255, 255, 255, 0);
    if (!WiFi.softAPConfig(AP_LOCAL_IP, AP_GATEWAY_IP, AP_NETWORK_MASK)) {
      Serial.println("AP Config Failed");
      return;
    }
    IPAddress IP = WiFi.softAPIP();
    clearDisplayBelowHeader();
    drawWiFi(); drawAP();
    drawText(1, 0, "Connected");
    drawText(2, 0, "IP Address");drawText(2, 62, IP.toString());
    Serial.print("AP IP address: ");
    Serial.println(IP);
    
  } else if(wifi_mode == 2) {
    Serial.println("WIFI_AP_STA mode");
    WiFi.mode(WIFI_AP_STA); // Mode 2
  } 
  else {
    Serial.println("Invalid WiFi mode");
    return;
  }
    
  byte count = 0;
  while(WiFi.status() != WL_CONNECTED && count < 10)
  {
    count ++;
    delay(500);
  }

  // if (WiFi.status() == WL_CONNECTED) {
  //   clearDisplayBelowHeader();
  //   drawText(1, 0, "Connected");
  //   drawText(2, 0, "IP Address");drawText(2, 62, String(IP));
  //   // drawText(2, 0, "IP Address");drawText(2, 62, String(WiFi.localIP()[3]));
  //   // Add the WiFi icon
  //   drawWiFi();
  //   delay(1000);
   
  //   clearDisplayBelowHeader();      
  // } 
  // else {
  //   drawText(1, 0, "WiFi connect failed!");
  // }  
}

//////////////////////
// Setup OTA        //
//////////////////////
inline void setupOTA()
{
  //No authentication by default
//  ArduinoOTA.setPassword(password);
  // LOGLN("Setting up OTA");
  
  ArduinoOTA.onStart([]()
  {
    clearDisplay();
    drawText(0, 0, "Start Updating....");

    // Serial.printf("Start Updating....Type:%s\n", (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem");
  });

  ArduinoOTA.onEnd([]()
  {
    clearDisplay();
    drawText(0, 0, "Update Complete!");
    // LOGLN("Update Complete!");

    ESP.restart();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {});

  ArduinoOTA.onError([](ota_error_t error)
  {
    // Serial.printf("Error[%u]: ", error);
    String info = "Error Info:";
    switch(error)
    {
      case OTA_AUTH_ERROR:
        info += "Auth Failed";
        // LOGLN("Auth Failed");
        break;

      case OTA_BEGIN_ERROR:
        info += "Begin Failed";
        // LOGLN("Begin Failed");        
        break;

      case OTA_CONNECT_ERROR:
        info += "Connect Failed";
        // LOGLN("Connect Failed");        
        break;

      case OTA_RECEIVE_ERROR:
        info += "Receive Failed";
        // LOGLN("Receive Failed");        
        break;

      case OTA_END_ERROR:
        info += "End Failed";
        // LOGLN("End Failed");        
        break;
    }

    clearDisplay();
    drawText(0, 0, info);
   
    ESP.restart();
  });

  ArduinoOTA.begin();
  
  drawText(5, 0, "Started OTA");
  
}