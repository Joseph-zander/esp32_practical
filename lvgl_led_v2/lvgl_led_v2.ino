#include "Led_Function.h"

// 实体变量的分配与初始化
MatrixPanel_I2S_DMA *matrix = nullptr;
lv_disp_draw_buf_t draw_buf;
lv_color_t buf[PANEL_RES_X * PANEL_RES_Y];

void setup() {
  Serial.begin(115200);
  delay(1000);  // 留出时间让串口监视器连接

  Serial.println("正在初始化 HUB75 点阵屏硬件驱动...");

  // 使用 LedFunction.h 中定义的引脚配置 HUB75 DMA 结构体
  HUB75_I2S_CFG config = get_matrix_config();
  // 分配内存并启动 DMA 矩阵屏驱动
  matrix = new MatrixPanel_I2S_DMA(config);
  if (not matrix->begin()) {
    Serial.println("❌ HUB75 初始化失败！请检查引脚映射或内存空间。");
    while (1)
      ;
  }
  matrix->setBrightness8(128);  // 设置全局屏幕亮度 (0-255)
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

  // ==================== 【绘制全屏矩形边框】 ====================
  // 1. 创建一个基础对象作为边框容器
  lv_obj_t *border_obj = lv_obj_create(scr);
  
  // 2. 强制将其大小设置为 64x32（占满整个点阵屏）
  lv_obj_set_size(border_obj, PANEL_RES_X, PANEL_RES_Y);
  lv_obj_align(border_obj, LV_ALIGN_CENTER, 0, 0);
  
  // 3. 【已修复】将其背景设置为完全透明（从 LV_OPA_TRANSPARENT 改为 LV_OPA_TRANSP）
  lv_obj_set_style_bg_opa(border_obj, LV_OPA_TRANSP, 0);
  
  // 4. 清除 LVGL 默认自带的实体阴影和内边距（防止边框变形或错位）
  lv_obj_set_style_shadow_width(border_obj, 0, 0);
  lv_obj_set_style_pad_all(border_obj, 0, 0);
  lv_obj_set_style_radius(border_obj, 0, 0); // 设为直角边框
  
  // 5. 设置边框粗细为 1 像素，颜色为纯红色
  lv_obj_set_style_border_width(border_obj, 1, 0);
  lv_obj_set_style_border_color(border_obj, lv_color_make(255, 0, 0), 0);
  // ==================================================================

  // 1. 创建用于显示“问题”的标签
  lv_obj_t *q_label = lv_label_create(scr);

  // 2. 设置长文本模式为：循环滚动模式 (跑马灯)
  lv_label_set_long_mode(q_label, LV_LABEL_LONG_SCROLL_CIRCULAR);

  // 3. 强制限制标签的宽度为 64 像素（点阵屏的物理全宽）
  lv_obj_set_width(q_label, PANEL_RES_X);

  // 4. 应用你的自定义 12px 中文字体
  lv_obj_set_style_text_font(q_label, &my_zh_font_14, 0);

  // 5. 设置文本颜色为纯绿色 (R=0, G=255, B=0)
  lv_obj_set_style_text_color(q_label, lv_color_make(0, 255, 0), 0);

  // 6. 填入你的长问题
  lv_label_set_text(q_label, "欢迎赵佳豪赵佳林光临中大国际！");

  // 7. 调整滚动速度（8 像素/秒，非常平稳清晰）
  lv_obj_set_style_anim_speed(q_label, 8, LV_PART_MAIN);

  // 8. 绝对对齐：让它在 64x32 像素的屏幕中垂直完美居中，水平靠左开始滚动
  lv_obj_align(q_label, LV_ALIGN_LEFT_MID, 0, 0);

  Serial.println("✅ UI 界面创建完毕。正在显示内容...");
}

void loop() {
  // 明确告知 LVGL 每次循环时间过去了 5 毫秒
  // 只有这样，LVGL 内部的动画(Anim)定时器才会运转，文字才会滚起来！
  lv_tick_inc(5);

  // 保持 LVGL 内部时钟、任务调度、后台渲染管线处于活跃状态
  lv_timer_handler();
  delay(5);
}
