#include <WiFi.h>
#include <WebServer.h>

// WiFi 配置
const char* ssid = "XiaoBao-WIFI";   //ZD_Office  XiaoBao-WIFI
const char* password = "20031999";   //12345678 20031999

// 创建 WebServer 对象，端口 80
WebServer server(80);

// HTML 页面内容
String getHTML(String plantName = "") {
  String html = "<!DOCTYPE html><html lang=\"zh-CN\">";
  html += "<meta charset=\"utf-8\">";
  html += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<title>文字输入测试</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; padding: 20px; }";
  html += "h1 { color: #2e7d32; }";
  html += "input[type=text] { padding: 10px; width: 80%; max-width: 300px; }";
  html += "input[type=submit] { padding: 10px 20px; margin: 10px; background-color: #4CAF50; color: white; border: none; border-radius: 5px; }";
  html += ".message { color: #388e3c; margin-top: 20px; font-size: 18px; }";
  html += "</style></head>";
  html += "<body>";
  html += "<h1>人机交互测试</h1>";
  html += "<p>请输入您的名字:</p>";
  html += "<form action=\"/submit\" method=\"POST\">";
  html += "<input type=\"text\" name=\"plantName\" placeholder=\"比如, 中大国际\">";
  html += "<br><input type=\"submit\" value=\"确  认\">";
  html += "</form>";
  if (plantName != "") {
    html += "<p class=\"message\">收到您的名字: " + plantName + "</p>";
  }
  html += "</body></html>";
  return html;
}

// 处理根路径请求
void handleRoot() {
  server.send(200, "text/html", getHTML());
}

// 处理植物名称提交请求
void handleSubmit() {
  if (server.hasArg("plantName")) {
    String plantName = server.arg("plantName");
    Serial.println("Received plant name: " + plantName);
    server.send(200, "text/html", getHTML(plantName));
  } else {
    server.send(200, "text/html", getHTML("No plant name received"));
  }
}

void setup() {
  // 初始化串口
  Serial.begin(115200);
  // 连接到 WiFi 网络
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // 打印 ESP32 的 IP 地址
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // 设置路由
  server.on("/", handleRoot);
  server.on("/submit", HTTP_POST, handleSubmit);
  
  // 启动服务器
  server.begin();
  Serial.println("Plant Web Server started");
}

void loop() {
  server.handleClient();
}