#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include <unistd.h>
#include <time.h>
#include "lvgl/src/drivers/evdev/lv_evdev.h"
#include <stdio.h>
#include "demo.h"

int main(void)
{
    lv_init();

    lv_display_t * disp = lv_linux_fbdev_create();
    lv_linux_fbdev_set_file(disp, "/dev/fb0");

    lv_indev_t * indev = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/touchscreen0");

    if(indev == NULL) {
        printf("Touch input error!\n");
    }

    demo_gui();

    /*Handle LVGL tasks*/
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}
