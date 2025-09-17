#include <Arduino.h>
#include <Ticker.h>
#include "TimeControl.h"

Ticker ticker;

void initTimer() {
  ticker.attach_ms(2000, handleTimer); // Call tickerCallback every 500 ms
}

void handleTimer() {
  log("Ticker triggered! LED toggled."); // Log timer event
}



void log(const char* text) {
  Serial.println(text); // Implement log function
}