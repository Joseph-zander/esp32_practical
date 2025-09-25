#include <esp32-hal-timer.h>

#define PIN_15 15
#define PIN_16 16
#define PIN_17 17
#define PIN_18 18
hw_timer_t *timer = NULL;  // 定时器句柄
// 定义要控制的引脚
const int pins[] = { PIN_15, PIN_16, PIN_17, PIN_18 };  // GPIO 15,16,17,18
const int numPins = 4; 
const int SECOND = 1000 * 1000;
int next = 0;

void IRAM_ATTR onTimer() {
  next = 0;
  int flag = digitalRead(pins[0]);
  Serial.print(" onTimer被调用, 设置IO口的电平为:");
  Serial.println(!flag);
  if (LOW == flag) {
    timerAlarm(timer, 50 * SECOND, true, 0);
  } else {
    timerAlarm(timer, 10 * SECOND, true, 0);
  }
  for (int i = 0; i < numPins; i++) {
    digitalWrite(pins[i], !digitalRead(pins[i]));  // 更新LED状态
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);  // 等待串口初始化
  Serial.println("程序开始，设置引脚为输出...");
  // 设置引脚为输出模式
  for (int i = 0; i < numPins; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);  // 更新LED状态
  }

  timer = timerBegin(1000000);            // 设置定时器频率为 1 MHz（1微秒分辨率）
  timerAttachInterrupt(timer, &onTimer);  // 绑定中断
  timerAlarm(timer, 10 * SECOND, true, 0);
  timerStart(timer);
}

void loop() {
  next = next + 1;
  Serial.print(">> ");
  Serial.print(next);
  Serial.println(" loop...");
  delay(5000);
}
