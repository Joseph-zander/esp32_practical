#include "ledFunction.h"

MatrixPanel_I2S_DMA *matrix = nullptr;
U8G2_FOR_ADAFRUIT_GFX u8g2;

void initMatrix() {
    HUB75_I2S_CFG mxconfig(W, H, 1);
    mxconfig.gpio.r1 = R1; mxconfig.gpio.g1 = G1; mxconfig.gpio.b1 = B1;
    mxconfig.gpio.r2 = R2; mxconfig.gpio.g2 = G2; mxconfig.gpio.b2 = B2;
    mxconfig.gpio.a  = A;  mxconfig.gpio.b  = B;  mxconfig.gpio.c  = C;  mxconfig.gpio.d = D;
    mxconfig.gpio.e  = E;
    mxconfig.gpio.lat = LAT; mxconfig.gpio.oe = OE; mxconfig.gpio.clk = CLK;
    
    mxconfig.clkphase = false; 

    matrix = new MatrixPanel_I2S_DMA(mxconfig);
    matrix->begin();
    matrix->setBrightness8(70); // 适中亮度，防刺眼
    matrix->clearScreen();

    u8g2.begin(*matrix);
    u8g2.setFontMode(1); // 背景透明
}

// 提示联网状态的实用函数
void showStatusText(const char* text, uint16_t color) {
    matrix->clearScreen();
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    u8g2.setForegroundColor(color);
    u8g2.setCursor(2, 20);
    u8g2.print(text);
}

void updateClockDisplay2(struct tm *timeinfo) {
    matrix->clearScreen();
    char dateBuf[16];
    char timeBuf[16];

    // 1. 顶层小字体：日期 (原厂原生字体，确保不换行)
    matrix->setTextSize(1); 
    matrix->setTextColor(matrix->color565(0, 200, 255)); 
    sprintf(dateBuf, "%04d/%02d/%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
    matrix->setCursor(2, 0); 
    matrix->print(dateBuf);

    // 2. 下方剩余空间：换用 U8g2 专用 profont12 大字（支持时分秒，完美不换行）
    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.setForegroundColor(matrix->color565(0, 255, 0)); 
    
    sprintf(timeBuf, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    
    // 🚀 居中算法：总宽48像素。(64 - 48) / 2 = 8 像素水平居中
    // 纵向居中：profont12 字高 12 像素，在下方 25 像素空间里，基准线定在 25 刚好上下平分居中！
    u8g2.setCursor(8, 25); 
    u8g2.print(timeBuf);
}

// 中文格式、精准居中渲染
void updateClockDisplay(struct tm *timeinfo) {
    // 1. 每次渲染先清空上一帧
    matrix->clearScreen();

    char dateBuf[12];
    char timeBuf[9];

    // ============================================================
    // 1. 顶层年月日：换用高清晰度 5x7 字库，精简为 YY-MM-DD 完美居左
    // ============================================================
    u8g2.setFont(u8g2_font_5x7_tf); // 🚀 完美解决粘连！极具辨识度的 X11 经典点阵字
    u8g2.setForegroundColor(matrix->color565(0, 200, 255)); // 天蓝色日期
    
    // 提取年份的后两位 (%02d) 缩短横向长度，留出呼吸感
    int shortYear = (timeinfo->tm_year + 1900) % 100;
    sprintf(dateBuf, "%02d-%02d-%02d", shortYear, timeinfo->tm_mon + 1, timeinfo->tm_mday);
    
    // 💡 居左微调：x=2 (左边留白2像素看起来更舒适)，y=7 (U8g2 5x7 字体的基准线设在 7 刚好完美贴顶)
    u8g2.setCursor(2, 7); 
    u8g2.print(dateBuf);


    // ============================================================
    // 2. 下方剩余空间：维持你最满意的原厂完美时分秒（水平+垂直双重居中）
    // ============================================================
    matrix->setTextSize(1); // 维持最漂亮的整数 1 原始电子字形
    matrix->setTextColor(matrix->color565(0, 255, 0)); // 翠绿色时间
    
    // 格式化出完整的 8 位 时:分:秒
    sprintf(timeBuf, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    
    // 保持你先前最满意的绝对物理居中坐标
    matrix->setCursor(8, 16); 
    matrix->print(timeBuf);
}

