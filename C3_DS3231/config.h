#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// 定义 WiFi 账号结构体
struct WiFiConfig {
    const char* ssid;
    const char* password;
};

// 配置多组 WiFi 账号
inline const WiFiConfig g_wifi_list[] = {
    {"XiaoBao-WIFI", "20031999"},
    {"ZD_Office", "88888888"},
    {"Magic Pro", "20031999"},
    {"ZDGJ_FreeWifi", NULL},
};

// 获取数组长度
inline const int WIFI_COUNT = sizeof(g_wifi_list) / sizeof(g_wifi_list[0]);

inline const int SYNC_TIMEOUT_MS = 20000; // 搜索多组 WiFi 建议增加超时时间
inline const int GMT_OFFSET_SEC  = 8 * 3600;

// ===============配置常量=============================================
inline const char* WIFI_SSID = "XiaoBao-WIFI";
inline const char* WIFI_PASSWORD = "20031999";
// ================================================================

/**
**从 C++17 开始，你可以通过 inline 告诉编译器：这个常量在多个文件里看起来是重复的，但实际上它们是同一个。
** 也可以使用如下定义
#define WIFI_SSID     "XiaoBao-WIFI"
#define WIFI_PASSWORD "20031999"
*/
#endif