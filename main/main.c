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
    esp_err_t err;
    err = add_lable(display, "Hello Espressif, Hello LVGL.");
    // err = add_menu(display, "Main");

    while(1){

        vTaskDelay(pdMS_TO_TICKS(30));
    }
}