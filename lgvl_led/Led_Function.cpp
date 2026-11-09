#include "Led_Function.h"

// 刷新回调函数
void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            uint16_t color = color_p->full; 
            matrix->drawPixel(x, y, color); // 已修正为标准的 drawPixel
            color_p++;
        }
    }
    lv_disp_flush_ready(disp_drv);
}

// 实现获取硬件配置的函数
HUB75_I2S_CFG get_matrix_config() {
    HUB75_I2S_CFG mxconfig(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN);
    
    mxconfig.gpio.r1 = R1_PIN;
    mxconfig.gpio.g1 = G1_PIN;
    mxconfig.gpio.b1 = BL1_PIN;
    mxconfig.gpio.r2 = R2_PIN;
    mxconfig.gpio.g2 = G2_PIN;
    mxconfig.gpio.b2 = BL2_PIN;
    
    mxconfig.gpio.a  = CH_A_PIN;
    mxconfig.gpio.b  = CH_B_PIN;
    mxconfig.gpio.c  = CH_C_PIN;
    mxconfig.gpio.d  = CH_D_PIN;
    mxconfig.gpio.e  = CH_E_PIN; 
    
    mxconfig.gpio.lat = LAT_PIN;
    mxconfig.gpio.oe  = OE_PIN;
    mxconfig.gpio.clk = CLK_PIN;
    
    return mxconfig;
}
