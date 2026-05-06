const int LED_PIN = 10;

const int freq = 5000;
/**PWM 占空比的分辨率（位数）*/
const int resolution = 8;

const int BRIGHT_20 = 51;
const int BRIGHT_100 = 255;

/*********** PIR *************/
const int PIR_PIN = 4;
volatile bool motionFlag = false;
/*****************************/

volatile unsigned long lastMotionTime = 0;
const unsigned long NO_MOTION_TIMEOUT = 10000;  // 10秒无人回20%

bool lightIsFull = false;
void IRAM_ATTR PIR_onRISING() {
  motionFlag = true;
  setFullLight();
  lastMotionTime = millis();
}

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  attachInterrupt(PIR_PIN, PIR_onRISING, RISING);

  ledcAttach(LED_PIN, freq, resolution);
  ledcWrite(LED_PIN, BRIGHT_20);

  lastMotionTime = millis();  // 忽略上电前 60 秒
}

void setFullLight() {
  if (lightIsFull) {
    return;
  }
  ledcWrite(LED_PIN, BRIGHT_100);
  lightIsFull = true;
}

void loop() {
  unsigned long now = millis();
  /* 忽略上电 60 秒内误触发 */
  if (now < 60000) {
    delay(10000);
    Serial.print("<60S,");
    Serial.print(60000 - now);
    Serial.println("返回。");
    return;
  }
  /* 有人移动 */
  if (motionFlag) {
    motionFlag = false;
    setFullLight();
    lastMotionTime = now;
  }
  Serial.print("loop2...");
  Serial.println(now - lastMotionTime);
  if (!lightIsFull) {
    lastMotionTime = 0;
    delay(1000);
    return;
  }
  /* 超时无人 → 回 20% */
  if (lightIsFull && now - lastMotionTime > NO_MOTION_TIMEOUT) {
    ledcWrite(LED_PIN, BRIGHT_20);
    lightIsFull = false;
    Serial.println("No motion → 20%");
  }
  delay(2000);
}