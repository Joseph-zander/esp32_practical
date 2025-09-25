#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#define myLED 8  // LED 引脚（根据你的板子调整，例如 GPIO10）

// WiFi 热点配置
const char *ssid = "ESP32C3_Hotspot";  // 热点名称
const char *password = "12345678";     // 密码（至少 8 位）
WiFiServer server(80);                 // HTTP 服务器端口 80

void setup() {
  Serial.begin(115200);
  pinMode(myLED, OUTPUT);
  digitalWrite(myLED, LOW);

  // 开启热点
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);  // 通常为 192.168.4.1

  // 启动服务器
  server.begin();
  Serial.println("Server started");
  Serial.print("Connect to WiFi: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);
}

void loop() {
  WiFiClient client = server.available();  // 监听客户端

  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    // 处理 HTTP 请求
    if (request.indexOf("/ON") != -1) {
      digitalWrite(myLED, HIGH);  // 打开 LED
    }
    if (request.indexOf("/OFF") != -1) {
      digitalWrite(myLED, LOW);   // 关闭 LED
    }

    // 发送 HTML 响应
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html><body>");
    client.println("<h1>ESP32-C3 Hotspot Control</h1>");
    client.println("<p><a href=\"/ON\"><button>ON</button></a></p>");
    client.println("<p><a href=\"/OFF\"><button>OFF</button></a></p>");
    client.println("</body></html>");
    delay(1);
    client.stop();
  }
}