#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include "AsyncTCP.h"



// 声明外部可引用的异步服务器实例
extern AsyncWebServer server;

// 初始化异步 Web 服务器的函数声明
void initAsyncWebServer(const char* ssid, const char* password);

#endif
