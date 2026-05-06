#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;
#define CLOCK_INTERRUPT_PIN 2
volatile int execCount = 0;

void onAlarm() {
  execCount++;
  rtc.clearAlarm(1);
  doSome();
  DateTime next = rtc.now() + TimeSpan(10);
  rtc.writeSqwPinMode(DS3231_OFF);
  rtc.setAlarm1(next, DS3231_A1_Second);
  rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
}

void doSome() {
  Serial.print("执行 doSome #");
  Serial.println(execCount);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(GPIO_NUM_6, GPIO_NUM_7);
  
  rtc.begin();
  rtc.writeSqwPinMode(DS3231_OFF);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  rtc.disableAlarm(2);
  pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), onAlarm, FALLING);
  DateTime first = rtc.now() + TimeSpan(10);
  rtc.setAlarm1(first, DS3231_A1_Second);
  rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
  Serial.println("DS3231定时器启动");
}

void loop() {
  delay(1000);
}
