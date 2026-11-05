<<<<<<< HEAD

#include "LedFunction.h"

// 全局变量定义
MatrixPanel_I2S_DMA* matrix;
U8G2_FOR_ADAFRUIT_GFX u8g2;

// // ===== 文字变量 =====
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

/**
**  设置滚动破折线
***/
void drawFlowDash() {
  // 上边：左→右
  for (int x = 0; x < W; x++)
    if (((x + off) % PERIOD) < dashLen)  // +off
      matrix->drawPixel(x, 0, borderColor);

  // 右边：上→下
  for (int y = 0; y < H; y++)
    if (((y + off) % PERIOD) < dashLen)  // +off
      matrix->drawPixel(W - 1, y, borderColor);

  // 下边：右→左
  for (int x = 0; x < W; x++)
    if (((x - off + 1000 * PERIOD) % PERIOD) < dashLen)  // -off
      matrix->drawPixel(x, H - 1, borderColor);

  // 左边：下→上
  for (int y = 0; y < H; y++)
    if (((y - off + 1000 * PERIOD) % PERIOD) < dashLen)  // -off
      matrix->drawPixel(0, y, borderColor);
}

/**
*  设置静态文件
***/
void drawCenteredText(const char* text, int y) {
  int len = strlen(text);
  int charCount = len / 3;
  int totalWidth = charCount * charWidth + (charCount - 1) * charSpacing;
  int x = (W - totalWidth) / 2;

  for (int i = 0; i < len; i += 3) {
    char buf[4] = { text[i], text[i + 1], text[i + 2], '\0' };
    u8g2.setCursor(x, y);
    u8g2.print(buf);
    x += charWidth + charSpacing;
  }
}

/**
*  设置滚动文字
***/
void drawScrollingText(const char* text, int y) {
  int len = strlen(text);
  int charCount = len / 3;
  int totalWidth = charCount * charWidth + (charCount - 1) * charSpacing;
  int cycleWidth = totalWidth + W;

  int x = W - (textOffset % cycleWidth);

  for (int i = 0; i < len; i += 3) {
    char buf[4] = { text[i], text[i + 1], text[i + 2], '\0' };
    u8g2.setCursor(x, y);
    u8g2.print(buf);
    x += charWidth + charSpacing;
  }
=======

#include "LedFunction.h"

// 全局变量定义
MatrixPanel_I2S_DMA* matrix;
U8G2_FOR_ADAFRUIT_GFX u8g2;

// // ===== 文字变量 =====
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

/**
**  设置滚动破折线
***/
void drawFlowDash() {
  // 上边：左→右
  for (int x = 0; x < W; x++)
    if (((x + off) % PERIOD) < dashLen)  // +off
      matrix->drawPixel(x, 0, borderColor);

  // 右边：上→下
  for (int y = 0; y < H; y++)
    if (((y + off) % PERIOD) < dashLen)  // +off
      matrix->drawPixel(W - 1, y, borderColor);

  // 下边：右→左
  for (int x = 0; x < W; x++)
    if (((x - off + 1000 * PERIOD) % PERIOD) < dashLen)  // -off
      matrix->drawPixel(x, H - 1, borderColor);

  // 左边：下→上
  for (int y = 0; y < H; y++)
    if (((y - off + 1000 * PERIOD) % PERIOD) < dashLen)  // -off
      matrix->drawPixel(0, y, borderColor);
}

/**
*  设置静态文件
***/
void drawCenteredText(const char* text, int y) {
  int len = strlen(text);
  int charCount = len / 3;
  int totalWidth = charCount * charWidth + (charCount - 1) * charSpacing;
  int x = (W - totalWidth) / 2;

  for (int i = 0; i < len; i += 3) {
    char buf[4] = { text[i], text[i + 1], text[i + 2], '\0' };
    u8g2.setCursor(x, y);
    u8g2.print(buf);
    x += charWidth + charSpacing;
  }
}

/**
*  设置滚动文字
***/
void drawScrollingText(const char* text, int y) {
  int len = strlen(text);
  int charCount = len / 3;
  int totalWidth = charCount * charWidth + (charCount - 1) * charSpacing;
  int cycleWidth = totalWidth + W;

  int x = W - (textOffset % cycleWidth);

  for (int i = 0; i < len; i += 3) {
    char buf[4] = { text[i], text[i + 1], text[i + 2], '\0' };
    u8g2.setCursor(x, y);
    u8g2.print(buf);
    x += charWidth + charSpacing;
  }
>>>>>>> 1016203e2e60d6cbbdd1c8a06538f08beba5414a
}