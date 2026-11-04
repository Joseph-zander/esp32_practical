#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WiFi.h>
#include <WiFiMulti.h> 
#include <ESPmDNS.h>
#include <Preferences.h>
#include "ESPAsyncWebServer.h"
#include "AsyncTCP.h"

// 声明外部可引用的异步服务器实例
extern AsyncWebServer server;
extern Preferences localStore;

// 初始化异步 Web 服务器的函数声明
void initAsyncWebServer();
void loadSavedText();
void writeText();
void loadSavedText();
#endif
