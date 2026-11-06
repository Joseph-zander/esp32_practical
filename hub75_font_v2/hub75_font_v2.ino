#include "LedFunction.h"
#include <U8g2_for_Adafruit_GFX.h>

// 创建显示屏和 U8g2 字体桥接指针
MatrixPanel_I2S_DMA *display = nullptr;
U8G2_FOR_ADAFRUIT_GFX u8g2_gfx;

// 定义屏幕尺寸，方便后续计算
const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;

void setup()
{
  Serial.begin(115200);
  HUB75_I2S_CFG config = get_matrix_config();
  display = new MatrixPanel_I2S_DMA(config);
  display->begin();
  display->setBrightness8(64);
  display->clearScreen();
  // 将 U8g2 字体功能绑定到全彩屏上
  u8g2_gfx.begin(*display);
}

void loop()
{

  // ==========================================
  // 【第一页】：智能控制
  // ==========================================

  // 1. 清屏并画外框
  display->fillScreen(display->color565(0, 0, 0));
  display->drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, display->color565(255, 0, 0));

  // 2. 准备字体
  u8g2_gfx.setFont(u8g2_font_wqy12_t_gb2312a);

  // 3. 动态居中计算
  char line1[] = "智能控制";
  int width1 = u8g2_gfx.getUTF8Width(line1);
  int x1 = (SCREEN_WIDTH - width1) / 2;
  // 垂直方向居中：12点阵字体在32像素高的屏幕中，y轴设在 20 左右视觉效果最好
  int y1 = (SCREEN_HEIGHT + 12) / 2 - 1;

  // 4. 绘制文字并推送到前台
  u8g2_gfx.setForegroundColor(display->color565(0, 255, 0)); // 绿色
  u8g2_gfx.setCursor(x1, y1);
  u8g2_gfx.print(line1);
  display->flipDMABuffer();

  // 5. 保持第一页显示 2 秒
  delay(2000);

  // ==========================================
  // 【第二页】：ESP32-S3
  // ==========================================

  // 1. 再次清屏并画外框（不清屏的话上一页的字会重叠）
  display->fillScreen(display->color565(0, 0, 0));
  display->drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, display->color565(255, 0, 0));

  // 2. 动态居中计算
  char line2[] = "ESP32-S3";
  int width2 = u8g2_gfx.getUTF8Width(line2);
  int x2 = (SCREEN_WIDTH - width2) / 2;
  int y2 = (SCREEN_HEIGHT + 12) / 2 - 1;

  // 3. 绘制文字并推送到前台
  u8g2_gfx.setForegroundColor(display->color565(0, 150, 255)); // 天蓝色
  u8g2_gfx.setCursor(x2, y2);
  u8g2_gfx.print(line2);
  display->flipDMABuffer();

  // 4. 保持第二页显示 2 秒
  delay(2000);
}
