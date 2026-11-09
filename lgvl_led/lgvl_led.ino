#include "Led_Function.h"

// 实体变量的分配与初始化
MatrixPanel_I2S_DMA *matrix = nullptr;
lv_disp_draw_buf_t draw_buf;
lv_color_t buf[PANEL_RES_X * PANEL_RES_Y]; 

void setup() {
    Serial.begin(115200);
    delay(1000); // 留出时间让串口监视器连接

    Serial.println("正在初始化 HUB75 点阵屏硬件驱动...");

    // 使用 LedFunction.h 中定义的引脚配置 HUB75 DMA 结构体
    HUB75_I2S_CFG config = get_matrix_config();
    // 分配内存并启动 DMA 矩阵屏驱动
    matrix = new MatrixPanel_I2S_DMA(config);
    if(not matrix->begin()){
        Serial.println("❌ HUB75 初始化失败！请检查引脚映射或内存空间。");
        while(1);
    }
    matrix->setBrightness8(128); // 设置全局屏幕亮度 (0-255)
    Serial.println("✅ HUB75 点阵屏驱动初始化成功。");

    // 初始化 LVGL 图形引擎核心
    lv_init();
    
    // 绑定刚才分配的屏幕像素显存块
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, PANEL_RES_X * PANEL_RES_Y);

    // 设置并向 LVGL 注册显示设备属性
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = PANEL_RES_X;
    disp_drv.ver_res = PANEL_RES_Y;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    Serial.println("✅ LVGL 图形引擎初始化成功。");

    // UI 页面设计：创建文本对象
    lv_obj_t *scr = lv_scr_act();
    // 强制将背景设置为纯黑，以完美贴合点阵屏发光颗粒
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0); 

    // 创建标签（Label）文本容器
    lv_obj_t *label = lv_label_create(scr);
    
    // 应用自定义的 12px 中文字体资产
    lv_obj_set_style_text_font(label, &my_zh_font_12, 0); 
    
    // 设置文本颜色（这里设置为纯红色。你可以通过 lv_color_make(R, G, B) 自定义）
    lv_obj_set_style_text_color(label, lv_color_make(255, 0, 0), 0); 
    
    // 填充文字内容
    lv_label_set_text(label, "佳豪，你好");
    
    // 强制让标签在 64x32 像素的网格中绝对居中对齐
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0); 
    
    Serial.println("✅ UI 界面创建完毕。正在显示内容...");
}

void loop() {
    // 保持 LVGL 内部时钟、任务调度、后台渲染管线处于活跃状态
    lv_timer_handler(); 
    delay(5);
}
