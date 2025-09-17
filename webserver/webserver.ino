#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include "AsyncTCP.h"

// 替换成你的WiFi名称和密码
const char* ssid = "XiaoBao-WIFI";
const char* password = "20031999";

// 创建一个异步 Web 服务器实例，监听端口80
AsyncWebServer server(80);

// HTML网页内容，使用原始字符串字面量
const char* htmlContent = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Async ESP32 Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
        .container { max-width: 600px; margin: auto; padding: 20px; border: 1px solid #ccc; border-radius: 10px; }
    </style>
</head>
<body>
    <div class="container">
        <h2>高性能异步 Web 服务器</h2>
        <p>这是一个异步服务器，可以同时处理多个请求。</p>
        <p>服务器时间: <span id="time"></span></p>
    </div>
    <script>
        function updateTime() {
            var now = new Date();
            document.getElementById('time').innerHTML = now.getHours() + ":" + now.getMinutes() + ":" + now.getSeconds();
        }
        setInterval(updateTime, 1000); // 每秒更新时间
    </script>
</body>
</html>
)rawliteral";

void setup() {
    Serial.begin(115200);

    // 连接 WiFi
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // 设置根目录处理函数
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", htmlContent);
    });

    // 错误处理，如果找不到页面则返回404
    server.onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain", "Not Found");
    });
    
    // 启动服务器
    server.begin();
    Serial.println("Async HTTP server started.");
}

void loop() {
  // 异步服务器的 loop() 函数中不需要 server.handleClient()
  // 所有的请求处理都在后台自动进行
}