#include <WiFi.h>

const char* ssid = "ESP32-C3-Hotspot";   // 开的热点名称
const char* password = "12345678";       // 热点密码

const char* sta_ssid = "XiaoBao-WIFI";       // 家里路由器 WiFi
const char* sta_password = "20031999";   // 家里 WiFi 密码

void setup() {
  Serial.begin(115200);

  // STA模式：连接路由器
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(sta_ssid, sta_password);

  Serial.print("Connecting to router");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to router, IP: ");
  Serial.println(WiFi.localIP());

  // AP模式：开热点
  bool apStarted = WiFi.softAP(ssid, password);
  if (apStarted) {
    Serial.println("Hotspot started");
    Serial.print("Hotspot IP: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("Hotspot start failed!");
  }
}

void loop() {
  // 在这里可以跑一个 WebServer 给手机访问
}
