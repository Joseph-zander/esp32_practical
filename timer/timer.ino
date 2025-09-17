#include <Arduino.h>
#include <Ticker.h>

#define LED_PIN 2
Ticker ticker;

void timerCallback() {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    Serial.println("Ticker triggered!");
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    ticker.attach_ms(1000, timerCallback); // 每 500 ms 调用
}

void loop() {
    Serial.println("Main loop running...");
    delay(3000); // 模拟其他任务
}