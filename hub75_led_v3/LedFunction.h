<<<<<<< HEAD
#ifndef LED_FUNCTION_H
#define LED_FUNCTION_H

#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <U8g2_for_Adafruit_GFX.h>

// 全局变量声明
extern MatrixPanel_I2S_DMA *matrix;

// 全局变量定义
extern U8G2_FOR_ADAFRUIT_GFX u8g2; 

//定义MCU的GPIO口
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


#ifndef W
#define W 64
#endif

#ifndef H
#define H 32
#endif


extern String page1Text ;
extern String page2Text;
extern int textY;
extern int charWidth;
extern int charSpacing;
extern int scrollSpeed;
extern int scrollDelay;

extern int dashLen;
extern int gapLen;
extern uint16_t borderColor;

// ===== 分页变量 =====
extern int currentPage;
extern unsigned long pageStartTime;
extern const unsigned long pageDuration;

extern int off;
extern int textOffset;
extern int PERIOD;

void drawFlowDash();
void drawCenteredText(const char* text, int y);
void drawScrollingText(const char* text, int y);

=======
#ifndef LED_FUNCTION_H
#define LED_FUNCTION_H

#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <U8g2_for_Adafruit_GFX.h>

// 全局变量声明
extern MatrixPanel_I2S_DMA *matrix;

// 全局变量定义
extern U8G2_FOR_ADAFRUIT_GFX u8g2;   // ← 补上这个定义

//定义MCU的GPIO口
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


#ifndef W
#define W 64
#endif

#ifndef H
#define H 32
#endif


extern String page1Text ;
extern String page2Text;
extern int textY;
extern int charWidth;
extern int charSpacing;
extern int scrollSpeed;
extern int scrollDelay;

extern int dashLen;
extern int gapLen;
extern uint16_t borderColor;

// ===== 分页变量 =====
extern int currentPage;
extern unsigned long pageStartTime;
extern const unsigned long pageDuration;

extern int off;
extern int textOffset;
extern int PERIOD;

void drawFlowDash();
void drawCenteredText(const char* text, int y);
void drawScrollingText(const char* text, int y);

>>>>>>> 1016203e2e60d6cbbdd1c8a06538f08beba5414a
#endif