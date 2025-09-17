#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <TFT_eSPI.h>
#include <time.h>
#include <Ticker.h>
#include "common.h"
#include "MyWebServer.h"
#include "MyWifi.h"
#include "eva.h"

TFT_eSPI tft;
//wifi最多连接3次，失败后就放弃
const int maxAttempts = 3;

// NTP 配置
const char* ntpServer = "0.asia.pool.ntp.org"; // 更快的中国/亚洲服务器
const long gmtOffset_sec = 8 * 3600;
const int daylightOffset_sec = 0;

RTC_DS3231 rtc;
Ticker t;
// 创建一个异步 Web 服务器实例，监听端口80
AsyncWebServer server(80);
void syncTime();
void tft_init();

//定义3个GPIO端口
int pins[] = {40, 41};
int numPins = 2;

#define SDA_PIN 8  // ESP32-S3的SDA引脚
#define SCL_PIN 9  // ESP32-S3的SCL引脚

// HTML网页内容，使用原始字符串字面量
const char* htmlContent = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Async Web Server</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <script src="https://unpkg.com/vue@3/dist/vue.global.js"></script>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; padding: 20px; }
        h1 { color: #2e7d32; }
        input[type=text] { padding: 10px; width: 80%; max-width: 300px; }
        input[type=submit] { padding: 10px 20px; margin: 10px; background-color: #4CAF50; color: white; border: none; border-radius: 5px; }
        .message { color: #388e3c; margin-top: 20px; font-size: 18px; }
    </style>
</head>
<body>
    <div id="app">
      <h1>人机交互系统</h1>
      <form @submit.prevent="submitForm">
        <input type="text" v-model="plantName" name="plantName" placeholder="比如, 中大国际" autocomplete="off" />
        <br>
        <input type="submit" value="确  认" />
      </form>
      <span v-show="isVisible">{{ plantName }}</span>
      <div class="message" v-if="message">{{ message }}</div>
    </div>
    <script>
    const { createApp, ref } = Vue;
        createApp({
            setup() {
                const plantName = ref('');
                const isVisible = ref(false);
                const message = ref('');
                function submitForm() {
                    isVisible.value = true;
                    message.value = `您输入的是：${plantName.value}`;
                }
                return {
                    plantName,
                    isVisible,
                    message,
                    submitForm
                };
            }
        }).mount('#app');
    </script>
</body>
</html>
)rawliteral";

int linePosition = 16;
int lineOffset = 4;
void getXY() {
  int16_t currentX = tft.getCursorX();
  int16_t currentY = tft.getCursorY();
  Serial.println();
  Serial.printf("Current cursor: X=%d, Y=%d", currentX, currentY);
}
void tft_init() {
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.loadFont(eva);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  getXY();
}

void timer_callback(){
    const int second = rtc.now().second();
    char timeBuffer[30];
    int len = formatDate(timeBuffer, sizeof(timeBuffer));
    if (len > 0) {
        log(timeBuffer, ">> timer_callback");
    } else {
      return;
    }
    if (second < 50) {
        if (digitalRead(pins[0]) == LOW) {
            log("开始检查checkState...HIGH，", timeBuffer);
            setPinsState(pins, numPins, HIGH);
        }
    } else {
        if(digitalRead(pins[0]) == HIGH) {
            log("开始检查checkState...LOW，", timeBuffer);
            setPinsState(pins, numPins, LOW);
        }
    }
}

void syncTime() {
    // 检测 RTC 是否存在
    if (!rtc.begin()) {
      log("未检测到 DS3231M 模块！");
      return;
    }
    // 检查 DS3231 是否失去电源（需要初始化）
    if (rtc.lostPower()) {
        log("DS3231 lost power, needs time setting");
    }
    log("开始同步NTP时间到DS3231M...");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                            timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
        Serial.println("DS3231 synchronized with NTP");
    } else {
        Serial.println("Failed to sync with NTP");
    }
}

void setup() {
    Serial.begin(115200);
    //digitalWrite(TFT_BL, HIGH);
    Wire.begin(SDA_PIN, SCL_PIN); 
    Serial.println(" Wire.begin(8, 9)。。。");

    /**----------------------------------------**/
    setPinsMode(pins, numPins, OUTPUT);
    pinMode(42, OUTPUT);

        // 初始化IO口为高电平
    setPinsState(pins, numPins, HIGH);
    digitalWrite(42, LOW);
    Serial.println("初始化tft...");
    tft_init();

    tft.println("初始化,尝试连接WIFI...");
    connectToWiFi(tft);
    if (WiFi.status() != WL_CONNECTED) {
        log("连接WIFI失败");
    }
    //t.attach_ms(1000, timer_callback);
    tft.setCursor(0, tft.getCursorY() + lineOffset);
    tft.print("IP 地址: ");
    tft.println(WiFi.localIP());
    tft.setCursor(0, tft.getCursorY() + lineOffset);
    tft.println("继电器连接测试...");
    tft.setCursor(55, 100);
    tft.println("中大国际商业中心");
    bindService(server);
    // 启动服务器
    server.begin();
    Serial.println("Async HTTP server started.");

}

void loop() {
      // 调用示例
    // char myBuffer[30];
    // int len = formatDate(myBuffer, sizeof(myBuffer));
    // if (len > 0) {
    //     log(myBuffer, ">> hello world");
    // }
    // 
    log("loop......");
    delay(5000);
}
