#pragma once
#include <sys/lock.h>
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"

#include "esp_lcd_io_i2c.h"
// #include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_sh1106.h"
// #include "esp_lcd_sh1107.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"

#include "esp_lvgl_port.h"
#include "lvgl.h"

// #define EXAMPLE_LVGL_PALETTE_SIZE      8

/// @brief Most members must be set, except for `master_bus_handle`
typedef struct Display_config{
    gpio_num_t io_scl;          //!Must be set before use                
    gpio_num_t io_sda;          //!Must be set before use    
    i2c_port_num_t port_num;    //!Must be set before use            
    uint32_t dev_addr;          //!Must be set before use             
    uint8_t height_res;         //!Must be set before use             
    uint8_t width_res;          //!Must be set before use             
    i2c_master_bus_handle_t master_bus_handle;
}Display_config;

typedef struct Display_t{
    lv_disp_t *disp;
}Display_t;

typedef Display_t *disp_handle;

disp_handle init_display(Display_config *cfg);

esp_err_t add_lable(disp_handle disp, char * txt);
esp_err_t add_menu(disp_handle display, char *name);
