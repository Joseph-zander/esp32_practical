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

// 将整数转换为字符串，填充前导零
void intToStr(int num, char* str, int width) {
  for (int i = width - 1; i >= 0; i--) {
    str[i] = (num % 10) + '0';
    num /= 10;
  }
  str[width] = '\0'; // 确保字符串终止
}