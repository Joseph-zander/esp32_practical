#ifndef LED_FUNCTION_H
#define LED_FUNCTION_H

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// 1. 屏幕物理尺寸配置
#define PANEL_RES_X 64  // 64列
#define PANEL_RES_Y 32  // 32行
#define PANEL_CHAIN 1   // 单块屏


// 2. 根据你的硬件完全映射 MCU 的 GPIO 口
#define R1_PIN 4
#define G1_PIN 5
#define BL1_PIN 6  // 对应你的 B1
#define R2_PIN 7
#define G2_PIN 1
#define BL2_PIN 2    // 对应你的 B2
#define CH_A_PIN 10  // 对应你的 A
#define CH_B_PIN 8   // 对应你的 B
#define CH_C_PIN 9   // 对应你的 C
#define CH_D_PIN 11  // 对应你的 D
#define CH_E_PIN -1  // 32高屏不需要 E 轴引脚
#define LAT_PIN 40   // 对应你的 LAT
#define OE_PIN 14    // 对应你的 OE
#define CLK_PIN 41   // 对应你的 CLK

// 3. 内置一个便捷的引脚初始化配置函数
inline HUB75_I2S_CFG get_matrix_config() {
  HUB75_I2S_CFG mxconfig(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN);

  // 将你的引脚宏分配给驱动库
  mxconfig.gpio.r1 = R1_PIN;
  mxconfig.gpio.g1 = G1_PIN;
  mxconfig.gpio.b1 = BL1_PIN;
  mxconfig.gpio.r2 = R2_PIN;
  mxconfig.gpio.g2 = G2_PIN;
  mxconfig.gpio.b2 = BL2_PIN;
  mxconfig.gpio.a = CH_A_PIN;
  mxconfig.gpio.b = CH_B_PIN;
  mxconfig.gpio.c = CH_C_PIN;
  mxconfig.gpio.d = CH_D_PIN;
  //e_pin 为空无需配置
  mxconfig.gpio.lat = LAT_PIN;
  mxconfig.gpio.oe = OE_PIN;
  mxconfig.gpio.clk = CLK_PIN;

  //打开双缓冲
  mxconfig.double_buff = true; // 👈 必须在这里显式开启双缓冲标志
  // ESP32-S3 驱动 64x32 推荐的默认时钟与反转配置
  mxconfig.clkphase = false;

  return mxconfig;
}


#endif