#include <WiFi.h>
#include <WiFiMulti.h> // 引入 WiFiMulti 库
#include <RTClib.h>
#include "time.h"
#include "config.h"
#include "network_utils.h"

// 引用主文件里的 rtc 对象
extern RTC_DS3231 rtc;

void wifiSyncTime() {
    WiFiMulti wifiMulti; // 创建 WiFiMulti 实例
    // 1. 注册所有已知的 WiFi 账号
    for (int i = 0; i < WIFI_COUNT; i++) {
        wifiMulti.addAP(g_wifi_list[i].ssid, g_wifi_list[i].password);
        Serial.printf("已添加备选 WiFi: %s\n", g_wifi_list[i].ssid);
    }

    Serial.println("正在寻找信号最强的已知 WiFi...");

    // 2. 尝试连接 (wifiMulti 会自动切换尝试), wifiMulti.run()为连接网络。
    unsigned long startAttemptTime = millis();
    while (wifiMulti.run() != WL_CONNECTED && millis() - startAttemptTime < SYNC_TIMEOUT_MS) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\n[错误] 所有 WiFi 均连接失败");
        return;
    }

    Serial.printf("\n[成功] 已连接至: %s\n", WiFi.SSID().c_str());
    
    // 3. 执行 NTP 校时
    configTime(GMT_OFFSET_SEC, 0, "ntp.aliyun.com", "pool.ntp.org");
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 10000)) {
        rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                            timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
        Serial.println("✅ RTC 硬件校时完成");
    }

    // 4. 清理并关闭
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

void wifiSyncTime3() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);
  WiFi.setTxPower(WIFI_POWER_11dBm);
  WiFi.begin("Magic Pro", "20031999");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(500);
    Serial.print("WiFi Status: ");
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL: Serial.println("No SSID Available"); break;
      case WL_CONNECT_FAILED: Serial.println("Connect Failed"); break;
      case WL_CONNECTION_LOST: Serial.println("Connection Lost"); break;
      case WL_DISCONNECTED: Serial.println("Disconnected"); break;
      case WL_IDLE_STATUS: Serial.println("Idle"); break;
      case WL_CONNECTED: Serial.println("Connected"); break;
      default: Serial.print("Unknown Status: "); Serial.println(WiFi.status()); break;
    }
    attempts++;
  }
    if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi.");
    Serial.print("Final WiFi Status: ");
    Serial.println(WiFi.status());
  }
}

/**
* 通过WIFI校对时间
*/
void wifiSyncTime2() {
  Serial.printf("正在连接 WiFi: %s ", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // 1. 带超时的连接逻辑
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
    delay(200);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n[错误] WiFi 连接超时");
    return;
  }
  Serial.println("\n[成功] WiFi 已连接，获取网络时间...");

  // 2. 配置 NTP
  configTime(8 * 3600, 0, "ntp.aliyun.com", "pool.ntp.org");

  // 3. 等待获取时间（getLocalTime 默认会等一段时间）
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 10000)) { // 增加 10 秒等待超时
    rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                        timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
    Serial.println("✅ RTC 硬件对时成功！");
  } else {
    Serial.println("❌ 无法从 NTP 服务器获取时间");
  }

    // 4. 彻底关闭 WiFi 节省功耗
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF); 
    Serial.println("WiFi 已关闭 (降功耗)");
  }