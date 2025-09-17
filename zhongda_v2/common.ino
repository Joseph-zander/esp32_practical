#include <Arduino.h>
#include <stdio.h>
#include <RTClib.h>
#include "common.h"

void log(const char* text) {
  Serial.println(text); 
}

void log(const char* title, const char* text) {
  Serial.print(title);
  Serial.println(text);
}

void log(const char* title, const char* text, const char* content) {
    Serial.print(title);
    Serial.print(text);
    Serial.println(content);
}

void log(String title, const char* text) {
  Serial.print(title);
  Serial.println(text);
}

void log(IPAddress ip, const char* prefix, Print& output) {
  char buffer[100];
  snprintf(buffer, sizeof(buffer), "%s %d.%d.%d.%d", prefix, ip[0], ip[1], ip[2], ip[3]);
  output.println(buffer);
}

// 将整数转换为字符串，填充前导零
void intToStr(int num, char* str, int width) {
  for (int i = width - 1; i >= 0; i--) {
    str[i] = (num % 10) + '0';
    num /= 10;
  }
  str[width] = '\0'; // 确保字符串终止
}

/**
* 用于批量对指定的针脚设置为同一个状态
*/
void setPinsState(int* pinArr, int count, uint8_t state) {
  for(int i=0; i< count; i++) {
    digitalWrite(pinArr[i], state);
  }
}

void setPinsMode(int* pinArr, int count, uint8_t mode) {
    for(int i=0; i< count; i++) {
      pinMode(pinArr[i], mode);
    }
}

/**
*  格式化RTC时间为字符串（YYYY-MM-DD HH:MM:SS），使用如下：
    char timeBuffer[20];
    formatDateTime(rtc->now(), timeBuffer);
    Serial.print("周期启动于: ");
    Serial.println(timeBuffer);
*/
void formatDateTime(DateTime dt, char* buffer) {
  // 年（4位）
  intToStr(dt.year(), buffer, 4);
  buffer[4] = '-';
  // 月（2位）
  intToStr(dt.month(), buffer + 5, 2);
  buffer[7] = '-';
  // 日（2位）
  intToStr(dt.day(), buffer + 8, 2);
  buffer[10] = ' ';
  // 时（2位）
  intToStr(dt.hour(), buffer + 11, 2);
  buffer[13] = ':';
  // 分（2位）
  intToStr(dt.minute(), buffer + 14, 2);
  buffer[16] = ':';
  // 秒（2位）
  intToStr(dt.second(), buffer + 17, 2);
  buffer[19] = '\0'; // 确保字符串终止
}

// 返回写入的字符数，或者在失败时返回负值
int formatDate(char* buffer, size_t bufferSize) {
    if (!rtc.begin()) {
        log("找不到 RTC 模块！");
        return -1; // 返回错误码
    }
    DateTime now = rtc.now();
    return snprintf(buffer, bufferSize, "%04d-%02d-%02d %02d:%02d:%02d",
                    now.year(), now.month(), now.day(),
                    now.hour(), now.minute(), now.second());
}





