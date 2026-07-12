#include <WiFi.h>

const int ctrlPin = 8;
const char* ssid     = "XiaoBao-WIFI";
const char* password = "20031999";

void connect() {
  WiFi.mode(WIFI_STA);   // 站模式
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - start > 20000) {   // 20秒超时
      Serial.println("\nWiFi connect timeout!");
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }


    // 关闭 WiFi
  //WiFi.mode(WIFI_OFF);
  //WiFi.disconnect();

}

void setup() {
  Serial.begin(115200);        // ← 加上这行，波特率 115200
  pinMode(ctrlPin, OUTPUT);
  digitalWrite(ctrlPin, HIGH);
  
  Serial.println("6N137 LED控制已启动");

  connect();
}

void loop() {
  digitalWrite(ctrlPin, LOW);
  Serial.println("LED ON");
  delay(5000);
  
  digitalWrite(ctrlPin, HIGH);
  Serial.println("LED OFF");
  delay(1000);
}