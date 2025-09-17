#include <WiFi.h>

const char* ssid = "XiaoBao-WIFI"; // 已确认正确的 SSID
const char* password = "20031999"; // 已确认正确的密码

void setup() {
  Serial.begin(115200);
  delay(1000); // 等待串口初始化

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("扫描 WiFi 网络...");
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("未找到任何网络");
  } else {
    Serial.print("找到 ");
    Serial.print(n);
    Serial.println(" 个网络：");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (信号强度: ");
      Serial.print(WiFi.RSSI(i));
      Serial.println(" dBm)");
    }
  }
  Serial.println(WiFi.macAddress());
  WiFi.begin(ssid, password);
  // 等待连接（15 秒超时）
  int attempts = 0;
  // 等待连接，最多尝试 30 次（15秒）
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
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
    Serial.print("Final WiFi Status: ");
    Serial.println(WiFi.status());
  }
}

void loop() {
  // 定期检查 WiFi 状态
  Serial.println("loop done.");
  delay(5000);
}