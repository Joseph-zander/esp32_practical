#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

MatrixPanel_I2S_DMA *matrix;

#define R1 4
#define G1 5
#define B1 6
#define R2 7
#define G2 15
#define B2 16
#define A 18
#define B 8
#define C 3
#define D 42
#define E -1
#define LAT 40
#define OE 2
#define CLK 41

void setup() {
  Serial.begin(115200);
  delay(500);

  HUB75_I2S_CFG::i2s_pins pins = {R1,G1,B1,R2,G2,B2,A,B,C,D,E,LAT,OE,CLK};
  
  // 关键测试点：改成 64,16 试试
  HUB75_I2S_CFG cfg(64, 32, 1, pins);
  
  cfg.driver = HUB75_I2S_CFG::FM6126A;
  cfg.i2sspeed = HUB75_I2S_CFG::HZ_10M;

  matrix = new MatrixPanel_I2S_DMA(cfg);
  if (!matrix->begin()) {
    Serial.println("INIT FAIL");
    while(1);
  }
  Serial.println("INIT OK");

  matrix->setBrightness8(40);
  matrix->fillScreen(0);

  // 全屏画红色
  matrix->fillScreen(matrix->color565(255, 0, 0));
}

void loop() {
  matrix->fillScreen(matrix->color565(255, 0, 0));  // 红
  delay(500);
  matrix->fillScreen(matrix->color565(0, 255, 0));  // 绿
  delay(500);
  matrix->fillScreen(matrix->color565(0, 0, 255));  // 蓝
  delay(500);
  matrix->fillScreen(matrix->color565(255, 255, 255));  // 白
  delay(500);
}