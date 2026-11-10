#ifndef LED_FUNCTION_H
#define LED_FUNCTION_H

#include <lvgl.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// 1. 硬件屏幕尺寸配置
#define PANEL_RES_X 64      // 屏幕宽度（像素）
#define PANEL_RES_Y 32      // 屏幕高度（像素）
#define PANEL_CHAIN 1       // 单块屏幕级联

// 2. GPIO 引脚硬件映射定义
#define R1_PIN   4
#define G1_PIN   5
#define BL1_PIN  6
#define R2_PIN   7
#define G2_PIN   1
#define BL2_PIN  2

#define CH_A_PIN 10
#define CH_B_PIN 8
#define CH_C_PIN 9
#define CH_D_PIN 11
#define CH_E_PIN -1  // 32高屏不需要 E 轴引脚

#define LAT_PIN  40
#define OE_PIN   14
#define CLK_PIN  41

// 3. 全局对象与缓冲区声明（使用 extern 避免重复定义）
extern MatrixPanel_I2S_DMA *matrix;
extern lv_disp_draw_buf_t draw_buf;
extern lv_color_t buf[PANEL_RES_X * PANEL_RES_Y]; 

HUB75_I2S_CFG get_matrix_config();

// 4. 声明自定义的低分辨率中文字体
// 必须确保 'my_zh_font_12.c' 文件存在于当前项目文件夹内！
LV_FONT_DECLARE(my_zh_font_12); 
LV_FONT_DECLARE(my_zh_font_14); 
// 5. 函数原型声明
void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

#endif // LED_FUNCTION_H
