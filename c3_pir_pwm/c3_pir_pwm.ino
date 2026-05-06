#define PIR_PIN 10
#define PWM_OUT_PIN 5

// 亮度参数 0~255
const unsigned long DELAY_MS = 5000;  //延迟5秒
const int BRIGHT_STANDBY  = 51;   // 20%亮度
const int BRIGHT_FULL     = 255;  // 100%亮度

// 状态变量
unsigned long leaveTime = 0;
bool humanDetectFlag = false;
int targetBright = BRIGHT_STANDBY;
int currentBright = BRIGHT_STANDBY;

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(PWM_OUT_PIN, OUTPUT);
  
  // 上电默认20%亮度
  analogWrite(PWM_OUT_PIN, currentBright);
}

void loop() {
  int sensorVal = digitalRead(PIR_PIN);
  unsigned long now = millis();

  // 检测到人，切满亮，重置计时
  if (sensorVal == HIGH) {
    humanDetectFlag = true;
    leaveTime = now;
    targetBright = BRIGHT_FULL;
  } 
  // 无人时判断是否超时5秒
  else {
    if (humanDetectFlag) {
      if (now - leaveTime >= DELAY_MS) {
        targetBright = BRIGHT_STANDBY;
        humanDetectFlag = false;
      }
    }
  }

  // 渐变缓变亮度，不刺眼
  if (currentBright < targetBright) {
    currentBright += 3;
  } else if (currentBright > targetBright) {
    currentBright -= 3;
  }

  analogWrite(PWM_OUT_PIN, currentBright);

  // 串口调试
  Serial.print("Sensor:");
  Serial.print(sensorVal);
  Serial.print(" Bright:");
  Serial.println(currentBright);
  delay(20);
}