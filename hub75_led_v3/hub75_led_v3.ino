#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include "LedFunction.h"
#include "webserver.h"

// 独立的 LED 硬件渲染任务（绑定到 Core 1 独占运行）
void LedDisplayTask(void* pvParameters) {
  pageStartTime = millis(); 
  while (1) {
    if (millis() - pageStartTime >= pageDuration) {
      currentPage = (currentPage == 0) ? 1 : 0;
      pageStartTime = millis();
    }

    matrix->fillScreen(0);
    drawFlowDash();
    
    if (currentPage == 0) {
      u8g2.setForegroundColor(matrix->color565(0, 255, 0));
      drawScrollingText(page1Text.c_str(), textY);
    } else {
      uint32_t currentSecond = millis() / 1000;
      int colorIndex = currentSecond % 5;
      uint16_t activeColor;
      switch (colorIndex) {
        case 0: activeColor = matrix->color565(0, 255, 255); break;
        case 1: activeColor = matrix->color565(0, 255, 0); break;
        case 2: activeColor = matrix->color565(255, 255, 255); break;
        case 3: activeColor = matrix->color565(255, 160, 16); break;
        case 4:
        default: activeColor = matrix->color565(255, 0, 255); break;
      }
      u8g2.setForegroundColor(activeColor);
      drawCenteredText(page2Text.c_str(), textY);
    }
    matrix->flipDMABuffer();
    
    off--;
    if (off < 0) {
      off = PERIOD - 1;
    }
    if (currentPage == 0) {
      textOffset += scrollSpeed;
    }

    vTaskDelay(pdMS_TO_TICKS(20)); // 限制帧率，释放内核
  }
}

// === 🚀 核心改进：全新的后台 WiFi 专属任务（挂载到 Core 0，低优先级） ===
void WiFiBackTask(void* pvParameters) {
  Serial.println("[WiFi任务] 后台 Wi-Fi 线程已拉起，开始在 Core 0 独立运行...");
  
  // 只要 WiFi 处于激活状态，且未连接成功，就在这里循环
  while (isWiFiActive && WiFi.status() != WL_CONNECTED) {
    // 直接调用你的异步轮询逻辑，它现在独占这个后台低优先级任务
    handleWiFiAsyncProgress();
    
    // 关键：每次轮询完，强制休眠 500 毫秒，把 Core 0 彻底让给系统射频中断
    vTaskDelay(pdMS_TO_TICKS(500)); 
  }
  
  Serial.println("[WiFi任务] 连接成功或已触发熔断，后台任务安全退出并销毁。");
  vTaskDelete(NULL); // 自我销毁任务，释放内存
}

void setup() {
  Serial.begin(115200);
  delay(1000); // N16R8 USB 唤醒延迟

  Serial.println("\r\n=========================================");
  Serial.println("★ ESP32-S3 N16R8 固件启动成功 ★");
  Serial.println("=========================================");

  // 加载闪存文本
  loadSavedText();

  // 1. 初始化异步服务器配置（此时仅仅是注册事件，不产生任何网络扫描，耗时 0 毫秒）
  initAsyncWebServer();

  // 2. 初始化 HUB75 引脚配置与屏幕
  HUB75_I2S_CFG::i2s_pins pins = { R1, G1, B1, R2, G2, B2, A, B, C, D, E, LAT, OE, CLK };
  HUB75_I2S_CFG cfg(W, H, 1, pins);
  cfg.driver = HUB75_I2S_CFG::FM6126A;  
  cfg.i2sspeed = HUB75_I2S_CFG::HZ_10M;
  cfg.double_buff = true;  
  cfg.clkphase = false;

  matrix = new MatrixPanel_I2S_DMA(cfg);
  if (!matrix->begin()) {
    Serial.println("INIT FAIL");
    while (1);
  }
  matrix->setBrightness8(35);  
  
  u8g2.begin(*matrix);
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);

  // 3. 屏幕刷新挂载到 Core 1（高优先级 3，确保显示最流畅）
  xTaskCreatePinnedToCore(LedDisplayTask, "LedTask", 8192, NULL, 3, NULL, 1);

  // 4. 🚀 关键改动：将 WiFi 连接彻底剥离到 Core 0（低优先级 1，绝不抢占屏幕资源）
  xTaskCreatePinnedToCore(WiFiBackTask, "WiFiTask", 4096, NULL, 1, NULL, 0);
}

void loop() {

}
