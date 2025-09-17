#include <Arduino.h>
#include <RTClib.h>

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

// 返回 const char* 版本（注意：使用 static 缓冲区）
const char* getMillisCStr() {
    static char buf[20];                  // 静态缓冲区，避免函数返回后失效
    unsigned long t = millis();
    sprintf(buf, "%lu", t);               // 格式化到字符串
    return buf;
}

void log(const char* text) {
  Serial.println(text); // Implement log function
}
void log(const char* title, const char* text) {
  Serial.print(title);
  Serial.println(text);
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

// 将整数转换为字符串，填充前导零
void intToStr(int num, char* str, int width) {
  for (int i = width - 1; i >= 0; i--) {
    str[i] = (num % 10) + '0';
    num /= 10;
  }
  str[width] = '\0'; // 确保字符串终止
}