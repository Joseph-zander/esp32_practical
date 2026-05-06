const int pin1 = 5;   // GPIO5
const int pin2 = 4;   // GPIO4
const int PIR_PIN = 7;
const int PIR_OUTPUT_PIN = 10;

enum {
  STATE_HIGH_40,
  STATE_LOW_20
} state = STATE_HIGH_40;

unsigned long stateStart = 0;

void setup() {
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(PIR_OUTPUT_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);

  digitalWrite(pin1, HIGH);   // GPIO5 高
  digitalWrite(pin2, LOW);    // GPIO4 低

  stateStart = millis();
}

void readPir() {
  int pirValue = digitalRead(PIR_PIN); // 读取红外传感器电平
  if (HIGH == pirValue) {
    digitalWrite(PIR_OUTPUT_PIN, HIGH);
    Serial.println("检测到人体，高电平");
  } else {
    digitalWrite(PIR_OUTPUT_PIN, LOW);
    Serial.println("无人体，低电平");
  }
}

void loop() {
  readPir();
  unsigned long now = millis();
  if (state == STATE_HIGH_40 && (now - stateStart >= 40000)) {
    // 切换到 20 秒低电平
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);

    state = STATE_LOW_20;
    stateStart = now;
  }
  else if (state == STATE_LOW_20 && (now - stateStart >= 20000)) {
    // 切换回 40 秒高电平
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);

    state = STATE_HIGH_40;
    stateStart = now;
  }
  delay(300);
}

