#pragma once
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "esp_lcd_io_i2c.h"
#include "driver/i2c_master.h"
// #include "lvgl/lvgl"


typedef struct Display_config{
    i2c_master_bus_handle_t master_bus_handle;
    gpio_num_t io_scl;
    gpio_num_t io_sda;
    i2c_port_num_t port_num;
    uint32_t dev_addr;
}Display_config;

typedef struct Display_t{
    // lv_obj *disp;
}Display_t;

typedef Display_t *disp_handle;

disp_handle init_display(Display_config *cfg);
