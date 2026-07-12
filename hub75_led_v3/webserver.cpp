#include "WebServer.h"
#include "LedFunction.h"
#include <ESPmDNS.h>

// 定义监听 80 端口的异步 Web 服务器实例
AsyncWebServer server(80);

// 封装的网页 HTML 源码（支持 UTF-8 编码，集成配置表单和前端动态时钟）
const char* htmlContent = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>LED 屏配置后台</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; margin-top: 30px; background-color: #f7f7f7; }
        .container { max-width: 500px; margin: auto; padding: 20px; border: 1px solid #ccc; border-radius: 10px; background: white; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }
        input[type="text"] { width: 90%; padding: 12px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; font-size: 16px; box-sizing: border-box; }
        input[type="submit"] { background-color: #007BFF; color: white; border: none; padding: 12px 24px; font-size: 16px; border-radius: 5px; cursor: pointer; transition: background 0.3s; }
        input[type="submit"]:hover { background-color: #0056b3; }
        .footer { font-size: 14px; color: #666; margin-top: 15px; }
    </style>
</head>
<body>
    <div class="container">
        <h2>LED屏配置后台</h2>
        <p>系统状态：异步监听中（零开销，不影响 LED 帧率）</p>
        
        <form action="/set" method="POST">
            <label><strong>第一页滚动文字：</strong></label><br>
            <input type="text" name="p1" placeholder="当前: 中大国际，欢迎您！" required>
            <br><br>
            <label><strong>第二页静态文字：</strong></label><br>
            <input type="text" name="p2" placeholder="当前: 工程部宣" required>
            <br><br>
            <input type="submit" value="保存并实时推送到屏幕">
        </form>

        <p class="footer">服务器本地时间: <span id="time">--:--:--</span></p>
    </div>
    <script>
        function updateTime() {
            var now = new Date();
            document.getElementById('time').innerHTML = now.getHours().toString().padStart(2,'0') + ":" + now.getMinutes().toString().padStart(2,'0') + ":" + now.getSeconds().toString().padStart(2,'0');
        }
        setInterval(updateTime, 1000);
        updateTime();
    </script>
</body>
</html>
)rawliteral";

/**
 * 初始化网络并注册异步服务器路由
 */
void initAsyncWebServer(const char* ssid, const char* password) {
  long startTime = millis();
  while (!Serial && (millis() - startTime < 3000)) {
    delay(10); // 最多等3秒，防止拔掉USB独立供电时卡死
  }
  // 1. 连接 WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
      // ★ 核心逻辑：开启本地域名解析，这样不需要知道 IP 也能访问
    if (MDNS.begin("led")) {  // 将域名注册为 led.local
      Serial.println("[mDNS] 域名解析服务启动成功！");
      Serial.println("[mDNS] 以后您可以直接在浏览器输入访问: http://led.local");
      MDNS.addService("http", "tcp", 80); // 将 HTTP 网页服务广播出去
    } else {
      Serial.println("[mDNS] 域名解析服务启动失败。");
    }
  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // 2. 注册路由回调处理函数
  // 根目录：直接下发配置网页
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/html", htmlContent);
  });

  // 表单接收接口：提取 POST 参数，无缝对接并修改 LED 端的全局变量
  server.on("/set", HTTP_POST, [](AsyncWebServerRequest *request){
      if (request->hasParam("p1", true) && request->hasParam("p2", true)) {
          // 线程安全直接给全局 String 变量赋值
          page1Text = request->getParam("p1", true)->value();
          page2Text = request->getParam("p2", true)->value();
          textOffset = 0; // 重置文字滚动偏移量，让新文本从头滚动
          
          // 返回带 UTF-8 支持的成功弹窗提示，并于 1.5 秒后跳转回主页
          String successHtml = "<html><head><meta charset='UTF-8'></head><body><center><h3>配置已成功接收！正在应用并返回...</h3></center><script>setTimeout(function(){window.location.href='/';},1500);</script></body></html>";
          request->send(200, "text/html", successHtml);
          Serial.println("通过异步 Web 收到新配置内容并成功更新！");
      } else {
          request->send(400, "text/plain", "Bad Form Parameter");
      }
  });

  // 404 页面未找到处理
  server.onNotFound([](AsyncWebServerRequest *request){
      request->send(404, "text/plain", "Not Found");
  });
  
  // 3. 启动异步 HTTP 服务器
  server.begin();
  Serial.println("Async HTTP server started.");
}
