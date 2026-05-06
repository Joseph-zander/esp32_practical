// 引脚定义
#define HUMAN_SENSOR_PIN  10  // 人体传感器输入引脚改为GPIO10
#define OUTPUT_PIN        5   // 控制输出引脚GPIO5

// 计时变量
unsigned long leaveTime = 0;
bool isHumanDetected = false;
const unsigned long DELAY_SEC = 5000; // 离开延时5000ms=5秒

void setup() {
  Serial.begin(115200);
  pinMode(HUMAN_SENSOR_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  
  // 初始输出低电平
  digitalWrite(OUTPUT_PIN, LOW);
}

void loop() {
  int sensorVal = digitalRead(HUMAN_SENSOR_PIN);
  unsigned long now = millis();

  // 检测到有人
  if (sensorVal == HIGH) {
    isHumanDetected = true;
    leaveTime = now; // 刷新离开计时起点
    digitalWrite(OUTPUT_PIN, HIGH);
  } 
  else { // 无人状态判断延时
    if (isHumanDetected) {
      // 已经离开超过5秒
      if (now - leaveTime >= DELAY_SEC) {
        digitalWrite(OUTPUT_PIN, LOW);
        isHumanDetected = false;
      }
    }
  }

  // 串口调试输出
  Serial.print("传感器值: ");
  Serial.print(sensorVal);
  Serial.print(" | 输出电平: ");
  Serial.println(digitalRead(OUTPUT_PIN));
  delay(1000);
}