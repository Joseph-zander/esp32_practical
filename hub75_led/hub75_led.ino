#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Adafruit_GFX.h>
#include <U8g2_for_Adafruit_GFX.h>

MatrixPanel_I2S_DMA *matrix;
U8G2_FOR_ADAFRUIT_GFX u8g2;

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

#define W 64
#define H 32

// ===== 文字变量 =====
String page1Text = "中大国际，欢迎您！";
String page2Text = "工程部宣";
int textY = 22;
int charWidth = 12;
int charSpacing = 2;
int scrollSpeed = 1;
int scrollDelay = 30;

// ===== 边框变量 =====
int dashLen = 6;
int gapLen = 4;
uint16_t borderColor = 0xF800;

// ===== 分页变量 =====
int currentPage = 0;
unsigned long pageStartTime = 0;
const unsigned long pageDuration = 5000;

int off = 0;
int textOffset = 0;
int PERIOD = dashLen + gapLen;

void drawFlowDash() {
  // 上边：左→右
  for (int x = 0; x < W; x++)
    if (((x + off) % PERIOD) < dashLen)      // +off
      matrix->drawPixel(x, 0, borderColor);

  // 右边：上→下
  for (int y = 0; y < H; y++)
    if (((y + off) % PERIOD) < dashLen)      // +off
      matrix->drawPixel(W - 1, y, borderColor);

  // 下边：右→左
  for (int x = 0; x < W; x++)
    if (((x - off + 1000 * PERIOD) % PERIOD) < dashLen)   // -off
      matrix->drawPixel(x, H - 1, borderColor);

  // 左边：下→上
  for (int y = 0; y < H; y++)
    if (((y - off + 1000 * PERIOD) % PERIOD) < dashLen)   // -off
      matrix->drawPixel(0, y, borderColor);
}

void drawCenteredText(const char* text, int y) {
  int len = strlen(text);
  int charCount = len / 3;
  int totalWidth = charCount * charWidth + (charCount - 1) * charSpacing;
  int x = (W - totalWidth) / 2;

  for (int i = 0; i < len; i += 3) {
    char buf[4] = {text[i], text[i+1], text[i+2], '\0'};
    u8g2.setCursor(x, y);
    u8g2.print(buf);
    x += charWidth + charSpacing;
  }
}

void drawScrollingText(const char* text, int y) {
  int len = strlen(text);
  int charCount = len / 3;
  int totalWidth = charCount * charWidth + (charCount - 1) * charSpacing;
  int cycleWidth = totalWidth + W;

  int x = W - (textOffset % cycleWidth);

  for (int i = 0; i < len; i += 3) {
    char buf[4] = {text[i], text[i+1], text[i+2], '\0'};
    u8g2.setCursor(x, y);
    u8g2.print(buf);
    x += charWidth + charSpacing;
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("进入setup...");
  textY = (H - 12) / 2 + 12 - 2;

  HUB75_I2S_CFG::i2s_pins pins = {R1, G1, B1, R2, G2, B2, A, B, C, D, E, LAT, OE, CLK};
  HUB75_I2S_CFG cfg(W, H, 1, pins);
  cfg.driver = HUB75_I2S_CFG::FM6126A;
  cfg.i2sspeed = HUB75_I2S_CFG::HZ_10M;

  matrix = new MatrixPanel_I2S_DMA(cfg);
  if (!matrix->begin()) {
    Serial.println("INIT FAIL");
    while (1);
  }
  matrix->setBrightness8(40);
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
  if (off < 0) off = PERIOD - 1;

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