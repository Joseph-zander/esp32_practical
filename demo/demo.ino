#include <Arduino.h>
#include <RTClib.h>
#include <Ticker.h>

RTC_DS3231 rtc;
Ticker t;

//定义3个GPIO端口
int pins[] = {35, 36, 37};
int numPins = 3;

void timer_callback(){
  char timeBuffer[20];
  formatDateTime(rtc.now(), timeBuffer);
  const int second = rtc.now().second();
  if (second < 40) {
     if (digitalRead(pins[0]) == LOW) {
        log("开始检查checkState...HIGH，", timeBuffer);
        setPinsState(pins, numPins, HIGH);
     }
  } else {
    if(digitalRead(pins[0]) == HIGH) {
        log("开始检查checkState...LOW，", timeBuffer);
        setPinsState(pins, numPins, LOW);
    }
  }
}

void setup() {
  Serial.begin(115200);
     // 检测 RTC 是否存在
  if (!rtc.begin()) {
    Serial.println("未检测到 DS3231M 模块！");
    return;
  }
  setPinsMode(pins, numPins, OUTPUT);
    // 初始化IO口为高电平
  setPinsState(pins, numPins, HIGH);
  t.attach_ms(1000, timer_callback);  
  // 设置按钮引脚为输入，使用内部上拉电阻

}

void loop() {
  char timeBuffer[20];
  formatDateTime(rtc.now(), timeBuffer);
  Serial.println(timeBuffer);
  delay(3000);
}