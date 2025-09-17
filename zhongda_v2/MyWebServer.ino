#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "MyWebServer.h"

void bindService(AsyncWebServer& server) {
     // 设置根目录处理函数
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/html; charset=utf-8", htmlContent);
        //request->send(200, "text/plain; charset=utf-8", "啥也不是");
    });

        // 错误处理，如果找不到页面则返回404
    server.onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain; charset=\"utf-8\"", "没有任何信息。。。");
    });
}