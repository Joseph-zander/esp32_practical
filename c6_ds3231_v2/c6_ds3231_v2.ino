#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include "time.h"

const char* ssid     = "XiaoBao-WIFI";
const char* password = "20031999";

RTC_DS3231 rtc;
const int wakeUpPin = 1;  // 连接到 DS3231 的 SQW/INT
void syncRTCTime();

// 联网校准 RTC 时间的函数
void syncRTCTime() {
    Serial.print("正在连接 WiFi...");
    WiFi.begin(ssid, password);
    
    int retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 20) {
        delay(500);
        Serial.print(".");
        retry++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi 已连接，同步网络时间...");
        configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");
        
        struct tm timeinfo;
        if (getLocalTime(&timeinfo)) {
            rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, 
                                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
            Serial.println("RTC 时间已校准！");
        }
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
    } else {
        Serial.println("\nWiFi 连接失败，无法校准时间。");
    }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin(6, 7);  // ESP32-C6 的 SDA, SCL
  Serial.println("系统开始初始化......");
  if (!rtc.begin()) {
    Serial.println("找不到 RTC!");
    while (1)
      ;
  }
  Serial.println("找到 RTC!");
  // 检查 RTC 是否因为电池没电停止了
if (rtc.lostPower()) {
    Serial.println("RTC 掉电了，重新校准时间...");
    syncRTCTime();
  } else {
    // 如果没掉电，我们读取当前 RTC 时间
    DateTime now = rtc.now();
    Serial.printf("当前 RTC 时间正常: %02d:%02d:%02d\n", now.hour(), now.minute(), now.second());
  }

  // 1. 清除旧的闹钟设置
  rtc.disableAlarm(1);
  rtc.disableAlarm(2);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  rtc.writeSqwPinMode(DS3231_OFF);  // 确保引脚用于中断而非方波

  Serial.println("ESP32-C6 已唤醒，执行任务中...");
  delay(1000);  // 模拟工作任务

  // 2. 计算下一次唤醒时间 (当前 + 30秒)
  DateTime now = rtc.now();
  DateTime nextWakeup = now + TimeSpan(30);

  // 3. 设置闹钟 1
  // DS3231_A1_Second 表示匹配秒数，也可以用更为复杂的匹配方式
  rtc.setAlarm1(nextWakeup, DS3231_A1_Second);

  Serial.printf("下一次唤醒时间: %02d:%02d:%02d\n", nextWakeup.hour(), nextWakeup.minute(), nextWakeup.second());

  // 4. 配置 ESP32 唤醒源 (低电平触发)
  // 对于 C6，建议使用 LP GPIO 并配合相应的唤醒函数
  esp_deep_sleep_enable_gpio_wakeup(1 << wakeUpPin, ESP_GPIO_WAKEUP_GPIO_LOW);

  Serial.println("进入深度睡眠...");
  Serial.flush();

  // 5. 睡觉
  esp_deep_sleep_start();
}

void loop() {
  // Deep Sleep 模式下不会进入 loop
}