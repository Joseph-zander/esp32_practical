<<<<<<< HEAD
#include "WebServer.h"
#include "LedFunction.h"


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

static WiFiMulti wifiMulti;
Preferences localStore;
bool isWiFiActive = true;   // 当前 Wi-Fi 射频是否处于开启状态

//系统初始化时，从闪存中读取保存的文字，如果没保存过则使用默认值
void loadSavedText() {
  // 以只读模式（true）打开名为 "led_storage" 的分区
  localStore.begin("led_storage", true);

  // 读取 "p1" 键对应的值，如果闪存里是空的，则使用第二个参数作为“默认保底值”
  page1Text = localStore.getString("p1", "中大国际，欢迎您！");
  page2Text = localStore.getString("p2", "工程部宣");

  localStore.end();  // 关闭存储分区
  Serial.println("[闪存] 已成功从本地存储中恢复历史文字配置！");
}

void writeText() {
  //同步将其强制锁进 Flash 闪存
  localStore.begin("led_storage", false);
  localStore.putString("p1", page1Text);
  localStore.putString("p2", page2Text);
  localStore.end();
  Serial.println("[闪存] 新文字已安全同步保存至持久化闪存分区！");
}

/**
 * 初始化网络（支持多 WiFi 轮询测试，且限制最大尝试次数）
 */
void initAsyncWebServer() {
  long startTime = millis();
  while (!Serial && (millis() - startTime < 3000)) {
    delay(10);
  }

  Serial.println("\n[WiFi] 正在初始化多后台 WiFi 扫描机制...");
  WiFi.mode(WIFI_STA);

  // 1. 定义和录入多组 Wi-Fi 账号密码
  const int AP_COUNT = 6;  // 这里写下你一共配置了多少组 Wi-Fi
  wifiMulti.addAP("Magic Pro", "20031999");
  wifiMulti.addAP("ZD_Office", "12345678");
  wifiMulti.addAP("ZD_Office", "87654321");
  wifiMulti.addAP("ZD_Office", "88888888");
  wifiMulti.addAP("ZDGJ_FreeWifi", "88888888");
  wifiMulti.addAP("XiaoBao-WIFI", "20031999");

  // 2. 核心限次逻辑：单组最大测试 10 次，换算为整体最大轮询总次数
  const int MAX_ATTEMPTS_PER_AP = 10;
  const int TOTAL_MAX_ATTEMPTS = AP_COUNT * MAX_ATTEMPTS_PER_AP;
  int currentAttempt = 0;
  bool isConnected = false;

  Serial.println("[WiFi] 开始轮流匹配可用 Wi-Fi 网络（每组最多测试 10 次）...");

  // 3. 执行限次轮询检测
  while (currentAttempt < TOTAL_MAX_ATTEMPTS) {
    if (wifiMulti.run() == WL_CONNECTED) {
      isConnected = true;
      break;  // 只要连上任意一个，立即跳出循环
    }
    currentAttempt++;
    Serial.printf("[WiFi] 正在尝试连接... (当前总进度: %d/%d)\n", currentAttempt, TOTAL_MAX_ATTEMPTS);
    delay(200);  // 每次尝试间歇 1 秒，给芯片足够的基带握手响应时间
  }

  // 4. 根据连接结果进行分支处理
  if (isConnected) {
    Serial.println("\n[WiFi] 成功建立网络连接！");
    Serial.print("Connected to SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // 开启本地域名解析
    if (MDNS.begin("led")) {
      Serial.println("[mDNS] 域名解析服务启动成功！浏览器访问: http://led.local");
      MDNS.addService("http", "tcp", 80);
    }

    // 注册根目录路由
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
      request->send(200, "text/html", htmlContent);
    });

    // 注册表单接收接口
    server.on("/set", HTTP_POST, [](AsyncWebServerRequest* request) {
      if (request->hasParam("p1", true) && request->hasParam("p2", true)) {
        page1Text = request->getParam("p1", true)->value();
        page2Text = request->getParam("p2", true)->value();
        //保存文本
        writeText();
        textOffset = 0;
        String successHtml = "<html><head><meta charset='UTF-8'></head><body><center><h3>配置已成功接收！正在应用并返回...</h3></center><script>setTimeout(function(){window.location.href='/';},1500);</script></body></html>";
        request->send(200, "text/html", successHtml);
        Serial.println("通过异步 Web 收到新配置内容并成功更新！");
      } else {
        request->send(400, "text/plain", "Bad Form Parameter");
      }
    });

    // 404 页面
    server.onNotFound([](AsyncWebServerRequest* request) {
      request->send(404, "text/plain", "Not Found");
    });

    // 启动异步 HTTP 服务器
    server.begin();
    Serial.println("Async HTTP server started.");

  } else {
    // 🔥 熔断保护：所有 Wi-Fi 尝试满 10 次均失败，放弃连网
    WiFi.disconnect(true);  //1. 关闭 Wi-Fi 模块节省功耗，防止干扰点阵屏硬件
    WiFi.mode(WIFI_OFF);    //2. 彻底关闭 Wi-Fi 射频硬件核心（WIFI_OFF），切断功耗与射频干扰
    isWiFiActive = false;
  }
}
=======
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
>>>>>>> 1016203e2e60d6cbbdd1c8a06538f08beba5414a
