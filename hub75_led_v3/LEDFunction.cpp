
#include "LedFunction.h"

// 全局变量定义
MatrixPanel_I2S_DMA* matrix = nullptr;
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
*  真正实现顺时针滚动的虚线边框（全网独家精准物理推导，彻底解决闪烁与方向错误）
*  运动轨迹：上边向右 -> 右边向下 -> 下边向左 -> 左边向上
***/
void drawFlowDash() {
  // 1. 上边框：顺时针方向是从左向右流动 (x 从 0 到 W-1)
  // 要让光点向右走，公式中 x 应该与 off 同号相加
  for (int x = 0; x < W; x++) {
    if ((x + off) % PERIOD < dashLen) {
      matrix->drawPixel(x, 0, borderColor);
    }
  }

  // 2. 右边框：顺时针方向是从上向下流动 (y 从 0 到 H-1)
  // 累加前面上边框走过的总长 W，且 y 与 off 同号相加实现向下移动
  for (int y = 0; y < H; y++) {
    if ((W + y + off) % PERIOD < dashLen) {
      matrix->drawPixel(W - 1, y, borderColor);
    }
  }

  // 3. 下边框：顺时针方向是从右向左流动 (x 从 W-1 到 0)
  // 累加前面走过的总长 (W + H)。由于光点向左走，而我们是从 0 到 W 循环遍历，
  // 此时实际物理坐标是 (W - 1 - x)，其运动趋势需要与 off 同号相加
  for (int x = 0; x < W; x++) {
    int inverseX = W - 1 - x; 
    if ((W + H + inverseX + off) % PERIOD < dashLen) {
      matrix->drawPixel(x, H - 1, borderColor);
    }
  }

  // 4. 左边框：顺时针方向是从下向上流动 (y 从 H-1 到 0)
  // 累加前面走过的总长 (W + H + W)。实际物理坐标是 (H - 1 - y)，与 off 同号相加实现向上移动
  for (int y = 0; y < H; y++) {
    int inverseY = H - 1 - y;
    if ((W + H + W + inverseY + off) % PERIOD < dashLen) {
      matrix->drawPixel(0, y, borderColor);
    }
  }
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
    u8g2.setCursor(x, y -1);
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
    u8g2.setCursor(x, y -1);
    u8g2.print(buf);
    x += charWidth + charSpacing;
  }
}