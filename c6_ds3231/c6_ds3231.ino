#include <WiFi.h>
#include "time.h"
#include <Wire.h>
#include "RTClib.h"

const char* ssid = "XiaoBao-WIFI";
const char* password = "20031999";
RTC_DS3231 rtc;
const int wakeUpPin = 2;
RTC_DATA_ATTR bool synced = false;

void setup() {
  Serial.begin(115200);
  Serial.println("\n===================系统启动================");

  // 1. 初始化DS3231
  Wire.begin(6, 7);
  if (!rtc.begin()) {
    Serial.println("DS3231初始化失败");
    while (1)
      ;
  }

  // 2. 显示当前时间
  showTime("当前时间");

  // 3. 同步网络时间（只同步一次）
  if (!synced && connectWiFi()) {
    getBeijingTime();
    synced = true;
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    showTime("新时间");
  }
  // 4. 设置RTC闹钟
  setRTCAlarm();

  // 4. 睡眠30秒
  Serial.println("睡眠30秒...");
  pinMode(wakeUpPin, INPUT_PULLUP);
  //esp_sleep_enable_gpio_wakeup();
  esp_deep_sleep_enable_gpio_wakeup(1 << wakeUpPin, ESP_GPIO_WAKEUP_GPIO_LOW);
  // 重要：给串口足够的时间输出
  Serial.flush();
  delay(100);

  Serial.println("进入深度睡眠...");
  esp_deep_sleep_start();
}

void loop() {}

// 设置DS3231闹钟
void setRTCAlarm() {
  Serial.println("设置DS3231闹钟...");

  // 获取当前时间
  DateTime now = rtc.now();

  // 计算下次唤醒时间
  DateTime alarmTime = now + TimeSpan(0, 0, 1, 0);

  // 设置闹钟1（每分钟触发一次）
  // 使用DS3231的每分钟闹钟模式
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  rtc.writeSqwPinMode(DS3231_OFF);

  rtc.disableAlarm(1);
  rtc.disableAlarm(2);

  // 设置闹钟为每分钟的0秒触发
  rtc.setAlarm1(alarmTime, DS3231_A1_Second);

  // 验证设置
  DateTime alarm1 = rtc.getAlarm1();
  Serial.print("读取闹钟: ");
  Serial.print(alarm1.hour());
  Serial.print(":");
  Serial.print(alarm1.minute());
  Serial.print(":");
  Serial.println(alarm1.second());

  // 重要：检查闹钟标志状态
  if (rtc.alarmFired(1)) {
    Serial.println("警告: 闹钟1已触发，正在清除...");
    rtc.clearAlarm(1);
  }

  if (rtc.alarmFired(2)) {
    Serial.println("警告: 闹钟2已触发，正在清除...");
    rtc.clearAlarm(2);
  }
}

// 连接WiFi
bool connectWiFi() {
  Serial.print("连接WiFi");
  WiFi.begin(ssid, password);
  for (int i = 0; i < 20; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(" 成功");
      return true;
    }

    delay(200);
    Serial.print(".");
  }
  Serial.println(" 失败");
  return false;
}

// 获取并保存北京时间
void getBeijingTime() {
  Serial.println("获取北京时间...");
  configTime(8 * 3600, 0, "ntp.aliyun.com");

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    // 保存到DS3231
    rtc.adjust(DateTime(
      timeinfo.tm_year + 1900,
      timeinfo.tm_mon + 1,
      timeinfo.tm_mday,
      timeinfo.tm_hour,
      timeinfo.tm_min,
      timeinfo.tm_sec));
    Serial.println("时间已保存到DS3231");
  } else {
    Serial.println("获取失败");
  }
}

// 显示时间
void showTime(const char* msg) {
  DateTime now = rtc.now();
  Serial.printf("%s: %02d:%02d:%02d\n", msg, now.hour(), now.minute(), now.second());
}