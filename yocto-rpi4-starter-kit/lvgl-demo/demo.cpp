#include "lvgl/lvgl.h"
#include "demo.h"


void demo_gui(void)
{
    lv_obj_t* screen = lv_obj_create(NULL);

    lv_obj_set_style_bg_color(screen, lv_color_hex(0xA0A0A0), 0); 
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    
    lv_obj_t* label = lv_label_create(screen);
    lv_label_set_text(label, "Hello, World!");
    lv_obj_center(label);

    lv_scr_load(screen);
}
