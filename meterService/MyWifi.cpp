#include "MyWifi.h"

// WiFi 凭证数组（可添加更多）
WiFiCredential wifiCredentials[] = {
  {"XiaoBao-WIFI", "20031999"},      // 第一个网络
  {"ZD_Office", "12345678"},  // 第二个网络
  {"Magic Pro", "20031999"}    // 第三个网络
};
int numCredentials = sizeof(wifiCredentials) / sizeof(wifiCredentials[0]);


bool connectToWifi() {
  if (WiFi.status() == WL_CONNECTED) {
      return true;
  }
  Serial.println("WIFI已连上!");
  // 循环尝试每个凭证
  for (int i = 0; i < numCredentials; i++) {
    Serial.print("尝试连接: ");
    Serial.println(wifiCredentials[i].ssid);

    WiFi.begin(wifiCredentials[i].ssid, wifiCredentials[i].password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {  // 尝试 20 秒
      delay(100);
      Serial.print(".");
      attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("WiFi 连接成功!");
      Serial.print("IP 地址: ");
      Serial.println(WiFi.localIP());
      return true;  // 成功后退出循环
    } else {
      Serial.println();
      Serial.println("连接失败，尝试下一个...");
      WiFi.disconnect();
    }
  }
  Serial.println("所有 WiFi 均连接失败!");
  return false;
}

// 获取当前 IP 地址的函数，返回 const char*
const char* getIpAddress() {
  static char ipBuffer[16] = "";  // 静态缓冲区存储 IP 地址（格式如 "192.168.255.255"）

  ipBuffer[0] = '\0';  // 清空缓冲区
  if (WiFi.status() == WL_CONNECTED) {
    IPAddress ip = WiFi.localIP();
    // 直接格式化 IP 地址到缓冲区
    snprintf(ipBuffer, sizeof(ipBuffer), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  }

  return ipBuffer;  // 返回静态缓冲区的指针
}

