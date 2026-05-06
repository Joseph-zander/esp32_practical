// 输出引脚
const int pinOut = 5;
// 人体红外传感器输入引脚
const int humanSensorPin = 6;

// 最大留人超时 30秒
const unsigned long STAY_TIMEOUT = 30000UL;
// 传感器防抖时间
const unsigned long DEBOUNCE_TIME = 200UL;

unsigned long humanDetectStart = 0;
unsigned long lastDebounceTime = 0;
int lastSensorState = LOW;
int currentSensorState;
bool humanDetectedFlag = false;

void setup() {
  Serial.begin(115200);
  pinMode(pinOut, OUTPUT);
  pinMode(humanSensorPin, INPUT);

  digitalWrite(pinOut, LOW);
  Serial.println("ESP32 C3 SuperMini 人体感应30秒超时控制器启动");
}

void loop() {
  unsigned long now = millis();
  currentSensorState = digitalRead(humanSensorPin);

  // 软件防抖处理
  if (currentSensorState != lastSensorState) {
    lastDebounceTime = now;
  }

  if ((now - lastDebounceTime) > DEBOUNCE_TIME) {
    if (currentSensorState != humanDetectedFlag) {
      humanDetectedFlag = currentSensorState;

      if (humanDetectedFlag == HIGH) {
        // 初次检测到人，重置计时
        humanDetectStart = now;
        Serial.println("检测到人体，开始30秒计时");
      } else {
        Serial.println("人体离开，输出关闭");
      }
    }
  }
  lastSensorState = currentSensorState;

  // 控制GPIO5输出
  if (humanDetectedFlag) {
    if (now - humanDetectStart < STAY_TIMEOUT) {
      digitalWrite(pinOut, HIGH);
      Serial.printf("有人停留中，剩余时间：%d秒 | GPIO5=HIGH\n", (STAY_TIMEOUT - (now - humanDetectStart)) / 1000);
    } else {
      digitalWrite(pinOut, LOW);
      Serial.println("有人停留超过30秒，自动关闭输出 GPIO5=LOW");
    }
  } else {
    digitalWrite(pinOut, LOW);
  }

  delay(100);
}