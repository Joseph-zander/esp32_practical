#include <esp32-hal-timer.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
// Wi-Fi 配置
const char* ssid = "XiaoBao-WIFI"; // 替换为你的 Wi-Fi 名称
const char* password = "20031999"; // 替换为你的 Wi-Fi 密码

// 定义 NTP 客户端
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 8 * 3600, 60000); // 时区设置为 CST（+8小时），更新间隔60秒

#define LED_PIN 0
/**
* 该例子基于ESP32 v3.30版本， 
* 不同版本，函数签名有些差异，注意区分。
*/
hw_timer_t *timer = NULL; // 定时器句柄

void IRAM_ATTR onTimer() {
  Serial.println(" onTimer被调用");
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));  // 更新LED状态
}

void setup() {
   Serial.begin(115200);
   pinMode(LED_PIN, OUTPUT);
  
  timer = timerBegin(1000000); // 设置定时器频率为 1 MHz（1微秒分辨率）
  timerAttachInterrupt(timer, &onTimer); // 绑定中断
  timerAlarm(timer, 5000*1000, true, 0);
  timerStart(timer);

  WiFi.mode(WIFI_STA);
  // 设置最大传输功率（增强信号）
  WiFi.setTxPower(WIFI_POWER_19_5dBm); // ESP32-C3 支持的最大功率
  WiFi.begin(ssid, password);
  // 连接 Wi-Fi
// 等待连接，最多尝试 10 秒
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print("WiFi Status: ");
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL: Serial.println("No SSID Available"); break;
      case WL_CONNECT_FAILED: Serial.println("Connect Failed"); break;
      case WL_CONNECTION_LOST: Serial.println("Connection Lost"); break;
      case WL_DISCONNECTED: Serial.println("Disconnected"); break;
      case WL_IDLE_STATUS: Serial.println("Idle"); break;
      case WL_CONNECTED: Serial.println("Connected"); break;
      default: Serial.print("Unknown Status: "); Serial.println(WiFi.status()); break;
    }
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi.");
  }

  // 初始化 NTP 客户端
  timeClient.begin();
}

void loop() {
  timeClient.update();
  // 获取格式化时间
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Current time: ");
  Serial.println(formattedTime);

  // 获取时间戳（Unix 时间）
  unsigned long epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);

  // 转换为本地时间（示例：获取年月日）
  time_t rawtime = timeClient.getEpochTime();
  struct tm* timeinfo = localtime(&rawtime);
  char timeStringBuff[50];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d %H:%M:%S", timeinfo);
  Serial.print("Local Time: ");
  Serial.print(timeStringBuff);
  
  Serial.println(" hello world!");
  delay(3000);
}

