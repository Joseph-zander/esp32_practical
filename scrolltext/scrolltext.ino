#include <TFT_eSPI.h>

#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setCursor(20, 40);
  tft.println("ST3775S Test");
  tft.setCursor(20, 70);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.println("hello world!");
}

void loop() {
  Serial.println("hello loop!");
  delay(1000);
}