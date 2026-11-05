#include "WebServer.h"
#include "LedFunction.h"
#include <ESPmDNS.h>

// 定义监听 80 端口的异步 Web 服务器实例
AsyncWebServer server(80);

// 封装的网页 HTML 源码
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

// 异步连接控制变量
static bool isServerConfigured = false;
static int currentAttempt = 0;
const int AP_COUNT = 6;
const int MAX_ATTEMPTS_PER_AP = 10;
const int TOTAL_MAX_ATTEMPTS = AP_COUNT * MAX_ATTEMPTS_PER_AP;

// 系统初始化时，从闪存中读取保存的文字
void loadSavedText() {
    localStore.begin("led_storage", true);
    page1Text = localStore.getString("p1", "中大国际，欢迎您！");
    page2Text = localStore.getString("p2", "工程部宣");
    localStore.end();
    Serial.println("[闪存] 已成功从本地存储中恢复历史文字配置！");
}

// 同步将其强制锁进 Flash 闪存
void writeText() {
    localStore.begin("led_storage", false);
    localStore.putString("p1", page1Text);
    localStore.putString("p2", page2Text);
    localStore.end();
    Serial.println("[闪存] 新文字已安全同步保存至持久化闪存分区！");
}

// 配置并启动 Web 服务器相关路由（仅在初次获取 IP 时调用一次）
void startWebServices() {
    if (isServerConfigured) return; // 避免重复注册

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
            writeText();
            textOffset = 0; // 重置点阵屏滚动偏移量
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

    server.begin();
    Serial.println("Async HTTP server started.");
    isServerConfigured = true;
}

// WiFi 事件回调函数：捕获底层事件，实现真正的异步
void onWiFiEvent(WiFiEvent_t event) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            startWebServices(); // 拿到IP后，立即在后台拉起服务器
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("[WiFi] 物理连接断开，底层尝试自动关联或等待下次多网轮询...");
            break;
        default:
            break;
    }
}

/**
 * 瞬间初始化的网络函数（零阻塞，直接返回）
 */
void initAsyncWebServer() {
    Serial.println("\n[WiFi] 正在异步初始化多后台 WiFi 扫描机制...");
    
    // 1. 注册网络事件监听
    WiFi.onEvent(onWiFiEvent);
    WiFi.mode(WIFI_STA);

    // 2. 录入多组 Wi-Fi 账号密码
    wifiMulti.addAP("Magic Pro", "20031999");
    wifiMulti.addAP("ZD_Office", "12345678");
    wifiMulti.addAP("ZD_Office", "87654321");
    wifiMulti.addAP("ZD_Office", "88888888");
    wifiMulti.addAP("ZDGJ_FreeWifi", "88888888");
    wifiMulti.addAP("XiaoBao-WIFI", "20031999");

    currentAttempt = 0;
    isWiFiActive = true;
    
    Serial.println("[WiFi] 异步配置就绪，LED 主程序开始运行，连接动作将在主循环切片执行。");
}

/**
 * 【关键核心】需要在你的主项目全局 loop() 中高频调用的非阻塞轮询函数
 */
void handleWiFiAsyncProgress() {
    // 如果已经连上，或者 WiFi 已经被熔断关闭，则直接跳过，零开销
    if (WiFi.status() == WL_CONNECTED || !isWiFiActive) {
        return;
    }

    // 利用时间戳切片代替 delay()，每隔 500ms 驱动一次 wifiMulti 状态机
    static unsigned long lastWiFiCheckTime = 0;
    if (millis() - lastWiFiCheckTime < 500) {
        return; 
    }
    lastWiFiCheckTime = millis();

    // 尝试进行一次非阻塞的 WiFi 匹配尝试
    if (wifiMulti.run() == WL_CONNECTED) {
        // 连接成功，底层会触发 ARDUINO_EVENT_WIFI_STA_GOT_IP 事件去启动服务器
        return;
    }
    // 计数器累加
    currentAttempt++;
    Serial.printf("[WiFi] 正在后台匹配可用 Wi-Fi... (当前切片进度: %d/%d)\n", currentAttempt, TOTAL_MAX_ATTEMPTS);
    if (currentAttempt >= TOTAL_MAX_ATTEMPTS) {
        Serial.println("\n[WiFi] 熔断保护触发：所有预设 WiFi 均不可用，已放弃连接并关闭射频。");
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        isWiFiActive = false;
    }
}
