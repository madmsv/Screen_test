#include <stdio.h>
#include "Display.h"
#include "ultrasonic.h"

void app_main(void)
{
    Display_config disp_cfg = {
        .dev_addr = 0x3C, //0x3C
        .height_res = 64,
        .width_res = 128,
        .io_scl = GPIO_NUM_6,
        .io_sda = GPIO_NUM_7,
        .port_num = 1, // ändra port num fär esp32c6 till 0
    };
    disp_handle display = init_display(&disp_cfg);
    esp_err_t err;

    ultrasonic_config_t sonic_cfg = {
        .echo_pin = GPIO_NUM_15,
        .trig_pin = GPIO_NUM_21,
    };
    
    ultrasonic_init(&sonic_cfg);

    err = add_lable(display, "Hello Espressif, Hello LVGL.");
    // err = add_menu(display, "Main");

    while(1){

        vTaskDelay(pdMS_TO_TICKS(30));
    }
}