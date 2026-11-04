#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#include "LedFunction.h"

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("进入setup...");
  textY = (H - 12) / 2 + 12 - 2;

  HUB75_I2S_CFG::i2s_pins pins = {R1, G1, B1, R2, G2, B2, A, B, C, D, E, LAT, OE, CLK};
  HUB75_I2S_CFG cfg(W, H, 1, pins);
  cfg.driver = HUB75_I2S_CFG::FM6126A;
  cfg.i2sspeed = HUB75_I2S_CFG::HZ_10M; //HZ_10M
  //cfg.double_buff = true;   // 开启双缓冲

  matrix = new MatrixPanel_I2S_DMA(cfg);
  if (!matrix->begin()) {
    Serial.println("INIT FAIL");
    while (1);
  }
  matrix->setBrightness8(30);
  u8g2.begin(*matrix);
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);

  pageStartTime = millis();
}

void loop() {
  matrix->fillScreen(0);
  drawFlowDash();

  u8g2.setForegroundColor(matrix->color565(0, 255, 0));

  if (currentPage == 0) {
    drawScrollingText(page1Text.c_str(), textY);
  } else {
    drawCenteredText(page2Text.c_str(), textY);
  }

  off--;
  if (off < 0) {
    off = PERIOD - 1;
  }

  if (currentPage == 0) {
    textOffset += scrollSpeed;
  }

  if (millis() - pageStartTime >= pageDuration) {
    currentPage = (currentPage + 1) % 2;
    pageStartTime = millis();
    textOffset = 0;
  }

  delay(scrollDelay);
}