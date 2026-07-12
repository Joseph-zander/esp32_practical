#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include "LedFunction.h"
#include "webserver.h"

// 你的 WiFi 名称和密码
const char* ssid     = "Magic Pro";
const char* password = "20031999";

// 独立的 LED 硬件渲染任务（绑定到 Core 1 独占运行）
void LedDisplayTask(void *pvParameters) {
  while (1) {
    matrix->fillScreen(0); 
    drawFlowDash();
    u8g2.setForegroundColor(matrix->color565(0, 255, 0));

    if (currentPage == 0) {
      drawScrollingText(page1Text.c_str(), textY);
    } else {
      drawCenteredText(page2Text.c_str(), textY);
    }

    matrix->flipDMABuffer();

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

    vTaskDelay(pdMS_TO_TICKS(scrollDelay)); 
  }
}


void setup() {
  Serial.begin(115200);
    // 针对 N16R8 物理 USB 的强行唤醒延迟：
  // 刚下载完程序或开机时，电脑和芯片需要时间建立 USB 握手，这里死等 3 秒
  delay(3000); 
  
  Serial.println("\r\n=========================================");
  Serial.println("★ ESP32-S3 N16R8 固件启动成功 ★");
  
  // 打印内存状态，验证你的 8MB PSRAM 是否被成功识别
  Serial.printf("当前总剩余堆内存 (Internal RAM): %d 字节\n", ESP.getFreeHeap());
  Serial.printf("当前外部 PSRAM 剩余内存: %d 字节\n", ESP.getFreePsram());
  Serial.println("=========================================");

    // 1. 调用模块化封装后的异步服务器初始化函数
  initAsyncWebServer(ssid, password);


  // 初始化 HUB75 引脚配置
  HUB75_I2S_CFG::i2s_pins pins = { R1, G1, B1, R2, G2, B2, A, B, C, D, E, LAT, OE, CLK };
  HUB75_I2S_CFG cfg(W, H, 1, pins);
  cfg.driver = HUB75_I2S_CFG::FM6126A; // 指定驱动芯片
  cfg.i2sspeed = HUB75_I2S_CFG::HZ_10M;
  cfg.double_buff = true;  // 开启双缓冲，彻底杜绝闪烁

  // ===================【针对闪烁的关键改动点】===================
  // 如果默认是 false，请改为 true；如果默认是 true，请改为 false。
  // 改变时钟相位可以彻底解决数据锁存交替时的“毛刺”和“微闪”
  cfg.clkphase = false; 
  // ============================================================

  // 初始化矩阵屏
  matrix = new MatrixPanel_I2S_DMA(cfg);
  if (!matrix->begin()) {
    Serial.println("INIT FAIL");
    while (1);
  }
  matrix->setBrightness8(20); // 设置亮度

  // === 关键：初始化中文字体桥接库 ===
  u8g2.begin(*matrix);
  // 设置文泉驿 12 点阵中文字体（需确保你的字串属于 GB2312 编码）
  u8g2.setFont(u8g2_font_wqy12_t_gb2312); 

    // 3. 屏幕刷新挂载到 Core 1
  xTaskCreatePinnedToCore(LedDisplayTask, "LedTask", 4096, NULL, 1, NULL, 1);
}

void loop() {

}

void loop4() {
  // 1. 【核心逻辑】每一帧开始时，都在“后台缓冲区”进行绘制，前台画面保持不变
  matrix->fillScreen(0); // 清空后台

  // 2. 绘制跑马灯跑折线边框
  drawFlowDash();

  // 3. 设置字体颜色并绘制文本
  u8g2.setForegroundColor(matrix->color565(0, 255, 0)); // 绿色文字

  if (currentPage == 0) {
    drawScrollingText(page1Text.c_str(), textY); // 绘制第一页（滚动）
  } else {
    drawCenteredText(page2Text.c_str(), textY);  // 绘制第二页（居中静态）
  }

  // 4. 【核心逻辑】后台所有图层画完后，一键“翻转”到前台，屏幕瞬间刷新
  matrix->flipDMABuffer();

  // 5. 更新下一帧的动画与分页逻辑数据
  off--;
  if (off < 0) {
    off = PERIOD - 1;
  }

  if (currentPage == 0) {
    textOffset += scrollSpeed;
  }

  // 检查页面停留时间，进行无缝切页
  if (millis() - pageStartTime >= pageDuration) {
    currentPage = (currentPage + 1) % 2;
    pageStartTime = millis();
    textOffset = 0; // 切换页面时重置滚动偏移量
  }

  // 帧率控制延时
  delay(scrollDelay);
}
