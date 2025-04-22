#include <stdio.h>
#include "Display.h"

void app_main(void)
{
    Display_config disp_cfg = {
        .dev_addr = 0x3C, //0x3C
        .height_res = 64,
        .width_res = 128,
        .io_scl = GPIO_NUM_12,
        .io_sda = GPIO_NUM_11,
        .port_num = 1,
    };
    disp_handle display = init_display(&disp_cfg);

    lv_obj_t *scr = lv_disp_get_scr_act(display->disp);
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(label, "Hello Espressif, Hello LVGL."); //"Hello Espressif, Hello LVGL."
    /* Size of the screen (if you use rotation 90 or 270, please use lv_display_get_vertical_resolution) */
    lv_obj_set_width(label, 128);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    while(1){

        vTaskDelay(pdMS_TO_TICKS(30));
    }
}