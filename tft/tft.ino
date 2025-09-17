#include <Arduino.h>
#include <TFT_eSPI.h>
#include "eva.h"

TFT_eSPI tft;

void setup() {
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.loadFont(eva);
  tft.setCursor(50, 5);
  tft.setTextSize(1);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("骆 宾 王");
  tft.println();
  tft.println("鹅，鹅，鹅，曲项向天歌。");
  tft.println();
  tft.println("白毛浮绿水，红掌拨清波。");
  tft.setCursor(55, 100);
  tft.print("中大国际商业中心");
}

void loop() {
  Serial.println("hello loop");
  delay(2000);
}
