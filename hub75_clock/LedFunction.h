#ifndef LED_FUNCTION_H
#define LED_FUNCTION_H

#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <U8g2_for_Adafruit_GFX.h>

// 屏幕尺寸定义
#ifndef W
#define W 64
#endif

#ifndef H
#define H 32
#endif

// 定义 MCU 的 GPIO 口 (根据你的硬件修改)
#define R1 4
#define G1 5
#define B1 6
#define R2 7
#define G2 1
#define B2 2
#define A 10           
#define B 8
#define C 9
#define D 11
#define E -1
#define LAT 40
#define OE 14
#define CLK 41

// 全局变量声明（由 ccpp 文件实际实例化）
extern MatrixPanel_I2S_DMA *matrix;
extern U8G2_FOR_ADAFRUIT_GFX u8g2; 

// 函数声明
void initMatrix();
void showStatusText(const char* text, uint16_t color);
void updateClockDisplay(struct tm *timeinfo);
#endif
