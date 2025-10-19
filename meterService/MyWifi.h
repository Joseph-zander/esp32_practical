#include <WiFi.h>

#ifndef MY_WIFI_H
#define MY_WIFI_H
  struct WiFiCredential { 
    const char* ssid; 
    const char* password; 
  };

 bool connectToWifi();
 const char* getIpAddress();

#endif