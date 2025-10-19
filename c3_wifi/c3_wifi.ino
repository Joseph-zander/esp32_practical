#include <Wire.h>
#include <RTClib.h>
#include "esp_sleep.h"

#define I2C_SDA 6
#define I2C_SCL 7
#define DS3231_INT_PIN 2

RTC_DS3231 rtc;
RTC_DATA_ATTR int bootCount = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.printf("\n=== 启动 #%d ===\n", ++bootCount);

  // 打印唤醒原因
  printWakeupReason();

  // 初始化I2C
  Wire.setPins(I2C_SDA, I2C_SCL);
  Wire.begin();
  delay(100);

  if (!rtc.begin()) {
    Serial.println("❌ DS3231初始化失败");
    //emergencySleep();
    return;
  }
  Serial.println("✅ DS3231连接成功");

  // 读取时间
  DateTime now = rtc.now();
  Serial.printf("当前时间: %02d:%02d:%02d\n",
                now.hour(), now.minute(), now.second());

  // ⭐⭐ 关键：清除中断标志 ⭐⭐
  clearDS3231Alarm();

  // 设置闹钟（60秒后）
  DateTime alarmTime = now + TimeSpan(0, 0, 0, 60);
  Serial.printf("下次闹钟: %02d:%02d:%02d\n",
                alarmTime.hour(), alarmTime.minute(), alarmTime.second());

  // 设置闹钟并启用中断
  if (setupDS3231Alarm(alarmTime)) {
    Serial.println("✅ 闹钟设置成功");
  } else {
    Serial.println("❌ 闹钟设置失败，使用定时器后备");
  }

  // 配置ESP32唤醒
  setupWakeup();

  // 执行任务
  doWork();

  // 进入睡眠
  Serial.println("\n💤 ======================进入深度睡眠============================");
  Serial.flush();
  delay(100);
  esp_deep_sleep_start();
}

void loop() {}

// 清除DS3231中断标志（兼容所有版本）
void clearDS3231Alarm() {
  // 方法1：尝试使用库函数
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  // 方法2：直接操作寄存器（确保清除）
  Wire.beginTransmission(0x68);
  Wire.write(0x0F);  // 控制/状态寄存器
  Wire.write(0x00);  // 清除A1F和A2F标志
  Wire.endTransmission();

  Serial.println("✅ 清除中断标志完成");
}

// 设置DS3231闹钟（兼容所有版本）
bool setupDS3231Alarm(const DateTime &dt) {
  bool success = false;

  // 设置闹钟时间为30秒后
  uint8_t alarm_seconds = ((dt.second() / 10) << 4) | (dt.second() % 10);
  uint8_t alarm_minutes = ((dt.minute() / 10) << 4) | (dt.minute() % 10);
  uint8_t alarm_hours = ((dt.hour() / 10) << 4) | (dt.hour() % 10);
  uint8_t alarm_day = ((dt.day() / 10) << 4) | (dt.day() % 10);

  // 设置闹钟1寄存器
  Wire.beginTransmission(0x68);
  Wire.write(0x07);  // Alarm1起始地址

  // 设置秒（A1M1=0，匹配秒）
  Wire.write(alarm_seconds & 0x7F);
  // 设置分（A1M2=0，匹配分）
  Wire.write(alarm_minutes & 0x7F);
  // 设置时（A1M3=0，匹配时）
  Wire.write(alarm_hours & 0x7F);
  // 设置日（A1M4=0，匹配日）
  Wire.write(alarm_day & 0x7F);

  if (Wire.endTransmission() == 0) {
    success = true;
    Serial.println("  闹钟时间设置完成");
  }

  if (success) {
    // 启用闹钟1中断
    Wire.beginTransmission(0x68);
    Wire.write(0x0E);  // 控制寄存器地址
    Wire.write(0x05);  // INTCN=1, A1IE=1（启用闹钟1中断）
    Wire.endTransmission();
    Serial.println("  闹钟中断已启用");
  }

  return success;
}

// 设置ESP32唤醒
void setupWakeup() {
  pinMode(DS3231_INT_PIN, INPUT_PULLUP);

  // 使用EXT0唤醒（低电平触发）
  //esp_sleep_enable_ext0_wakeup((gpio_num_t)DS3231_INT_PIN, 0);

  // 添加后备定时器（60秒）
  esp_deep_sleep_enable_gpio_wakeup(DS3231_INT_PIN, ESP_GPIO_WAKEUP_GPIO_LOW);
  Serial.println("✅ 唤醒配置完成");
}

// 打印唤醒原因
void printWakeupReason() {
  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
  switch (cause) {
    case ESP_SLEEP_WAKEUP_EXT0:
      Serial.println("🔋 唤醒原因: DS3231闹钟");
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("⏱️  唤醒原因: 内部定时器");
      break;
    case ESP_SLEEP_WAKEUP_UNDEFINED:
      Serial.println("🔌 唤醒原因: 上电复位");
      break;
    default:
      Serial.printf("❓ 唤醒原因: 未知 (%d)\n", cause);
      break;
  }
}

// 执行任务
void doWork() {
  Serial.println("\n🔧 执行任务中...");
  delay(2000);
  Serial.println("✅ 任务完成");
}

// 紧急睡眠
void emergencySleep() {
  Serial.println("⚠️ 使用定时器唤醒（30秒）");
  esp_sleep_enable_timer_wakeup(60 * 1000000);
  esp_deep_sleep_start();
}