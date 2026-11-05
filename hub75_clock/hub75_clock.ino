#include <Arduino.h>
#include <WiFi.h>
#include "ledFunction.h"

const char* ssid     = "XiaoBao-WIFI";
const char* password = "20031999";
const char* ntpServer = "ntp.aliyun.com"; 

void setup() {
     Serial.begin(115200);
    
    // 1. 初始化屏幕
    initMatrix();
    
    // 2. 显示正在联网提示
    showStatusText("正在联网...", matrix->color565(255, 165, 0)); // 橙色提示
    
    WiFi.begin(ssid, password);
    
    // 限制联网超时时间为 10 秒，防止无网时无限死机
    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED && timeout < 20) {
        delay(500);
        timeout++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        // 3. 联网成功，显示绿色状态
        showStatusText("联网成功!", matrix->color565(0, 255, 0));
        delay(800);
        
        // 4. 配置时区并自动从网络同步时间（东八区北京时间：8 * 3600 = 28800）
        configTime(28800, 0, ntpServer                            );
        
        // 等待底层自动完成第一次 NTP 握手（最多等待 3 秒）
        struct tm timeinfo;
        int retry = 0;
        while (!getLocalTime(&timeinfo) && retry < 6) {
            delay(500);
            retry++;
        }
    } else {
        // 联网失败，提示离线运行
        showStatusText("无网运行", matrix->color565(255, 0, 0));
        delay(1500);
        
        // 兜底时间（如果完全没网，从这个时间开始走时）
        struct tm tm_init;
        tm_init.tm_year = 2026 - 1900; tm_init.tm_mon = 11 - 1; tm_init.tm_mday = 5;
        tm_init.tm_hour = 22; tm_init.tm_min = 45; tm_init.tm_sec = 0;
        time_t t = mktime(&tm_init);
        struct timeval tv = { .tv_sec = t };
        settimeofday(&tv, NULL);
    }
    
    matrix->clearScreen();
}

void loop() {
    struct tm timeinfo;
    
    // getLocalTime 会直接读取 ESP32-S3 内部硬件 RTC 计数器，时间精度极高
    if (getLocalTime(&timeinfo)) {
        updateClockDisplay(&timeinfo);
    }
    
    // 每 150 毫秒高刷一次，确保秒针跳变丝滑、瞬间响应
    delay(150); 
}
