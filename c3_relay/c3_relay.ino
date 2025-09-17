#include <esp32-hal-timer.h>

#define PIN_0 0
#define PIN_1 1
#define PIN_2 2
#define PIN_3 3
#define PIN_5 5
hw_timer_t *timer = NULL;  // 定时器句柄

// 定义要控制的引脚
const int pins[] = { PIN_0, PIN_1, PIN_2, PIN_3, PIN_5 };  // GPIO 0, 1, 2, 3
const int numPins = 5;                                     // 引脚数量
int interval = 50;
int next = 0;

void IRAM_ATTR onTimer() {
  next = 0;
  int flag = digitalRead(pins[0]);
  Serial.print(" onTimer被调用, 设置IO口的电平为:");
  Serial.println(!flag);
  if (LOW == flag) {
    timerAlarm(timer, 50 * 1000 * 1000, true, 0);
  } else {
    timerAlarm(timer, 10 * 1000 * 1000, true, 0);
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
  timerAlarm(timer, 10 * 1000 * 1000, true, 0);
  timerStart(timer);
}

void loop() {
  next = next + 1;
  Serial.print(">> ");
  Serial.print(next);
  Serial.println(" loop...");
  delay(5000);
}
