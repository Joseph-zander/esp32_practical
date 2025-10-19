// 定义引脚
const int buttonPin = 8;    // 按钮连接到引脚 8
const int outputPin = 9;    // 输出（例如 LED）连接到引脚 9

// 变量
int pressCount = 0;         // 记录有效按键次数

void setup() {
  Serial.begin(115200); 
  pinMode(buttonPin, INPUT_PULLUP); // 设置按钮引脚为输入并启用内部上拉电阻
  pinMode(outputPin, OUTPUT);       // 设置输出引脚为输出
  digitalWrite(outputPin, LOW);    // 初始化输出引脚为低电平
}

void loop() {
  // 读取当前按钮状态
  int reading = digitalRead(buttonPin);
  if (reading == HIGH) {
    return;
  }
  delay(100);
  if (digitalRead(buttonPin) == HIGH) {
    return;
  }
  pressCount = pressCount + 1;
  if (pressCount > 10000 && pressCount % 2 == 0) {
    pressCount = 0;
  }
  Serial.print("pressCount % 2 = ");
  Serial.println(pressCount % 2);
  digitalWrite(outputPin, (pressCount % 2 > 0) ? HIGH : LOW); 
  delay(200);
}