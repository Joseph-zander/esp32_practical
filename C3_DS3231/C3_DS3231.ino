#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include "time.h"

// 引入你的独立配置文件
#include "config.h"
#include "network_utils.h" // 只引入声明文件

RTC_DS3231 rtc;

void setup() {
   Serial.begin(115200);
  delay(2000);

  WiFi.mode(WIFI_STA);
  
  // 核心步骤：将发射功率限制在 11dBm（默认是 20dBm）
  // 这能减少近一半的瞬时峰值电流，防止电压跌落导致断连
  //WiFi.setTxPower(WIFI_POWER_11dBm); 
  
  WiFi.begin("XiaoBao-WIFI", "20031999");
  //WiFi.begin("Magic Pro", "20031999");

  Serial.print("Connecting");
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 40) {
    delay(500);
    Serial.print(".");
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n连接成功！");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.printf("\n连接失败，最终状态码: %d\n", WiFi.status());
    // 如果还是 6，尝试连接手机热点做最后验证
  }
}

void setup2() {
  Serial.begin(115200);
  Wire.begin(GPIO_NUM_8, GPIO_NUM_9);
  wifiSyncTime3(); 
  while (!rtc.begin()) {
    Serial.println("未检测到 RTC，10秒后重试...");
    delay(10000);    // 真正的延时函数
    //ESP.restart();  // 软件重启芯片
  }
  Serial.println("RTC 连接成功！");

  // 检查唤醒原因
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:
      Serial.println("闹钟响了，起床干活！");  //RTC唤醒了ESP32-C3
      // 这里可以读取传感器数据并保存
      break;
    case ESP_SLEEP_WAKEUP_UNDEFINED:
    default:
      Serial.println("检测到冷启动，正在同步 NTP 时间...");
      wifiSyncTime3();  // 只有这里才费电跑网络
      break;
  }
  // 设置下一次 30 秒后的闹钟
  setupNextAlarm();

  // 使用 ext1 唤醒，支持多个引脚
  gpio_num_t wakeup_pin = GPIO_NUM_4;  // 选择你要作为唤醒的引脚 (这里使用 GPIO 4)
  esp_deep_sleep_enable_gpio_wakeup(1ULL << wakeup_pin, ESP_GPIO_WAKEUP_GPIO_LOW);

  Serial.println("Entering deep sleep......");
  Serial.flush();
  delay(100);
  // #ESP32 开始休眠
  esp_deep_sleep_start();
}

/**设置下个闹钟的到来时间*/
void setupNextAlarm() {
  rtc.disableAlarm(1);
  rtc.clearAlarm(1);

  DateTime now = rtc.now();
  // 计算下一次 30 秒的时刻
  int nextSecond = (now.second() < 30) ? 30 : 0;
  // 设置闹钟 1：匹配秒数
  rtc.setAlarm1(DateTime(0, 0, 0, 0, 0, nextSecond), DS3231_A1_Second);
}




void loop() {
}
