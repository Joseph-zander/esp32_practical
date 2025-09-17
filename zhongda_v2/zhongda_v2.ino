#include <Arduino.h>
#include <RTClib.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <time.h>
#include <Ticker.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>             // SPI library
#include "common.h"
#include "MyWebServer.h"


// Define pins for ESP32-S3
#define TFT_SCK    13  // Custom SCK pin (instead of GPIO18)
#define TFT_MOSI   12  // Custom MOSI pin (instead of GPIO23)
#define TFT_RST    4   // Reset pin
#define TFT_DC     5   // Data/Command pin
#define TFT_CS     15  // Chip select
// Initialize ST7735 with custom pins (software SPI)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

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
int pins[] = {35, 36, 37};
int numPins = 3;
// WiFi 凭据（请替换为你的 WiFi 名称和密码）
const char* ssid = "XiaoBao-WIFI";   //XiaoBao-WIFI  ZD_Office
const char* password = "20031999";   //20031999      12345678

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

void tft_init() {
 // Initialize the display
  tft.initR(INITR_BLACKTAB);  // For common ST7735S modules

  tft.setRotation(1);  // Rotate 90 degrees clockwise (160x128)
  // Fill screen with black
  tft.fillScreen(ST7735_BLACK);

  // Set text color and size
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);

  // Set cursor position
  tft.setCursor(10, 60);

  // Print "Hello World!"
  tft.println("Hello World!");

  Serial.println("Done!");
}
void timer_callback(){
    const int second = rtc.now().second();
    char timeBuffer[30];
    int len = formatDate(timeBuffer, sizeof(timeBuffer));
    if (len > 0) {
        log(timeBuffer, ">> timer_callback");
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
    /**----------------------------------------**/
    setPinsMode(pins, numPins, OUTPUT);
        // 初始化IO口为高电平
    setPinsState(pins, numPins, HIGH);
    /***-------------------------------------------*/
    // 连接 WiFi
    WiFi.begin(ssid, password);
    log("Connecting to WiFi: ", ssid);
    bool connected = true;
    int count = 0;
    while (WiFi.status() != WL_CONNECTED) {
        connected = false;
        count = count + 1;
        if (count < 20) {
            delay(500);
            log("... ");
            continue;
        }
        break;
    }
    if (WiFi.status() == WL_CONNECTED) {
        connected = true;
        syncTime();
        // 关闭WiFi
        //WiFi.disconnect(true, true);  // 清除配置和断开连接
        //WiFi.mode(WIFI_OFF);          // 关闭WiFi模块
        log("时间同步完成，关闭wifi.");
    }
    if (!connected) {
        log("连接WIFI失败。WiFi:", ssid);
    }
    t.attach_ms(1000, timer_callback);
    tft_init();
    log(WiFi.localIP(), "Connection successful, IP address is: ");
   // Set cursor position
    tft.setCursor(10, 80);

    // Print "Hello World!"
    tft.println("IP address is: ");
    // Set cursor position
    tft.setCursor(30, 100);
    tft.println(WiFi.localIP());
    bindService(server);
    // 启动服务器
    server.begin();
    Serial.println("Async HTTP server started.");
}

void loop() {
    // 调用示例
    char myBuffer[30];
    int len = formatDate(myBuffer, sizeof(myBuffer));
    if (len > 0) {
        log(myBuffer, ">> hello world");
    }
    delay(3000);
}
