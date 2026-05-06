// 引脚定义
#define HUMAN_SENSOR_PIN  10  // 人体传感器输入引脚改为GPIO10
#define OUTPUT_PIN        5   // 控制输出引脚GPIO5

// 计时变量
bool isHumanDetected = false;

void setup() {
  Serial.begin(115200);
  pinMode(HUMAN_SENSOR_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  
  // 初始输出低电平
  digitalWrite(OUTPUT_PIN, LOW);
}

void loop() {
  Serial.println("");
  int sensorVal = digitalRead(HUMAN_SENSOR_PIN);
  Serial.print(">>> PIR检测的值为:");
  Serial.println(sensorVal);

  // 检测到有人
  if (sensorVal == HIGH) {
    Serial.println("高电平...");
    isHumanDetected = true;
    digitalWrite(OUTPUT_PIN, HIGH);
  } 
  else { // 无人状态判断延时
    Serial.println("低电平，");
    if (isHumanDetected) {
      // 已经离开超过5秒
      Serial.println("低电平111111111......");
      digitalWrite(OUTPUT_PIN, LOW);
      isHumanDetected = false;
    }
  }

  // 串口调试输出
  Serial.print("传感器值: ");
  Serial.print(sensorVal);
  Serial.print(" | 输出电平: ");
  Serial.println(digitalRead(OUTPUT_PIN));
  delay(1000);
}