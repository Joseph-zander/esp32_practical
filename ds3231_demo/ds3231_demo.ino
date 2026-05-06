#include <WiFi.h>
#include <Wire.h>
#include <RTClib.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
/**
*  C6完整版完美通过! C3/C6 Super Mini均有问题，不是代码问题。
*/

RTC_DS3231 rtc;
#define I2C_SDA 6
#define I2C_SCL 7
#define DS3231_INT_PIN 4
// 闹钟延时几秒后唤醒
#define WAIT_SECOND 60
#define NTP_OFFSET 8 * 60 * 60  // 东八区 UTC+8
#define WIFI_SSID "XiaoBao-WIFI"
#define WIFI_PASS "20031999"


// 深度睡眠保持的标记变量(放在RTC_DATA_ATTR里)
RTC_DATA_ATTR bool firstBoot = true;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", NTP_OFFSET);
void syncNTPtoRTC();

String getRTCTimeString(const DateTime& now, bool dateOnly);
void setupAlarm();

void setup() {
  Serial.begin(115200);
  delay(5000);

  Serial.println("\n===== ESP 启动 =====");

  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000);

  if (!rtc.begin()) {
    Serial.println("未找到DS3231模块！");
    while (1)
      ;
  }

  // 2. NTP 自动对时（每次上电都对一次）
  if (firstBoot) {
    Serial.println("第一次启动，先校对DS321的时间...");
    syncNTPtoRTC();
  }
  firstBoot = false;

  // if (rtc.lostPower()) {
  //   Serial.println("DS3231时间丢失，重新设置初始时间......");
  //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // }

  /*可以做到10s唤醒一次。
   esp_sleep_enable_timer_wakeup(10 * 1000000); // 10 秒
   esp_deep_sleep_start();
   */
  // 必须关闭方波，让INT脚输出闹钟中断
  rtc.writeSqwPinMode(DS3231_OFF);

  // 清理旧闹钟
  rtc.clearAlarm(1);
  rtc.disableAlarm(2);

  // 设置 10 秒后闹钟
  setupAlarm();

  if (rtc.alarmFired(1)) {
    rtc.clearAlarm(1);
    Serial.println("检查到闹钟标识位已重置，清理标识");
  }

  pinMode(DS3231_INT_PIN, INPUT_PULLUP);
  esp_deep_sleep_enable_gpio_wakeup(1ULL << DS3231_INT_PIN, ESP_GPIO_WAKEUP_GPIO_LOW);

  Serial.println("准备进入深度睡眠,等待DS3231报警唤醒...");
  delay(3000);
  Serial.print(getRTCTimeString(rtc.now(), false));
  Serial.println("进入深度睡眠模式...");
  Serial.flush();

  esp_deep_sleep_start();
}

// ==================== NTP 对时函数 ====================
void syncNTPtoRTC() {
  Serial.print("连接 WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int retry = 0;

  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi 连接成功！");
    timeClient.update();

    // 获取 NTP 时间
    unsigned long epoch = timeClient.getEpochTime();
    DateTime ntpTime(epoch);

    // 写入 RTC
    rtc.adjust(ntpTime);
    Serial.println("✅ NTP 对时完成，已同步到 DS3231");
  } else {
    Serial.println("\n❌ WiFi 连接失败，使用旧 RTC 时间");
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);  // 关闭WiFi省电
}

void setupAlarm() {
  DateTime now = rtc.now();
  // 计算未来时间（自动处理进位）
  DateTime alarmTime = now + TimeSpan(0, 0, 0, WAIT_SECOND);
  // 闹钟模式：匹配 时分秒日 → 到点就触发
  rtc.setAlarm1(alarmTime, DS3231_A1_Second);
}

String getRTCTimeString(const DateTime& now, bool dateOnly) {
  char buf[32];
  snprintf(buf, sizeof(buf),
           dateOnly
             ? "%04d-%02d-%02d"
             : "%04d/%02d/%02d %02d:%02d:%02d",
           now.year(),
           now.month(),
           now.day(),
           now.hour(),
           now.minute(),
           now.second());

  return String(buf);
}

void loop() {
  // DateTime now = rtc.now();
  // Serial.println(getRTCTimeString(now, false));
  // delay(10000);
}
