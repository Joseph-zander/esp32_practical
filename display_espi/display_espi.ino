#include <Arduino_GFX_Library.h>      // 核心图形库
#include <SPI.h>                      // SPI 库
#include <U8g2_for_Adafruit_GFX.h>    // U8g2 字体适配库

// 定义引脚（自定义 GPIO，避免 GPIO23）
#define TFT_CS    15  // 片选 (Chip Select)
#define TFT_DC    5   // 数据/命令 (Data/Command)
#define TFT_RST   4   // 复位 (Reset)
#define TFT_MOSI  12  // MOSI (SDA)
#define TFT_SCLK  13  // SCK (SCL)

// 创建 SPI 数据总线
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, -1);  // -1 表示不使用 MISO

// 创建 ST7735 显示对象（128x160，横屏调整）
Arduino_GFX *gfx = new Arduino_ST7735(
  bus,                  // 数据总线
  TFT_RST,             // 复位引脚
  0,                   // 初始旋转（稍后设置横屏）
  false,               // 非 IPS 屏幕
  128,                 // 宽度
  160,                 // 高度
  2,                   // X 偏移（适配 ST7735S 模块）
  1                    // Y 偏移
);

// 创建 U8g2 字体适配对象
U8g2_for_Adafruit_GFX u8g2;

// 定义中文字体（U8g2 支持）
#ifdef U8G2_FONT_UNIFONT_T_CHINESE2
  #define CHINESE_FONT u8g2_font_unifont_t_chinese2
#else
  #define CHINESE_FONT u8g2_font_wqy12_t_chinese2
#endif

void setup() {
  // 初始化串口
  Serial.begin(115200);
  delay(200);  // 等待串口稳定
  Serial.println("ST7735S Arduino_GFX Horizontal Chinese Test");

  // 初始化显示屏
  Serial.println("Before gfx->begin()");
  gfx->begin();
  Serial.println("After gfx->begin()");

  // 初始化 U8g2 字体适配
  Serial.println("Initializing U8g2 font adapter...");
  u8g2.begin(*gfx);  // 将 U8g2 绑定到 GFX 对象

  // 设置横屏模式（160x128）
  Serial.println("Setting rotation to landscape...");
  gfx->setRotation(1);  // 顺时针 90 度

  // 清屏为黑色
  Serial.println("Filling screen with black...");
  gfx->fillScreen(BLACK);

  // 显示英文 "Hello World!"
  Serial.println("Setting English text properties...");
  gfx->setTextColor(WHITE);
  gfx->setTextSize(2);  // 字体大小 2
  gfx->setCursor(20, 30);  // 横屏位置，英文在上方
  Serial.println("Printing English text...");
  gfx->println("Hello World!");

  // 显示中文 "你好"
  Serial.println("Setting Chinese text properties...");
  u8g2.setFont(CHINESE_FONT);  // 使用 U8g2 字体
  u8g2.setForegroundColor(YELLOW);  // 中文用黄色区分
  u8g2.setCursor(20, 70);  // 中文在下方
  Serial.println("Printing Chinese text...");
  u8g2.print("你好");

  Serial.println("Setup complete!");
}
void loop() {
  Serial.println("loop!");
  // 空闲，可添加动画
  delay(2000);
}