#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;  // 创建 RTC 对象

#define SDA_PIN 8  // ESP32-S3的SDA引脚
#define SCL_PIN 9  // ESP32-S3的SCL引脚



void setup() {
  Serial.begin(115200);
  Wire.begin(8, 9); 
  Serial.println(" Wire.begin(8, 9)。。。");

   // 检测 RTC 是否存在
  if (!rtc.begin()) {
    Serial.println("未检测到 DS3231M 模块！");
    return;
  }
  Serial.println("DS3231初始化成功");
  if (rtc.lostPower()) {
      Serial.println("RTC丢失时间，设置初始时间");
      //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // 使用编译时间初始化
      //Serial.println("DS3231初始化(使用编译时间初始化)...");
  }
  Serial.println("DS3231初始化成功22222");

}

void loop() {
  // 读取当前时间
  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  Serial.print("温度: ");
  Serial.print(rtc.getTemperature());
  Serial.println(" °C");
  delay(2000);
}