#include "MyWifi.h"
#include <esp32-hal-timer.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

const int buttonPin = 8;    // 按钮连接到引脚 8
const int outputPin = 9;    // 输出（例如 LED）连接到引脚 9
hw_timer_t* timer = NULL;  // 定时器句柄

// 变量
int pressCount = 0;         // 记录有效按键次数

// 创建一个异步 Web 服务器实例，监听端口80
AsyncWebServer server(80);
void httpPost();
void startWebServer();
void IRAM_ATTR onTimer();
// HTML网页内容，使用原始字符串字面量
const char* htmlContent = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>文件上传</title>
    <style>
        html, body {
            margin: 0;
            padding: 0;
            width: 100%;
            height: 100%;
            overflow-x: hidden;
        }
            #main {
                width: 100%; 
                border: none;
                min-height: 600px; 
                display: block; 
            }
    </style>
</head>
<body>
    <iframe id="main" src="http://m.xacss.icu/hospital/page/excel" frameorder="0" scrolling="auto"></iframe>
</body>
</html>
)rawliteral";

void IRAM_ATTR onTimer() {
    //Serial.print("检查WIFI连接状态。。。");
    connectToWifi();
}

void setup() {
    Serial.begin(115200);
    pinMode(buttonPin, INPUT_PULLUP); // 设置按钮引脚为输入并启用内部上拉电阻
    pinMode(outputPin, OUTPUT);       // 设置输出引脚为输出
    digitalWrite(outputPin, LOW);    // 初始化输出引脚为低电平

    timer = timerBegin(1000000);            // 设置定时器频率为 1 MHz（1微秒分辨率）
    timerAttachInterrupt(timer, &onTimer);  // 绑定中断
    timerAlarm(timer, 10 * 1000 * 1000, true, 0);
    timerStart(timer);

    /**连接WIFI服务器*/
    bool result = connectToWifi();
    if (!result) {
        return;
    }
    //报告当前IP地址
    httpPost();

    // 启动 Web 服务器
    startWebServer();
}

void httpPost() {
    const char* ip = getIpAddress();
    HTTPClient http;
    http.begin("http://m.xacss.icu/hospital/api/ip");
    http.addHeader("Content-Type", "application/json");
    String ipAddress = WiFi.localIP().toString();
    String postData = "{\"ip\":\""+ipAddress+"\"}";
    int httpCode = http.POST(postData);
    if (httpCode > 0) {
      String response = http.getString();  // 获取响应
      Serial.println("HTTP Code: " + String(httpCode));  // 201 表示成功
      Serial.println("Sent JSON: " + postData);
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error on POST: " + String(httpCode));
    }
    http.end();  // 关闭连接
}

void startWebServer() {
  // 设置根目录处理函数
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", htmlContent);
  });

  // 错误处理，如果找不到页面则返回404
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not Found");
  });

  // 启动服务器
  server.begin();
  Serial.println("Async HTTP server started.");
}

void loop() {
    // 读取当前按钮状态
    int reading = digitalRead(buttonPin);
    if (reading == HIGH) {
        return;
    }
    delay(100);
    if (digitalRead(buttonPin) == HIGH) {
        return;
    }
    pressCount = pressCount + 1;
    if (pressCount > 10000 && pressCount % 2 == 0) {
        pressCount = 0;
    }
    Serial.print("pressCount % 2 = ");
    Serial.println(pressCount % 2);
    digitalWrite(outputPin, (pressCount % 2 > 0) ? HIGH : LOW); 
    delay(200);
}
