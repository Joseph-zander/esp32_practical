#include "MyWifi.h"

// 存储多组 Wi-Fi 凭据（SSID和密码）
const char* ssid[] = {"ZD_Office", "XiaoBao-WIFI"};
const char* password[] = {"12345678", "20031999"};
const int numCredentials = 3; // 凭据组的数量
int currentCredentialIndex = 0; // 当前正在尝试的凭据索引

bool connectToWiFi(TFT_eSPI& tft) {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  int attempts = 0; // 记录当前凭据的尝试次数
  // 遍历所有凭据组
  for (int i = 0; i < numCredentials; i++) {
    currentCredentialIndex = i; // 更新当前索引
    if (i > 0) {
      tft.setCursor(0, tft.getCursorY() + 8);
    }
    tft.setCursor(0, tft.getCursorY() + 4);
    tft.print(ssid[currentCredentialIndex]);
    tft.print("...");
    WiFi.begin(ssid[currentCredentialIndex], password[currentCredentialIndex]);
    // 尝试连接10次
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
      delay(500);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      tft.setCursor(tft.getCursorX(), tft.getCursorY());
      tft.println("已连接！");
      return true; // 连接成功
    } else {
      //Serial.println("\n连接失败，尝试下一组凭据.");
      attempts = 0; // 重置尝试次数
    }
  }
  Serial.println("所有凭据组均连接失败。");
  return false; // 所有凭据都尝试失败
}

void closeWiFi() {
    WiFi.disconnect(true, true);  // 清除配置和断开连接
    WiFi.mode(WIFI_OFF);          // 关闭WiFi模块
}