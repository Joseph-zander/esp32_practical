#include <WiFi.h>
#include <Wire.h>
#include <RTClib.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// NTP客户端设置
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "cn.pool.ntp.org", 0, 60000);  // UTC时间，更新间隔60秒

#define SDA_PIN 8  // ESP32-S3的SDA引脚
#define SCL_PIN 9  // ESP32-S3的SCL引脚
// WiFi凭据（替换为你的WiFi信息）
const char* ssid = "XiaoBao-WIFI";  // 替换为你的WiFi名称
const char* password = "20031999";  // 替换为你的WiFi密码
RTC_DS3231 rtc;  // DS3231M对象（单一声明）

void connectWifi() {
  // 连接WiFi
  Serial.print("连接到WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi连接成功");
    Serial.print("IP地址: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi连接失败，请检查SSID和密码");
    return;
  }

  // 初始化NTP客户端
  timeClient.begin();
  timeClient.setTimeOffset(28800);  //获取北京时间
  // 获取并同步时间
  if (timeClient.update()) {
    Serial.println("从NTP服务器获取时间成功");
    unsigned long epochTime = timeClient.getEpochTime();
    DateTime ntpTime(epochTime);

    // 同步到DS3231M
   // rtc.adjust(ntpTime);


    Serial.print("NTP时间: ");
    Serial.print(ntpTime.year(), DEC);
    Serial.print('/');
    Serial.print(ntpTime.month(), DEC);
    Serial.print('/');
    Serial.print(ntpTime.day(), DEC);
    Serial.print(" ");
    Serial.print(ntpTime.hour(), DEC);
    Serial.print(':');
    Serial.print(ntpTime.minute(), DEC);
    Serial.print(':');
    Serial.print(ntpTime.second(), DEC);
    Serial.println();


    Serial.println("DS3231M时间已同步");
  } else {
    Serial.println("无法获取NTP时间");
  }

  // 断开WiFi以节省电源（可选）
  WiFi.disconnect();
  Serial.println("WiFi已断开");
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN); 
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


  connectWifi();
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
  delay(3000);
}