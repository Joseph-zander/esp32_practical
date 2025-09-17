#include <Arduino.h>
#include <Ticker.h>
#include "TimeControl.h"

#define LED_PIN 2 // GPIO 2 for LED on ESP32-S3

void setup() {
  Serial.begin(115200); // Initialize serial communication
  pinMode(LED_PIN, OUTPUT);
  initTimer(); // Initialize Ticker
  log("ESP32-S3 Ticker Project Started"); // Log startup message
}

void loop() {
  log2("Main loop running..."); // Log main loop activity
  delay(3000); // Simulate other tasks (non-blocking for Ticker)
}